#include <iostream>
#include "emulator.h"

using namespace std;

ioEmul::ioEmul(): shmKey(123456), shmSize(1024)
{
  /* Allocate a shared memory segment. */
  shmId = shmget(shmKey, shmSize, IPC_CREAT | S_IRUSR | S_IWUSR);

  /* Attach the shared memory segment. */
  shmAddr = (char*)shmat(shmId, 0, 0);
   
  cout << __func__ << ": shared memory attached at: " << (int*)shmAddr << endl;
}



ioEmul::~ioEmul()
{
  /* Detach the shared memory segment. */
  shmdt(shmAddr);
  /* Deallocate the shared memory segment.*/
  shmctl (shmId, IPC_RMID, 0);
  
}
