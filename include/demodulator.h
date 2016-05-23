#ifndef __DAB__DEMODULATOR
#define __DAB__DEMODULATOR

#include <types/common_types.h>
#include <types/transmission_mode.h>

#include <cstdint>
#include <vector>

namespace dab
  {

  struct demodulator
    {
    demodulator(__internal_common::types::transmission_mode const & mode, sample_queue_t & samples);

    void operator()();

    private:
      __internal_common::sample_t get_sample(std::int32_t const phaseShift);
      void get_samples(std::vector<__internal_common::sample_t> & target, std::int16_t count, std::int32_t phaseShift);

      __internal_common::types::transmission_mode const m_mode;
      sample_queue_t & m_sampleQueue;

      std::int32_t m_phaseShift{};
      std::vector<__internal_common::sample_t> m_oscilator{};
      std::vector<__internal_common::sample_t> m_symbolBuffer{};
      float m_signalLevel{};

      std::int16_t m_fineCorrection{};
      std::int32_t m_coarseCorrection{};
    };

  }

#endif

