#ifndef _ADDER_H_
#define _ADDER_H_

#include <iostream>
using namespace std;

#include <systemc>
#include <systemc.h>
using namespace sc_dt;
using namespace sc_core;

class adder : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<sc_ufixed_fast<4, 1>> i_a_port;
  sc_fifo_in<sc_ufixed_fast<4, 1>> i_b_port;
  sc_fifo_out<sc_ufixed_fast<4, 1>> o_sum_port;

  void do_add() {
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
  adder(sc_module_name name, int i) : id(i) { 
    SC_THREAD(do_add);  
    sensitive << i_clk.pos();
    dont_initialize();
  }

  sc_ufixed_fast<4, 1> i_a() { return _i_a; }
  sc_ufixed_fast<4, 1> i_b() { return _i_b; }
  sc_ufixed_fast<4, 1> o_sum() { return _o_sum; }

private:
  const int id;
  sc_ufixed_fast<4, 1> _i_a;
  sc_ufixed_fast<4, 1> _i_b;
  sc_ufixed_fast<4, 1> _i_c;
  sc_ufixed_fast<4, 1> temp;
  sc_ufixed_fast<4, 1> _o_sum;
 
};

#endif
