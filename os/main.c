#include "defines.h"
#include "takos.h"
#include "interrupt.h"
#include "lib.h"


/*** run system task and user thread ***/
static int start_threads(int argc, char *argv[])
{

  tk_run(test11_1_main, "test11_1", 1, 0x100, 0, NULL);
  tk_run(test11_2_main, "test11_2", 2, 0x100, 0, NULL);
  
  /*** change priority to 15 and become idle thread ***/
  tk_chpri(15);
  INTR_ENABLE;
  while (1) {
    asm volatile ("sleep");
  }
  return 0;
}


int main(void) 
{
  INTR_DISABLE; /*disable interrupt*/

  puts("takos boot succeed!\n");
  /*** LED chika! at port1 ***/
//  unsigned char *p1;
  // p1 = 0xfffd2;
  //*(unsigned char *)0xee002 = 0xff;
  //*p1 = 0x00; //off
  //  asm volatile("sleep");

  /*** kernel thread ***/
  tk_start(start_threads, "idle", 0, 0x100, 0, NULL);
  /*** never return here ***/
  return 0;
}
