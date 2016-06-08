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


#define AISAMPL 50

namespace csys
{
  class logger;



struct aiport
{
  int         index;
  int         len;
  int         ai_sum;
  int         gain;
  int         buffer[AISAMPL];
  
  aiport(): index(0), len(AISAMPL), ai_sum(0), gain(0) {}
  
  void update(int ai_stream, int& ai_mean, float& aiv)
  {
    buffer[index++] = ai_stream;
    ai_sum += ai_stream;
    
    if(index > len - 1)
    {
      ai_mean = ai_sum/len;
      aiv          = static_cast<float>(ai_mean)/static_cast<float>(0x1FFF/0x000A);
      index      = 0;
      ai_sum    = 0;
    }
  }
};

struct aoport
{
  int gain;
  float vout;
  aoport(): gain(0) {}
  void update(float aov, int& ao)
  { ao = static_cast<int>( (aov)*static_cast<float>(0x1FFF/0x000A) ); }
};



class io
{
private:
  static size_t    DO;
  static size_t    DI;
  int                   slot_di;
  int                   slot_do;
  int                   slot_ai;
  int                   slot_ao;
  aiport              aip[8];
  aoport             aop[8];
  
public:
  static logger& ioLog;
  io(int dI, int dO, int aI, int aO);
  ~io();
  void getDI() const;
  void setDO();
  bool get_di_bit(int pos) const;
  /*   TODO  combine two in one   */
  void set_do_bit(int pos);
  void rset_do_bit(int pos);
  
  size_t extractDI() { return DI; }
  size_t extractDO() { return DO; }

/*    data range  0x0000 ~ 0x1FFF, Voltage Input 0.0 ~ +10.0 V */
/*    ch       -  channel number in ai slot                                        */
/*    ai        -  input value updated with every set_ai() call           */
/*    aim     -  mean input value                                                   */
/*    aiv      -  mean input value measured in voltage                 */
  void get_ai(int ch, int& ai, int& aim, float& aiv);
  
/*  data range: 0x0000 ~ 0x3FFF,  Voltage Output: -10.0 ~ +10.0 V)  */
/*    ch        -  channel number in ao slot                                             */
/*    ao        -  output value updated with every set_ao() call              */
/*    aov      -  output value measured in voltage                              */
  void set_ao(int ch, int& ao, float& aov);
  float get_ao(int ch);
};


}

#endif





