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

#include "discrete.h"
#include "io.h"
#include "commac.h"

using namespace std;
using namespace csys;

dI::dI(const char *lbl, int position): 
dev(lbl, time::Sec1), pos_(position)
{
  pair<devMapIterator, bool> ret;
  ret = deviceMap_.insert(pair<string, dev*>(label_, this));
  if (false == ret.second) 
  {
    exit(1);
  }
}

dI::~dI()
{
  devMapIterator it = deviceMap_.find(label_);
  
  if(it == deviceMap_.end())
  {
    return;
  }
  else
  { deviceMap_.erase(it); }
  
}







void dI::process()
{
  emit_ = false;
  /*     poll IO     */
  
  if(!init_)
  {
    init_ = true;
  }
  
  error_handler();
  
  
//   if(cs_.command != ps_.command)
//   {
//     time_redge = time_global;
//     switch(io_cs.command)
//     {
//       case cmd::NOCMD:
//         break;
//         
//       case cmd::CLOSE:
//         io_cs.do_control = false;
//         sys_io.rset_do_bit(p_control);
//         state = device_state::CHANGED;
//         break;
//         
//       case cmd::OPEN:
//         io_cs.do_control = true;
//         sys_io.set_do_bit(p_control);
//         state = device_state::CHANGED;
//         break;
//         
//       case cmd::INIT:
//         io_cs.do_control = false;
//         sys_io.rset_do_bit(p_control);
//         state = device_state::CHANGED;
//         io_cs.error = err::NOERR;
//         break;
//         
//       default:
//         break;
//     }
//   }
  
  
  request_handler();
  
  /*  notify CLIENT that something has changed  */
  if(rts_ || cs_ != ps_) 
  { rts_ = false; emit_ = true; }
  ps_ = cs_;
}

/*****************      internal methods     **********************/


void dI::error_handler()
{
//   if(cmd::NOCMD == io_cs.command)
//   {
//     /*  close contact bouncing  */
//     if(io_cs.di_closed != io_ps.di_closed)
//     { 
//       state = device_state::ALARM;
//       io_cs.error = err::EBNCL;
//     }
//     
//     /*  open contact bouncing  */
//     if(io_cs.di_open != io_ps.di_open)
//     { 
//       state = device_state::ALARM;
//       io_cs.error = err::EBNOP;
//     }
//   }  
}



void dI::request_handler()
{
  
}







void dI::serialize()
{
  
}


void dI::unserialize()
{
  
}




