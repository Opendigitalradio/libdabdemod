#ifndef __DAB_UTIL__DEINTERLEAVER
#define __DAB_UTIL__DEINTERLEAVER

#include <constants/transmission_modes.h>

#include <cstdint>
#include <vector>

namespace dab
  {

  namespace __internal_demod
    {

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

