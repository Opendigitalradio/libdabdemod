#include "util/fft.h"

#include <cstring>

/* please check the associated header for references to the original source */

namespace dab
  {
  using namespace dab::__internal_common;

  namespace __internal_demod
    {

    fft::fft(std::uint32_t const length, int direction)
      : m_length{length},
        m_direction{direction},
        m_data{static_cast<sample_t *>(fftwf_malloc(sizeof(sample_t) * m_length))}
      {
      std::memset(m_data, 0, sizeof(sample_t) * m_length);
      m_plan = fftwf_plan_dft_1d(m_length,
                                 reinterpret_cast<fftwf_complex *>(m_data),
                                 reinterpret_cast<fftwf_complex *>(m_data),
                                 m_direction,
                                 FFTW_ESTIMATE);
      }

    fft::~fft()
      {
      fftwf_destroy_plan(m_plan);
      fftwf_free(m_data);
      }

    sample_t * fft::data() const
      {
      return m_data;
      }

    void fft::operator()()
      {
      fftwf_execute(m_plan);

      if(m_direction == FFTW_BACKWARD)
        {
        auto const scalingFactor = 1.0 / float(m_length);
        for(auto elementIdx = 0u; elementIdx < m_length; ++elementIdx)
          {
          m_data[elementIdx] *= scalingFactor;
          }
        }
      }

    }

  }

