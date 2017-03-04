#include <clocale>
#include <pthread.h>
#include <typeinfo>

#include "io.h"
#include "cloop.h"
#include "fifo.h"
#include "discrete_device.h"
#include "serial_device.h"
#include "analog_device.h"
#include "logger.h"
#include "remote.h"
#include "tools.h"
#include "serializer.h"

using namespace csys;


#include <vector>
int main()
{ 
  
  int a = 0xffff;
  char b = 'T';
  float c = 3.141592;
  int d = 0xffffffff;
  const char* p = "test string 1";
  const char* str2 = "test string 2";

  int aa, dd;
  char bb;
  float cc;
  char pp[32];
  char pstr2[32];
  
  csys::serializer ss;
  
  ss.serialize<int>(a);  
  ss.serialize<char>(b);
  ss.serialize_cstring(p);
  ss.sign_block("bl1");
 
  ss.serialize<float>(c);
  ss.serialize_cstring(str2);
  ss.serialize<int>(d);
  
  ss.sign_block("bl2");
  
  std::cout << "done\n";
  
  
  ss.dump();
  ss.reset();
  
  ss.read_block(pp);
  std::cout << "block id: " << pp << " " << ss.len << std::endl;
  ss.deserialize<int>(&aa);
  ss.deserialize<char>(&bb);
  ss.deserialize_cstring(pstr2);
  std::cout << aa << std::endl << bb << std::endl;  
  
  
  
  
  ss.read_block(pp);
  std::cout << "block id: " << pp << " " << ss.len << std::endl;
  ss.deserialize<float>(&cc);
  ss.deserialize_cstring(pp);
  ss.deserialize<int>(&dd);
 std::cout << cc << std::endl << dd << std::endl;
 std::cout << ss.size << std::endl;
//   ss.dump();
  
      
  
  
    
  return 0;
/****************************/
   
  int bufferLen = 2048;
  
  remote eth(bufferLen); 
  eth.init();
  
  device::bufferLen = bufferLen;
  device::os << std::fixed;
  device::is << std::fixed;
  io sys_io;

  
  cloop loop;
  fifo ff(bufferLen);
  ff.start();
  
  
  csys::time comport_frame = csys::time::msSec500;
  csys::time comport_link = csys::time::Sec5;  
  
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
  
  dev::discreteDevice discrete1("DISCRETE1", 0, 1, 6);
  dev::discreteDevice discrete2("DISCRETE2", 2, 3, 7);
  dev::discreteDevice discrete3("DISCRETE3", 4, 5, 8);
  dev::discreteDevice discrete4("DISCRETE4", 6, 9, 11);
  
//   dev::serialDevice serial1("SERIAL1", "COM0", 1);
//   dev::serialDevice serial2("SERIAL2", "COM0", 2);
 
  dev::analogDevice analog1("ANALOG1", 1, 1);
  dev::analogDevice analog2("ANALOG2", 2, 2);
  dev::analogDevice analog3("ANALOG3", 3, 3);

//   tools::diModule di_module;
//   tools::doModule do_module;
//   tools::aoModule ao_module;
//   tools::aiModule ai_module;

  
  loop.enable(sys_io, &ff, &eth);
  
 return 0; 
}
























