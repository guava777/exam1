#include "stim.h"
#define CLOCK_PERIOD 1.0

int sc_main(int argc, char *argv[]) {
  
  //Create modules and signals
  stim testbench("testbench");
  adder dut("dut", 1);
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  //Create sc_signal channels
  sc_signal<sc_uint<WIDTH> > s_i_a_msg;
  sc_signal<bool> s_i_a_rdy;
  sc_signal<bool> s_i_a_vld;
  sc_signal<sc_uint<WIDTH + 1> > s_o_sum_msg;
  sc_signal<bool> s_o_sum_rdy;
  sc_signal<bool> s_o_sum_vld;

  //Connect FIFO channels with modules
  testbench.i_clk(clk);
  testbench.o_rst(rst);
  dut.i_clk(clk);
  dut.i_rst(rst);
  testbench.o_a.msg(s_i_a_msg);
  testbench.o_a.vld(s_i_a_vld);
  testbench.o_a.rdy(s_i_a_rdy);
  testbench.i_sum.msg(s_o_sum_msg);
  testbench.i_sum.vld(s_o_sum_vld);
  testbench.i_sum.rdy(s_o_sum_rdy);
  dut.i_a_port.msg(s_i_a_msg);
  dut.i_a_port.vld(s_i_a_vld);
  dut.i_a_port.rdy(s_i_a_rdy);
  dut.o_sum_port.msg(s_o_sum_msg);
  dut.o_sum_port.vld(s_o_sum_vld);
  dut.o_sum_port.rdy(s_o_sum_rdy);

  sc_start(65, SC_NS);
  return 0;
}
