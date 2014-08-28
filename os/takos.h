#ifndef _TAKOS_H_INCLUDED_
#define _TAKOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

/*** system call ***/
tk_thread_id_t tk_run(tk_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]); //run thread
void tk_exit(void); //exit from thread
int tk_wait(void);
int tk_sleep(void);
int tk_wakeup(tk_thread_id_t id);
tk_thread_id_t tk_getid(void);
int tk_chpri(int priority);
void *tk_malloc(int size);
int tk_free(void *p);
int tk_send(tk_msgbox_id_t id, int size, char *p);
tk_thread_id_t tk_recv(tk_msgbox_id_t id, int *sizep, char **pp);


/*** library function ***/
void tk_start(tk_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]); //init thread
void tk_sysdown(void); //call when fatal error occurs
void tk_syscall(tk_syscall_type_t type, tk_syscall_param_t *param); // run syscall

/*** user thread ***/
int test11_1_main(int argc, char *argv[]);
int test11_2_main(int argc, char *argv[]);

#endif
