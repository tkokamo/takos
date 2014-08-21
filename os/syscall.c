#include "defines.h"
#include "takos.h"
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

