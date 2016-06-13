/*
 * From SDR-J
 */

#ifndef __DAB_UTIL__FFT
#define __DAB_UTIL__FFT

#include <constants/transmission_modes.h>
#include <types/common_types.h>

#include <fftw3.h>

#include <cstdint>
#include <vector>

namespace dab
  {

  namespace __internal_demod
    {

    /**
     * @internal
     * @author Felix Morgner
     *
     * @brief A fftw3f wrapper
     *
     * This class is a wrapper around fftw3f
     * Most of this implementation is reproduced and adapted from Jan van Katwijk's
     * dab-rpi (https://github.com/JvanKatwijk/dab-rpi) which is released under GPLv2.
     */
    struct fft
      {
      explicit fft(std::uint32_t const length, int direction);
      ~fft();

      void operator()();

      __internal_common::sample_t * data() const;

      private:
        std::uint32_t const m_length;
        int const m_direction;
        __internal_common::sample_t * m_data;

        fftwf_plan m_plan;
      };

    }

  }

#endif

