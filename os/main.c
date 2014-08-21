#include "defines.h"
#include "takos.h"
#include "interrupt.h"
#include "lib.h"

tk_thread_id_t test09_1_id;
tk_thread_id_t test09_2_id;
tk_thread_id_t test09_3_id;

/*** run system task and user thread ***/
static int start_threads(int argc, char *argv[])
{
  test09_1_id = tk_run(test09_1_main, "test09_1", 1, 0x100, 0, NULL);
  test09_2_id = tk_run(test09_2_main, "test09_2", 2, 0x100, 0, NULL);
  test09_3_id = tk_run(test09_3_main, "test09_3", 3, 0x100, 0, NULL);
  
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

  /*** kernel thread ***/
  tk_start(start_threads, "idle", 0, 0x100, 0, NULL);
  /*** never return here ***/
  return 0;
}
