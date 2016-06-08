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


#include <termios.h>
#include <string>
#include <queue>
#include <list>

#ifndef  _SERIAL_H
#define _SERIAL_H

namespace csys
{

class logger;


/*   fill request  and pass message to the port 
 *   port << message 
 *   test if there is any data available
 *   port >> message ? 1 : 0
 *   no read was done if response length is zero  */   
struct message
{
  std::string label;
  std::string request;
  std::string response;
  int command;
  char eot;
  message(const char* lbl): label(lbl) {}
  ~message() {}
  void clear() { request.clear(); response.clear(); command = 0; }
};

/*
 *     Serial Programming Guide for POSIX Operating Systems   
 *             http://www.cmrr.umn.edu/~strupp/serial.html
 */

class serial
{
private:
  int                     counter;
  std::string         path;
  struct termios  settings;
  struct termios  old_settings;
  int                     fd;
  int                     swap_len;
  bool                  swap;
  /*  used to define frame/link timeout  */
  time  timeout_frame;
  time  timeout_link;
  /*   always contains time of last successful write()   */
  time  time_write;
  /*   off when write() succeed, on when read() succeed  */  
  bool            frame_done;
  char*           buffer_swap;
  char*           buffer_in;
  char*           buffer_out;
  /* on if port is getting reply within timeout_link, off otherwise    */
  bool            link;
  char            eot;

  bool write_buffer(int len);
  int read_some();
  void error_handler();
  
  time   time_redge;
  bool             listen;
  const int      retry_allowed;
  int                retry_write;  
  int                retry_read;
  std::queue<message>request_queue;
  std::list<message>response_list;
  std::list<message>::iterator resIt;
  
public:
  static          logger& sLog;
  static bool  session;
  static int     bufferLen;
  serial(const char* port_path, int baud, time tm_frame, time tm_link);
  ~serial();
  void init();
  /*  generic method, must be called once per scan */
  void process();
  bool get_link() { return link; }
  time get_tmlink() { return timeout_link; }
  time get_timeout() { return timeout_frame; }
  
  /*  push new request to request_queue   */
  void operator << (const message& req) { request_queue.push(req); }
  /*  test if reply arrived; return true if there is a message in reply_queue   */
  bool operator >> (message& res)
  { 
    if(response_list.empty()) { return false; }
    for(resIt = response_list.begin(); resIt != response_list.end(); resIt++)
    {
      if(res.label == resIt->label)
      {
        res = *resIt;
        response_list.erase(resIt);
        return true;
      }  
    }
    return false;
  }
  
};

}
#endif






