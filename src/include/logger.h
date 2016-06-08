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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "timespec.h"

#ifndef _LOGGER_H
#define _LOGGER_H

namespace csys
{

struct logLevel
{ enum level { DEFAULT, SPECIAL, CDEBUG, FDEBUG, DDEBUG, SDEBUG, IODEBUG, EDEBUG }; };

struct logType
{ enum type { DEVICE, FIFO, CLOCK, SERIAL, IO, ETH, JRNL }; };


class nullStreamBuffer : public std::streambuf
{
  char dummyBuffer[64];
protected:
  virtual int overflow( int c )
  {
    setp(dummyBuffer, dummyBuffer + sizeof(dummyBuffer)) ;
    return (c == EOF) ? '\0' : c ;
  }
};

class nullstream: public nullStreamBuffer, public std::ostream
{
public:
  nullstream():std::ostream(this) {}
};



class logger
{
public:
  int currentLevel;
  std::ostream& getstream(const int restrictedLevel, const int type);
  static logger& instance();
  /*  keeps time fetched with last cloop::get_time() call, always equal cloop::time_global  */
  static time time_global;
  
protected:
  static logger*                 pInstance;
  static const char* const fileDev;
  static const char* const fileCom;
  static const char* const fileClock;
  static const char* const fileSerial;
  static const char* const fileIO;
  static const char* const fileRemote;
  static const char* const fileJournal;
  
  std::ofstream        devStream;
  std::ofstream        comStream;
  std::ofstream        clockStream;
  std::ofstream        serialStream;
  std::ofstream        ioStream;
  std::ofstream        remoteStream;
  std::ofstream        journalStream;
  nullstream     nullStream;
  
/*  Embedded class to make sure the single dLog
     instance gets deleted on program shutdown. */
  friend class Cleanup;
  class Cleanup
  {
  public:
    ~Cleanup();
  };
  
private:
  logger();
  virtual ~logger();
};

}
#endif


