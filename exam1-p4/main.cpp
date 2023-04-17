#include "initiator.h"
#include "target.h"
#include "buffer.h"
#include "bus.h"

int sc_main(int argc, char* argv[])
{
  Initiator initiator("initiator");
  Adder   adder("adder");
  Buffer   buffer("buffer");
  Bus<1,2>   bus("bus");

  // Bind initiator socket to target socket
  initiator.socket.bind( *(bus.targ_socket[0]) );
  ( *(bus.init_socket[0]) ).bind( adder.socket );
  ( *(bus.init_socket[1]) ).bind( buffer.socket );
  
  sc_start();
  return 0;
}
