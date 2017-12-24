#include <clocale>
#include <pthread.h>
#include <typeinfo>

#include "io.h"
#include "cloop.h"
#include "transport.h"
#include "discrete.h"

using namespace csys;


int main()
{ 
  int bufferLen = 2048;
  
  transport eth(bufferLen); 
  eth.init();  
 
  io sys_io;
  cloop loop;
  
  new dI("di1", 0);
  dI di2("di2", 1);
  dI di3("di3", 0);
  dI di4("di4", 1);
  dI di22("di22", 0);
  

  
  loop.enable(sys_io, &eth);
//  
 return 0; 
}
























