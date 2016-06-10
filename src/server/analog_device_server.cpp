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
#include "analog_device.h"
#include "logger.h"
#include "io.h"


using namespace std;
using namespace csys;
using namespace dev;

analogDevice::analogDevice(const char* lbl, int ai, int ao): 
device(lbl, time::msSec100), ai_ch(ai), ao_ch(ao)
{
  time_redge            = 0; 
  tolerance               = 0.05f;
  state                      = device_state::DISABLED;  
  io_cs.error              = err::NOERR;
  io_cs.command      = cmd::NOCMD;
  
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

analogDevice::~analogDevice()
{
  deviceMapIterator it = deviceMap.find(label);
  
  if(it == deviceMap.end())
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    <<  __func__ << ": device not found" <<  endl;
    return;
  }
  else
  { deviceMap.erase(it); }
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
  <<  __func__ << ": device removed" <<  endl;
}






void analogDevice::process(io &sys_io)
{
  emit = false;
  
  /*     poll IO     */
  io_cs.aiv = sys_io.get_ai(ai_ch);
  humanize_ai();
  
  
  if(!init)
  {
    init = true;
    io_cs.command = cmd::INIT;
  }
  
  error_handler();
  
  if(io_cs.command != io_ps.command)
  { 
    time_redge = time_global;
    switch(io_cs.command)
    {
      case cmd::NOCMD:
        break;
        
      case cmd::INIT:
        io_cs.aoh = 0.0;
        humanize_ao();
        sys_io.set_ao(ao_ch, io_cs.aov);
        state = device_state::DISABLED;
        break;
        
      case cmd::SETP:
        humanize_ao();
        sys_io.set_ao(ao_ch, io_cs.aov);
        state = device_state::ENABLED;
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


void analogDevice::error_handler()
{
  
}


void analogDevice::request_handler()
{
  
  if(timeout < time_global - time_redge)
  {
    io_cs.command = cmd::NOCMD;
  }
  
}



void analogDevice::serialize()
{
  os << setprecision(4);
  os << label << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.aiv << delim
  << io_cs.aih << delim
  << io_cs.aov << delim
  << io_cs.aoh << delim;
  
  
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim
  << state << delim 
  << io_cs.error << delim
  << io_cs.command << delim
  << io_cs.aiv << delim
  << io_cs.aih << delim
  << io_cs.aov << delim
  << io_cs.aoh << endl;
}


void analogDevice::unserialize()
{
  is >> io_cs.command
  >> io_cs.aoh;
  
  dLog.getstream(logLevel::DDEBUG, logType::DEVICE) << label << delim 
  <<  __func__ << delim << io_cs.command << delim << io_cs.aoh << endl;
}

void analogDevice::humanize_ai()
{ io_cs.aih = io_cs.aiv*io_cs.flow/(io_cs.vmax); }

void analogDevice::humanize_ao()
{ io_cs.aov = io_cs.aoh*io_cs.vmax/io_cs.flow; }




