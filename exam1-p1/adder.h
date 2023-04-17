#ifndef _ADDER_H_
#define _ADDER_H_

#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_dt;
using namespace sc_core;

const int WIDTH = 2;

#include "ready_valid_port.h"

class adder : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  //i_a input ports: message, ready, valid signals
  rdyvld_port_in<sc_uint<WIDTH> > i_a_port;
  //i_b input ports: message, ready, valid signals
  rdyvld_port_in<sc_uint<WIDTH> > i_b_port;
  //o_sum output ports: message, ready, valid signals
  rdyvld_port_out<sc_uint<WIDTH+1> > o_sum_port;

  void do_add() {
    //initilize handshaking signals
    i_a_port.rdy.write(false); 
    i_b_port.rdy.write(false); 
    o_sum_port.vld.write(false);
    temp = 0;
    while (true) {
        _i_a = i_a_port.read();
        _i_b = i_b_port.read();
        _i_c = temp;
        temp = _i_a;
        wait();
        _o_sum = _i_a/2 + _i_b/3 + _i_c/6;
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

  sc_uint<WIDTH> i_a() { return _i_a; }
  sc_uint<WIDTH> i_b() { return _i_b; }
  sc_uint<WIDTH+1> o_sum() { return _o_sum; }

private:
  const int id;
  sc_uint<WIDTH> _i_a;
  sc_uint<WIDTH> _i_b;
  sc_uint<WIDTH + 1> _o_sum;

 
};

#endif
