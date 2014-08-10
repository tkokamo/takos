#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

#define NULL ((void *) 0) /*define of null pointer*/
#define SERIAL_DEFAULT_DEVICE 1

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

typedef uint32 tk_thread_id_t; //thread id
typedef int (*tk_func_t)(int argc, char *argv[]); //main function for thread
typedef void (*tk_handler_t)(void); //intr handler

#endif
