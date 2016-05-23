#ifndef __DAB__DEMODULATOR
#define __DAB__DEMODULATOR

#include <types/common_types.h>
#include <types/transmission_mode.h>

#include <array>
#include <cstdint>

namespace dab
  {

  struct demodulator
    {
    demodulator(__internal_common::types::transmission_mode const & mode, sample_queue_t & samples);

    void operator()();

    private:
      __internal_common::sample_t get_sample(std::int32_t const phaseShift);

      __internal_common::types::transmission_mode const m_mode;
      sample_queue_t & m_sampleQueue;

      std::int32_t m_phaseShift{};
      std::vector<__internal_common::sample_t> m_oscilator{};
      float m_signalLevel{};
    };

  }

#endif

