PREFIX	= /usr/local
ARCH	= h8300-elf
BINDER	= $(PREFIX)/bin
ADDNAME	= $(ARCH)-


#tool utility
AR	= $(BINDER)/$(ADDNAME)ar
AS	= $(BINDER)/$(ADDNAME)as 
CC	= $(BINDER)/$(ADDNAME)gcc 
LD	= $(BINDER)/$(ADDNAME)ld
NM 	= $(BINDER)/$(ADDNAME)nm
OBJCOPY	= $(BINDER)/$(ADDNAME)objcopy
OBJDUMP = $(BINDER)/$(ADDNAME)objdump
RANLIB	= $(BINDER)/$(ADDNAME)ranlib
STRIP	= $(BINDER)/$(ADDNAME)strip

H8WRITE = ../../tools/h8write/h8write

#FreeBSD-4.x: /dev/cuaaX, FreeBSD-6.x: /dev/cuadX, FreeBSD(USB): /dev/cuaUx
#Linux:/dev/ttySx, Linux(USB):/dev/ttyUSBx, Windows:comX
HWRITE_SERDEV = /dev/ttyUSB0

OBJS	= vector.o startup.o main.o
OBJS   += lib.o serial.o xmodem.o elf.o

TARGET = kzload

CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
#CFLAGS += -mint32 
CFLAGS += -I.
#CFLAGS += -g
CFLAGS += -Os
CFLAGS += -DKZLOAD

#linker option
LFLAGS = -static -T ld.scr -L.

all :	$(TARGET)

$(TARGET) :	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
		cp $(TARGET) $(TARGET).elf
		$(STRIP) $(TARGET)

.c.o:	$<
	$(CC) -c $(CFLAGS) $<

.s.o:	$<
	$(CC) -c $(CFLAGS) $<

$(TARGET).mot : $(TARGET)
		$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image : 	$(TARGET).mot

write : 	$(TARGET).mot
		$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)

clean :
	rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot
