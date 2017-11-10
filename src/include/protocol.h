#include <cstring>
#include <cstdlib>
#include <iostream>

#include <fstream>

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

/*
 * 
 */ 


namespace csys
{
  class prot
  {
  public:
    struct type{enum unit_type{AI, AO, DI, DO, SR};};
    struct com{enum command{IN, RD, WR};};
    
    prot();
    void validate();

  };
}

#endif