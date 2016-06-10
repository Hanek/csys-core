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


io::io(): n_aip(AIPORT), n_aop(AOPORT)
{
  /* initialize hardware here  */  
  if(!do_board_init() ||
      !di_board_init() ||
      !ai_board_init() ||
      !ao_board_init())
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

bool io::get_di_bit(int pos) const
{ return (1<<pos)&DI; }
 
void io::set_do_bit(int pos) 
{ DO = (1<<pos)|DO; }

void io::rset_do_bit(int pos) 
{ DO = (~(1<<pos))&DO; } 



float io::get_ai(int ch)  
{
  if(ch < 0 || ch > n_aip)
  { return 0.00f; }  
  
  return aip[ch].convert(aip[ch].raw);
} 


float io::get_aim(int ch)  
{
  if(ch < 0 || ch > n_aip)
  { return 0.00f; }  
  
  return aip[ch].convert(aip[ch].aim);
} 




void io::set_ao(int ch, float aov)
{
  if(ch < 0 || ch > n_aop)
    { return;}
    
  aop[ch].vout = aov;
  aop[ch].convert();
  aop[ch].update = true;
}


float io::get_ao(int ch)
{
  if(ch < 0 || ch > n_aip)
  { return 0.00f; }  
 
  return aop[ch].vout;
}
  


void io::io_update() 
{  /*  called every scan  */
  do_board_update();  
  di_board_update();
  ao_board_update();
  ai_board_update();
}



/*****************************  hardware access  **************************************/

bool io::do_board_init() { return true; }
bool io::di_board_init() { return true; }
bool io::ai_board_init() { return true; }
bool io::ao_board_init() { return true; }
void io::close_all() {}


void io::do_board_update() 
{
  /*  hardware library call, set do  */
//  DO_32(slot_do, DO); 
}

void io::di_board_update()
{ 
  /*  hardware library call, get di  */
//   DI = ~DI_32(slot_di); 
}

void io::ai_board_update()
{ 
  for(int ch = 0; ch < n_aip; ch++)
  {
    /*  hardware library call, get ai  */
//     aip[ch].raw = GetAiValueFromChannel(ch);
    aip[ch].update();
  }
}

void io::ao_board_update()
{ 
  for(int ch = 0; ch < n_aop; ch++)
  { 
    if(aop[ch].update)
    { 
      /*  hardware library call, set ao  */
//       SetAoValueChannel(ch, aop[ch].raw);
      aop[ch].update = false;
    }
  }

}
  