/*
 * Based on SDR-J
 */
#include "demodulator.h"

#include <cmath>

namespace dab
  {

  using namespace __internal_common;
  using namespace __internal_common::types;

  demodulator::demodulator(transmission_mode const & mode, sample_queue_t & samples)
    : m_mode{mode},
      m_sampleQueue{samples}
    {
    m_oscilator.resize(2048000);

    for(std::size_t index{}; index < m_oscilator.size(); ++index)
      {
      m_oscilator[index] = sample_t(cos(2.0f * M_PI * index / m_oscilator.size()), sin(2.0 * M_PI * index / m_oscilator.size()));
      }
    }

  void demodulator::operator()()
    {

    }

  sample_t demodulator::get_sample(std::int32_t const phaseShift)
    {
    sample_t sample{};
    m_sampleQueue.wait_dequeue(sample);

    m_phaseShift = (m_phaseShift - phaseShift + 2048000) % 2048000;
    sample *= m_oscilator[m_phaseShift];
    m_signalLevel = 0.00001 * std::abs(sample) + (1 - 0.00001) * m_signalLevel;

    return sample;
    }

  }
