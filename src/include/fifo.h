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
#include <sstream>

#include <errno.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>

#ifndef _FIFO_H
#define _FIFO_H

namespace csys
{
  
class logger;

/*    Communication class driven by Length-Value protocol
 *    first 4 byte treated as 4byte integer which holds length of Value field.
 *    buffer_in[] must be supplied with null-terminated string
 */ 
 
class fifo
{
private:
  int               writefd;
  int               readfd;
  int               messageLenOut, messageLenIn;
  int               pathLen, bufferLen;
  int               synLim, synCounter;
  int               readLim;
  bool             sync_sent, sync_arrived;
  char*           path;
  std::string    serverFifo;
  std::string    clientFifo;

public:
  fifo(int);
  ~fifo();
  static bool  error;
  static logger& fLog;
  bool            connection;
  bool            synack;
  bool            syn;
  char*          buffer_in;
  char*          buffer_out;
  

  /* called once */
  void start();
  void auth();


  /*         these are called once per scan         */
  bool poll(const std::stringstream& os, std::stringstream& is);
  bool read_fifo();
  void write_fifo();
private:
  /*	extract data from stringstream and store at buffer_out */
  bool buffer_update(const std::stringstream& os);

};


}
#endif



