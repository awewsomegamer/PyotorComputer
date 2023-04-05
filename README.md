# Pyotor Computer
An 8-bit computer I built for my parents

# What
This is an 8-bit computer. It is meant to be like a Commodore 64 with some improvements.
There will be a custom assembler that will be used for `.asm` files and a custom emulator used for emulating the operating system and hardware components.

## Emulator
The emulator is designed to follow closely with how the real hardware will work. Optimizations will be prevalent on the smaller steps (not using a PLA and suc (as the real CPU would), just directly preforming the instruction).

# Specs
`Processor` - 65C02<br>
`Memory` - See "Memory"<br>
`Operating System` - <br>
`Graphics` - <br>
`Sound` - <br>
`Connectivity` - 2 PS/2 Ports, 2 Serial Ports<br>

## Processor
The processor used is the 65C02 microprocessor. This is because it fixes some bugs within the original 6502 design and it is still available.

## Memory
There is a total of 64 KiB of addressable memory. <br>

`$0000` -> `$00FF` - 256 B Zero Page <br>
`$0100` -> `$01FF` - 256 B Stack <br>
`$0200` -> `$407F` - 16 KB video memory or general purpose RAM (1) <br>
`$4080` -> `$BD7F` - 32 KB general purpose RAM, intended for user programs <br>
`$BD80` -> `$DCBF` - 8 KB dedicated Kernal RAM <br>
`$DCC0` -> `$FFFF` - ~8 KB dedicated Kernal ROM (2) <br>


(1): If **bit 0** at address **$0001 is set**, then the 16 KB of RAM is dedicated to **video memory**. If **bit 0** at address **$0001 is clear**, then the 16 KB of RAM is **dedicated to general purpose RAM**.

(2): There is a little bit more memory for the Kernal ROM than *exactly* 8 KB.


## Connectivity
### PS/2 Ports
The PS/2 Ports are apart of the design so that the computer can interface with later technologies that can be converted to a PS/2 type.

Currently the only planned support for a PS/2 device is a PS/2 keyboard (such as the IBM Model M).

### Serial Ports
These ports are apart of the design so that

