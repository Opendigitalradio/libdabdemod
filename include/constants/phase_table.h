#ifndef __DAB_CONSTANTS__PHASE_TABLE
#define __DAB_CONSTANTS__PHASE_TABLE

#include <constants/transmission_modes.h>

#include <array>
#include <cmath>
#include <cstdint>

namespace dab
  {

  namespace __internal_demod
    {

    struct phase_table_entry
      {
      std::int16_t const minimumCarrier;
      std::int16_t const maximumCarrier;
      std::uint8_t const i;
      std::uint8_t const n;
      };

    namespace constants
      {

      phase_table_entry constexpr kPhaseTableTransmissionMode1[] = {
        {-768, -737, 0, 1},
        {-736, -705, 1, 2},
        {-704, -673, 2, 0},
        {-672, -641, 3, 1},
        {-640, -609, 0, 3},
        {-608, -577, 1, 2},
        {-576, -545, 2, 2},
        {-544, -513, 3, 3},
        {-512, -481, 0, 2},
        {-480, -449, 1, 1},
        {-448, -417, 2, 2},
        {-416, -385, 3, 3},
        {-384, -353, 0, 1},
        {-352, -321, 1, 2},
        {-320, -289, 2, 3},
        {-288, -257, 3, 3},
        {-256, -225, 0, 2},
        {-224, -193, 1, 2},
        {-192, -161, 2, 2},
        {-160, -129, 3, 1},
        {-128,  -97, 0, 1},
        {-96,   -65, 1, 3},
        {-64,   -33, 2, 1},
        {-32,    -1, 3, 2},
        {  1,    32, 0, 3},
        { 33,    64, 3, 1},
        { 65,    96, 2, 1},
        { 97,   128, 1, 1},
        { 129,  160, 0, 2},
        { 161,  192, 3, 2},
        { 193,  224, 2, 1},
        { 225,  256, 1, 0},
        { 257,  288, 0, 2},
        { 289,  320, 3, 2},
        { 321,  352, 2, 3},
        { 353,  384, 1, 3},
        { 385,  416, 0, 0},
        { 417,  448, 3, 2},
        { 449,  480, 2, 1},
        { 481,  512, 1, 3},
        { 513,  544, 0, 3},
        { 545,  576, 3, 3},
        { 577,  608, 2, 3},
        { 609,  640, 1, 0},
        { 641,  672, 0, 3},
        { 673,  704, 3, 0},
        { 705,  736, 2, 1},
        { 737,  768, 1, 1}
      };

      phase_table_entry constexpr kPhaseTableTransmissionMode2[] = {
        {-192, -161, 0,  2},
        {-160, -129, 1,  3},
        {-128,  -97, 2,  2},
        {-96,   -65, 3,  2},
        {-64,   -33, 0,  1},
        {-32,    -1, 1,  2},
        {1,      32, 2,  0},
        {33,     64, 1,  2},
        {65,     96, 0,  2},
        {97,    128, 3,  1},
        {129,   160, 2,  0},
        {161,   192, 1,  3},
      };

      phase_table_entry constexpr kPhaseTableTransmissionMode3[] = {
        {-96, -65, 0, 2},
        {-64, -33, 1, 3},
        {-32,  -1, 2, 0},
        {  1,  32, 3, 2},
        { 33,  64, 2, 2},
        { 65,  96, 1, 2},
      };

      phase_table_entry constexpr kPhaseTableTransmissionMode4[] = {
        {-384, -353, 0, 0},
        {-352, -321, 1, 1},
        {-320, -289, 2, 1},
        {-288, -257, 3, 2},
        {-256, -225, 0, 2},
        {-224, -193, 1, 2},
        {-192, -161, 2, 0},
        {-160, -129, 3, 3},
        {-128,  -97, 0, 3},
        {-96,   -65, 1, 1},
        {-64,   -33, 2, 3},
        {-32,    -1, 3, 2},
        {  1,    32, 0, 0},
        { 33,    64, 3, 1},
        { 65,    96, 2, 0},
        { 97,   128, 1, 2},
        { 129,  160, 0, 0},
        { 161,  192, 3, 1},
        { 193,  224, 2, 2},
        { 225,  256, 1, 2},
        { 257,  288, 0, 2},
        { 289,  320, 3, 1},
        { 321,  352, 2, 3},
        { 353,  384, 1, 0}
      };

      std::array<std::array<std::uint8_t, 32>, 4> constexpr kTimeFrequencyPhaseTable {{
          {{ 0, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 2, 2, 1, 1, 0, 2, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 2, 2, 1, 1 }},
          {{ 0, 3, 2, 3, 0, 1, 3, 0, 2, 1, 2, 3, 2, 3, 3, 0, 0, 3, 2, 3, 0, 1, 3, 0, 2, 1, 2, 3, 2, 3, 3, 0 }},
          {{ 0, 0, 0, 2, 0, 2, 1, 3, 2, 2, 0, 2, 2, 0, 1, 3, 0, 0, 0, 2, 0, 2, 1, 3, 2, 2, 0, 2, 2, 0, 1, 3 }},
          {{ 0, 1, 2, 1, 0, 3, 3, 2, 2, 3, 2, 1, 2, 1, 3, 2, 0, 1, 2, 1, 0, 3, 3, 2, 2, 3, 2, 1, 2, 1, 3, 2 }}
      }};

      inline float calculate_phi(__internal_common::types::transmission_mode const mode, std::int16_t carrier)
        {
        phase_table_entry const * table = nullptr;
        auto tableLength = 0;

        switch(mode.id)
          {
          case 1:
            table = kPhaseTableTransmissionMode1;
            tableLength = sizeof(kPhaseTableTransmissionMode1) / sizeof(phase_table_entry);
            break;
          case 2:
            table = kPhaseTableTransmissionMode2;
            tableLength = sizeof(kPhaseTableTransmissionMode2) / sizeof(phase_table_entry);
            break;
          case 3:
            table = kPhaseTableTransmissionMode3;
            tableLength = sizeof(kPhaseTableTransmissionMode3) / sizeof(phase_table_entry);
            break;
          case 4:
            table = kPhaseTableTransmissionMode4;
            tableLength = sizeof(kPhaseTableTransmissionMode4) / sizeof(phase_table_entry);
            break;
          }

        for(int row = 0; row < tableLength; ++row)
          {
          auto const & entry = table[row];

          if(entry.minimumCarrier <= carrier && carrier <= entry.maximumCarrier)
            {
            return M_PI / 2 * (kTimeFrequencyPhaseTable[entry.i][carrier - entry.minimumCarrier] + entry.n);
            }
          }

        return 0;
        }
      }

    }

  }

#endif

