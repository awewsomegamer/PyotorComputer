MEMORY {
	ram : ORIGIN = 0x0000, LENGTH = 0xFFFF
	kernal_rom (rx) : ORIGIN = 0xDCC0, LENGTH = 0x233A
	vectors : ORIGIN = 0xFFFA, LENGTH = 0x0006
	user_ram (rw) : ORIGIN = 0x0200, LENGTH = 0xBB80
	zp : ORIGIN = 0x0000, LENGTH = 0x0100
}

SECTIONS {
	.text : { 
		*(.text)
		*(.text*)
	} > kernal_rom
	.rodata : ALIGN(0x400) {
		*(.rodata)
		*(.rodata*)
	} > kernal_rom
	
	/*_vectors_start = 0xFFFA;
	.vectors _vectors_start : {
		KEEP(*(.flag));
	} > kernal_rom*/
	
	.data : {
		*(.data)
		*(.data*)
	} > user_ram
	.bss : {
		*(.bss)
		*(.bss*)
	} > user_ram

	/*DISCARD {
		*(.call_main)
		*(.fini_rts)
		*(.no_init..Lstatic_stack)
	}*/

}

__rc0 = 0x05;
INCLUDE imag-regs.ld
ASSERT(__rc0 == 0x05, "Inconsistent zero page map")
ASSERT(__rc31 == 0x05+0x1f, "Inconsistent zero page map")

OUTPUT_FORMAT { FULL(kernal_rom) SHORT(IRQ_HANDLER) SHORT(main) SHORT(NMI_HANDLER) }
/* SHORT(IRQ_HANDLER) SHORT(main) SHORT(NMI_HANDLER) */
/* FULL(vectors) */

