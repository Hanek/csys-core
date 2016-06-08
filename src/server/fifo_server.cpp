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

#include <fstream>
#include <errno.h>
#include <fcntl.h>
#include <sstream>

#include "fifo.h"
#include "logger.h"

using namespace std;
using namespace csys;


void fifo::start()
{
  if((mkfifo(serverFifo.c_str(), 0666) < 0) && (errno != EEXIST))
  {
    int errnum = errno;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << " mkfifo() failed: " << strerror(errnum) << endl;
    exit(1);
  }
  
  if((readfd = open(serverFifo.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
  {
    int errnum = errno;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << " open() failed: " << strerror(errnum) << endl;
    unlink(serverFifo.c_str());
    exit(1);
  }
}

/*******************************************************************************************/

void fifo::auth()
{
  if(!syn)
  {
    /* read incoming messages */
    if(!read_fifo())
    { return; }
    
    if('S' == buffer_in[0] &&
        'Y' == buffer_in[1] &&
        'N' == buffer_in[2])
    {
      syn = true;
      strcpy(path, buffer_in+3);
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << ": syn arrived: " << buffer_in<< endl;
    }
    return;
  }
  
  if(!synack)
  {
    /*  open writebale end to client path once SYN is arrived */	
    if((writefd = open(path, O_RDWR | O_NONBLOCK)) < 0)
    {
      error = true;
      int errnum = errno;
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << " open(" << path <<  ") failed: " << strerror(errnum) << endl;
    }
    messageLenOut = 0x06;
    memcpy(buffer_out, &messageLenOut, sizeof(messageLenOut));
    memcpy(buffer_out + sizeof(messageLenOut), "SYNACK", messageLenOut);
    messageLenOut += sizeof(messageLenOut);
    
    /*    send SYNACK to client     */
    write_fifo();
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": synack to client sent: " <<  buffer_out + sizeof(messageLenOut) << endl;
    synack = true;
    return;
  }

  if(syn && synack)
  {
    /* read until ACK is arrived */
    if(!read_fifo())
    { return; }
    
    if(0 == strcmp("ACK", buffer_in))
    { 
      connection = true;
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << ": client connected... " <<  buffer_in << endl;
    }
  }
  return;
}












