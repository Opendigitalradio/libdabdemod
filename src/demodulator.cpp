/*
 * Based on SDR-J
 */
#include "demodulator.h"

#include <constants/sample_rate.h>
#include <literals/binary_literal.h>

#include <array>
#include <cmath>

namespace dab
  {

  using namespace __internal_common;
  using namespace __internal_common::types;

  demodulator::demodulator(transmission_mode const & mode, sample_queue_t & samples)
    : m_mode{mode},
      m_sampleQueue{samples}
    {
    m_oscilator.resize(kDefaultSampleRate);

    for(std::size_t index{}; index < m_oscilator.size(); ++index)
      {
      m_oscilator[index] = sample_t(cos(2.0f * M_PI * index / m_oscilator.size()), sin(2.0 * M_PI * index / m_oscilator.size()));
      }

    m_symbolBuffer.resize(m_mode.frame_symbols + 1 * m_mode.symbol_duration);
    }

  void demodulator::operator()()
    {
    using namespace __internal_common::literals;
    auto constexpr bufferMask = 1111111111111111_b;

    auto bufferIndex = 0;
    auto currentSignalLevel = 0.0f;

    for(auto sampleIdx = 0; sampleIdx < 10 * m_mode.symbol_duration; ++sampleIdx)
      {
      auto sample = get_sample(0);
      currentSignalLevel += abs(sample);
      }

    m_signalLevel = currentSignalLevel / (10 * m_mode.symbol_duration);
    std::array<float, bufferMask + 1> envelopeBuffer{};

notSynced:

    for(auto sampleIdx = 0; sampleIdx < m_mode.symbol_duration; ++sampleIdx)
      {
      auto sample = get_sample(0);
      envelopeBuffer[bufferIndex] = std::abs(sample);
      bufferIndex = (bufferIndex + 1) & bufferMask;
      }

    auto currentStrength = 0.0f;

    for(auto idx = bufferIndex - 50; idx < bufferIndex; ++idx)
      {
      currentStrength += envelopeBuffer[idx];
      }

    auto tries = 0;

    while(currentStrength / 50 > 0.4f * m_signalLevel)
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

//syncOnPhase:

    for(auto sampleIdx = 0; sampleIdx < m_mode.useful_duration; ++sampleIdx)
      {
      m_symbolBuffer[sampleIdx] = get_sample(m_coarseCorrection + m_fineCorrection);
      envelopeBuffer[bufferIndex] = std::abs(m_symbolBuffer[sampleIdx]);
      currentStrength += envelopeBuffer[bufferIndex] - envelopeBuffer[(bufferIndex + envelopeBuffer.size() - 50) & bufferMask];
      bufferIndex = (bufferIndex + 1) & bufferMask;
      ++tries;
      }
    }

  sample_t demodulator::get_sample(std::int32_t const phaseShift)
    {
    sample_t sample{};
    m_sampleQueue.wait_dequeue(sample);

    m_phaseShift = (m_phaseShift - phaseShift + kDefaultSampleRate) % kDefaultSampleRate;
    sample *= m_oscilator[m_phaseShift];
    m_signalLevel = 0.00001 * std::abs(sample) + (1 - 0.00001) * m_signalLevel;

    return sample;
    }

  void demodulator::get_samples(std::vector<sample_t> & target, std::int16_t count, std::int32_t phaseShift)
    {
    for(std::int16_t iteration{}; iteration < count; ++count)
      {
      m_sampleQueue.wait_dequeue(target[iteration]);
      m_phaseShift = (m_phaseShift - phaseShift + kDefaultSampleRate) % kDefaultSampleRate;
      target[iteration] *= m_oscilator[m_phaseShift];
      m_signalLevel = 0.00001 * std::abs(target[iteration]) + (1 - 0.00001) * m_signalLevel;
      }
    }

  }
