	.h8300h
	.section .text
	.global	_start
	#	.type	_start,@function
_start:
	mov.l	#_bootstack,sp
	jsr	@_main

1:
	bra	1b

	#dispatch is not related to startup, but put the function here because it must be written in assembler
	.global _dispatch
	#	.type	_dispatch,@fuction
_dispatch:
	mov.l	@er0, er7 //thread's sp as a parameter
	mov.l	@er7+, er0
	mov.l	@er7+, er1
	mov.l	@er7+, er2
	mov.l	@er7+, er3
	mov.l	@er7+, er4
	mov.l	@er7+, er5
	mov.l	@er7+, er6
	rte

	
	