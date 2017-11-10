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
#include "dev.h"
#include "serializer.h"

using namespace std;
using namespace csys;

bool dev::connection_ = false;
bool dev::session_    = false;
map<string,dev*> dev::deviceMap_;
  
csys::time dev::time_global(false);

serializer dev::os_;
serializer dev::is_;

dev::dev(const char* lbl, time tm): label_(lbl), timeout_(tm)
{
  init_       = false;
  emit_       = false;
  rts_        = false;
  time_redge_ = time_global;
  time_fedge_ = time_global;
  
  if(!session_)
  { session_ = true; }
}


dev::~dev() 
{
};


#ifdef SERVER

void dev::generic_controller_module(const bool connected)
{
  devMapIterator dmit;
  char devLabel[32] = {0};

  if(!is_.empty())
  {
    while(is_.read_block(devLabel))
    {
      dmit = deviceMap_.find(devLabel);
      
      if(dmit == deviceMap_.end())
      {
        /* jump to the next block */
        memset(devLabel, 0x00, sizeof(devLabel));
        continue;
      }
      dmit->second->unserialize();
      memset(devLabel, 0x00, sizeof(devLabel));
    }
  }
  
  is_.clear();
  os_.clear();
  
  if(!dmit->second->connection_ && connected)
  { /*  send device's state to client when connection established */
    for(dmit = deviceMap_.begin(); dmit != deviceMap_.end(); dmit++)
    { 
      dmit->second->process();  
      dmit->second->emit_ = true;
      dmit->second->serialize();
    }
    dmit->second->connection_ = true;
    return;
  }

  for(dmit = deviceMap_.begin(); dmit != deviceMap_.end(); dmit++)
  {
    dmit->second->process(); 
      
    if(!connected)
    { dmit->second->connection_ = false; }
    
    if(dmit->second->emit_ && connected)
    { dmit->second->serialize(); }
  }
  
}

#endif



#ifdef CLIENT

std::vector<std::string> dev::devNewborn_;

void dev::generic_controller_module(const bool connected)
{
  devMapIterator dmit;
  char devLabel[32] = {0};
 
  connection_ = connected;
  
  while(is_.read_block(devLabel))
  {
    dmit = deviceMap_.find(devLabel);
    
    if(dmit == deviceMap_.end())
    {
      /* device is not registered */
      devNewborn_.push_back(devLabel);
      memset(devLabel, 0x00, sizeof(devLabel));
      continue; 
    }
    dmit->second->unserialize();
    memset(devLabel, 0x00, sizeof(devLabel));
  }
  
  for(dmit = deviceMap_.begin(); dmit != deviceMap_.end(); dmit++)
  {
    dmit->second->process();
    
    if(dmit->second->emit_ && connected)
    {
      dmit->second->serialize(); 
      dmit->second->reset_emit();
    }
  }
  
  
  is_.clear();
  os_.clear();
}


void dev::generic_command_module()
{
  /* provide access to system devices */
    
}


void dev::populate_widgets()
{
  devMapIterator dmit;
  for(dmit = dev::deviceMap_.begin(); dmit != dev::deviceMap_.end(); dmit++)
  { dmit->second->build_widget(); }
}


#endif




