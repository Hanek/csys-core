#include <clocale>
#include <pthread.h>
#include <typeinfo>

#include "io.h"
#include "cloop.h"
#include "fifo.h"
// #include "discrete_device.h"
// #include "serial_device.h"
// #include "analog_device.h"
#include "logger.h"
#include "transport.h"
#include "tools.h"
#include "discrete.h"

using namespace csys;


int main()
{ 
   
  int bufferLen = 2048;
  
  transport eth(bufferLen); 
  eth.init();
  
  device::bufferLen = bufferLen;
//   device::os << std::fixed;
//   device::is << std::fixed;
  io sys_io;

  
  cloop loop;
  fifo ff(bufferLen);
  ff.start();
  
  
//   csys::time comport_frame = csys::time::msSec500;
//   csys::time comport_link = csys::time::Sec5;  
  
  serial::bufferLen = bufferLen;
//   serial port0("/dev/ttyUSB0", B9600, comport_frame, comport_link);
//   
//   std::pair<serialMapIterator, bool> ret;
//   ret = device::serialMap.insert(std::pair<std::string, serial*>("COM0", &port0));
//   if (false == ret.second) 
//   {
//     std::cout << "COM0 registration failed" << std::endl; 
//     exit(1);
//   }
  
//   dev::discreteDevice discrete1("DISCRETE1", 0, 1, 6);
//   dev::discreteDevice discrete2("DISCRETE2", 2, 3, 7);
//   dev::discreteDevice discrete3("DISCRETE3", 4, 5, 8);
//   dev::discreteDevice discrete4("DISCRETE4", 6, 9, 11);
  
//   dev::serialDevice serial1("SERIAL1", "COM0", 1);
//   dev::serialDevice serial2("SERIAL2", "COM0", 2);
 
//   dev::analogDevice analog1("ANALOG1", 1, 1);
//   dev::analogDevice analog2("ANALOG2", 2, 2);
//   dev::analogDevice analog3("ANALOG3", 3, 3);

//   tools::diModule di_module;
//   tools::doModule do_module;
//   tools::aoModule ao_module;
//   tools::aiModule ai_module;
  
  dI di1("di1", 0);
  dI di2("di2", 1);
  dI di3("di3", 0);
  dI di4("di4", 1);
  

  
  loop.enable(sys_io, &ff, &eth);
  
 return 0; 
}
























