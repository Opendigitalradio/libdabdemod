#ifndef __DAB__DEMODULATOR
#define __DAB__DEMODULATOR

#include "util/fft.h"
#include "util/phase_reference.h"
#include "util/symbol_handler.h"

#include <types/common_types.h>
#include <types/transmission_mode.h>

#include <atomic>
#include <cstdint>
#include <future>
#include <vector>

namespace dab
  {

  /**
   * @author Felix Morgner
   *
   * @brief A DAB signal demodulator
   *
   * This class demodulates the DAB signal into OFDM symbols ready for decoding.
   * Most of this implementation is reproduced and adapted from Jan van Katwijk's
   * dab-rpi (https://github.com/JvanKatwijk/dab-rpi) which is released under GPLv2.
   */
  struct demodulator
    {
    /**
     * @brief Construct a demodulator for a specific mode
     *
     * @param samples The queue containing the complex baseband samples
     * @param symbols The target queue for demodulated OFDM symbols
     * @param mode The DAB transmission mode
     */
    demodulator(sample_queue_t & samples, symbol_queue_t & symbols, __internal_common::types::transmission_mode const & mode);

    ~demodulator();

    /**
     * @brief Start the demodulator.
     *
     * @note This call blocks until #stop is called
     */
    void run();

    /**
     * @brief Stop the demodulator
     */
    void stop();

    private:
      static auto constexpr kSearchRange = 72;
      static auto constexpr kCorrelationLength = 24;

      __internal_common::sample_t get_sample(std::int32_t const phaseShift);
      void get_samples(__internal_common::sample_t * target, std::int16_t count, std::int32_t phaseShift);
      std::int16_t handle_prs();

      __internal_common::types::transmission_mode const m_mode;
      sample_queue_t & m_sampleQueue;
      __internal_demod::phase_reference m_phaseReference;
      __internal_demod::fft m_correlationFft;

      std::vector<__internal_common::sample_t> m_oscilator;
      std::vector<__internal_common::sample_t> m_symbolBuffer;
      std::vector<float> m_correlationBuffer;

      __internal_demod::symbol_handler m_symbolHandler;

      std::int32_t m_phaseShift{};

      float m_signalLevel{};

      std::int16_t m_fineCorrection{};
      std::int32_t m_coarseCorrection{};

      std::atomic_bool m_stop{};

      bool m_synchronizeOnPRS{true};
      bool m_didSync{};

      std::vector<float> m_refArg{};
      std::future<void> m_symbolHandlerFuture{};
    };

  }

#endif

