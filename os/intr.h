#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

/* defines software interrupt vector */

#define SOFTVEC_TYPE_NUM 3

/*** index of trapa instrunction ***/
#define SOFTVEC_TYPE_SOFTERR 0
#define SOFTVEC_TYPE_SYSCALL 1
#define SOFTVEC_TYPE_SERINTR 2 


#endif
