#ifndef __DAB_UTIL__SYMBOL_HANDLER
#define __DAB_UTIL__SYMBOL_HANDLER

#include "util/fft.h"
#include "util/deinterleaver.h"

#include <readerwriterqueue.h>
#include <types/common_types.h>
#include <constants/transmission_modes.h>

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

      explicit symbol_handler(__internal_common::types::transmission_mode const & mode,
                              moodycamel::BlockingReaderWriterQueue<std::vector<float>> & symbolQueue);

      void run();

      void stop();

      void handle_prs(std::vector<__internal_common::sample_t> const & symbol);

      void handle_fic(std::vector<__internal_common::sample_t> const & symbol);

      void handle_msc(std::vector<__internal_common::sample_t> const & symbol);

      private:
        enum struct symbol_type
          {
          prs,
          fic,
          msc
          };

        void process_prs(std::vector<__internal_common::sample_t> && symbol);

        void process_fic(std::vector<__internal_common::sample_t> && symbol);

        void process_msc(std::vector<__internal_common::sample_t> && symbol);

        __internal_common::types::transmission_mode const m_mode;

        std::vector<__internal_common::sample_t> m_phaseReference;

        moodycamel::BlockingReaderWriterQueue<std::pair<symbol_type, std::vector<__internal_common::sample_t>>> m_symbolBuffer;

        moodycamel::BlockingReaderWriterQueue<std::vector<float>> & m_symbolQueue;

        fft m_fft;

        deinterleaver m_deinterleaver;

        std::atomic_bool m_running{};
      };

    }

  }

#endif

