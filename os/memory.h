#ifndef _TAKOS_MEMORY_H_INCLUDED_
#define _TAKOS_MEMORY_H_INCLUDED_

int tkmem_init(void); //init dynamic memory
void *tkmem_alloc(int size); //allocate memory
void tkmem_free(void *mem); //free memory


#endif
