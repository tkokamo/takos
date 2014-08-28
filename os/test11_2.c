#include "defines.h"
#include "takos.h"
#include "lib.h"

int test11_2_main(int argc, char *argv[])
{
  char *p;
  int size;

  puts("test11_2 started\n");

  /*send static msg*/
  puts("test11_2 send in.\n");
  tk_send(MSGBOX_ID_MSGBOX1, 15, "static memory\n");
  puts("test11_2 send out.\n");

  /*send dynamic msg*/
  p = tk_malloc(18);
  strcpy(p, "allocated memory\n");
  puts("test11_2 send in.\n");
  tk_send(MSGBOX_ID_MSGBOX1, 18, p);
  puts("test11_2 send out.\n");

  /*recv in static*/
  puts("test11_2 recv in.\n");
  tk_recv(MSGBOX_ID_MSGBOX2, &size, &p); /*recv*/
  puts("test11_2 recv out.\n");
  puts(p);

  /*recv dynamic msg */
  puts("test11_2 recv in.\n");
  tk_recv(MSGBOX_ID_MSGBOX2, &size, &p);
  puts("test11_2 recv out.\n");
  puts(p);
  tk_free(p);

  puts("test11_2 exit.\n");

  return 0;
}
