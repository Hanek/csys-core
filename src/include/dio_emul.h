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

#include <cstdlib>

#ifndef _DIO_EMUL_H
#define _DIO_EMUL_H


namespace csys
{
#ifdef CLIENT
  class window;
#endif
  
class dioEmul: public device
{
private:


  
public:
  dioEmul();
  ~dioEmul();
  
  bool get_di_bit(int pos) const;
  void set_do_bit(int pos);
  void rset_do_bit(int pos);
};


}

#endif





