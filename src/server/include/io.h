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

#ifndef _IO_H
#define _IO_H

#define AIPORT 8
#define AOPORT 8
#define AISAMPL 50

namespace csys
{
  class logger;



struct aiport
{
  /*  actual value, updated every scan  */
  int raw;
  /*  mean value, updated every nsampl scan  */
  int aim;
  
  int    index;
  int    nsampl;
  long sum;
  
  aiport(): raw(0), aim(0), 
  index(0), nsampl(AISAMPL), sum(0) {}
  
  void update()
  {
    index++;
    sum += static_cast<long>(raw);
    
    if(index > nsampl - 1)
    {
      aim     = sum/static_cast<long>(nsampl);
      index  = 0;
      sum    = 0;
    }
  }
  
  float convert(int ai)
  { return static_cast<float>(ai)/static_cast<float>(0x1FFF/0x000A); }
};



struct aoport
{
  /*  actual value  */
  int raw;
  /*  voltage  */
  float vout;
  /*  on if value has to be updated  */
  bool update;
  
  aoport(): raw(0.0f), vout(0), update(true) {}
  
  /*  convert voltage  */
  void convert()
  { raw = static_cast<int>( (vout)*static_cast<float>(0x1FFF/0x000A) ); }
};



class io
{
private:
  static size_t    DO;
  static size_t    DI;

  aiport         aip[AIPORT];
  aoport        aop[AOPORT];
  const int n_aip;
  const int n_aop;
  
private:
  /*  hardware init */
  bool do_board_init();
  bool di_board_init();
  bool ai_board_init();
  bool ao_board_init();
  void close_all();
  
  /*  hardware update  */
  void do_board_update();
  void di_board_update();
  void ai_board_update();
  void ao_board_update();

  
public:
  static logger& ioLog;
  io();
  ~io();
  
  /*  called every scan, hardware access  */
  void io_update();
  
  bool get_di_bit(int pos) const;
  void set_do_bit(int pos);
  void rset_do_bit(int pos);
  
  size_t extractDI() { return DI; }
  size_t extractDO() { return DO; }
  
  void set_ao(int ch, float aov);
  /*  retrieve most recent values  */ 
  float get_ao(int ch);
  float get_ai(int ch);
  /*  retrieve mean value  */ 
  float get_aim(int ch);
};


}

#endif





