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
  if((mkfifo(clientFifo.c_str(), S_IFIFO|0666) < 0) && (errno != EEXIST))
  {
    int errnum = errno;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << " mkfifo() failed: " << strerror(errnum) << endl;
    exit(1);
  }
  
  if((writefd = open(serverFifo.c_str(), O_WRONLY | O_NONBLOCK)) < 0)
		{
      int errnum = errno;
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << " open() failed: " << strerror(errnum) << endl;
      unlink(serverFifo.c_str());
      exit(1);
    }
    
    if((readfd = open(clientFifo.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
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
    /* send SYN + pipe path to initiate three-way handshake  */
    strcpy(path, clientFifo.c_str());
    messageLenOut = strlen(path) + 0x03;
    memcpy(buffer_out, &messageLenOut, sizeof(messageLenOut)); 
    memcpy(buffer_out + sizeof(messageLenOut), "SYN", 3);
    memcpy(buffer_out + sizeof(messageLenOut) + 3, path, strlen(path));
    messageLenOut += sizeof(messageLenOut);
    write_fifo();
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": synack to client sent: " <<  buffer_out + sizeof(messageLenOut) << endl;
    syn = true;
    return;
  }
  
  
  if(!synack)
  {
    /*  read until SYNACK is here */
    if(!read_fifo())
    { return; }
    
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": synack arrived: " << buffer_in<< endl;
    buffer_in[messageLenIn] = '\0';
    
    if(0 == strcmp("SYNACK", buffer_in))
    { synack = true; }
    else
    {
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << ": handshake failed: " << buffer_in<< endl;
      unlink(clientFifo.c_str());
      unlink(serverFifo.c_str());
      exit(1);
    }
    return;
  }

  if(syn && synack)
  {
    /* send ACK to complete authentication  */
    messageLenOut = 0x03;
    memcpy(buffer_out, &messageLenOut, sizeof(messageLenOut));
    memcpy(buffer_out + sizeof(messageLenOut), "ACK", messageLenOut);
    messageLenOut += sizeof(messageLenOut);
    messageLenOut += sizeof(messageLenOut);
    write_fifo();
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": connected to server... " <<  buffer_in << endl;
    connection = true;
  }
  return;
}

















