MEMORY { ram : ORIGIN = 0x0000, LENGTH = 0xFFFF
	 kernal_rom (rx) : ORIGIN = 0xDCC0, LENGTH = 0x233A
	 user_ram (rw) : ORIGIN = 0x0200, LENGTH = 0xBB80
	 zp : ORIGIN = 0x0000, LENGTH = 0x0100 }

SECTIONS { INCLUDE c.ld }

__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc0 == 0x00, "Inconsistent zero page map")
ASSERT(__rc31 == 0x1f, "Inconsistent zero page map")

OUTPUT_FORMAT { FULL(kernal_rom) SHORT(IRQ_HANDLER) SHORT(main) SHORT(NMI_HANDLER) }