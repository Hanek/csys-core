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


#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>

#include "remote.h"
#include "logger.h"
#include "commac.h"


/*   TODO    shutdown, review  */
 
using namespace std;
using namespace csys;

logger& remote::eLog = logger::instance(); 

/*   COMMON METHODS   */

/*   called on every scan by main app thread   */
void remote::poll(stringstream& os, stringstream& is)
{
  if(!(os.str().empty()))
  { /*   signal manager   */
    pthread_mutex_lock(&mutex_mngr);
    send_queue.push(os.str());
    pthread_cond_signal(&cond_mngr);
    pthread_mutex_unlock(&mutex_mngr);
  }
  
  
  pthread_mutex_lock(&mutex_recv);
  if(0 != recv_queue.size())
  {/*  copy data to device::is */
    is << recv_queue.front();
    recv_queue.pop();
  }
  pthread_mutex_unlock(&mutex_recv);
}



remote::~remote()
{
  pthread_cond_destroy(&cond_mngr);
  pthread_mutex_destroy(&mutex_mngr);
   
  pthread_cond_destroy (&cond_send);
  pthread_mutex_destroy(&mutex_send);
   
  pthread_cond_destroy(&cond_recv);
  pthread_mutex_destroy(&mutex_recv);
   
  close(sockfd);
   
#ifdef SERVER  
  close(clifd); 
#endif
  pthread_exit(NULL);
}



 void remote::manage_queue()
 {
   while(true) 
   {  
     pthread_mutex_lock(&mutex_mngr);
     pthread_cond_wait(&cond_mngr, &mutex_mngr);
     if(!is_sending() && disconnect_flag)
     {
       /*  signal sender   */
       pthread_mutex_lock(&mutex_send);
       pthread_cond_signal(&cond_send);
       pthread_mutex_unlock(&mutex_send);
       
       pthread_mutex_unlock(&mutex_mngr); 
       return;
     }
     
     
     /*   signal arrived and sender is not busy   */
     if(!is_sending() && !send_queue.empty())
     {
       send_buffer.clear();
       send_buffer = send_queue.front();
       send_queue.pop();

       /*  signal sender   */
       pthread_mutex_lock(&mutex_send);
       pthread_cond_signal(&cond_send);
       pthread_mutex_unlock(&mutex_send);
     }
     
     pthread_mutex_unlock(&mutex_mngr);  
   }
   
 }

 

/*    type-length-value standard  to ensure data consistency   */
/*    [ 4 byte integer - length ] [ message ], being sent in two runs, first 4 bytes, then remaining   */
bool remote::try_send(const char* buf, const int bufLen)
{
  if(!buf)
  { return false; }
  int res, bytes_sent, len, fd;

  
#ifdef SERVER
  fd = clifd;
#endif
#ifdef CLIENT
  fd = sockfd;
#endif
  
/*   send message length   */
  res = 0;
  len = sizeof(bufLen);

  while(true)
  {
    bytes_sent = send(fd, (char*)&bufLen + res, len, MSG_NOSIGNAL);
    if(-1 == bytes_sent)
    { cout << __func__ << " failed: " << strerror(errno) << endl; exit(1);  }
    
    if(bytes_sent != len)
    {/*   send the remaining   */
      res += bytes_sent;
      len -= bytes_sent;
    }
    else
    { break; } 
  }
  


/*   send message   */
  res = 0;
  len = bufLen;
  
  while(true)
  {
    bytes_sent = send(fd, buf + res, len, MSG_NOSIGNAL);
    if(-1 == bytes_sent)
    { cout << __func__ << " failed: " << strerror(errno) << endl; exit(1);  }
    
    if(bytes_sent != len)
    {/*   send the remaining   */
      res += bytes_sent;
      len -= bytes_sent;
    }
    else
    { break; }
  }
  
  eLog.getstream(logLevel::EDEBUG, logType::ETH) 
  << __func__ << ": bufLen:" << bufLen << delim << "buf:" << buf << endl; 
  
  return true;
}
 

 
 
/*    type-length-value standard  to ensure data consistency   */
/*    [ 4 byte integer - length ] [ message ], being sent in two runs   */
bool remote::try_recv(char* buf, int bufLen)
{
  if(!buf)
  { return false; }
  

  int mesLen;
  int res, bytes_recv, len, fd;
  
#ifdef SERVER
  fd = clifd;
#endif
#ifdef CLIENT
  fd = sockfd;
#endif
  
  
/*   receive message length   */
  res = 0;
  len = sizeof(mesLen);

  while(true)
  {
    bytes_recv = recv(fd, (char*)&mesLen + res, sizeof(mesLen), 0);
    if(-1 == bytes_recv)
    { /*   some error   */ return false;  }
    
    if(0 == bytes_recv)
    {  /*    other side has closed socket   */ return false;  }
    
    if(bytes_recv != len)
    {/*   receive the remaining   */
      res += bytes_recv;
      len -= bytes_recv;
    }
    else
    { break; } 
  }

  /*  TODO  */
  if(mesLen > bufLen - 1)
  {
    
  }
    
  /*   receive message   */
  res = 0;
  len = mesLen;
  while(true)
  {
    bytes_recv = recv(fd, buf + res, len, 0);
    if(-1 == bytes_recv)
    { /*   some error   */ return false;  }
    
    if(0 == bytes_recv)
    { /*    other side has closed socket   */ return false; }
    
    if(bytes_recv < len)
    {/*   receive the remaining   */
      res += bytes_recv;
      len -= bytes_recv;
    }
    else
    { break; } 
  }
  
  eLog.getstream(logLevel::EDEBUG, logType::ETH)
  << __func__ << ": mesLen:" << mesLen << delim << "buf:" << buf << endl; 
  
  return true;
}
 
 
 
 


void remote::enable_send() 
 {
  
   while(true)
   {  
     pthread_mutex_lock(&mutex_send);
     
     send_flag = false;
     pthread_cond_wait(&cond_send, &mutex_send);
     if(disconnect_flag)
     {
       pthread_mutex_unlock(&mutex_send);
       return;
     }
     
     send_flag = true;     
     try_send(send_buffer.c_str(), send_buffer.length());

     /*  signal manager   */
     pthread_mutex_lock(&mutex_mngr);
     pthread_cond_signal(&cond_mngr);
     pthread_mutex_unlock(&mutex_mngr);
     
     pthread_mutex_unlock(&mutex_send);
   }
   
 }
 


void remote::enable_recv()
 {
   char buff[bufferLen];
  
   while(true)
   {
     recv_buffer.clear();
     memset(buff, 0x00, sizeof(buff));
     
     if(!try_recv(buff, sizeof(buff)))
     {/*   stop reading   */
       disconnect();
       return;
     }
     recv_buffer = buff;
      

     pthread_mutex_lock(&mutex_recv);
     recv_queue.push(recv_buffer);
     pthread_mutex_unlock(&mutex_recv); 
   }
 }
 
 
 
 


#ifdef SERVER

/**********************   threads   ***************************/

void* manager_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  
  while(true)
  {
    if(eth->accept_client())
    {/*   client arrived   */
      eth->manage_queue();
    }
  }
  
  return 0;
}



void* send_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  
  while(true)
  {
    eth->enable_send();
  }
  
  return 0;
}


void* recv_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  
  while(true)
  {
    if(!eth->sleep_recv())
    { continue; }
    
    eth->enable_recv();
  }
  
  return 0;
}




void remote::init()
{
  if(0 != pthread_create(&eth_mngr, NULL, manager_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
  
  if(0 != pthread_create(&eth_send, NULL, send_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
  
  if(0 != pthread_create(&eth_recv, NULL, recv_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
}



void remote::disconnect()
{
  /*  signal manager   */
  pthread_mutex_lock(&mutex_mngr);
  disconnect_flag = true;
  connection       = false;
  close(clifd);
  pthread_cond_signal(&cond_mngr);
  pthread_mutex_unlock(&mutex_mngr);
  
  eLog.getstream(logLevel::DEFAULT, logType::ETH)
  << __func__ << ": client disconnected..." << endl; 
}


/**************************************************************************************/



bool remote::sleep_recv()
{
  bool res;
  pthread_mutex_lock(&mutex_recv);
  pthread_cond_wait(&cond_recv, &mutex_recv);
  res = connection;
  pthread_mutex_unlock(&mutex_recv);
  
  return res;
}




void sigchld_handler_rem(int s)
{
  if(s)
  {}
  
  while(waitpid(-1, NULL, WNOHANG) > 0);
}


void *get_in_addr_rem(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}




remote::remote(int bLen) : port("3490"), backlog(1)
{
  eLog.getstream(logLevel::DEFAULT, logType::ETH)
  << delim << "---------------------------------- remote::remotel():session started... ----------------------------------" << endl; 
  
  yes                      = 1;
  send_flag            = false;
  disconnect_flag    = false;
  connection          = false;
  
  bufferLen = bLen;
  
  pthread_cond_init (&cond_mngr, NULL);
  pthread_mutex_init(&mutex_mngr, NULL);
  
  pthread_cond_init (&cond_send, NULL);
  pthread_mutex_init(&mutex_send, NULL);
  
  pthread_cond_init (&cond_recv, NULL);
  pthread_mutex_init(&mutex_recv, NULL);
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  
  if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) 
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }
  
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) 
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
      p->ai_protocol)) == -1) 
    {
      perror("server: socket");
    continue;
    }
      
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) 
      {
        perror("setsockopt");
        exit(1);
      }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
          close(sockfd);
          perror("server: bind");
          continue;
        }
        
        break;
  }
  
  if (p == NULL)  
  {
    cout << __func__ <<  ": failed to bind" << endl;
    exit(1);
  }
  freeaddrinfo(servinfo);
}




bool remote::accept_client()
{
  if (listen(sockfd, backlog) == -1) 
  {
    perror("listen");
    exit(1);
  }
  
  sa.sa_handler = sigchld_handler_rem; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) 
  {
    perror("sigaction");
    exit(1);
  }
 
  
  sin_size = sizeof their_addr;
  clifd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
  inet_ntop(their_addr.ss_family, get_in_addr_rem((struct sockaddr *)&their_addr), s, sizeof s);
  
  
  
  /*   extract peer ip and port   */
  socklen_t len;
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];
  int port;
  
  len = sizeof addr;
  getpeername(clifd, (struct sockaddr*)&addr, &len);
  

  if (addr.ss_family == AF_INET) 
  {
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
  } 
  else 
  { 
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
    port = ntohs(s->sin6_port);
    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
  }

  eLog.getstream(logLevel::DEFAULT, logType::ETH)
  << __func__ << ": got connection from: " <<  ipstr << ":" << port << endl; 


  
  connection = true;
  disconnect_flag = false;

  /*   signal receiver thread    */
  pthread_mutex_lock(&mutex_recv);
  pthread_cond_signal(&cond_recv);
  pthread_mutex_unlock(&mutex_recv);
  
  return connection;
}
#endif



#ifdef CLIENT

/**********************   threads   ***************************/

void* manager_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  
  while(true)
  {
    eth->manage_queue();
  }
  
  return 0;
}



void* send_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  eth->enable_send();
  return 0;
}


void* recv_thread(void* arg)
{
  if(!arg){exit(1);}
  
  remote* eth = (remote*)arg;
  
  while(true)
  {
    eth->enable_recv();
  }
  
  return 0;
}




void remote::init()
{
  if(0 != pthread_create(&eth_mngr, NULL, manager_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
  
  if(0 != pthread_create(&eth_send, NULL, send_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
  
  if(0 != pthread_create(&eth_recv, NULL, recv_thread, this))
  {
    cout << __func__ << ": pthread_create() failed" << endl;
    exit(EXIT_FAILURE);
  }
}



void remote::disconnect()
{
  /*  signal manager   */
  pthread_mutex_lock(&mutex_mngr);
  disconnect_flag = true;
  connection       = false;
  close(sockfd);
  pthread_cond_signal(&cond_mngr);
  pthread_mutex_unlock(&mutex_mngr);
  
  eLog.getstream(logLevel::DEFAULT, logType::ETH)
  << __func__ << ": server disconnected..." << endl; 
}



/*   get sockaddr, IPv4 or IPv6   */
void* get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


bool remote::accept_client()
{ return true; }

remote::remote(const char* server, int bLen): port("3490")
{
  send_flag         = false;
  connection       = false;
  disconnect_flag  = false;
  bufferLen = bLen;
  
  pthread_cond_init (&cond_mngr, NULL);
  pthread_mutex_init(&mutex_mngr, NULL);
  
  pthread_cond_init (&cond_send, NULL);
  pthread_mutex_init(&mutex_send, NULL);
  
  pthread_cond_init (&cond_recv, NULL);
  pthread_mutex_init(&mutex_recv, NULL);
  
  eLog.getstream(logLevel::DEFAULT, logType::ETH)
  << delim << "---------------------------------- remote::remotel():session started... ----------------------------------" << endl; 
  
  memset(&hints, 0, sizeof hints); 
  hints.ai_family = AF_UNSPEC;  
  hints.ai_socktype = SOCK_STREAM;
  
  if (0 != (rv = getaddrinfo(server, port.c_str(), &hints, &servinfo))) 
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }
  
  for(p = servinfo; p != NULL; p = p->ai_next) 
  {
    if (-1 == (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)))
    {
      perror("client: socket");
      continue;
    }
    
    if (-1 == connect(sockfd, p->ai_addr, p->ai_addrlen)) 
    {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    break;
  }
  
  if (p == NULL) 
  {
    cout << __func__ <<  ": failed to bind" << endl;
    exit(1);
  }
  connection = true;
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  freeaddrinfo(servinfo); 
}

#endif
