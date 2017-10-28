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


#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include "timespec.h"
#include "serializer.h"

#ifdef SERVER
#include "serial.h"
#endif

#ifndef _DEV_H
#define _DEV_H

namespace csys
{
  /* base class for simple devices */

class dev
{
protected:
  const      std::string label_;
  /*    true after initialization   */
  bool       init_;                       
  /*    true when message is going to be transfered   */
  bool       emit_;  
  time       time_redge_;
  time       time_fedge_;
  /* define poll interval */
  time       timeout_;   
  /*  0.00...1.00  */
  float      tolerance_;             
  
public:
  static     std::map<std::string,dev*> deviceMap_;
  /*  true when client connected   */
  static     bool connection_;   
  static     bool session_;
  /*   request to send, SAME AS emit !!   */
  bool       rts_;
  static serializer is_;
  static serializer os_;

public:
  std::string get_label() { return label_; }
  time&       get_tm() { return timeout_; }
  /*  keeps time fetched with last cloop::get_time() call, always equals cloop::time_global  */
  static time time_global;
  void set_emit() { emit_   = true; }
  void reset_emit() { emit_ = false; }
  
  virtual void process()     = 0;
  virtual void serialize()   = 0;
  virtual void unserialize() = 0;
  virtual bool is_valid()    = 0;
  virtual ~dev()             = 0;

public:
  dev(const char* lbl, time tm);
  static void  generic_controller_module(const bool connected);

#ifdef CLIENT
    /* CLI for device managment */

#endif
};

typedef std::map<std::string,dev*>::iterator devMapIterator;

}
#endif







