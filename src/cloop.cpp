#include <cerrno>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>

#include "cloop.h"
#include "device.h"
#include "logger.h"
#include "fifo.h"
#include "remote.h"
#include "commac.h"

#include "dev.h"

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
  device::time_global  = time;
  logger::time_global  = time; 
}


cloop::cloop() 
{ 
  crep                   = false;
  first_cycle          = true;
  error                  = false;
  
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


void cloop::get_connection_info(string& reply, bool connected, bool type)
{
  reply = connected ? "connected\t\t  yes\n" : "connected\t\t  no \n" ;
  reply = reply + (type ? "client\t\t\t  local   \n" : "client\t\t\t  ethernet\n" );
}


/*******************************************************************************************/

/*    TODO   interrupt/signal/exception handler    */
#ifdef SERVER

void cloop::enable(io& sys_io, fifo* ff, remote* eth)
{
  if(!ff || !eth)
  { cout << __func__<< ": no valid gateway provided!" << endl; exit(1); }
  
  
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

    
    /*      job start      */
    if(!eth->connection)
    {
      ff->poll(device::os, device::is);  
      device::generic_controller_module(sys_io, ff->connection);
    }
    else
    {
      eth->poll(device::os, device::is); 
      device::generic_controller_module(sys_io, eth->connection);
    }
    
    
    
    
    
    
    
    
    /*     job end        */
    
    get_time(time_jmp);
    measure();
    
    if(ff->connection)
    { get_connection_info(reply, true, true); }
    if(eth->connection)
    { get_connection_info(reply, true, false); }
    
    if(!eth->connection && !ff->connection)
    {  get_connection_info(reply, false, true); }
    
    
    
    console_report(reply);
    scanCounter++;
    nsleep(sleep);
    reply = "";  
    
  }
}
#endif


/*******************************************************************************************/


#ifdef CLIENT

void cloop::enable(fifo* ff, remote* eth)
{
  if(!ff && !eth)
  { cout << __func__ << ": no valid gateway provided!" << endl; exit(1); }
  
  if(ff && eth)
  { cout << __func__ << ": too many gateways provided!" << endl; exit(1); }
  
  string reply;
  
  #ifdef REMOTE
  
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
    
    eth->poll(device::os, device::is); 
    device::generic_controller_module(eth->connection);
    /*      job start      */
    
    
    
    
    /*     job end        */
    get_time(time_jmp);
    measure();
    get_connection_info(reply, eth->connection, false);
    //       reply = reply + ((eth->connection) ? "connection eth\t\t  true\n" : "connection\t\t  false\n" );
    console_report(reply);
    scanCounter++;
    nsleep(sleep);
    /* clear streams  */
    reply = "";
  }
  
  
  #else
  /*   local   */
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
    
    /*       fifo data transfer     */
    ff->poll(device::os, device::is); 
    device::generic_controller_module(ff->connection);
    /*      job start      */
    
    
    /*     job end        */  
    get_time(time_jmp);
    measure();
    get_connection_info(reply, ff->connection, true);
    console_report(reply);
    scanCounter++;
    nsleep(sleep);
    /* clear streams  */
    reply = "";
  }
  
  #endif
  
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
















