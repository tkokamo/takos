#ifndef _SERIAL_H_INCLUDED_
#define _SERIAL_H_INCLUDED_

int serial_init(int index); /*init a device*/
int serial_is_send_enable(int index); /*check whether it is able to send a character*/
int serial_send_byte(int index, unsigned char b); /* send a character*/
int serial_is_recv_enable(int index); /*check whether recv enabled*/
unsigned char serial_recv_byte(int index); /*recv 1 byte*/

#endif
