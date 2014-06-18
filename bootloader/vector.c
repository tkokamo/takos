#include "defines.h"

extern void start(void);	/*start up*/
extern void intr_softerr(void); /*software error*/
extern void intr_syscall(void); /*system call*/
extern void intr_serintr(void); /*serial interrupt*/

/*
 *	interrupt vecrot
 *	
 */
void (*vectors[])(void) = {
 start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 intr_syscall, intr_softerr, intr_softerr, intr_softerr,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
 intr_serintr, intr_serintr, intr_serintr, intr_serintr,
 intr_serintr, intr_serintr, intr_serintr, intr_serintr, 
 intr_serintr, intr_serintr, intr_serintr, intr_serintr,
};
