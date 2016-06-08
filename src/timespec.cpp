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

#include "timespec.h"
#include <iomanip>
#include <cstring>

using namespace std;

const long csys::time::nsec10L9 = 1000000000;
const long csys::time::nsec10L8 = 100000000;
const long csys::time::nsec10L7 = 10000000;
const long csys::time::nsec10L6 = 1000000; 

const std::pair<long int, long int> csys::time::zero        = std::make_pair(0, 0);
const std::pair<long int, long int> csys::time::msSec10 = std::make_pair(0, 10000000);
const std::pair<long int, long int> csys::time::msSec20 = std::make_pair(0, 20000000);
const std::pair<long int, long int> csys::time::msSec30 = std::make_pair(0, 30000000);
const std::pair<long int, long int> csys::time::msSec40 = std::make_pair(0, 40000000);
const std::pair<long int, long int> csys::time::msSec50 = std::make_pair(0, 50000000);

const std::pair<long int, long int> csys::time::msSec100 = std::make_pair(0, 100000000);
const std::pair<long int, long int> csys::time::msSec150 = std::make_pair(0, 150000000);
const std::pair<long int, long int> csys::time::msSec200 = std::make_pair(0, 200000000);
const std::pair<long int, long int> csys::time::msSec300 = std::make_pair(0, 300000000);
const std::pair<long int, long int> csys::time::msSec400 = std::make_pair(0, 400000000);
const std::pair<long int, long int> csys::time::msSec500 = std::make_pair(0, 500000000);
const std::pair<long int, long int> csys::time::msSec600 = std::make_pair(0, 600000000);
const std::pair<long int, long int> csys::time::msSec700 = std::make_pair(0, 700000000);
const std::pair<long int, long int> csys::time::msSec800 = std::make_pair(0, 800000000);
const std::pair<long int, long int> csys::time::msSec900 = std::make_pair(0, 900000000);

const std::pair<long int, long int> csys::time::Sec1 = std::make_pair(1, 0);
const std::pair<long int, long int> csys::time::Sec2 = std::make_pair(2, 0);
const std::pair<long int, long int> csys::time::Sec3 = std::make_pair(3, 0);
const std::pair<long int, long int> csys::time::Sec4 = std::make_pair(4, 0);
const std::pair<long int, long int> csys::time::Sec5 = std::make_pair(5, 0);


const std::pair<long int, long int> csys::time::Sec10 = std::make_pair(10, 0);
const std::pair<long int, long int> csys::time::Sec20 = std::make_pair(20, 0);
const std::pair<long int, long int> csys::time::Sec30 = std::make_pair(30, 0);
const std::pair<long int, long int> csys::time::Sec40 = std::make_pair(40, 0);

const std::pair<long int, long int> csys::time::Sec180 = std::make_pair(180, 0); 
const std::pair<long int, long int> csys::time::Sec600 = std::make_pair(600, 0);

csys::time::time() 
{
  t.tv_sec     = 0;
  t.tv_nsec   = 0;
  hi_res        = true;
}
  

csys::time::time(bool res)
{
  t.tv_sec     = 0;
  t.tv_nsec   = 0;
  hi_res        = res;
}
 

csys::time::time(long int sec, long int nsec)
{
  t.tv_sec     = sec;
  t.tv_nsec   = nsec;
  hi_res        = true;
}

csys::time::time(std::pair<long int, long int> timeval)
{
  t.tv_sec     = timeval.first;
  t.tv_nsec   = timeval.second;
  hi_res        = true;
}


void csys::time::set(long int sec, long int nsec)
{
  t.tv_sec = sec;
  t.tv_nsec = nsec;
}


void csys::time::reset()
{
  t.tv_sec = 0;
  t.tv_nsec = 0;
}
 

csys::time& csys::time::operator = (const csys::time& rhs)
{
  t.tv_sec = rhs.t.tv_sec;
  t.tv_nsec = rhs.t.tv_nsec;
  return *this;
}





ostream& csys::operator << (ostream& os, const csys::time tm)
{
  int len = 0x32;
  char buffer[len];
  memset(buffer, '\0', len);
  
  switch (tm.hi_res)
  {
    case true:
      if(tm < time::zero)
      { os << "        -" << setw(1) << (-tm.get().tv_sec) - 1 << ":" << setw(9) << time::nsec10L9 - tm.get().tv_nsec; }
      else
      { os << setw(10) << tm.get().tv_sec << ":" << setw(9) << tm.get().tv_nsec; }
      break;
    
    case false:
      struct tm* timeinfo;
      timeinfo = localtime(&(tm.get().tv_sec));
      strftime (buffer, len, "%F %T", timeinfo);
      os << buffer;
      break;  
  }
  return os;
}


void csys::set_timestamp(const csys::time& tm, ostream& os)
{
  int len = 0x32;
  struct tm* timeinfo;  
  char buffer[len];
  memset(buffer, '\0', len);  
  
  timeinfo = localtime(&(tm.get().tv_sec)); 
  strftime (buffer, len, "%F %T  ", timeinfo); 
  
  os << buffer;
}




bool csys::time::operator == (const time& rhs)
{ return (t.tv_sec == rhs.t.tv_sec && t.tv_nsec == rhs.t.tv_nsec) ? true : false; }


bool csys::time::operator < (const time& rhs)
{
  if(t.tv_sec > rhs.t.tv_sec)
    return false;
  
  if(t.tv_sec < rhs.t.tv_sec)
    return true;
  
  if(t.tv_nsec >= rhs.t.tv_nsec)
    return false;

  return true;
}


bool csys::time::operator >  (const time& rhs)
{
  if(t.tv_sec > rhs.t.tv_sec)
    return true;
  
  if(t.tv_sec < rhs.t.tv_sec)
    return false;
  
  if(t.tv_nsec > rhs.t.tv_nsec)
    return true;
  
  return false;	
}


bool csys::time::operator == (const time& rhs) const
{ return (t.tv_sec == rhs.t.tv_sec && t.tv_nsec == rhs.t.tv_nsec) ? true : false; }


bool csys::time::operator < (const time& rhs) const
{
  if(t.tv_sec > rhs.t.tv_sec)
    return false;
  
  if(t.tv_sec < rhs.t.tv_sec)
    return true;
  
  if(t.tv_nsec >= rhs.t.tv_nsec)
    return false;
  
  return true;
}


bool csys::time::operator >  (const time& rhs) const
{
  if(t.tv_sec > rhs.t.tv_sec)
    return true;
  
  if(t.tv_sec < rhs.t.tv_sec)
    return false;
  
  if(t.tv_nsec > rhs.t.tv_nsec)
    return true;
  
  return false;
}   

csys::time csys::time::operator + (const csys::time& rhs) const
{
  csys::time sum(csys::time::zero);
  sum.t.tv_sec = t.tv_sec + rhs.t.tv_sec;
  sum.t.tv_nsec = t.tv_nsec + rhs.t.tv_nsec;
  
  if(nsec10L9 <= sum.t.tv_nsec)
  {
    sum.t.tv_sec++;
    sum.t.tv_nsec = sum.t.tv_nsec - nsec10L9;
  }
  return sum;
}


csys::time csys::time::operator - (const csys::time& rhs) const
{
  csys::time sub(csys::time::zero);
  sub.t.tv_sec = t.tv_sec - rhs.t.tv_sec;
  
  if(t.tv_nsec >= rhs.t.tv_nsec)
  {
    sub.t.tv_nsec = t.tv_nsec - rhs.t.tv_nsec;
  }
  else
  {
    sub.t.tv_sec--;
    sub.t.tv_nsec = nsec10L9 - rhs.t.tv_nsec + t.tv_nsec;
  }
  return sub;
}

 







