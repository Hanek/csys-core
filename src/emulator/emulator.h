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



// #include <stdio.h>
// #include <string.h>
// #include <sys/shm.h>
// #include <sys/stat.h>
// 
// int main ()
// {
//   key_t shm_key = 6166529;
//   const int shm_size = 1024;
// 
//   int shm_id;
//   char* shmaddr, *ptr;
//   int next[2];
// 
//   printf ("writer started.\n");
// 
//   /* Allocate a shared memory segment. */
//   shm_id = shmget (shm_key, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR);
// 
//   /* Attach the shared memory segment. */
//   shmaddr = (char*) shmat (shm_id, 0, 0);
// 
//   printf ("shared memory attached at address %p\n", shmaddr);
// 
//   /* Start to write data. */
//   ptr = shmaddr + sizeof (next);
//   next[0] = sprintf (ptr, "mandy") + 1;
//   ptr += next[0];
//   next[1] = sprintf (ptr, "73453916") + 1;
//   ptr += next[1];
//   sprintf (ptr, "amarica");
//   memcpy(shmaddr, &next, sizeof (next));
//   printf ("writer ended.\n");
// 
//   /*calling the other process*/
//   system("./read");
// 
//   /* Detach the shared memory segment. */
//   shmdt (shmaddr);
//   /* Deallocate the shared memory segment.*/
//   shmctl (shm_id, IPC_RMID, 0);
// 
//   return 0;
// }

// #include <stdio.h>
// #include <sys/shm.h>
// #include <sys/stat.h>
// 
// int main ()
// {
//   key_t shm_key = 6166529;
//   const int shm_size = 1024;
// 
//   int shm_id;
//   char* shmaddr, *ptr;
//   char* shared_memory[3];
//   int *p;
// 
//   /* Allocate a shared memory segment. */
//   shm_id = shmget (shm_key, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR);
// 
//   /* Attach the shared memory segment. */
//   shmaddr = (char*) shmat (shm_id, 0, 0);
// 
//   printf ("shared memory attached at address %p\n", shmaddr);
// 
//   /* Start to read data. */
//   p = (int *)shmaddr;
//   ptr = shmaddr + sizeof (int) * 2;
//   shared_memory[0] = ptr;
//   ptr += *p++;
//   shared_memory[1] = ptr;
//   ptr += *p;
//   shared_memory[2] = ptr;
//   printf ("0=%s\n", shared_memory[0]);
//   printf ("1=%s\n", shared_memory[1]);
//   printf ("2=%s\n", shared_memory[2]);
// 
//   /* Detach the shared memory segment. */
//   shmdt (shmaddr);
//   return 0;
// }




