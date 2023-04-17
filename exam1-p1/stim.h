#ifndef _STIM_H_
#define _STIM_H_

#include <iomanip>
using std::setw;
#include "adder.h"
#include "ready_valid_port.h"


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

SC_MODULE(stim) {

  sc_in_clk i_clk;
  sc_out<bool> o_rst;
  //o_a output ports: message, ready, valid signals
  rdyvld_port_out<sc_uint<WIDTH> > o_a;
  //i_sum input ports: message, ready, valid signals
  rdyvld_port_in<sc_uint<WIDTH+1> > i_sum;
  //Store the previous inputs to FIFOs
  sc_uint<WIDTH> t_a;
  sc_uint<1> t_a_rdy;
  sc_uint<1> t_a_vld;
  sc_uint<WIDTH+1> t_sum;
  sc_uint<1> t_sum_rdy;
  sc_uint<1> t_sum_vld;

  void stim_gen() {    
    for (int a = 0; a < 128; a++) {
        wait();
        o_a.write(x_input_signal[a]);
    }
  }

  void sink() {
    while(true) {
      t_sum=i_sum.read();
      wait();
    }
  }

  void monitor() {
    cout << setw(8) << "sum vld" << endl;
    while (true) {
      wait();
      t_a=o_a.msg.read();
      t_a_rdy=o_a.rdy.read();
      t_a_vld=o_a.vld.read();
      t_sum=i_sum.msg.read();
      t_sum_rdy=i_sum.rdy.read();
      t_sum_vld=i_sum.vld.read();
      cout << setw(8) << sc_time_stamp();
      cout << setw(8) << t_a.to_string(SC_BIN);
      cout << setw(8) << t_a_rdy.to_string(SC_BIN);
      cout << setw(8) << t_a_vld.to_string(SC_BIN);
      cout << setw(8) << t_sum.to_string(SC_BIN);
      cout << setw(8) << t_sum_rdy.to_string(SC_BIN);
      cout << setw(8) << t_sum_vld.to_string(SC_BIN) << endl;
      cout << "===============================================================================================" << endl;
    }
  }

  SC_CTOR(stim) {

    SC_THREAD(stim_gen);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(sink);
    sensitive << i_clk.pos();

    // the first row of output table
    SC_THREAD(monitor);
    sensitive << i_clk.pos();
 
  }
};

#endif
