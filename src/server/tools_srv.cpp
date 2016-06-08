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

#include "tools.h"
#include "logger.h"
#include "commac.h"
#include "io.h"

using namespace std;
using namespace csys;
using namespace tools;

 
/************************************************************************************************/
/*                                                               diModule                                                                        */
/***********************************************************************************************/

diModule::diModule(): device("diModule", time::Sec1), nchan(DI_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "ctror: device already exists" <<  endl;
    cout << "ctor: " << label << " already exists" << endl;
    exit(1);
  }
}


diModule::~diModule()
{
  deviceMapIterator it = deviceMap.find(label);
  
  if(it == deviceMap.end())
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "dtor: device not found" <<  endl;
    return;
  }
  else
  { deviceMap.erase(it); }
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
  << "dtor: device removed" <<  endl;
}



void diModule::process(io &sys_io)
{
  emit = false;
  /*     poll IO     */
  io_cs.DI = sys_io.extractDI();
  
  if(!init)
  { init = true; }
  
  /*  notify CLIENT that something has changed  */
  if(rts || io_cs != io_ps) { rts = false; emit = true; }
  io_ps = io_cs;
  
}


void diModule::serialize()
{
  os << label << delim
      << io_cs.DI << delim;
}

void diModule::unserialize()
{
}




/************************************************************************************************/
/*                                                               doModule                                                                        */
/***********************************************************************************************/


doModule::doModule(): device("doModule", time::Sec1), nchan(DO_NCHAN)
{
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "ctror: device already exists" <<  endl;
    cout << "ctor: " << label << " already exists" << endl;
    exit(1);
  }
}


doModule::~doModule()
{
  deviceMapIterator it = deviceMap.find(label);
  
  if(it == deviceMap.end())
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    << "dtor: device not found" <<  endl;
    return;
  }
  else
  { deviceMap.erase(it); }
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
  << "dtor: device removed" <<  endl;
}



void doModule::process(io &sys_io)
{
  emit = false;
  /*     poll IO     */
  io_cs.DO = sys_io.extractDO();
  
  if(!init)
  { init = true; }
  
  /*  notify CLIENT that something has changed  */
  if(io_cs != io_ps) { emit = true; }
  io_ps = io_cs;
  
}


void doModule::serialize()
{
  os << label << delim
  << io_cs.DO << delim;
}

void doModule::unserialize()
{
}







/************************************************************************************************/
/*                                                               aiModule                                                                        */
/***********************************************************************************************/


aiModule::aiModule(): device("aiModule", time::Sec1), nchan(AI_NCHAN)
{
  time_redge = time_global;
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    <<  __func__ << ": device already exists" <<  endl;
    cout <<  __func__ << delim  << label << " already exists" << endl;
    exit(1);
  }
}


aiModule::~aiModule()
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



void aiModule::process(io &sys_io)
{
  emit = false;
  /*     poll IO     */
  for(int ch = 0; ch < nchan; ch++)
  {
    int ai, aim;
    sys_io.get_ai(ch, ai, aim, io_cs.ai[ch]);
    io_cs.ai[ch] = 0;
  }
  
  if(!init)
  { init = true; }
  
  /*   frame switcher   */
  if(time_global - time_redge > timeout)
  { 
    /*  notify CLIENT  */
    emit = true;
    time_redge = time_global;
  }
}


void aiModule::serialize()
{
  os << setprecision(4);
  os << label << delim;
  
  for(int ch = 0; ch < nchan; ch++)
  { os << io_cs.ai[ch] << delim; }
}

void aiModule::unserialize()
{
}








/************************************************************************************************/
/*                                                               aoModule                                                                        */
/***********************************************************************************************/


aoModule::aoModule(): device("aoModule", time::Sec1), nchan(AO_NCHAN)
{
  time_redge = time_global;
  pair<deviceMapIterator, bool> ret;
  ret = deviceMap.insert ( pair<string, device*>(label, this));
  if (false == ret.second) 
  {
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE) << label << delim 
    <<  __func__ << ": device already exists" <<  endl;
    cout <<  __func__ << delim  << label << " already exists" << endl;
    exit(1);
  }
}


aoModule::~aoModule()
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



void aoModule::process(io &sys_io)
{
  emit = false;
  
  
  if(!init)
  { init = true; }
  
  /*   frame switcher   */
  if(time_global - time_redge > timeout)
  { 
    /*  notify CLIENT  */
    emit = true;
    time_redge = time_global;
    for(int ch = 0; ch < nchan; ch++)
    { io_cs.ao[ch] = sys_io.get_ao(ch); }
  }
}


void aoModule::serialize()
{
  os << setprecision(4);
  os << label << delim;
  
  for(int ch = 0; ch < nchan; ch++)
  { os << io_cs.ao[ch] << delim; }
}

void aoModule::unserialize()
{
}


