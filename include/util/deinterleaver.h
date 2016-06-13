#ifndef __DAB_UTIL__DEINTERLEAVER
#define __DAB_UTIL__DEINTERLEAVER

#include <constants/transmission_modes.h>

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
     * @brief A DAB frequency deinterleaver
     *
     * This class deinterleaves the DAB OFDM carriers.
     * Most of this implementation is reproduced and adapted from Jan van Katwijk's
     * dab-rpi (https://github.com/JvanKatwijk/dab-rpi) which is released under GPLv2.
     */
    struct deinterleaver
      {
      explicit deinterleaver(__internal_common::types::transmission_mode const & mode);

      std::int16_t map(std::int16_t const carrier);

      private:
        std::vector<std::int16_t> m_permutationTable;
      };

    }

  }

#endif

