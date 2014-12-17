#include "defines.h"
#include "takos.h"
#include "interrupt.h"
#include "syscall.h"

/*** system call ***/
tk_thread_id_t tk_run(tk_func_t func, char *name, int priority, int stacksize, int argc, char *argv[])
{
  tk_syscall_param_t param;
  param.un.run.func = func;
  param.un.run.name = name;
  param.un.run.priority = priority;
  param.un.run.stacksize = stacksize;
  param.un.run.argc = argc;
  param.un.run.argv = argv;
  tk_syscall(TK_SYSCALL_TYPE_RUN, &param);
  return param.un.run.ret;
}


void tk_exit(void)
{
  tk_syscall(TK_SYSCALL_TYPE_EXIT, NULL);
}

int tk_wait(void)
{
  tk_syscall_param_t param;
  tk_syscall(TK_SYSCALL_TYPE_WAIT, &param);
  return param.un.wait.ret;
}

int tk_sleep(void)
{
  tk_syscall_param_t param;
  tk_syscall(TK_SYSCALL_TYPE_SLEEP, &param);
  return param.un.sleep.ret;
}

int tk_wakeup(tk_thread_id_t id)
{
  tk_syscall_param_t param;
  param.un.wakeup.id = id;
  tk_syscall(TK_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

tk_thread_id_t tk_getid(void)
{
  tk_syscall_param_t param;
  tk_syscall(TK_SYSCALL_TYPE_GETID, &param);
  return param.un.getid.ret;
}

int tk_chpri(int priority)
{
  tk_syscall_param_t param;
  param.un.chpri.priority = priority;
  tk_syscall(TK_SYSCALL_TYPE_CHPRI, &param);
  return param.un.chpri.ret;
}

void *tk_malloc(int size)
{
  tk_syscall_param_t param;
  param.un.malloc.size = size;
  tk_syscall(TK_SYSCALL_TYPE_MALLOC, &param);
  return param.un.malloc.ret;
}

int tk_free(void *p)
{
  tk_syscall_param_t param;
  param.un.free.p = p;
  tk_syscall(TK_SYSCALL_TYPE_FREE, &param);
  return param.un.free.ret;
}

int tk_send(tk_msgbox_id_t id, int size, char *p)
{
  tk_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  tk_syscall(TK_SYSCALL_TYPE_SEND, &param);
  return param.un.send.ret;
}

tk_thread_id_t tk_recv(tk_msgbox_id_t id, int *sizep, char **pp)
{
  tk_syscall_param_t param;
  param.un.recv.id = id;
  param.un.recv.sizep = sizep;
  param.un.recv.pp = pp;
  tk_syscall(TK_SYSCALL_TYPE_RECV, &param);
  return param.un.recv.ret;
}

int tk_setintr(softvec_type_t type, tk_handler_t handler)
{
  tk_syscall_param_t param;
  param.un.setintr.type = type;
  param.un.setintr.handler = handler;
  tk_syscall(TK_SYSCALL_TYPE_SETINTR, &param);
  return param.un.setintr.ret;
}

/*** service calls ***/
int tk_wakeup(tk_thread_id_t id)
{
  tk_syscall_param_t param;
  param.un.wakeup.id = id;
  tk_syscall(TK_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

void *tx_malloc(int size)
{
  tk_syscall_param_t param;
  param.un.malloc.size = size;
  tk_srvcall(TK_SYSCALL_TYPE_MALLOC, &param);
  return param.un.malloc.ret;
}

int tx_free(void *p)
{
  tk_syscall_param_t param;
  param.un.free.p = p;
  tk_srvcall(TK_SYSCALL_TYPE_FREE, &param);
  return param.un.free.ret;
}

int tx_send(tk_msgbox_id_t id, int size, char *p)
{
  tk_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  tk_srvcall(TK_SYSCALL_TYPE_SEND, &param);
  return param.un.send.free;
}
