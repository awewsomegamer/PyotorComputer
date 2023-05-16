_entry: bra _entry

_irq_handler:
_nmi_handler: rti

.segment "VECTORS"
	.addr _irq_handler
	.addr _entry
	.addr _nmi_handler