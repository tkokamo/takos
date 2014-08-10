#include "defines.h"
#include "takos.h"
#include "syscall.h"

/*** system call ***/
tk_thread_id_t tk_run(tk_func_t func, char *name, int stacksize, int argc, char *argv[])
{
  tk_syscall_param_t param;
  param.un.run.func = func;
  param.un.run.name = name;
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
