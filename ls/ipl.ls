OUTPUT_FORMAT("binary");
IPLBASE = 0x7c00;

SECTIONS {
	. = IPLBASE;
	.text	: {*(.text)}
	.data	: {*(.data)}
	. = IPLBASE + 502;
	.sign	: {
		LONG(0x00000001); LONG(0x00000020);
		SHORT(0xaa55);
	}
}
