#include <cerrno>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>

#include "cloop.h"
#include "dev.h"
#include "logger.h"
#include "transport.h"
#include "commac.h"
#include "discrete.h"

#ifdef SERVER
#include "io.h"
#endif





using namespace std;
using namespace csys;

/*   csys::time::msSec20   */
const csys::time cloop::scantime(0, 20000000); 
const long int cloop::width = 10;


csys::time cloop::time_global(false);
logger& clLog = logger::instance();
bool cloop::error;

void cloop::get_time(time& time)
{
  if(0 != clock_gettime(CLOCK_REALTIME, &time.get()))
  {
    cloop::error          = true;
    int errnum = errno;
    clLog.getstream(logLevel::DEFAULT, logType::CLOCK)
    << __func__ << ": failed, errno "
    << errnum << delim << strerror(errnum) << endl;
  }
  cloop::time_global    = time;
  dev::time_global  = time;
  logger::time_global  = time; 
}


cloop::cloop() 
{ 
  crep         = false;
  first_cycle  = true;
  error        = false;
  
  get_time(time_beg);
  time_beg.reset();  
  time_in.reset();
  time_jmp.reset();
  time_pre.reset();
  job.reset();
  elapsed.reset(); 
  sleep.reset();
  elapsed_sum.reset();
  scandelay.reset();
  latency.reset();
  scanbased_time.reset();
  scanCounter = 0;
  
  clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
  << " scantime: " << scantime << endl
  << "                                    "
  << "scan             "
  << "time_beg                          "
  << "time_in                              "
  << "time_jmp                        "
  << "job                      "
  << "elapsed                 "
  << "sleep                     " 
  << "elapsed_sum        "
  << "scandelay" << endl;
}



cloop::~cloop()
{ clLog.getstream(logLevel::DEFAULT, logType::CLOCK)  << __func__<< ": closing session..." << endl; }



void cloop::nsleep(time& time)
{
  if(0 != nanosleep(&time.get(), NULL))
  {
    error          = true;
    int errnum = errno;
    clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
    << __func__<< ": failed, errno "
    << errnum << delim << strerror(errnum) << endl;
  }
}


void cloop::timeval_output()
{
  clLog.getstream(logLevel::CDEBUG, logType::CLOCK)
  << setw(5) << scanCounter << delim
  << time_beg << delim
  << time_in << delim
  << time_jmp << delim
  << job << delim
  << elapsed << delim
  << sleep << delim
  << elapsed_sum << delim
  << scandelay << delim << endl;
}


void cloop::measure()
{
  job = time_jmp - time_in;
  elapsed = time_jmp - time_pre;
  
  if(job < time::zero)
  {
    error = true;
    clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
    << __func__<< ": job negative value: " << job << endl;
  }
  
  if(elapsed < time::zero)
  {
    error = true;
    clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
    << __func__<< ": elapsed negative value" << elapsed << endl;
  }
  
  
  if(job < scantime)
  {
    if(scandelay > latency)
    {
      if(scandelay > time::zero)
      {
        sleep.reset();
        scandelay = scandelay - (scantime - job);
      }
      else
      { sleep = scantime - job; }
    }
    else
    {
      if(latency > time::zero)
      {
        sleep.reset();
        latency = latency - (scantime - job);
      }
      else
      { sleep = scantime - job; }
    }
  }
  else 
  {
    error = true;
    
    if(job > scantime + scantime)
    {
      clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
      << __func__<< ": job > scantime two times: " << job << endl;
    }
    else
    {
      clLog.getstream(logLevel::DEFAULT, logType::CLOCK) 
      << __func__<< ": job >= scantime: " << job << endl;
    }
    scandelay = scandelay + (job - scantime);
    sleep.reset();
  }
  
  
  if(0 == scanCounter%width)
  {/*  TODO  works within limited range !!   */
    scanbased_time = scanbased_time + time(0, (scantime.get().tv_nsec*width));
    latency = (time_jmp - time_beg) - scanbased_time;
  }
  
  elapsed_sum = elapsed_sum + elapsed;
  
  #ifdef SERVER
  timeval_output();
  #endif
  
  time_pre = time_jmp;
}



void cloop::console_report(string& str)
{
  if(!crep)
  {
    crep = true;
    cout << "\033[2J";
  }
  
  cout << "\033[1;1H";
  cout << "=========================================" << endl;
  
  #ifdef SERVER
  cout << "server stats\t\t\t" << setw(9) << scanCounter << endl; 
  #else
  cout << "client stats\t\t\t" << setw(9) << scanCounter << endl; 
  #endif
  
  cout << "-----------------------------------------" << endl;
  cout << "scan time\t " << scantime << endl;
  cout << "time remaining\t " << sleep << endl;
  cout << "scan delay\t " << scandelay << endl;
  cout << "latency\t\t " << latency << endl;
  cout << "time elapsed\t " << elapsed_sum << endl;
  cout << "scanbased time\t " << scanbased_time << endl;
  cout << str << endl;
  cout << "=========================================" << endl;
}


void cloop::get_connection_info(string& reply, bool connected)
{
  reply = connected ? "connected\t\t  yes\n" : "connected\t\t  no \n" ;
}


/*******************************************************************************************/

/*    TODO   signal/exception handler    */
#ifdef SERVER

void cloop::enable(io& sys_io, transport* eth)
{
  if(!eth)
  { cerr << __func__<< ": no transport provided!" << endl; exit(1); }
  
  
  string reply;
  while(true)
  {
    get_time(time_in);
    
    if(first_cycle)
    {
      time_beg    = time_in;
      time_jmp    = time_beg;
      time_pre    = time_beg;
      first_cycle  = false;
    }
    
    /*  IO refresh  */ 
    sys_io.io_update();
    /* data interchange and dispatch */
    eth->poll(dev::os_, dev::is_); 
    dev::generic_controller_module(eth->connection);
    
    /*      job start      */
    
    
    
    
    
    
    
    /*     job end        */
    get_time(time_jmp);
    measure();
    
    get_connection_info(reply, eth->connection);
    console_report(reply);
    scanCounter++;
    nsleep(sleep);
    reply = "";  
    
  }
}
#endif


/*******************************************************************************************/


#ifdef CLIENT


void instantiate_devices(std::vector<std::string>& devNewborn)
{
  std::vector<std::string>::iterator it;
  for(it = devNewborn.begin(); it != devNewborn.end(); it++)
  {
    new dI(it->c_str());
  }
  devNewborn.clear();
}



void cloop::enable(transport* eth)
{
  if(!eth)
  { cerr << __func__<< ": no transport provided!" << endl; exit(1); }

  string reply;  
  while(true)
  { 
    get_time(time_in);
    if(first_cycle)
    {
      time_beg     = time_in;
      time_jmp     = time_beg;
      time_pre     = time_beg;
      first_cycle = false;
    }
    
    /* data interchange and dispatch */
    eth->poll(dev::os_, dev::is_); 
  dev::generic_controller_module(eth->connection);
    if(!dev::devNewborn_.empty())
    { instantiate_devices(dev::devNewborn_); }
    dev::generic_command_module();
    /*      job start      */
    
    
    
    /*     job end        */
    get_time(time_jmp);
    measure();
    get_connection_info(reply, eth->connection);
    console_report(reply);
    scanCounter++;
    nsleep(sleep);
    /* clear streams  */
    reply = "";
  }  
}

#endif







/*************************************    Timers  ******************************************/



bool timer::start(time sp) 
{
  if(ready)
  {
    ready = false;
    active = true;
    st = cloop::time_global;
    tm = sp;
    return true;
  }
  return false;
}

bool timer::is_valid()
{
  if(active)
  {
    if(cloop::time_global - st > tm)
    {
      ready = true;
      active = false;
      return false;
    }
    return true; 
  }
  return false;
}

void timer::reset()
{
  active = false;
  ready = true;
}
















