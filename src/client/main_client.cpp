#include <iostream>
#include <pthread.h>

#include "cloop.h"
#include "transport.h"

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
  loop.enable(&eth);
  
  return 0;
}












