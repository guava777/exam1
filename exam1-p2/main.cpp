#include "stim.h"
#include <systemc.h>
#define CLOCK_PERIOD 1.0

int sc_main(int argc, char *argv[]) {
  
  //Create modules and signals
  stim testbench("testbench");
  adder dut("dut", 1);
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  //Create FIFO channels
  sc_fifo<sc_ufixed_fast<4, 1> > fifo_i_a;
  sc_fifo<sc_ufixed_fast<4, 1> > fifo_i_b;
  sc_fifo<sc_ufixed_fast<4, 1> > fifo_o_sum;

  //Connect FIFO channels with modules
  testbench.i_clk(clk);
  testbench.o_rst(rst);
  dut.i_clk(clk);
  dut.i_rst(rst);
  testbench.o_a(fifo_i_a);
  testbench.o_b(fifo_i_b);
  testbench.i_sum(fifo_o_sum);
  dut.i_a_port(fifo_i_a);
  dut.i_b_port(fifo_i_b);
  dut.o_sum_port(fifo_o_sum);

  sc_start(32, SC_NS);
  return 0;
}
