/*
 * queue requirements:
 * - keep bytes array of random length;
 * - allocate memory automatically;
 */ 


#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>

#include <fstream>

#ifndef _QUEUE_H
#define _QUEUE_H

namespace csys
{
  
  class queue
  {
    
  private:
    class node
    {
    public:
      char* buf_;
      int len_;
      node* next_;
      node(const char* buf, int len)
      {
        buf_ = new(std::nothrow) char[len];
        if(!buf)
        {
        }
        memcpy(buf_, buf, len);
        len_  = len;
        next_ = NULL;
      }
    };
    
    node* head_;
        
  public:    
    queue()
    {
      head_ = NULL;
    }
    
    void push(const char* buf, int len)
    {
      node* c;
      node* n = head_;
      node* t = new(std::nothrow) node(buf, len);
      if(!n)
      {
      }
      
      if(!head_)
      {
        head_ = t;
        return;
      }
      
      while(n)
      {
        c = n;
        n = n->next_;
      }       
      c->next_ = t;
    }
    
    void pop(char** buf, int& len)
    {
      if(!head_)
      {
        *buf = NULL;
        len = 0;
        return;
      }
      
      *buf = head_->buf_;
      len = head_->len_;
      node* t = head_;
      head_ = head_->next_;
      delete t;
    }
    
    void release_mem(char* buf)
    {
      delete [] buf;
    }
    
    void dump()
    { 
      node* t = head_;
      std::ofstream file("que.bin", std::ios::out | std::ios::binary);
      while(t)
      {
        file.write("node:", 5);
        file.write(t->buf_, t->len_);
        t = t->next_;
      }
      
      file.close();
    }
    
  };
}
#endif

