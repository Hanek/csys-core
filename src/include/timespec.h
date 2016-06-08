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


#include <iostream>

#ifndef _TIMESPEC_H
#define _TIMESPEC_H

namespace csys
{

/*   wrapper for struct timespec     */
class time
{
public:
  static const long nsec10L9;
  static const long nsec10L8;
  static const long nsec10L7;
  static const long nsec10L6;
  
  static const std::pair<long int, long int> zero;
  static const std::pair<long int, long int> msSec10;
  static const std::pair<long int, long int> msSec20;
  static const std::pair<long int, long int> msSec30;
  static const std::pair<long int, long int> msSec40;
  static const std::pair<long int, long int> msSec50;
  
  static const std::pair<long int, long int> msSec100;
  static const std::pair<long int, long int> msSec150;
  static const std::pair<long int, long int> msSec200;
  static const std::pair<long int, long int> msSec300;
  static const std::pair<long int, long int> msSec400;
  static const std::pair<long int, long int> msSec500;
  static const std::pair<long int, long int> msSec600;
  static const std::pair<long int, long int> msSec700;
  static const std::pair<long int, long int> msSec800;
  static const std::pair<long int, long int> msSec900;
  
  static const std::pair<long int, long int> Sec1;
  static const std::pair<long int, long int> Sec2;
  static const std::pair<long int, long int> Sec3;
  static const std::pair<long int, long int> Sec4;
  static const std::pair<long int, long int> Sec5;
  
  static const std::pair<long int, long int> Sec10;
  static const std::pair<long int, long int> Sec20;
  static const std::pair<long int, long int> Sec30;
  static const std::pair<long int, long int> Sec40;
  
  static const std::pair<long int, long int> Sec180;
  static const std::pair<long int, long int> Sec600;

private:
  struct timespec t;

public:
  /*  define display formatting  */
  bool hi_res; 
 
  time();
  time(bool res);
  time(long int sec, long int nsec);
  time(std::pair<long int, long int>);

  struct timespec& get() { return t; }
  const struct timespec& get() const { return t; }
  void set(long int sec, long int nsec);
  void reset();

  bool operator == (const time& rhs);
  bool operator != (const time& rhs) { return !operator == (rhs); }
  bool operator <  (const time& rhs);
  bool operator >  (const time& rhs);

  bool operator == (const time& rhs) const;
  bool operator != (const time& rhs) const { return !operator == (rhs); }
  bool operator <  (const time& rhs) const;
  bool operator >  (const time& rhs) const;
  
  time& operator = (const time& rhs);
  time operator + (const time& rhs) const;
  time operator - (const time& rhs) const;
};

std::ostream& operator << (std::ostream& os, const time obj);
void set_timestamp(const time& time, std::ostream& os);

}
#endif




