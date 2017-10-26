#include <iostream>
#include <pthread.h>

#include "cloop.h"
#include "fifo.h"
// #include "discrete_device.h"
// #include "serial_device.h"
// #include "analog_device.h"
// #include "window.h"
// #include "journal.h"
#include "transport.h"
// #include "tools.h"

#include "discrete.h"

using namespace csys;
using namespace std;




int main(int argc, char** argv)
{
  if(!argc && !argv)
  {}
  
  
  int bufferLen = 2048;
  
  transport eth(argv[1], bufferLen);
  eth.init();
  
  cloop loop;
  
  dI di1("di1");
  dI di2("di2");
  dI di3("di3");
  dI di4("di4");
  
  
  
//   fifo ff(bufferLen);
//   ff.start();
  
  loop.enable(NULL, &eth);

  
  return 0;
}












