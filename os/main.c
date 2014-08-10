#include "defines.h"
#include "takos.h"
#include "interrupt.h"
#include "lib.h"

/*** run system task and user thread ***/
static int start_threads(int argc, char *argv[])
{
  tk_run(test08_1_main, "command", 0x100, 0, NULL);
  return 0;
}


int main(void) 
{
  INTR_DISABLE; /*disable interrupt*/

  puts("takos boot succeed!\n");

  /*** kernel thread ***/
  tk_start(start_threads, "start", 0x100, 0, NULL);
  /*** never return here ***/
  return 0;
}
