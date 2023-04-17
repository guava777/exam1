#ifndef INITIATOR_H
#define INITIATOR_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

#include <cstdint>
#include "map.h"

const float x_input_signal={0.500,   0.525,   0.549,   0.574,   0.598,   0.622,   0.646,   0.670,
                            0.693,   0.715,   0.737,   0.759,   0.780,   0.800,   0.819,   0.838,
                            0.856,   0.873,   0.889,   0.904,   0.918,   0.931,   0.943,   0.954,
                            0.964,   0.972,   0.980,   0.986,   0.991,   0.995,   0.998,   1.000,
                            1.000,   0.999,   0.997,   0.994,   0.989,   0.983,   0.976,   0.968,
                            0.959,   0.949,   0.937,   0.925,   0.911,   0.896,   0.881,   0.864,
                            0.847,   0.829,   0.810,   0.790,   0.769,   0.748,   0.726,   0.704,
                            0.681,   0.658,   0.634,   0.610,   0.586,   0.562,   0.537,   0.512,
                            0.488,   0.463,   0.438,   0.414,   0.390,   0.366,   0.342,   0.319,
                            0.296,   0.274,   0.252,   0.231,   0.210,   0.190,   0.171,   0.153,
                            0.136,   0.119,   0.104,   0.089,   0.075,   0.063,   0.051,   0.041,
                            0.032,   0.024,   0.017,   0.011,   0.006,   0.003,   0.001,   0.000,
                            0.000,   0.002,   0.005,   0.009,   0.014,   0.020,   0.028,   0.036,
                            0.046,   0.057,   0.069,   0.082,   0.096,   0.111,   0.127,   0.144,
                            0.162,   0.181,   0.200,   0.220,   0.241,   0.263,   0.285,   0.307,
                            0.330,   0.354,   0.378,   0.402,   0.426,   0.451,   0.475,   0.500}

// Initiator module generating generic payload transactions
SC_MODULE(Initiator)
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Initiator> socket;

  SC_CTOR(Initiator)
      : socket("socket") // Construct and name socket
  {
    SC_THREAD(thread_process);

    // Set the global quantum
    // Please modify this number to test different quantum
    // All initiators will synchronize at this quantum duration
    m_qk.set_global_quantum(sc_time(5, SC_NS));
    m_qk.reset();
  }

  void set_read_trans(tlm::tlm_generic_payload & trans, unsigned long int addr, unsigned char mask[],
                      unsigned char rdata[], int dataLen)
  {
    // Set up the payload fields. Assume everything is 4 bytes.
    trans.set_read();
    trans.set_address((sc_dt::uint64)addr);

    trans.set_byte_enable_length((const unsigned int)dataLen);
    trans.set_byte_enable_ptr((unsigned char *)mask);

    trans.set_data_length((const unsigned int)dataLen);
    trans.set_data_ptr((unsigned char *)rdata);
    trans.set_streaming_width((const unsigned int)dataLen);
    trans.set_dmi_allowed(false); // Mandatory initial value
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  }

  void set_write_trans(tlm::tlm_generic_payload & trans, unsigned long int addr, unsigned char mask[],
                       unsigned char wdata[], int dataLen)
  {
    // Set up the payload fields. Assume everything is 4 bytes.
    trans.set_write();
    trans.set_address((sc_dt::uint64)addr);

    trans.set_byte_enable_length((const unsigned int)dataLen);
    trans.set_byte_enable_ptr((unsigned char *)mask);

    trans.set_data_length((const unsigned int)dataLen);
    trans.set_data_ptr((unsigned char *)wdata);
    trans.set_streaming_width((const unsigned int)dataLen);
    trans.set_dmi_allowed(false); // Mandatory initial value
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  }

  void thread_process()
  {
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);

    // Generate two transaction of write and read
    for (int i = 0; i < 2; i += 2)
    {
      // Get first integer from buffer
      delay = m_qk.get_local_time();
      set_read_trans((*trans), BUFFER_ADDR_BASE + i + 16, 0, reinterpret_cast<unsigned char *>(&data[i]), 2);
      socket->b_transport(*trans, delay); // Blocking transport call

      if (trans->is_response_error())
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      // Increment local time with delay returned from b_transport()
      m_qk.inc(delay);
      if (m_qk.need_sync())
        m_qk.sync();

      // Get second integer from buffer
      delay = m_qk.get_local_time();

      set_read_trans((*trans), BUFFER_ADDR_BASE + i + 20, 0, reinterpret_cast<unsigned char *>(&data[i + 1]), 4);
      socket->b_transport(*trans, delay); // Blocking transport call

      if (trans->is_response_error())
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      // Increment local time with delay returned from b_transport()
      m_qk.inc(delay);
      if (m_qk.need_sync())
        m_qk.sync();

      // Prepare payload for the first input R1
      delay = m_qk.get_local_time();
      set_write_trans((*trans), ADDER_INPUT_ADDR_BASE + ADDER_INPUT_R1, 0, reinterpret_cast<unsigned char *>(&data[i]), 4);
      socket->b_transport(*trans, delay); // Blocking transport call with current local time

      if (trans->is_response_error())
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "trans = " << (trans->get_command() ? 'W' : 'R')
           << ", data = " << data[0] << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;

      // Increment local time with delay returned from b_transport()
      m_qk.inc(delay);
      if (m_qk.need_sync())
        m_qk.sync();

      // Prepare payload for the first input R2
      set_write_trans((*trans), ADDER_INPUT_ADDR_BASE + ADDER_INPUT_R2, 0, reinterpret_cast<unsigned char *>(&data[i + 1]), 4);
      socket->b_transport(*trans, delay); // Blocking transport call with current local time

      if (trans->is_response_error())
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "trans = " << (trans->get_command() ? 'W' : 'R')
           << ", data = " << data[1] << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;

      // Increment local time with delay returned from b_transport()
      m_qk.inc(delay);
      if (m_qk.need_sync())
        m_qk.sync();

      // Read back results
      delay = m_qk.get_local_time();
      set_read_trans((*trans), ADDER_OUTPUT_ADDR_BASE + ADDER_OUTPUT_R1, 0, reinterpret_cast<unsigned char *>(&result), 2);
      socket->b_transport(*trans, delay); // Blocking transport call

      if (trans->is_response_error())
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "trans = " << (trans->get_command() ? 'W' : 'R')
           << ", result = " << result << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;

      // Increment local time with delay returned from b_transport()
      m_qk.inc(delay);
      if (m_qk.need_sync())
        m_qk.sync();
    }
  }

  tlm_utils::tlm_quantumkeeper m_qk; // Quantum keeper for temporal decoupling
  sc_ufixed_fast<4, 1> data[2];
  sc_ufixed_fast<4, 1> result;
};

#endif
