#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"
#include "consdrv.h"

#define CONS_BUFFER_SIZE 24

static struct consreg {
  tk_thread_id_t id; // thread which uses console
  int index;         // serial number (SCI)

  char *send_buf;    // send buffer
  char *recv_buf;    // recv buffer
  int send_len;      // send length
  int recv_len;      // recv length

  /*** dummy variable ***/
  long dummy[3];
} consreg[CONSDRV_DEVICE_NUM];

/* 
 * send_char() and send_string() are called from intr_processing and threads and send buffer is non reentrant 
 * so before these functions are called from threads, set cpu stato to cli.
 */
/*** send the first character in send buffer ***/
static void send_char(strcut consreg *cons)
{
  int i;
  serial_send_byte(cons->index, cons->send_buf[0]);
  cons->send_len--;
  /*** shift send buffer ***/
  for (i = 0; i < cons->send_len; i++)
    cons->send_buf[i] = cons->send_buf[i + 1]; // Can't I get better performance if I use ring buffer.
}


/*** write string to send buffer, and start send ***/
static void send_string(struct consreg *cons, char *str, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (str[i] == '\n') { //\n -> \r
      cons->send_buf[cons->send_len++] = '\r';
    }
    cons->send_buf[cons->send_lenn++] = str[i];
  }
  /*
   * if sending is not valid, start sending.
   * if sneding is valid, sending already started and we need to do nothing.
   */
  if (cons->send_len && !serial_intr_is_send_enable(cons->index)) {
    serial_intr_send_enable(cons->index); /*** enable sending ***/
    send_char(cons);
  }
}

