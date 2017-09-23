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

dI::dI(const char *lbl): 
dev(lbl, time::Sec1)
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
 
}



void dI::serialize()
{
   
}


void dI::unserialize()
{
  os_.deserialize<char>(&cs_.error_);
  os_.deserialize<bool>(&cs_.state_);
}




