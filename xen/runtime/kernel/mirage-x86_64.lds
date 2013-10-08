OUTPUT_FORMAT("binary", "binary", "binary")
OUTPUT_ARCH(i386:x86-64)
ENTRY(start_kernel_hvm)
SECTIONS
{
  . = 0x0000000000100000;
  _text = .;			/* Text and read-only data */
 .text : {
	*(.text)
	*(.gnu.warning)
	}

  _etext = .;			/* End of text section */
  . = ALIGN(4096);
  _erodata = .;

  .data : {			/* Data */
        *(.rodata)
        *(.rodata.*)
	*(.data)

        *(.mltext)
        *(.mldata)
        *(.mlrodata)
	}

  _edata = .;			/* End of data section */

  . = ALIGN(4096);
  _mlstart = . ;		/* Start of ML section */

  .ml : {
  }

  _mlend = . ;			/* End of ML section */

  . = ALIGN(4096);
  __bss_start = .;		/* BSS */

  .bss : {
	*(.bss)
        *(.bss .bss.*)
        *(COMMON)
        *(.lbss .lbss.*)
        *(LARGE_COMMON)
	}

  . = ALIGN(4096);
  __bss_stop = .;
  _end = . ;

  /* Sections to be discarded */
  /DISCARD/ : {
	*(.text.exit)
	*(.data.exit)
	*(.exitcall.exit)
	}

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
}
