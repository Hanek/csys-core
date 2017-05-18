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

#ifdef SERVER
map<string,serial*> dev::serialMap_;
#endif 
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
//   deviceMapIterator dmit;
//   serialMapIterator smit;
//   string testLabel;

//   if(!is.str().empty())
//   {
//     /* dispatch messages from stringstream */
//      while(is >> testLabel)
//     {
//       dmit = dev::deviceMap.find(testLabel);
//       
//       if(dmit == dev::deviceMap.end())
//       {
//         testLabel.clear();
//         continue;
//       }
//       dmit->second->unserialize();
//       testLabel.clear();
//     }
//   }
//   
//   is.clear();
//   dev::is.str(string());
//   os.clear();
//   dev::os.str(string());
//  
//   for(smit = dev::serialMap.begin(); smit != dev::serialMap.end(); smit++)
//   { smit->second->process(); } 
//   
//   
//   if(!dmit->second->connection && connected)
//   { /*  send device's states to client when connection established */
//     for(dmit = dev::deviceMap.begin(); dmit != dev::deviceMap.end(); dmit++)
//     { 
//       dmit->second->process(sys_io);  
//       dmit->second->emit = true;
//       dmit->second->serialize();
//     }
//     dmit->second->connection = true;
//     return;
//   }
// 
//   for(dmit = dev::deviceMap.begin(); dmit != dev::deviceMap.end(); dmit++)
//   {
//     dmit->second->process(sys_io); 
//       
//     if(!connected)
//     { dmit->second->connection = false; }
//     
//     if(dmit->second->emit && connected)
//     { dmit->second->serialize(); }
//   }
  
}

#endif



#ifdef CLIENT



// void dev::generic_controller_module(const bool connected)
// {
//   deviceMapIterator dmit;
//   string testLabel;
//   
//   dev::connection = connected;
//   
//   /* dispatch messages from stringstream */
//     while(is >> testLabel)
//     {
//       dmit = dev::deviceMap.find(testLabel);
//       
//       if(dmit == dev::deviceMap.end())
//       {
//         testLabel.clear();
//         continue;
//       }
//       dmit->second->unserialize();
//       testLabel.clear();
//     }
//     
//     
//     is.clear();
//     dev::is.str(string());
//     os.clear();
//     dev::os.str(string());
//     
//   
//   for(dmit = dev::deviceMap.begin(); dmit != dev::deviceMap.end(); dmit++)
//   { 
//     /*     locking devices in the main loop   */
//     pthread_mutex_lock(dmit->second->glock());
//    
//     dmit->second->process(); 
//     if(dmit->second->emit && connected)
//     { 
//       dmit->second->serialize(); 
//       dmit->second->reset_emit();
//     }
//     
//     pthread_mutex_unlock(dmit->second->glock());
//   }
// }

 
#endif




