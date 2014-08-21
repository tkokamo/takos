#include "defines.h"
#include "takos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6 
#define THREAD_NAME_SIZE 15
#define PRIORITY_NUM 16

/*** thread context ***/
typedef struct _tk_context {
  uint32 sp; 
} tk_context;

/*** Task Control Block (TCB) ***/
typedef struct _tk_thread {
  struct _tk_thread *next;
  char name[THREAD_NAME_SIZE + 1];
  char *stack; //top address of stack pointer
  int priority;
  uint32 flags; 
#define TK_THREAD_FLAG_READY (1 << 0)


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
} readyque[PRIORITY_NUM]; //ready queue for every priority

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

  /*** if current thread is not ready, do nothing ***/
  if (!(current->flags & TK_THREAD_FLAG_READY)) {
    return 1;
  }

  /*** pull current thread from the head of the ready queue ***/
  readyque[current->priority].head = current->next;
  if (readyque[current->priority].head == NULL) {
    readyque[current->priority].tail = NULL;
  }
  current->flags &= ~TK_THREAD_FLAG_READY; //disable ready flag because current thread is not in the ready queue any more
  current->next = NULL;
  
  return 0;
}

/*** push current thread to the tail of the ready queue ***/
static int putcurrent(void)
{
  if (current == NULL) {
    return -1;
  }
  
  /*** if current thread is ready, do nothing ***/
  if (current->flags & TK_THREAD_FLAG_READY) {
    return 1;
  }

  if (readyque[current->priority].tail) {
    readyque[current->priority].tail->next = current;
  } else {
    readyque[current->priority].head = current;
  }
  current->flags |= TK_THREAD_FLAG_READY; //enable ready flag
  readyque[current->priority].tail = current;
  
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
static tk_thread_id_t thread_run(tk_func_t func, char *name, int priority, int stacksize, int argc, char *argv[])
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
  thp->priority = priority;
  thp->flags = 0;

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

  /*** set program counter. if the priority equals to 0, forbid interruption***/
  *(--sp) = (uint32)thread_init | ((uint32)(priority ? 0 : 0xc0) << 24); // set to pc when the thread is dispatched
  
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

/*** syscall: tk_wait ***/
static int thread_wait(void)
{
  putcurrent();
  return 0;
}

/*** syscall: tk_sleep ***/
static int thread_sleep(void)
{
  return 0; //thread remains out of ready queue 
}

/*** syscall: tk_wakeup ***/
static int thread_wakeup(tk_thread_id_t id)
{
  /*** put the thread calling thread_wakeup to the tail of ready queue ***/
  putcurrent();

  /*** put the specified thread by t to the tail of ready queue and wake up ***/
  current = (tk_thread *)id; 
  putcurrent();

  return 0;

}

/*** syscall: tk_getid ***/
static tk_thread_id_t thread_getid(void)
{
  putcurrent();
  return (tk_thread_id_t) current; //TCB address is thread ID
}

/*** syscall: tk_chpri ***/
static int thread_chpri(int priority)
{
  int old = current->priority;
  if (priority >= 0)
    current->priority = priority; //change priority
  putcurrent(); //put the thread to new ready queue
  return old;
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
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name, p->un.run.priority, p->un.run.stacksize, p->un.run.argc, p->un.run.argv); //call tk_run
    break;
  case TK_SYSCALL_TYPE_EXIT: /*tk_exit*/
    thread_exit(); // do not write return value not to delete TCB
    break;
  case TK_SYSCALL_TYPE_WAIT: /*tk_wait*/
    p->un.wait.ret = thread_wait();
    break;
  case TK_SYSCALL_TYPE_SLEEP: /*tk_sleep*/
    p->un.sleep.ret = thread_sleep();
    break;
  case TK_SYSCALL_TYPE_WAKEUP: /*tk_wakeup*/
    p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
    break;
  case TK_SYSCALL_TYPE_GETID: /*tk_getid*/
    p->un.getid.ret = thread_getid();
    break;
  case TK_SYSCALL_TYPE_CHPRI: /*tk_chpri*/
    p->un.chpri.ret = thread_chpri(p->un.chpri.priority);
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
  int i;
  
  /*** search ready thread ***/
  for (i = 0; i < PRIORITY_NUM; i++) {
    if (readyque[i].head)
      break;
  }
  if (i == PRIORITY_NUM)
    tk_sysdown();
  current = readyque[i].head;
}

/*** call system call from the current thread ***/
static void syscall_intr(void)
{
  syscall_proc(current->syscall.type, current->syscall.param);
}

/*** software error occurrs ***/
static void softerr_intr(void)
{
  puts(current->name);
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
void tk_start(tk_func_t func, char *name, int priority, int stacksize, int argc, char *argv[])
{
  /*** init current ***/
  current = NULL;

  memset(readyque, 0, sizeof(readyque));
  memset(threads, 0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));

  /*** register interrupt handlers ***/
  setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
  setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

  /*** call function directly because this function can not call system calls ***/
  current = (tk_thread *)thread_run(func, name, priority, stacksize, argc, argv);

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
