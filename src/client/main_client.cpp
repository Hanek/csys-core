#include <iostream>
#include <pthread.h>

#include "window.h"
#include "cloop.h"
#include "transport.h"
#include "discrete.h" 

using namespace csys;
using namespace std;

window& app = window::instance();

void* run_gui_thread(void* arg)
{
  if(arg)
  {}
  
  sleep(1);
  dev::populate_widgets();
  
  window::show();
  
  return 0;
}


int main(int argc, char** argv)
{
  if(!argc && !argv)
  {}
  
  pthread_t gui;
  int bufferLen = 2048;
  
  transport eth(argv[1], bufferLen);
  eth.init();
  
  dI di22("di22");
  
  if(0 != pthread_create(&gui, NULL, run_gui_thread, NULL))
  {
    cout << "pthread_create() failure" << endl;
    exit(EXIT_FAILURE);
  }
  
  cloop loop;  
  loop.enable(&eth);
  
  return 0;
}












