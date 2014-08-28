#include "defines.h"
#include "takos.h"
#include "lib.h"

int test11_1_main(int argc, char *argv[])
{
  char *p;
  int size;

  puts("test11_1 started\n");

  /*recv in static*/
  puts("test11_1 recv in.\n");
  tk_recv(MSGBOX_ID_MSGBOX1, &size, &p); /*recv*/
  puts("test11_1 recv out.\n");
  puts(p);

  /*recv dynamic msg */
  puts("test11_1 recv in.\n");
  tk_recv(MSGBOX_ID_MSGBOX1, &size, &p);
  puts("test11_1 recv out.\n");
  puts(p);
  tk_free(p);

  /*send static msg*/
  puts("test11_1 send in.\n");
  tk_send(MSGBOX_ID_MSGBOX2, 15, "static memory\n");
  puts("test11_1 send out.\n");

  /*send dynamic msg*/
  p = tk_malloc(18);
  strcpy(p, "allocated memory\n");
  puts("test11_1 send in.\n");
  tk_send(MSGBOX_ID_MSGBOX2, 18, p);
  puts("test11_1 send out.\n");

  puts("test11_1 exit.\n");

  return 0;
}
