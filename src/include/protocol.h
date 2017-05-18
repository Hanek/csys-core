#include <cstring>
#include <cstdlib>
#include <iostream>

#include <fstream>

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

/*
 * 
 * 
 * 
 * 
 * required alphabet:
 * 
 * ai
 * ao
 * di
 * do
 * 
 * must follow web-socket standarts
 * 
 * human interface <-> server message sample:
 * 
 * 
 * server <-> real-time core message sample:
 * kn1 ao3=2343 ao1=3434 ao2=343 do0=0 do1=1 di0=1 serial1=raise power  
 * 
 * server core and its layer runs the same real time loop, interchanging
 * messages through either fifo or socket, following next rules:
 * 
 * core -> layer
 * - whenever device properies change(definign its internal structure dd) 
 * - client registred
 * 
 * core <- layer
 * - fromntend issued command
 * 
 * there is no need in any additional abstraction for core <-> layer communication,
 * since its seems to be simple data translation from core to layer, so we must have exactly the same
 * set of data in layer app. So there should be no requests or responses.
 */ 


namespace csys
{
  class protocol
  {
    void arrange(const char*)
    {
      /* accept arguments list and convert to message */
      
    }
    

  };
}

#endif