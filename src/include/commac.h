#ifndef _COMMAC_H
#define _COMMAC_H

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <stdio.h>
#include <string.h>

#define delim " "
#define UIPATH "../resources/"




inline std::string dbl_to_str(double x, int prec) 
{
  std::ostringstream o;

  if(prec <= 0)
  { o << std::fixed; }
  else
  { o << std::setprecision(prec); }

  if (!(o << x))
  { o << "0"; }
  
  return o.str();
}

inline std::string int_to_str(int x)
{
  std::ostringstream o;
  if (!(o << x))
  { o << "0"; }
  
  return o.str();
}

inline std::string uint_to_str(unsigned int x)
{
  std::ostringstream o;
  if (!(o << x))
  { o << "0"; }
  
  return o.str();
}



#ifdef CLIENT
inline std::string ntrans(const char* name, int state)
{
  int buf_len = 100;
  if(strlen(name) > buf_len - strlen(UIPATH) - 14)
  { return std::string(""); }
  
  char buf[buf_len];
  strcpy(buf, UIPATH);
  strcpy(buf + strlen(buf), name);
  
  switch(state)
  {
    case 0:
      strcpy(buf + strlen(buf), "_disabled.png");
      break;
      
    case 1:
      strcpy(buf + strlen(buf), "_changed.png");
      break;
      
    case 2:
      strcpy(buf + strlen(buf), "_enabled.png");
      break;
      
    case 3:
      strcpy(buf + strlen(buf), "_alarm.png");
      break;
      
  }
  
  return std::string(buf);
}
#endif


#endif



