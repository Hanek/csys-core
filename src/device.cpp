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

#include <stdlib.h> 

#include "commac.h"
#include "logger.h"
#include "device.h"


using namespace std;
using namespace csys;

bool device::connection = false;
bool device::session       = false;
map<string,device*> device::deviceMap;

csys::time device::time_global(false);

stringstream device::os;
stringstream device::is;
logger& device::dLog = logger::instance();

#ifdef SERVER
map<string,serial*> device::serialMap;
int device::bufferLen;

 
device::device(const char* lbl, time tm): label(lbl), timeout(tm)
{
  init          = false;
  emit        = false;
  rts           = false;
  state        = device_state::DISABLED;
  time_redge = time_global;
  time_fedge = time_global;
  
  if(!session)
  {
    session = true;
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)
    << "---------------------------------- device::device():session started... ---------------------------------- " << endl;
  }
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE)
  << label << delim << "device::device()" << endl;
}
#endif



#ifdef CLIENT  
#include "journal.h"
journal& device::jOur             = journal::instance();

device::device(const char* lbl, time tm):  label(lbl), timeout(tm)
{
  init          = false;
  emit        = false;
  rts           = false;
  state        = device_state::DISABLED;
  time_redge = time_global;
  time_fedge = time_global;

  pthread_mutex_init(&lock, NULL);

  
  if(!session)
  {
    session = true;
    dLog.getstream(logLevel::DEFAULT, logType::DEVICE)
    << "---------------------------------- device::device():session started... ---------------------------------- " << endl;
  }
  
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE) 
  << label << delim << "device::device()" << endl;
}
#endif



device::~device() 
{
#ifdef CLIENT  
  pthread_mutex_destroy(&lock);
#endif
  dLog.getstream(logLevel::DEFAULT, logType::DEVICE)
  << "device::~device()" << endl;
};



#ifdef SERVER

void device::generic_controller_module(io &sys_io, const bool connected)
{
  deviceMapIterator dmit;
  serialMapIterator smit;
  string testLabel;

  if(!is.str().empty())
  {
    /* dispatch messages from stringstream */
     while(is >> testLabel)
    {
      dmit = device::deviceMap.find(testLabel);
      
      if(dmit == device::deviceMap.end())
      {
        testLabel.clear();
        continue;
      }
      dmit->second->unserialize();
      testLabel.clear();
    }
  }
  
  is.clear();
  device::is.str(string());
  os.clear();
  device::os.str(string());
 
  for(smit = device::serialMap.begin(); smit != device::serialMap.end(); smit++)
  { smit->second->process(); } 
  
  
  if(!dmit->second->connection && connected)
  { /*  send device's states to client when connection established */
    for(dmit = device::deviceMap.begin(); dmit != device::deviceMap.end(); dmit++)
    { 
      dmit->second->process(sys_io);  
      dmit->second->emit = true;
      dmit->second->serialize();
    }
    dmit->second->connection = true;
    return;
  }

  for(dmit = device::deviceMap.begin(); dmit != device::deviceMap.end(); dmit++)
  {
    dmit->second->process(sys_io); 
      
    if(!connected)
    { dmit->second->connection = false; }
    
    if(dmit->second->emit && connected)
    { dmit->second->serialize(); }
  }
  
}

#endif



#ifdef CLIENT



void device::generic_controller_module(const bool connected)
{
  deviceMapIterator dmit;
  string testLabel;
  
  device::connection = connected;
  
  /* dispatch messages from stringstream */
    while(is >> testLabel)
    {
      dmit = device::deviceMap.find(testLabel);
      
      if(dmit == device::deviceMap.end())
      {
        testLabel.clear();
        continue;
      }
      dmit->second->unserialize();
      testLabel.clear();
    }
    
    
    is.clear();
    device::is.str(string());
    os.clear();
    device::os.str(string());
    
  
  for(dmit = device::deviceMap.begin(); dmit != device::deviceMap.end(); dmit++)
  { 
    /*     locking devices in the main loop   */
    pthread_mutex_lock(dmit->second->glock());
   
    dmit->second->process(); 
    if(dmit->second->emit && connected)
    { 
      dmit->second->serialize(); 
      dmit->second->reset_emit();
    }
    
    pthread_mutex_unlock(dmit->second->glock());
  }
}


void device::populate_widgets()
{
  deviceMapIterator dmit;
  for(dmit = device::deviceMap.begin(); dmit != device::deviceMap.end(); dmit++)
  { dmit->second->build_widget(); }
}

 
#endif




