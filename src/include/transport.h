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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <queue>
#include <pthread.h>

#include "timespec.h"

#ifndef _TRANSPORT_H
#define _TRANSPORT_H


namespace csys
{
  class serializer;
  
  class transport
  {
private:
  /*   server: listen on sockfd  */  
  /*   client:  connect on sockfd   */ 
  int                        sockfd;
  int                        rv;
  char                       s[INET6_ADDRSTRLEN];
  int bufferLen;
  std::string port;
  
#ifdef SERVER
  /*   server: accept on clifdfd  */  
  int     clifd;
  int     backlog;
#endif
  
  int                   yes;
  socklen_t             sin_size;
  struct addrinfo   hints, *servinfo, *p;
  struct sockaddr_storage their_addr;    
  struct sigaction  sa;
  
  pthread_t eth_mngr;
  pthread_t eth_send;
  pthread_t eth_recv;
  
  /*   synchronization primitives   */
  pthread_cond_t       cond_mngr;
  pthread_mutex_t     mutex_mngr;
  
  pthread_cond_t       cond_recv;
  pthread_mutex_t     mutex_recv;
  
  pthread_cond_t       cond_send;
  pthread_mutex_t     mutex_send;
  
  bool try_send(const char* buf, const int len);
  bool try_recv(char* buf, int len);
  void disconnect();
  
  /*   on while sending   */
  bool          send_flag;
  bool          disconnect_flag;
  
  std::string send_buffer; 
  std::string recv_buffer;
  std::queue<std::string>send_queue;
  std::queue<std::string>recv_queue;
   
  bool is_sending() 
  {
    bool res;
    pthread_mutex_lock(&mutex_send);
    res = send_flag;
    pthread_mutex_unlock(&mutex_send);
    return res;
  }
  
public:
  bool connection;
#ifdef CLIENT
  transport(const char*, int);
#endif
#ifdef SERVER
  transport(int);
#endif
  ~transport();
  void init();
  
  /*   called every scan   */
  void poll(serializer&, serializer&);

  bool accept_client();
  void enable_send(); 
  void enable_recv(); 
  void manage_queue();
  bool sleep_recv();
}; 

}

#endif





