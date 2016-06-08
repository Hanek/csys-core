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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "timespec.h"
#include "commac.h"
#include "serial.h"
#include "logger.h" 

using namespace std;
using namespace csys;

logger& serial::sLog = logger::instance(); 
bool serial::session   = false;
int    serial::bufferLen;

/*     version3     */
serial::serial(const char* port_path, int baud, time tm_frame, time tm_link): 
path(port_path), swap(false),
timeout_frame(tm_frame), timeout_link(tm_link), time_write(logger::time_global),
frame_done(true), link(true), time_redge(0,0), listen(false), retry_allowed(1), retry_write(0), retry_read(0)
{
  if(!session)
  {  
    session = true; 
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL)
    << path << delim << "---------------------------------- serial::serial():session started... ----------------------------------" << endl; 
  }
  
  int rc;
  memset(&settings, 0, sizeof(settings));
  
  fd = open(port_path, O_RDWR | O_NONBLOCK);     
  if(fd < 0)
  {
    int errnum = errno;
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
    << "serial::serial():open() failed, errno " << errnum << delim << strerror(errnum) << endl;
    cout << "unable to open serial port... " << port_path << endl;
    exit(1);
  }
    if((rc = tcgetattr(fd, &settings)) < 0)
    {
      int errnum = errno;
      sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
      << "serial::serial():tcgetattr() failed, errno " << errnum << delim << strerror(errnum) << endl;
      cout << "unable to get serial port attributes..." << endl;
      exit(EXIT_FAILURE);
    }
  
  old_settings = settings;
  cfsetospeed(&settings, baud);
  cfsetispeed(&settings, baud);
  settings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
  /*    choosing unprocessed input  */
  settings.c_oflag &= ~OPOST;
  settings.c_lflag &= ~(ECHO | ECHONL | ECHOE |ICANON | ISIG | IEXTEN);
  /*      No parity (8N1)           */
  settings.c_cflag &= ~PARENB;
  settings.c_cflag &= ~CSTOPB;
  settings.c_cflag &= ~CSIZE;
  settings.c_cflag |= CS8;
  /*
   *    completely non-blocking read - the call is satisfied 
   *    immediately directly from the driver's input queue
   */
  settings.c_cc[VMIN]  = 0;
  settings.c_cc[VTIME] = 0; 

  
    if((rc = tcsetattr(fd, TCSANOW, &settings)) < 0)
    {
      int errnum = errno;
      sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
      << "serial::serial():tcsetattr() failed, errno " << errnum << delim << strerror(errnum) << endl;
      cout << "unable to set serial port attributes..." << endl;
      exit(EXIT_FAILURE);
    }
    
    /*   allocate buffers  */
    buffer_swap = new(std::nothrow) char[bufferLen];
    buffer_in      = new(std::nothrow) char[bufferLen];
    buffer_out    = new(std::nothrow) char[bufferLen];
    
    if(!buffer_swap || !buffer_in || !buffer_out)
    {
      sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
      << "serial(): bad alloc" << endl;
      cout << path << "serial(): bad alloc" << endl;
      exit(1);
    }
}



    

serial::~serial()
{
  delete[] buffer_swap;
  delete[] buffer_in;
  delete[] buffer_out;
  
  tcsetattr(fd, TCSANOW, &old_settings);
  sLog.getstream(logLevel::DEFAULT, logType::SERIAL)
  << path << delim << "serial::serial(): session closing..." << endl;
  close(fd);
}


void serial::init()
{
  /*  TODO  */
  link = true;
  time_write = logger::time_global;
}



bool serial::write_buffer(int len)
{
  /*   return false if attempt to write failed, true otherwise   */
  if(0 == len)
  { return true; }

  int res = write(fd, buffer_out, len);
 
  if(res != len)
  {
    int errnum = errno;
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim <<
    request_queue.front().label << delim << "serial::write_buffer():write() failed, errno " << errnum << delim << strerror(errnum) << endl;
    return false;
  }
  
  if(frame_done) 
  { 
    time_write = logger::time_global;
    frame_done = false;
  }
 
  return true;
}





int serial::read_some()
{
  /*   return amount of characters arrived   */
  int res  = 0;
  memset(buffer_in, 0, bufferLen);
  
  res = read(fd, buffer_in, bufferLen);
  /*    no read occured   */
  if(res <= 0)
  { return 0; }

  /*   buffer overflow control, abort swapping    */
  if(swap && (swap_len + res > bufferLen))
  {
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
    << "serial::read_some():swap failed, overflow: " << swap_len << delim << res << delim <<  buffer_swap << buffer_in << endl;
    swap = false;
    return 0;
  }
  
  for(int i = 0; i < res; i ++)
  { 
    if(eot == buffer_in[i])
    {/*   end of transfer   */
      res = i + 1;
      sLog.getstream(logLevel::SDEBUG, logType::SERIAL) << path << delim 
      << "serial::read_some():eot arrived: " << res << delim<< buffer_in << endl;
    }
  }
  
  if(eot != buffer_in[res - 1])
  {/*   part of message being recieved, copy to swap buffer   */
    sLog.getstream(logLevel::SDEBUG, logType::SERIAL) << path << delim 
    << "serial::read_some():swap: " << res << delim<< buffer_in << delim << hex << (int)buffer_in[res] << endl;
    if(!swap)
    {
      swap        = true;
      swap_len  = res;
      memset(buffer_swap, 0, bufferLen);
      memcpy(buffer_swap, buffer_in, res);
    }
    else
    {
      memcpy(buffer_swap + swap_len, buffer_in, res);
      swap_len += res;
    }
    return 0;
  }

  
  /*   message complete, eot arrived   */
  if(swap)
  {
    /*    finalize swapping    */
    swap = false;
    memcpy(buffer_swap + swap_len, buffer_in, res);
    swap_len +=res;
    memcpy(buffer_in, buffer_swap, bufferLen);
    res = swap_len;
    sLog.getstream(logLevel::SDEBUG, logType::SERIAL) << path << delim 
    << "serial::read_some():swap finalize: " << res << delim<< buffer_in << endl;
  }

  
  if(res < bufferLen)
  { buffer_in[res + 1] = 0x00; }
  else
  { buffer_in[bufferLen] = 0x00; }
  
  sLog.getstream(logLevel::SDEBUG, logType::SERIAL) << path << delim 
  << "serial::read_some(): " << buffer_in << endl;
  
  frame_done = true;
  return res;
}


/*    generic controller method called from process()   */
void serial::error_handler()
{
  

/*  no link if no reply since last write_buffer() call  */
  if(logger::time_global -  time_write > timeout_link && link)
  {
    link = false;
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
    << "serial::read_some():no link " <<  endl;
  }
  
  /*   link recovery   */
  if(frame_done && !link)
  {
    link = true;
    time_write = logger::time_global;
    sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim 
    << "serial::read_some():link recovered" << endl;
  }
}









/*    generic controller method, must bee called every scan   */
void serial::process()
{    
  if(request_queue.empty())
  { return; } 

  error_handler();
  
  if(listen)
  {
    /*   poll serial port   */
    if(0 < read_some())
    {
      /*  read done, copy incoming buffer and move 
       *  message from request to response queue  */
      listen = false;
      retry_read = 0;
      request_queue.front().response =  buffer_in;
      response_list.push_back(request_queue.front());
      request_queue.pop();
      sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim <<
      response_list.front().label << delim << "serial::process(): read done, push: " << buffer_in << endl;
    }
  }
  
  
  /*   new frame   */
  if(logger::time_global - time_redge > timeout_frame)
  {
    time_redge = logger::time_global;
    
    if(listen)
    {
      retry_read++;
      if(retry_read > retry_allowed)
      {
        /* no read done, move message from request to response queue  */
        listen = false;
        retry_read = 0;
        request_queue.front().response =  "";
        response_list.push_back(request_queue.front());
        request_queue.pop();
        sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim <<
        response_list.front().label << delim << "serial::process(): no read done, push empty response... " << endl;
        if(request_queue.empty())
        { return; }
      }
      /*  no read done, write the same message and try to read again  */
      listen = false;
    }
    
    
    if(request_queue.empty())
    { return; } 
    /*   try to send message  */
    size_t len = request_queue.front().request.length();
    memcpy(buffer_out, request_queue.front().request.c_str(), len);
    
    if(!write_buffer(len))
    {
      retry_write++;
      if(retry_write > retry_allowed)
      { /*   no more attemps to send  */
        request_queue.front().response =  "";
        response_list.push_back(request_queue.front());
        request_queue.pop();      
        listen = false;
        retry_write = 0; 
        sLog.getstream(logLevel::DEFAULT, logType::SERIAL) << path << delim <<
        response_list.front().label << delim << "process(): " << retry_allowed << " attemps to write failed... " << buffer_out << endl;
      }
      return;
    }  

    eot = request_queue.front().eot;
    listen = true;
  }
  
} 





