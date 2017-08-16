#ifndef __DAB_UTIL__SYMBOL_HANDLER
#define __DAB_UTIL__SYMBOL_HANDLER

#include "dab/util/fft.h"
#include "dab/util/deinterleaver.h"

#include <dab/types/queue.h>
#include <dab/types/common_types.h>
#include <dab/constants/transmission_modes.h>

#include <atomic>
#include <cstdint>
#include <utility>
#include <vector>

namespace dab
  {

  namespace __internal_demod
    {

    /**
     * @internal
     * @author Felix Morgner
     *
     * @brief DAB symbol handler
     *
     * This class processes OFDM symbols in preparation for decoding
     * Most of this implementation is reproduced and adapted from Jan van Katwijk's
     * dab-rpi (https://github.com/JvanKatwijk/dab-rpi) which is released under GPLv2.
     */
    struct symbol_handler
      {

      explicit symbol_handler(internal::types::transmission_mode const & mode,
                              internal::queue<std::vector<float>> & symbolQueue);

      void run();

      void stop();

      void handle_prs(std::vector<internal::sample_t> const & symbol);

      void handle_fic(std::vector<internal::sample_t> const & symbol);

      void handle_msc(std::vector<internal::sample_t> const & symbol);

      private:
        enum struct symbol_type
          {
          prs,
          fic,
          msc
          };

        void process_prs(std::vector<internal::sample_t> && symbol);

        void process_fic(std::vector<internal::sample_t> && symbol);

        void process_msc(std::vector<internal::sample_t> && symbol);

        internal::types::transmission_mode const m_mode;

        std::vector<internal::sample_t> m_phaseReference;

        internal::queue<std::pair<symbol_type, std::vector<internal::sample_t>>> m_symbolBuffer;

        internal::queue<std::vector<float>> & m_symbolQueue;

        fft m_fft;

        deinterleaver m_deinterleaver;

        std::atomic_flag m_continue{};
      };

    }

  }

#endif

