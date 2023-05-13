.import _init
.import _nmi_int, _irq_int

.segment "VECTORS"

.addr _nmi_int
.addr _init
.addr _irq_int