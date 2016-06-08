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


#include <gtk/gtk.h>
#include <map>
#include <iostream>
#include <string>
#include <sstream>

#include "journal.h"
#include "timespec.h"

#ifdef SERVER
#include "serial.h"
#endif

#ifndef _DEVICE_H
#define _DEVICE_H

namespace csys
{
class logger;
class io;
class streamer;

struct device_state 
{ enum state { DISABLED, CHANGED, ENABLED, ALARM }; };


/*       base class for all system's devices      */

class device
{
protected:
  const              std::string label;
  /*    true after initialization   */
  bool                init;                       
  /*    true when message is going to be transfered   */
  bool                emit;  
  int                   state;
  int                   state_ps;
  time       time_redge;
  time       time_fedge;
  /* define poll interval */
  time       timeout;   
  /* define time to reach setpoint */
  time       sp_timeout;
  time       sp_redge;
  /*  0.00...1.00  */
  float                 tolerance;             
  
public:
  static               std::map<std::string,device*> deviceMap;
  /*  true when client connected   */
  static               bool connection;   
  static               bool session;
  /*   request to send, SAME AS emit !!   */
  bool                 rts;

public:
  static               logger& dLog;
  static               streamer& dStream;
  int                   get_state() { return state; }
  std::string       get_label() { return label; }
  time&    get_tm() { return timeout; }
  static               std::stringstream os;
  static               std::stringstream is;
  /*  keeps time fetched with last cloop::get_time() call, always equals cloop::time_global  */
  static time time_global;

  virtual void    serialize()       = 0;
  virtual void    unserialize()   = 0;
  virtual bool    is_valid()         = 0;
  virtual ~device()                  = 0;

#ifdef SERVER
public:
  device(const char* lbl, time tm);
  static std::map<std::string,serial*> serialMap;
  static void  generic_controller_module(io &sys_io, const bool connected);
  virtual void  process(io &sys_io) = 0;
  static int bufferLen;
#endif

#ifdef CLIENT
public:
  device(const char* lbl, time tm);
  /*   synchronization between main loop and gui done with this lock   */
private:
  pthread_mutex_t lock; 
  
  /* on everytime unserialize() called, time_handler() doens't update widget if update_flag is off      */
  bool  update_flag; 
public:
  static journal& jOur;
  /*    pass this to jOur after trasnlate() is done   */
  std::stringstream hdev_state;
  virtual  void translator() = 0;
  virtual void translate_error(int) = 0;
  virtual void translate_state(int) = 0;
  virtual void translate_command(int) = 0;
  
  static void  generic_controller_module(const bool connected);
  static void  populate_widgets();
  virtual void  process() = 0;
  virtual void  build_widget() = 0;
  
  pthread_mutex_t* glock() { return &lock; }
  bool get_update_flag() { return update_flag; }
  void set_update_flag() { update_flag = true; }
  void reset_update_flag() { update_flag = false; }
  void set_emit() { emit = true; }
  void reset_emit() { emit = false; }
#endif
};

typedef std::map<std::string,device*>::iterator deviceMapIterator;
#ifdef SERVER
typedef std::map<std::string,serial*>::iterator serialMapIterator;
#endif

}
#endif







