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

#include "fifo.h"
#include "commac.h"
#include "logger.h"


using namespace std;
using namespace csys;

logger& fifo::fLog = logger::instance();
bool fifo::error;


fifo::fifo(int bLen)
{
  messageLenIn        = 0;
  messageLenOut     = 0;
  pathLen                  = 32;
  bufferLen                = bLen;
  synLim                    = 24; 
  synCounter             = 0;
  readLim                  = 5;
  sync_sent                = false;        
  sync_arrived           = false;
  connection              = false;
  synack                    = false;
  syn                          = false;
  error                       = false;
  int wflags                = fcntl(writefd, F_GETFL, 0);
  int rflags                 = fcntl(readfd, F_GETFL, 0);
  fcntl(writefd, F_SETFL, wflags | O_NONBLOCK);
  fcntl(readfd, F_SETFL, rflags | O_NONBLOCK);
  serverFifo = "/tmp/fIf0Srv";
  clientFifo =  "/tmp/fIf0cli";
  
  path          = new(std::nothrow) char[pathLen];
  buffer_in   = new(std::nothrow) char[bufferLen];
  buffer_out = new(std::nothrow) char[bufferLen];
  
  if(!path || !buffer_in || !buffer_out)
  {
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": bad alloc" << endl;
    cout <<  __func__  <<  ": bad alloc" << endl;
    exit(1);
  }
  
  memset(path, 0x00, pathLen);
  fLog.getstream(logLevel::DEFAULT, logType::FIFO)
  <<  __func__  << ": is running..." << endl;
}


fifo::~fifo()
{
  delete[] path;
  delete[] buffer_in;
  delete[] buffer_out;
  
  close(writefd);
  close(readfd);
  unlink(serverFifo.c_str());
  unlink(clientFifo.c_str());
  fLog.getstream(logLevel::DEFAULT, logType::FIFO)
  <<  __func__  << ": is closed..." << endl;
}






bool fifo::read_fifo()
{
int read_attempt = 0;

while(true)
{
  /*    try to read message    */	
  if(read(readfd, &messageLenIn, sizeof(messageLenIn)) != sizeof(messageLenIn) ||
    read(readfd, buffer_in, messageLenIn) != messageLenIn)
  {
    if(EAGAIN == errno || EEXIST == errno || EINVAL == errno 
      || ENOTTY) /*  does occur on 3.13.0-61-generic   */
    { return false; }
    else
    {
      error = true;
      int errnum = errno;
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << ": read() failed: " << strerror(errnum) << endl;
      
      if(EPIPE == errnum)
      { exit(1); }
    }
  }
  
  if(0 != messageLenIn)
  { break; } 
   
  if(read_attempt > readLim)
  {
    error = true;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO) 
    <<  __func__  << ": exceeded maximum number of attempts " << readLim << endl;
    return false;
  }
  ++read_attempt;
}

  fLog.getstream(logLevel::FDEBUG, logType::FIFO)
  <<  __func__  << ": " << buffer_in << endl;

return true;
}
 
 
 
void fifo::write_fifo()
{
  fLog.getstream(logLevel::FDEBUG, logType::FIFO)
  <<  __func__  << ": " << buffer_out << endl;
  
  if(write(writefd, buffer_out, messageLenOut) != messageLenOut)
  {/*  TODO  need some action  */
    error = true;
    int errnum = errno;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": write() failed: " << strerror(errnum) << endl;
    
    if(EPIPE == errnum)
    { exit(1); }
  }
}



bool fifo::buffer_update(const stringstream& os)
{
  const string& message = os.str();
  const char* pMessage = message.c_str();
  messageLenOut = message.length();
  
  fLog.getstream(logLevel::FDEBUG, logType::FIFO)
  <<  __func__  << ": " << message << endl;
  
  if(messageLenOut > bufferLen - (int)sizeof(messageLenOut) - 1)
  {/*  TODO  swapping  */
    error = true;
    fLog.getstream(logLevel::DEFAULT, logType::FIFO)
    <<  __func__  << ": message length exceeded maximum size: " 
    << bufferLen << delim << messageLenOut << delim << message << endl;
    messageLenOut = bufferLen - (int)sizeof(messageLenOut) - 1;
  }
  
  memcpy(buffer_out, &messageLenOut, sizeof(messageLenOut));
  memcpy(buffer_out + sizeof(messageLenOut), pMessage, messageLenOut);
  messageLenOut += sizeof(messageLenOut);	

return true;
} 




bool fifo::poll(const stringstream& os, stringstream& is)
{
  memset(buffer_out, 0x00, bufferLen);
  memset(buffer_in, 0x00, bufferLen);
  
  if(!connection)
  {
    auth();
    return false;
  }   
  
  synCounter++;
  
  if(synCounter > synLim)
  {
    if(!sync_sent && os.str().empty())
    {/* send syncing message */
      messageLenOut = 0x07;
      memcpy(buffer_out, &messageLenOut, sizeof(messageLenOut)); 
      memcpy(buffer_out + sizeof(messageLenOut), "syncing", messageLenOut);
      messageLenOut += sizeof(messageLenOut);
      write_fifo();
      sync_sent = true;
    }
  }
  /*   write only if there is something to write */
  if(!os.str().empty())
  {
    buffer_update(os);
    write_fifo();
  }

  if(read_fifo())
  {
    if(0 == strcmp(buffer_in, "syncing"))
    { sync_arrived = true; }
    else
    { is << buffer_in ; }
  }

  if(sync_arrived && sync_sent) 
  {
    synCounter = 0;
    sync_arrived = false;
    sync_sent      = false;
  }
   
#ifdef SERVER
   if(synCounter > synLim*2)
   {
     fLog.getstream(logLevel::DEFAULT, logType::FIFO)
     <<  __func__  << ": client disconnected..." << endl;
      close(writefd);
      synCounter     = 0;
      sync_arrived   = false;
      sync_sent       = false;
      connection     = false;
      synack           = false;
      syn                = false;
   }
#endif

#ifdef CLIENT
   if(synCounter > synLim*2)
   {
      close(writefd);
      close(readfd);
      fLog.getstream(logLevel::DEFAULT, logType::FIFO)
      <<  __func__  << ": connection is lost, closing..." << endl;
      exit(1);
   }
#endif  
return true;
}




