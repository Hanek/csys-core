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
#include "protocol.h"
#include "io.h"
#include "commac.h"

using namespace std;
using namespace csys;

dI::dI(const char *lbl, int position): 
dev(lbl, time::Sec1), pos_(position)
{
  prot::type::AI;
  pair<devMapIterator, bool> ret;
  ret = deviceMap_.insert(pair<string, dev*>(label_, this));
  if (false == ret.second) 
  {
    exit(1);
  }
  timeout_    = time(time::Sec1);
  time_redge_ = time_global + timeout_;
}

dI::~dI()
{
  devMapIterator it = deviceMap_.find(label_);
  
  if(it != deviceMap_.end())
  { 
    delete(it->second);
    deviceMap_.erase(it); 
  }
}







void dI::process()
{
  emit_ = false;
  /*     poll IO     */
  
  if(!init_)
  {
    init_ = true;
  }
            
  /* emulate here state flipping */
  if(time_redge_ < time_global)
  {
    cs_.state_ = (cs_.state_) ? false : true;
    time_redge_ = time_global + timeout_;
  }
  
  error_handler();
  request_handler();
  
  /*  notify CLIENT that something has changed  */
  if(rts_ || cs_ != ps_) 
  { rts_ = false; emit_ = true; }
  ps_ = cs_;
}

/*****************      internal methods     **********************/


void dI::error_handler()
{
}



void dI::request_handler()
{
  
}


void dI::serialize()
{
  os_.serialize<char>(cs_.error_);
  os_.serialize<bool>(cs_.state_);
  os_.sign_block(label_.c_str());
  
}


void dI::unserialize()
{
  
}




