#include "dab/util/phase_reference.h"
#include "dab/constants/phase_table.h"

#include <cstring>

/* please check the associated header for references to the original source */

namespace dab
  {

  using namespace internal;
  using namespace internal::types;

  namespace __internal_demod
    {

    phase_reference::phase_reference(transmission_mode const & mode, std::int16_t const signalLevel)
      : m_mode{mode},
        m_signalLevel{signalLevel},
				m_table(m_mode.useful_duration),
				m_forward{m_mode.fft_length, FFTW_FORWARD},
				m_backward{m_mode.fft_length, FFTW_BACKWARD}
      {
      for(auto carrier = 1; carrier <= m_mode.carriers / 2; ++carrier)
        {
        auto phiK = constants::calculate_phi(m_mode, carrier);
        m_table[carrier] = sample_t(cos(phiK), sin(phiK));
        phiK = constants::calculate_phi(m_mode, -carrier);
        m_table[m_mode.useful_duration - carrier] = sample_t(cos(phiK), sin(phiK));
        }
      }

    std::int32_t phase_reference::index(sample_t * data, std::uint32_t const size)
      {
      auto fwdFftBuffer = m_forward.data();
      std::memcpy(fwdFftBuffer, data, size * sizeof(sample_t));
      if(size < m_mode.useful_duration)
        {
        std::memset(&fwdFftBuffer[size], 0, (m_mode.useful_duration - size) * sizeof(sample_t));
        }

      m_forward();

      auto bwdFftBuffer = m_backward.data();
      for(auto idx = 0; idx < m_mode.useful_duration; ++idx)
        {
        bwdFftBuffer[idx] = fwdFftBuffer[idx] * std::conj(m_table[idx]);
        }
      m_backward();

      auto sum = 0.0f;
      for(auto idx = 0; idx < m_mode.useful_duration; ++idx)
        {
        sum += std::abs(bwdFftBuffer[idx]);
        }

      auto max = -10000.0f;
      auto maxIndex = -1;
      for(auto idx = 0u; idx < size; ++idx)
        {
        auto magnitude = std::abs(bwdFftBuffer[idx]);

        if(magnitude > max)
          {
          maxIndex = idx;
          max = magnitude;
          }
        }

      if(max < m_signalLevel * sum / size)
        {
        return -std::abs(max * size / sum) - 1;
        }
      else
        {
        return maxIndex;
        }
      }

    std::vector<sample_t> const & phase_reference::table()
      {
      return m_table;
      }

    }

  }

