#include <iostream>
#include <pthread.h>

#include "cloop.h"
#include "fifo.h"
#include "discrete_device.h"
#include "serial_device.h"
#include "analog_device.h"
#include "window.h"
#include "journal.h"
#include "remote.h"
#include "tools.h"

using namespace csys;
using namespace std;



window& app = window::instance(); 

void* run_gui_thread(void* arg)
{
  if(arg)
  {}

  device::populate_widgets();
  
  
  journal& jRef = journal::instance();
  jRef.build_widget();
  
  
  window::show();

 
  return 0;
}






int main(int argc, char** argv)
{
  if(!argc && !argv)
  {}
  
  int bufferLen = 2048;
  
  device::os << std::fixed;
  device::is << std::fixed;
  pthread_t gui;
  
  cloop loop;
  
  dev::discreteDevice discrete1("DISCRETE1");
  dev::discreteDevice discrete2("DISCRETE2");
  dev::discreteDevice discrete3("DISCRETE3");
  dev::discreteDevice discrete4("DISCRETE4");
  
//   dev::serialDevice serial1("SERIAL1");
//   dev::serialDevice serial2("SERIAL2");
  
  dev::analogDevice analog1("ANALOG1");
  dev::analogDevice analog2("ANALOG2");
  dev::analogDevice analog3("ANALOG3");
  
  tools::diModule di_module;
  tools::doModule do_module;
  tools::aiModule ai_module;
  tools::aoModule ao_module;

#ifdef REMOTE
 
  remote eth(argv[1], bufferLen);
  eth.init();
  
  if(0 != pthread_create(&gui, NULL, run_gui_thread, NULL))
  {
    cout << "pthread_create() failure" << endl;
    exit(EXIT_FAILURE);
  }
  
  
  loop.enable(NULL, &eth);

#else
  
  fifo ff(bufferLen);
  ff.start();
  
  if(0 != pthread_create(&gui, NULL, run_gui_thread, NULL))
  {
    cout << "pthread_create() failure" << endl;
    exit(EXIT_FAILURE);
  }
  
  loop.enable(&ff, NULL);

#endif
  
  return 0;
}












