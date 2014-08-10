#ifndef _TAKOS_SYSCALL_H_INCLUDED_
#define _TAKOS_SYSCALL_H_INCLUDED_

#include "defines.h"

/*** define the number of system call ***/
typedef enum {
  TK_SYSCALL_TYPE_RUN = 0,
  TK_SYSCALL_TYPE_EXIT,
} tk_syscall_type_t;

/*** define params for system call ***/
typedef struct {
  union {
    struct { //params for tk_run
      tk_func_t func;
      char *name;
      int stacksize;
      int argc;
      char **argv;
      tk_thread_id_t ret;
    } run;
    struct { //params for tk_exit
      int dummy;
    } exit;
  } un;
} tk_syscall_param_t;

#endif
