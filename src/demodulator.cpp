#include "demodulator.h"

#include <constants/sample_rate.h>
#include <literals/binary_literal.h>

#include <array>
#include <cmath>
#include <cstring>

/* please check the associated header for references to the original source */

namespace
  {
  struct poison_pill {};
  }

namespace dab
  {

  using namespace __internal_common;
  using namespace __internal_common::types;

  demodulator::demodulator(sample_queue_t & samples, symbol_queue_t & symbols, transmission_mode const & mode)
    : m_mode{mode},
      m_sampleQueue{samples},
      m_phaseReference{m_mode, 3},
      m_correlationFft{m_mode.useful_duration, FFTW_FORWARD},
      m_oscilator(kDefaultSampleRate),
      m_symbolBuffer((m_mode.frame_symbols + 1) * m_mode.symbol_duration),
      m_correlationBuffer(kSearchRange + kCorrelationLength),
      m_symbolHandler{m_mode, symbols}
    {
    for(std::size_t index{}; index < m_oscilator.size(); ++index)
      {
      m_oscilator[index] = sample_t(cos(2.0f * M_PI * index / m_oscilator.size()), sin(2.0 * M_PI * index / m_oscilator.size()));
      }

    m_refArg.resize(kCorrelationLength);
    for(int idx{}; idx < kCorrelationLength; ++idx)
      {
      m_refArg[idx] = std::arg(m_phaseReference.table()[(m_mode.useful_duration + idx) % m_mode.useful_duration] *
                               std::conj(m_phaseReference.table()[(m_mode.useful_duration + idx + 1) % m_mode.useful_duration]));
      }

    m_symbolHandlerFuture = std::async(std::launch::async, [&]{ m_symbolHandler.run(); });
    }

  demodulator::~demodulator()
    {
    m_symbolHandler.stop();
    m_symbolHandlerFuture.get();
    }

  void demodulator::run() try
    {
    using namespace __internal_common::literals;
    auto constexpr bufferMask = 111111111111111_b;

    auto bufferIndex = 0;

    auto previous1 = 1000;
    auto previous2 = 1000;

    for(auto sampleIdx = 0; sampleIdx < 10 * m_mode.symbol_duration; ++sampleIdx)
      {
      m_signalLevel += abs(get_sample(0));
      }

    std::array<float, bufferMask + 1> envelopeBuffer{};
    auto const carrierSpacing = 1536 / m_mode.carriers * 1000;

notSynced:

    for(auto sampleIdx = 0; sampleIdx < m_mode.symbol_duration - 50; ++sampleIdx)
      {
      auto sample = get_sample(0);
      envelopeBuffer[bufferIndex] = std::abs(sample);
      bufferIndex = (bufferIndex + 1) & bufferMask;
      }

    auto currentStrength = 0.0f;

    for(auto idx = 0; idx < 50; ++idx)
      {
      auto sample = get_sample(0);
      envelopeBuffer[bufferIndex] = abs(sample);
      currentStrength += envelopeBuffer[bufferIndex];
      bufferIndex = (bufferIndex + 1) & bufferMask;
      }

    auto tries = 0;

    while(currentStrength / 50 > 0.5f * m_signalLevel)
      {
      auto sample = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex] = std::abs(sample);
      currentStrength += envelopeBuffer[bufferIndex] - envelopeBuffer[(bufferIndex + envelopeBuffer.size() - 50) & bufferMask];
      bufferIndex = (bufferIndex + 1) & bufferMask;
      ++tries;

      if(tries > 2 * m_mode.symbol_duration)
        {
        goto notSynced;
        }
      }

    while(currentStrength / 50 < 0.75f * m_signalLevel)
      {
      auto sample = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex] = std::abs(sample);
      currentStrength += envelopeBuffer[bufferIndex] - envelopeBuffer[(bufferIndex + envelopeBuffer.size() - 50) & bufferMask];
      bufferIndex = (bufferIndex + 1) & bufferMask;
      ++tries;
      if(tries > 3 * m_mode.symbol_duration)
        {
        goto notSynced;
        }
      }

syncOnPhase:

    for(auto sampleIdx = 0; sampleIdx < m_mode.useful_duration; ++sampleIdx)
      {
      m_symbolBuffer[sampleIdx] = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex] = std::abs(m_symbolBuffer[sampleIdx]);
      currentStrength += envelopeBuffer[bufferIndex] - envelopeBuffer[(bufferIndex + envelopeBuffer.size() - 50) & bufferMask];
      bufferIndex = (bufferIndex + 1) & bufferMask;
      ++tries;
      }

    auto startIndex = m_phaseReference.index(m_symbolBuffer.data(), m_mode.useful_duration);
    if(startIndex < 0 )
      {
      goto notSynced;
      }

    std::memmove(m_symbolBuffer.data(), m_symbolBuffer.data() + startIndex,
                (m_mode.useful_duration - startIndex) * sizeof(sample_t));
    auto symbolBufferIndex = m_mode.useful_duration - startIndex;

    m_didSync = true;
    get_samples(&m_symbolBuffer[symbolBufferIndex],
                m_mode.useful_duration - symbolBufferIndex,
                m_coarseCorrection + m_fineCorrection);

    m_symbolHandler.handle_prs({m_symbolBuffer.cbegin(), m_symbolBuffer.cbegin() + m_mode.useful_duration});

    if(m_synchronizeOnPRS)
      {
      int correction = handle_prs();
      if(!correction && !previous1 && !previous2)
        {
        m_synchronizeOnPRS = false;
        }
      else if(correction != 100)
        {
        m_coarseCorrection += correction * carrierSpacing;
        if(std::abs(m_coarseCorrection) > 35000)
          {
          m_coarseCorrection = 0;
          }
        previous2 = previous1;
        previous1 = correction;
        }
      }

    auto frequencyCorrection = sample_t{};
    auto offset = m_mode.guard_duration;
    auto usefulEnd = offset + m_mode.useful_duration;

    for(int idx{}; idx < m_mode.fic_symbols; ++idx)
      {
      get_samples(m_symbolBuffer.data(), m_mode.symbol_duration, m_coarseCorrection + m_fineCorrection);

      for(int j = m_mode.useful_duration; j < m_mode.symbol_duration; ++j)
        {
        frequencyCorrection += m_symbolBuffer[j] * conj(m_symbolBuffer[j - m_mode.useful_duration]);
        }

      m_symbolHandler.handle_fic({m_symbolBuffer.cbegin() + offset, m_symbolBuffer.cbegin() + usefulEnd});
      }

    for(int i = 0; i < m_mode.msc_symbols; ++i)
      {
      get_samples(m_symbolBuffer.data(), m_mode.symbol_duration, m_coarseCorrection + m_fineCorrection);

      for(int j = m_mode.useful_duration; j < m_mode.symbol_duration; ++j)
        {
        frequencyCorrection += m_symbolBuffer[j] * conj(m_symbolBuffer[j - m_mode.useful_duration]);
        }

      m_symbolHandler.handle_msc({m_symbolBuffer.cbegin() + offset, m_symbolBuffer.cbegin() + usefulEnd});
      }

    m_fineCorrection += 0.1 * std::arg(frequencyCorrection) / M_PI * carrierSpacing / 2;

    bufferIndex = 0;
    currentStrength = 0;
    for(int i{}; i < m_mode.null_duration - 50; ++i)
      {
      auto sample = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex++] = std::abs(sample);
      }

    for(int i = m_mode.null_duration - 50; i < m_mode.null_duration; ++i)
      {
      auto sample = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex] = std::abs(sample);
      currentStrength += envelopeBuffer[bufferIndex++];
      }

    tries = 0;

    if(m_fineCorrection > carrierSpacing / 2)
      {
      m_coarseCorrection += carrierSpacing;
      m_fineCorrection -= carrierSpacing;
      }
    else if(m_fineCorrection < -carrierSpacing / 2)
      {
      m_coarseCorrection -= carrierSpacing;
      m_fineCorrection += carrierSpacing;
      }

    goto syncOnPhase;
    }
  catch(poison_pill const &)
    {
    return;
    }

  void demodulator::stop()
    {
    m_stop.store(true, std::memory_order_release);
    }

  sample_t demodulator::get_sample(std::int32_t const phaseShift)
    {
    sample_t sample{};

    while(!m_sampleQueue.wait_dequeue_timed(sample, 10))
      {
      if(m_stop.load(std::memory_order_acquire))
        {
        throw poison_pill{};
        }
      }

    m_phaseShift -= phaseShift;
    m_phaseShift = (m_phaseShift + kDefaultSampleRate) % kDefaultSampleRate;
    sample *= m_oscilator[m_phaseShift];
    m_signalLevel = 0.00001 * std::abs(sample) + (1 - 0.00001) * m_signalLevel;

    return sample;
    }

  void demodulator::get_samples(sample_t * target, std::int16_t count, std::int32_t phaseShift)
    {
    for(std::int16_t iteration{}; iteration < count; ++iteration)
      {
      target[iteration] = get_sample(phaseShift);
      }
    }

  std::int16_t demodulator::handle_prs()
    {
    auto fftBuffer = m_correlationFft.data();
    memcpy(fftBuffer, m_symbolBuffer.data(), m_mode.useful_duration * sizeof(sample_t));
    m_correlationFft();

    for(int idx{}; idx < kSearchRange + kCorrelationLength; ++idx)
      {
      auto baseIndex = m_mode.useful_duration - kSearchRange / 2 + idx;
      auto f = fftBuffer[baseIndex % m_mode.useful_duration];
      auto g = conj(fftBuffer[(baseIndex + 1) % m_mode.useful_duration]);
      auto h = arg(f * g);
      m_correlationBuffer[idx] = h;
      }

    auto currentMax = 0.f;
    auto oldMax = 0.f;
    auto index = 100;

    for(int idx{}; idx < kSearchRange; ++idx)
      {
      auto sum = 0.f;

      for(int pos{}; pos < kCorrelationLength; ++pos)
        {
        sum += std::abs(m_refArg[pos] * m_correlationBuffer[idx + pos]);
        if(sum > currentMax)
          {
          oldMax = currentMax;
          currentMax = sum;
          index = idx;
          }
        }
      }

    auto ret = m_mode.useful_duration - kSearchRange / 2 + index - m_mode.useful_duration;

    return ret;
    }

  }
