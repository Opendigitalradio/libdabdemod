#include "util/symbol_handler.h"

#include <cstring>

#include <iostream>

namespace dab
  {

  using namespace __internal_common;
  using namespace __internal_common::types;

  namespace __internal_demod
    {

    symbol_handler::symbol_handler(transmission_mode const & mode,
                                   moodycamel::BlockingReaderWriterQueue<std::vector<float>> & symbolQueue)
      : m_mode{mode},
        m_phaseReference(m_mode.useful_duration),
        m_symbolQueue{symbolQueue},
        m_fft{mode.useful_duration, FFTW_FORWARD},
        m_deinterleaver{mode}
      {

      }

    void symbol_handler::run()
      {
      m_running.store(true, std::memory_order_release);

      while(m_running.load(std::memory_order_acquire))
        {
        std::pair<symbol_type, std::vector<sample_t>> entry{};
        while(!m_symbolBuffer.wait_dequeue_timed(entry, 10))
          {
          if(!m_running.load(std::memory_order_acquire))
            {
            return;
            }
          }

        switch(entry.first)
          {
          case symbol_type::prs:
            process_prs(std::move(entry.second));
            break;
          case symbol_type::fic:
            process_fic(std::move(entry.second));
            break;
          case symbol_type::msc:
            process_msc(std::move(entry.second));
            break;
          }
        }
      }

    void symbol_handler::stop()
      {
      m_running.store(false, std::memory_order_release);
      }

    void symbol_handler::handle_prs(std::vector<sample_t> const & symbol)
      {
      m_symbolBuffer.enqueue(std::make_pair(symbol_type::prs, symbol));
      }

    void symbol_handler::handle_fic(std::vector<sample_t> const & symbol)
      {
      m_symbolBuffer.enqueue(std::make_pair(symbol_type::fic, symbol));
      }

    void symbol_handler::handle_msc(std::vector<sample_t> const & symbol)
      {
      m_symbolBuffer.enqueue(std::make_pair(symbol_type::msc, symbol));
      }

    void symbol_handler::process_prs(std::vector<sample_t> && symbol)
      {
      auto fftBuffer = m_fft.data();
      std::memcpy(fftBuffer, symbol.data(), symbol.size() * sizeof(sample_t));
      m_fft();
      std::memcpy(m_phaseReference.data(), fftBuffer, m_phaseReference.size() * sizeof(sample_t));
      }

    void symbol_handler::process_fic(std::vector<sample_t> && symbol)
      {
      auto fftBuffer = m_fft.data();
      std::memcpy(fftBuffer, symbol.data(), symbol.size() * sizeof(sample_t));
      m_fft();

      auto bits = std::vector<float>(m_mode.symbol_bits);

      for(auto carrierIndex = 0; carrierIndex < m_mode.carriers; ++carrierIndex)
        {
        auto mappedIndex = m_deinterleaver.map(carrierIndex);
        if(mappedIndex < 0)
          {
          mappedIndex += m_mode.useful_duration;
          }

        auto r1 = fftBuffer[mappedIndex] * std::conj(m_phaseReference[mappedIndex]);
        m_phaseReference[mappedIndex] = fftBuffer[mappedIndex];
        auto ab1 = std::abs(r1);

        bits[carrierIndex] = r1.real() / ab1;
        bits[carrierIndex + m_mode.carriers] = r1.imag() / ab1;
        }

      m_symbolQueue.enqueue(std::move(bits));
      }

    void symbol_handler::process_msc(std::vector<sample_t> && symbol)
      {
      auto fftBuffer = m_fft.data();
      std::memcpy(fftBuffer, symbol.data(), symbol.size() * sizeof(sample_t));
      m_fft();

      auto bits = std::vector<float>(m_mode.symbol_bits);

      for(auto carrierIndex = 0; carrierIndex < m_mode.carriers; ++carrierIndex)
        {
        auto mappedIndex = m_deinterleaver.map(carrierIndex);
        if(mappedIndex < 0)
          {
          mappedIndex += m_mode.useful_duration;
          }

        auto r1 = fftBuffer[mappedIndex] * std::conj(m_phaseReference[mappedIndex]);
        m_phaseReference[mappedIndex] = fftBuffer[mappedIndex];
        auto ab1 = std::abs(r1);

        bits[carrierIndex] = r1.real() / ab1;
        bits[carrierIndex + m_mode.carriers] = r1.imag() / ab1;
        }

      m_symbolQueue.enqueue(std::move(bits));
      }

    }

  }

