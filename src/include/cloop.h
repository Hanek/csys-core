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


#include "timespec.h"

#ifndef _CLOOP_H
#define _CLOOP_H


namespace csys
{
  
/*  cloop used to maintain scan time of loop execution, measure latency and performance */ 

class fifo;
class io;
class transport;

class cloop
{
private:
  /*  define IO refresh rate  */
  static const time    scantime;
  /*  define synchronization window  */
  static const long int         width;
  long int                      scanCounter;
  time     time_beg;
  time     time_in;
  time     time_jmp;
  time     time_pre;
  time     job;
  time     elapsed;
  time     sleep;
  time     elapsed_sum;
  time     scandelay;
  time     latency;
  time     scanbased_time;
  bool               server;
  bool               first_cycle;
  bool               crep;
  
  void nsleep(time& time);
  void measure();
  void console_report(std::string& str);
  void timeval_output();
  void get_connection_info(std::string&, bool, bool);

public:
  cloop();
  ~cloop();

  static bool error;
  /*  get system time */
  static void get_time(time& time);
  /*  keeps time fetched with last get_time() call  */
  static time time_global;
  time& pTime() { return time_jmp; }
  
#ifdef SERVER
  void enable(io& sys_io, fifo* ff, transport* eth);
#endif	

#ifdef CLIENT
  void enable(fifo* ff, transport* eth);
#endif

};


class timer
{
private:
  bool active;
  bool ready;
  time st;
  time tm;
public:
  timer(): active(false), ready(true) {} 
  bool start(time sp);
  bool is_valid();
  void reset();
};

}
#endif





