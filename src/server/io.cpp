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

/*  TODO   complete review  */

#include <cstring>
#include <iostream>

#include "io.h"
#include "logger.h"
#include "commac.h"

using namespace csys;
using namespace std;

size_t io::DI(0);
size_t io::DO(0);

logger& io::ioLog = logger::instance();


io::io()
{
  /* initialize hardware here  */  
  if(!do_card_init() ||
      !di_card_init() ||
      !ai_card_init() ||
      !ao_card_init())
  {
    string error("failed to initialize hardware...");
    ioLog.getstream(logLevel::DEFAULT, logType::IO)
    <<  __func__  << ": " << error << endl;
    cout << error << endl;  
    exit(1); 
  }
} 
 
io::~io()
{ close_all(); }
  
   
 
void io::getDI() const 
{ 
//   DI = ~DI_32(slot_di); 
}   
 
void io::setDO() 
{ 
//   DO_32(slot_do, DO); 
}

bool io::get_di_bit(int pos) const
{ 
  return (1<<pos)&DI; 
  return true;
}

 
void io::set_do_bit(int pos)
{ 
  DO = (1<<pos)|DO; 
}

void io::rset_do_bit(int pos) 
{ 
  DO = (~(1<<pos))&DO; 
} 


void io::get_ai(int ch, int& ai, int& aim, float& aiv)  
{
//   I8017_SetChannelGainMode (slot_ai, ch, aip[ch].gain, 0);
//   ai = I8017_GetCurAdChannel_Hex(slot_ai); 
  
  aip[ch].update(ai, aim, aiv);
} 

void io::set_ao(int ch, int& ao, float& aov)
{
    if(ch || ao || aov) {}
  
  if(ch < 0 || ch > (int)(sizeof(aop)/sizeof(aop[0])) )
    { return;}
    
  aop[ch].vout = aov;
  
//   aop[ch].update(aov, ao);
//   I8024_VoltageHexOut(slot_ao, ch, 0x1FFF + ao);
}


float io::get_ao(int ch)
{
  if(ch < 0 || ch > (int)(sizeof(aop)/sizeof(aop[0])) )
  { return 0.00f; }
  return aop[ch].vout;
}
  