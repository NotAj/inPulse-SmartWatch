ENTRY(start);

MEMORY {
	flash : ORIGIN = 4K, LENGTH = 28K
	ram : ORIGIN = 0x40000000, LENGTH = 8K
}

/*
 * stack end - 32 because the IAP RAM uses the top 32 bytes of RAM
 */
_stack_end = 0x40000000 + 8K - 32 - 4;

SECTIONS {
	. = 0;
	. = ALIGN(0x4);

/* the very first dword in the next bank is the image checksum */

	kernel_struct 0x1000 : { KEEP(*(.kernel_struct)) } >flash

/* The main application has the remaining 28kB of flash to play with. */

	.text ALIGN(0x4) : {
		*(.text.main)
		*(.text .text.* .gnu.linkonce.t.*)
		*(.glue_7 .glue_7t)
		*(.rodata .rodata.*)
	} >flash

	_etext = .;

	.data : {
		_data = .;
		*(.data .data.*)
	} >ram AT>flash

	_data_src = LOADADDR(.data);
	_data_len = SIZEOF(.data);

	.bss : {
		_bss = .;
		*(.bss .bss.*)
	} >ram

	_bss_len = SIZEOF(.bss);

/*
 * Align here to ensure that the .bss section occupies space up to
 * _end.  Align after .bss to ensure correct alignment even if the
 * .bss section disappears because there are no input sections.
 */
	. = ALIGN(0x4);
}

	. = ALIGN(0x4);
	_end = .;
	_bss_end__ = .; __bss_end__ = .; __end__ = .;
	PROVIDE (end = .);

	ENTRY(main);
	/*PROVIDE(allo_struct = 0x20);*/
	PROVIDE(_initdata_src = LOADADDR(.data));
	PROVIDE(_initdata_len = SIZEOF(.data));
	PROVIDE(alloword = 0xe0024044);

