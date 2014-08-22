#include "defines.h"
#include "takos.h"
#include "lib.h"
#include "memory.h"

/*** header located top of memory block field ***/
typedef struct _tkmem_block {
  struct _tkmem_block *next;
  int size;
} tkmem_block;

/*** memory pool ***/
typedef struct _tkmem_pool {
  int size;
  int num;
  tkmem_block *free;
} tkmem_pool;

/*** define memory pool ***/
static tkmem_pool pool[] = {
  {16, 8, NULL}, {32, 8, NULL}, {64, 8, NULL},
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

/*** init memory pool ***/
static int tkmem_init_pool(tkmem_pool *p)
{
  int i;
  tkmem_block *mp;
  tkmem_block **mpp;
  extern char freearea;
  static char *area = &freearea;

  mp = (tkmem_block *)area;

  /*** put all field to free list ***/
  mpp = &p->free;
  for (i = 0; i < p->num; i++) {
    *mpp = mp;
    memset(mp, 0, sizeof(*mp));
    mp->size = p->size;
    mpp = &(mp->next);
    mp = (tkmem_block *)((char *)mp + p->size);
    area += p->size;
  }

  return 0;
}

/*** init dynamic mem ***/
int tkmem_init(void)
{
  int i;
  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    tkmem_init_pool(&pool[i]);
  }
  
  return 0;
}

/*** alloc ***/
void *tkmem_alloc(int size)
{
  int i;
  tkmem_block *mp;
  tkmem_pool *p;

  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    
    /*** size check ***/
    if (size <= p->size - sizeof(tkmem_block)) { 
      if (p->free == NULL) {
	/*** there are no free space ***/
	tk_sysdown();
	return NULL;
      }

      /*** allocate freed list ***/
      mp = p->free;
      p->free = p->free->next;
      mp->next = NULL;

      /*** return usable field pointer ***/
      return mp + 1;
    }
  }

  /*** size is greater than the allowed sizes ***/
  tk_sysdown();
  return NULL;
}

/*** free ***/
void tkmem_free(void *mem)
{
  int i;
  tkmem_block *mp;
  tkmem_pool *p;

  /*** get memory block header (should) exists at mem - 1 ***/
  mp = ((tkmem_block *) mem - 1);
  
  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    if (mp->size == p->size) {
      mp->next = p->free;
      p->free = mp;
      return;
    }
  }

  /*** something wrong ***/
  tk_sysdown();
}
