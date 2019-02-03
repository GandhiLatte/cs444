#include "syscalls.h"
#include "io.h"

int
fork1(void)
{
  int pid = fork();
  if (pid == -1)
    fatal("fork failed! out of memory?\n");
  return pid;
}

