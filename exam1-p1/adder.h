#ifndef _ADDER_H_
#define _ADDER_H_

#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_dt;
using namespace sc_core;

#include "ready_valid_port.h"


class adder : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  rdyvld_port_in<float> i_a_port;
  rdyvld_port_out<float> o_sum_port;

  void do_add() {
    //initilize handshaking signals
    i_a_port.rdy.write(false); 
    o_sum_port.vld.write(false);
    temp = 0;

    while (true) {
      x[0] = temp;
      x[1] = i_a_port.read();
      x[2] = i_a_port.read();
      temp = x[2];
      float _o_sum = x[2]/2 + x[1]/3 + x[0]/6;
      o_sum_port.write(_o_sum);
      wait();
    }
  }

  SC_HAS_PROCESS(adder);
  adder(sc_module_name name, int i) : id(i){ 

    SC_THREAD(do_add);  
    sensitive << i_clk.pos();
    dont_initialize();
    //reset_signal_is(i_rst, false); 
  }

  sc_ufixed_fast<4,1,SC_TRN, SC_WRAP,2> i_a() { return _i_a; }
  sc_ufixed_fast<4,1,SC_TRN, SC_WRAP,2> o_sum() { return _o_sum; }

private:
  const int id;
  sc_ufixed_fast<4,1,SC_TRN, SC_WRAP,2> _i_a;
  sc_ufixed_fast<4,1,SC_TRN, SC_WRAP,2> _o_sum;
  float temp;
  sc_ufixed_fast<4,1,SC_TRN, SC_WRAP,2> x[3];
 
};

#endif
