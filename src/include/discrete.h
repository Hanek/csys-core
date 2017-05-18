#include <iostream>
#include <string.h>
#include "dev.h"

#ifndef _DISCRETE_H
#define _DISCRETE_H


namespace csys
{
  class dI : public dev
  {
  public:
    struct err{enum error{NOERR, HWERR};};
    struct cmd{enum command {NOCMD};};
        
  private:
    struct data
    {
      char error_;
      bool state_;
      data(): error_(err::NOERR), state_(false) {}
      bool operator == (const data& rhs)
            {
              if(error_ != rhs.error_ || state_ != rhs.state_)
                {return false;}
              return true;
            }
            bool operator != (const data& rhs) {return !operator == (rhs);} 
          
    };
    
  private:
    data cs_;
    data ps_;
    
    /* input bit position on hw stack */ 
    int pos_;             
    
    void error_handler();
    void request_handler();
  
  public:
    dI(const char *lbl, int position);
    ~dI();
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == cs_.error_) ? true : false ; }
    void process();
  };
  
  class dO
  {
  public:
    struct err{enum error{NOERR, HWERR};};
    struct cmd{enum command {NOCMD, SET, UNSET};};
    
  private:
    struct data
    {
      char error_;
      bool state_;
      data(): error_(err::NOERR), state_(false) {}
      bool operator == (const data& rhs)
            {
              if(error_ != rhs.error_ || state_ != rhs.state_)
                {return false;}
              return true;
            }
            bool operator != (const data& rhs) {return !operator == (rhs);} 
          
    };
  
  private:
    data cs_;
    data ps_;
    
    /* output bit position on hw stack */ 
    int pos_;             
    
    void error_handler();
    void request_handler();
  
  public:
    dO(const char* lbl, int position);
    ~dO();
    void serialize();
    void unserialize();
    bool is_valid() { return (err::NOERR == cs_.error_) ? true : false ; }
    void process();
  };
};
#endif






