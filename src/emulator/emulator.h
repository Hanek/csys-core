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
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>


#ifndef _IO_EMUL_H
#define _IO_EMUL_H



/*
 *  run its own window 
 *  display/change io parameters
 *  read/write changes to shared memory
 */
  
class ioEmul
{
private:
  
  key_t shmKey;
  const int shmSize;;
  
  int shmId;
  char* shmAddr;
  char* pShm;
  int next[2]; 
  
public:
  ioEmul();
  ~ioEmul();
  
  bool get_di_bit(int pos) const;
  void tweak_do_bit(int pos, bool state);
};



#endif






