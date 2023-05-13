.import _stop
.export _irq_int, _nmi_int

.segment "CODE"

.PC02

_nmi_int:
	rti

_irq_int:
	phx
	tsx
	pha
	inx
	inx
	lda $100, x
	and #$10
	bne break

; IRQ
	pla
	plx

	rti

break:
	jmp _stop	