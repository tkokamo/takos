#include "defines.h"
#include "takos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6 
#define THREAD_NAME_SIZE 15

/*** thread context ***/
typedef struct _tk_context {
  uint32 sp; 
} tk_context;

/*** Task Control Block (TCB) ***/
typedef struct _tk_thread {
  struct _tk_thread *next;
  char name[THREAD_NAME_SIZE + 1];
  char *stack; //top address of stack pointer

  struct { //parameters to thread_init()
    tk_func_t func; //main function of a thread
    int argc; 
    char **argv;
  } init;

  struct { // buffer for syscall
    tk_syscall_type_t type;
    tk_syscall_param_t *param;
  } syscall;

  tk_context context;
} tk_thread;

/*** ready queue ***/
static struct {
  tk_thread *head;
  tk_thread *tail;
} readyque;

static tk_thread *current;
static tk_thread threads[THREAD_NUM];
static tk_handler_t handlers[SOFTVEC_TYPE_NUM];

void dispatch(tk_context *context);

/*** pull current thread from the ready queue ***/
static int getcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  /*** pull current thread from the head of the ready queue ***/
  readyque.head = current->next;
  if (readyque.head == NULL) {
    readyque.tail = NULL;
  }
  current->next = NULL;
  
  return 0;
}

/*** push current thread to the tail of the ready queue ***/
static int putcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  if (readyque.tail) {
    readyque.tail->next = current;
  } else {
    readyque.head = current;
  }
  readyque.tail = current;
  
  return 0;
}

static void thread_end(void)
{
  tk_exit(); //call kz syscall
}

/*** start up thread ***/
static void thread_init(tk_thread *thp)
{
  /*** call main function of the thread ***/
  thp->init.func(thp->init.argc, thp->init.argv);
  thread_end();
}

/*** syscall: tk_run() ***/
static tk_thread_id_t thread_run(tk_func_t func, char *name, int stacksize, int argc, char *argv[])
{
  int i;
  tk_thread *thp;
  uint32 *sp;
  extern char userstack; //user stack field defined in ld.scr
  static char *thread_stack = &userstack;

  /*** search unused TCB ***/
  for (i = 0; i < THREAD_NUM; i++) {
    thp = &threads[i];
    if (!thp->init.func) { //get it!
      break;
    } 
  }
  if (i == THREAD_NUM) 
    return -1;
  
  memset(thp, 0, sizeof(thp)); //zero clear

  /*** setup TCB ***/
  strcpy(thp->name, name);
  thp->next = NULL;
  
  thp->init.func = func;
  thp->init.argc = argc;
  thp->init.argv = argv;

  /*** allocate stack ***/
  memset(thread_stack, 0, stacksize);
  thread_stack += stacksize; //update userstack;
  
  thp->stack = thread_stack;

  /*** init stack ***/
  sp = (uint32 *)thp->stack;
  *(--sp) = (uint32)thread_end; 

  /*** set program counter ***/
  *(--sp) = (uint32)thread_init; // set to pc when the thread is dispatched
  
  *(--sp) = 0; //ER6
  *(--sp) = 0; //ER5
  *(--sp) = 0; //ER4
  *(--sp) = 0; //ER3
  *(--sp) = 0; //ER2
  *(--sp) = 0; //ER1
	  
  /*** params to thread_init ***/
  *(--sp) = (uint32)thp;

  /*** ??? ***/
  thp->context.sp = (uint32)sp;

  /*** put thread calling syscall to the tail of the ready queue ***/
  putcurrent();

  /*** put created thread to the tail of the ready queue ***/
  current = thp;
  putcurrent();

  return (tk_thread_id_t) current;
}

/*** syscall: tk_exit ***/
static int thread_exit(void)
{
  /*************************************
   * primarily, free stack and enable to reuse
   *
   *************************************/
  puts(current->name);
  puts(" EXIT.\n");
  memset(current, 0, sizeof(*current));
  return 0; 
}

/*** register interrupt handler ***/
static int setintr(softvec_type_t type, tk_handler_t handler)
{
  static void thread_intr(softvec_type_t type, unsigned long sp);

  /*** set os handler ***/
  softvec_setintr(type, thread_intr);

  handlers[type] = handler;

  return 0;
}

/*** call system call ***/
static void call_functions(tk_syscall_type_t type, tk_syscall_param_t *p)
{
  switch (type) {
  case TK_SYSCALL_TYPE_RUN: /*tk_run*/
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name, p->un.run.stacksize, p->un.run.argc, p->un.run.argv); //call tk_run
    break;
  case TK_SYSCALL_TYPE_EXIT: /*tk_exit*/
    thread_exit(); // do not write return value not to delete TCB
    break;
  default:
    break;
  }
}

/*** processing system call ***/
static void syscall_proc(tk_syscall_type_t type, tk_syscall_param_t *p)
{
  getcurrent(); //remove the thread that called syscall_proc from the ready queue
  call_functions(type, p);
}

/*** thread scheduling ***/
static void schedule(void)
{
  if (!readyque.head)
    tk_sysdown();

  current = readyque.head;
}

/*** call system call from the current thread ***/
static void syscall_intr(void)
{
  syscall_proc(current->syscall.type, current->syscall.param);
}

/*** software error occurrs ***/
static void softerr_intr(void)
{
  puts("current->name");
  puts(" DOWN\n");
  getcurrent();
  thread_exit(); //force quit
}

/*** interrupt entrance for thread ***/
static void thread_intr(softvec_type_t type, unsigned long sp)
{
  /*** store current thread context ***/
  current->context.sp = sp;

  if (handlers[type])
    handlers[type]();

  schedule(); /*thread scheduling*/

  dispatch(&current->context);

  /*** never return here ***/
}

/*** run initial thread ***/
void tk_start(tk_func_t func, char *name, int stacksize, int argc, char *argv[])
{
  /*** init current ***/
  current = NULL;

  readyque.head = readyque.tail = NULL;
  memset(threads, 0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));
\
  /*** register interrupt handlers ***/
  setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
  setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

  /*** call function directly because this function can not call system calls ***/
  current = (tk_thread *)thread_run(func, name, stacksize, argc, argv);

  /*** dispatch ***/
  dispatch(&current->context);

  /*** never return here ***/
}

void tk_sysdown(void)
{
  puts("system error!\n");
  while(1)
    ;
}

/*** library function for calling syscall ***/
void tk_syscall(tk_syscall_type_t type, tk_syscall_param_t *param)
{
  current->syscall.type = type;
  current->syscall.param = param;
  asm volatile("trapa #0"); //issue trap interruption
}
