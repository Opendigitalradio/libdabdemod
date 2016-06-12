#include "util/deinterleaver.h"

#include <stdexcept>
#include <string>

namespace dab
  {

  namespace __internal_demod
    {

    using namespace __internal_common::types;

    namespace
      {
      std::vector<std::int16_t> initialiaze_permutations(std::int16_t length, std::int16_t V1, std::int16_t lwb, std::int16_t upb)
        {
        std::vector<std::int16_t> temp(length);
        std::vector<std::int16_t> permutations(length);

        for(auto carrier = 1; carrier < length; ++carrier)
          {
          temp[carrier] = (13 * temp[carrier - 1] + V1) % length;
          }

        auto permutationIndex = 0;
        for(auto carrier = 0; carrier < length; ++carrier)
          {
          if(temp[carrier] == length / 2)
            {
            continue;
            }

          if((temp[carrier] < lwb) || (temp[carrier] > upb))
            {
            continue;
            }

          permutations[permutationIndex++] = temp[carrier] - length / 2;
          }

        return permutations;
        }
      }


    deinterleaver::deinterleaver(transmission_mode const & mode)
      {
      switch(mode.id)
        {
        case 1:
          m_permutationTable = initialiaze_permutations(mode.useful_duration, 511, 256, 256 + mode.carriers);
          break;
        case 2:
          m_permutationTable = initialiaze_permutations(mode.useful_duration, 127, 64, 64 + mode.carriers);
          break;
        case 3:
          m_permutationTable = initialiaze_permutations(mode.useful_duration, 63, 32, 32 + mode.carriers);
          break;
        case 4:
          m_permutationTable = initialiaze_permutations(mode.useful_duration, 255, 128, 128 + mode.carriers);
          break;
        default:
          throw std::invalid_argument{"Invalid transmission mode " + std::to_string(mode.id)};
        }
      }

    std::int16_t deinterleaver::map(std::int16_t const carrier)
      {
      return m_permutationTable[carrier];
      }

    }

  }

