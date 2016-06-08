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

#include "discrete_device.h"
#include "logger.h"
#include "io.h"
#include "commac.h"

using namespace std;
using namespace csys;
using namespace dev;

discreteDevice::discreteDevice(const char *lbl, int op, int cl, int cntr): 
device(lbl, time::Sec1), p_open(op), p_closed(cl), p_control(cntr)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    <<  __func__  << delim << " already exists" <<  endl;
    cout <<  __func__ << delim << label << " already exists" << endl;
    exit(1);
  }
}

discreteDevice::~discreteDevice()
{
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







void discreteDevice::process(io &sys_io)
{
  emit = false;
  /*     poll IO     */
  io_cs.di_open    = sys_io.get_di_bit(p_open);
  io_cs.di_closed  = sys_io.get_di_bit(p_closed);
  
  if(!init)
  {
    init = true;
    io_cs.command = cmd::CLOSE;
  }
  
  error_handler();
  
  
  if(io_cs.command != io_ps.command)
  {
    time_redge = time_global;
    switch(io_cs.command)
    {
      case cmd::NOCMD:
        break;
        
      case cmd::CLOSE:
        io_cs.do_control = false;
        sys_io.rset_do_bit(p_control);
        state = device_state::CHANGED;
        break;
        
      case cmd::OPEN:
        io_cs.do_control = true;
        sys_io.set_do_bit(p_control);
        state = device_state::CHANGED;
        break;
        
      case cmd::INIT:
        io_cs.do_control = false;
        sys_io.rset_do_bit(p_control);
        state = device_state::CHANGED;
        io_cs.error = err::NOERR;
        break;
        
      default:
        break;
    }
  }
  
  
  request_handler();
  
  /*  notify CLIENT that something has changed  */
  if(rts || io_cs != io_ps || state != state_ps) { rts = false; emit = true; }
  io_ps = io_cs;
  state_ps = state;
}

/*****************      internal methods     **********************/


void discreteDevice::error_handler()
{
  if(cmd::NOCMD == io_cs.command)
  {
    /*  close contact bouncing  */
    if(io_cs.di_closed != io_ps.di_closed)
    { 
      state = device_state::ALARM;
      io_cs.error = err::EBNCL;
    }
    
    /*  open contact bouncing  */
    if(io_cs.di_open != io_ps.di_open)
    { 
      state = device_state::ALARM;
      io_cs.error = err::EBNOP;
    }
  }  
}



void discreteDevice::request_handler()
{
  if(timeout < time_global - time_redge)
  {
    switch(io_cs.command)
    {
      case cmd::NOCMD:
        return;
        
      case cmd::CLOSE:
        if(is_closed())
        {
          state = device_state::DISABLED;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": closed" <<  endl;
        }
        else
        {
          state = device_state::ALARM;
          io_cs.error = err::ETMCL;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": exceeded timeout" <<  endl;
        }
        break;
        
      case cmd::OPEN:
        if(is_open())
        {
          state = device_state::ENABLED;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": open" <<  endl;
        }
        else
        {
          state = device_state::ALARM;
          io_cs.error = err::ETMOP;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": opening failed, exceeded timeout" <<  endl;
        }
        break;
        
      case cmd::INIT:
        if(is_closed())
        {
          state = device_state::DISABLED;
          io_cs.command = cmd::NOCMD;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": closed" <<  endl;
        }
        else
        {
          state = device_state::ALARM;
          io_cs.error = err::ETMCL;
          dLog.getstream(logLevel::DEFAULT, logType::DEVICE)  << label << delim 
          <<  __func__ << ": closing failed, exceeded timeout" <<  endl;
        }    
        break;
        
      default:
        break;
    }
    io_cs.command = cmd::NOCMD;
  }
  
}







void discreteDevice::serialize()
{
  os << label << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.di_open << delim
  << io_cs.di_closed << delim
  << io_cs.do_control << delim;
  
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
  <<  __func__ << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.di_open << delim
  << io_cs.di_closed << delim
  << io_cs.do_control << endl;
}


void discreteDevice::unserialize()
{
  is >> io_cs.command;
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE)  << label << delim 
  <<  __func__ << delim << io_cs.command << endl;
}




