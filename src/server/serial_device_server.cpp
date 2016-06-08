// Copyright (C)  2014, 2015, 2016  Alexander Golant
// 
// This file is part of csys project. This project is free 
// software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>


#include <cstdlib>
#include <sstream>
#include <cmath> 

#include "commac.h"
#include "serial_device.h"
#include "logger.h"
#include "io.h"


using namespace std;
using namespace csys;
using namespace dev;

serialDevice::serialDevice(const char* lbl, const char* port_alias, char addr):
device(lbl, time::Sec4), 
port(*device::serialMap.find(port_alias)->second), msg(*(new message(lbl))), dev_addr(addr)
{
  msg.eot = 0x0D;
  io_cs.error            = err::NOERR;
  io_cs.command    = cmd::NOCMD;
  io_cs.error            = err::NOERR;
  io_cs.link              = true;
  send                     = false;
  receive                 = false;
  state                    = device_state::DISABLED;
  
  /*  test serialMap.find() return value   */
  if(device::serialMap.end() == device::serialMap.find(port_alias))
  {
    cout <<  __func__ << delim << label << ": requested port [" << port_alias << "] not found" << endl;
    exit(1);
  }
  
  /* match device timeout with serial port  */
  if(timeout < port.get_timeout() || timeout > port.get_tmlink())
  {
    cout <<  __func__ << delim << label << ": device timeout is out of range" << endl;
    exit(1);
  }
  
  io_ps = io_cs;
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert (pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
    <<  __func__  << delim << " already exists" <<  endl;
    cout <<  __func__ << delim << label << " already exists" << endl;
    exit(1);
  }
  
  request  = new(std::nothrow) char[bufferLen];
  reply      = new(std::nothrow) char[bufferLen];
  
  if(!request || !reply)
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << delim
    <<  __func__ << ": bad alloc" <<  endl;
    cout <<  __func__ << ": bad alloc" <<  endl;
    exit(1);
  }
}


serialDevice::~serialDevice()
{
  delete[] request;
  delete[] reply;
  
  deviceMapIterator it = deviceMap.find(label);
  if(it == deviceMap.end())
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
    <<  __func__ << ": device not found" <<  endl;
    return;
  }
  else
  { deviceMap.erase(it); } 
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
  <<  __func__ << ": device removed" <<  endl;
}





void serialDevice::process(io &sys_io)
{
  emit          = false;
  
  /*   out of use   */
  if(&sys_io){}
  
  if(!init)
  {
    init = true;
    io_cs.command = cmd::INIT;
  }
  
  try_push();
  error_handler();
  
  /*    frame switcher   */
  if(time_global - time_redge > timeout)
  {
    try_pop();
    
    send = true;
    receive = true;
    time_redge = time_global;
    
    switch(io_cs.command)
    {
      case cmd::NOCMD:
        break;
        
      case cmd::POLL:
        break;
        
      case cmd::INIT:
        port.init();
        io_cs.error = err::NOERR;
        state = device_state::ENABLED;
        break;
        
      default:
        break;
    }
    /*  patch message into port buffer_out  */
    request_handler();
  }
  
  sender();
  receiver();
  
  /*  notify CLIENT that something has changed  */
  if(rts || io_cs != io_ps || state != state_ps) { rts = false; emit = true; }
  io_ps = io_cs;
  state_ps = state;
} 

/*****************      internal methods     **********************/

void serialDevice::try_push()
{
  if(io_cs.command != io_ps.command)  
  {
    /*   push new request   */  
    reqQue.push(io_cs);
    dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
    <<  __func__ << delim << io_cs.command << delim << io_ps.command 
    << delim << "reqQue size: " << reqQue.size() << endl;       
    io_cs = io_ps;   
  }
}

void serialDevice::try_pop()
{
  /*  release request from the queue   */
  if(!reqQue.empty())
  {
    io_cs = reqQue.front();
    reqQue.pop();
    
    dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
    <<  __func__ << delim << io_cs.command << endl; 
  }
}


/*  TODO    unused port separate logic  */
void serialDevice::error_handler()
{
  /*   copy port state into device   */
  io_cs.link     = port.get_link();  
  
  if(norep && port.get_link())
  {
    norep = false;
    io_cs.error = err::TMERR;
    state = device_state::CHANGED;
    dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
    <<  __func__ << ": no reply " << endl;
  }
  
  
  if(!port.get_link())
  {
    io_cs.link = false;
    io_cs.error = err::NOLNK;
    state = device_state::ALARM;
  }
  
  /*    port error_link registration    */
  if(!port.get_link() && io_cs.link != io_ps.link)
  { 
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
    <<  __func__ << ": no link" << endl;
  }
  
  /*    error_link recovery    */
  if(port.get_link() && io_cs.link != io_ps.link)
  {
    io_cs.link = true;
    io_cs.error = err::NOERR;
    state = device_state::ENABLED;
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
    <<  __func__ << ": link recovery" <<  endl;
  }
}


void serialDevice::sender()
{
  if(send)
  {
    send = false;
    msg.clear();
    msg.request = request;
    msg.command = io_cs.command;
    port << msg;
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
    <<  __func__ << delim << msg.request << endl;
  }
}


void serialDevice::receiver()
{
  if(receive)
  {
    if(port >> msg)
    {      
      
      if(0 == msg.response.size())
      {
        norep = true;
        io_cs.command = cmd::POLL;
        dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
        <<  __func__ << ": zero sized message" << endl;
        return;
      }
      
      receive = false;
      norep = false;
      memset(reply, 0, bufferLen);
      strcpy(reply, msg.response.c_str());
      dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
      <<  __func__ << delim << msg.response << endl;
      dispatch();
    }
  }
}


void serialDevice::request_handler()
{
  memset(request, 0x00, bufferLen);
  
  switch(io_cs.command)
  {
    case cmd::NOCMD:
      return;
      
    case cmd::POLL:
      strcpy(request, "loopback");
      break;
      
    case cmd::INIT:
      strcpy(request, "init");
      break;
      
    default:
      return;
  }
  patch();
}



void serialDevice::patch()
{
  int len = strlen(request);
  request[len] = msg.eot;
}



float counter = 0;
void serialDevice::dispatch()
{
  switch(io_cs.command)
  {
    case cmd::NOCMD:
      return;
      
    case cmd::POLL:
      io_cs.data = counter++;
      break;
      
    case cmd::INIT:
      io_cs.data = 0;
      io_cs.command = cmd::POLL;
      break;
      
    default:
      return;
  }
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
  <<  __func__ << delim << io_cs.data << endl;
}



void serialDevice::serialize()
{
  os << label << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.data << delim
  << io_cs.link << delim;
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
  <<  __func__ << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.data << delim
  << io_cs.link  << endl;
}

void serialDevice::unserialize()
{
  is >> io_cs.command;
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
  <<  __func__ << delim << io_cs.command << endl;
}












