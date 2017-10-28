#include <iostream>
#include <pthread.h>

#include "cloop.h"
#include "transport.h"
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
  
  loop.enable(&eth);
  
  return 0;
}












