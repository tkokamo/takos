#include "defines.h"
#include "takos.h"
#include "lib.h"

int test09_1_main(int argc, char *argv[])
{
  puts("test09_1 started.\n");
  
  puts("test09_1 sleep in.\n");
  tk_sleep();
  puts("test09_1 sleep out.\n");

  puts("test09_1 chpri in.\n");
  tk_chpri(3);
  puts("test09_1 chpri out.\n");

  puts("test09_1 wait in.\n");
  tk_wait();
  puts("test09_1 wait out.\n");

  puts("test09_1 trap in.\n");
  asm volatile("trapa #1");
  puts("test09_1 trap out.\n");
  
  puts("test09_1 exit.\n");

  return 0;
}
