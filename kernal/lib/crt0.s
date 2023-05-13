.export _init, _exit
.import _main

.export __STARTUP__ : absolute = 1
.import __RAM_START__, __RAM_SIZE__
.import copydata, zerobss, initlib, donelib

.include "zeropage.inc"

.segment "STARTUP"

_init:
	ldx #$FF
	txs
	cld

	lda #.LOBYTE(__RAM_START__ + __RAM_SIZE__)
	sta sp
	lda #.HIBYTE(__RAM_START__ + __RAM_SIZE__)
	sta sp + 1

	jsr zerobss
	jsr copydata
	jsr initlib

	jsr _main

_exit:
	jsr donelib
	brk