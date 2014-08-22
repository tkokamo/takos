#ifndef _TAKOS_SYSCALL_H_INCLUDED_
#define _TAKOS_SYSCALL_H_INCLUDED_

#include "defines.h"

/*** define the number of system call ***/
typedef enum {
  TK_SYSCALL_TYPE_RUN = 0,
  TK_SYSCALL_TYPE_EXIT,
  TK_SYSCALL_TYPE_WAIT,
  TK_SYSCALL_TYPE_SLEEP,
  TK_SYSCALL_TYPE_WAKEUP,
  TK_SYSCALL_TYPE_GETID,
  TK_SYSCALL_TYPE_CHPRI,
  TK_SYSCALL_TYPE_MALLOC,
  TK_SYSCALL_TYPE_FREE,
} tk_syscall_type_t;

/*** define params for system call ***/
typedef struct {
  union {
    struct { //params for tk_run
      tk_func_t func;
      char *name;
      int priority;
      int stacksize;
      int argc;
      char **argv;
      tk_thread_id_t ret;
    } run;
    struct { //params for tk_exit
      int dummy;
    } exit;
    struct {
      int ret;
    } wait;
    struct {
      int ret;
    } sleep;
    struct {
      tk_thread_id_t id;
      int ret;
    } wakeup;
    struct {
      tk_thread_id_t ret;
    } getid;
    struct {
      int priority;
      int ret;
    } chpri;
    struct {
      int size;
      void *ret;
    } malloc;
    struct {
      char *p;
      int ret;
    } free;
  } un;
} tk_syscall_param_t;

#endif
