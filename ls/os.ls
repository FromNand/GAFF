OUTPUT_FORMAT("binary")

MEMORY {
	FILE (rwx) : ORIGIN = 0,	LENGTH = 0x400000	/* セグメントをファイルに出力するときに使用する */
	TEXT (rwx) : ORIGIN = 0,	LENGTH = 0x200000	/* TEXTはベースが0x300000のセグメント */
	DATA (rw)  : ORIGIN = 0x100000, LENGTH = 0x200000	/* DATAはベースが0x100000のセグメント */
}

SECTIONS {

	.head : {			/* 出力ファイル(allcobj.bin)の0番地から並べる　実際に配置されるのは0x300000 */
		. = ALIGN(4);
		_shead = .;
		LONG(0x10000)		/* 0x0  : stack+.data+heap の大きさ（4KBの倍数） */
		LONG(0x69726148)	/* 0x4  : シグネチャ "Hari" */
		LONG(0x600000)		/* 0x8  : .stackのアドレスのトップ */
		LONG(0x100000)		/* 0xc  : .data転送先 */
		LONG(SIZEOF(.data))	/* 0x10 : .dataサイズ */
		LONG(LOADADDR(.data))	/* 0x14 : .dataの初期値列のファイル位置 オブジェクトファイルの中でのオフセット */
		LONG(0xE9000000)	/* 0x18 : ジャンプ命令「0xE9000000」 */
		LONG(GAFF - 0x20)	/* 0x1c : ジャンプ命令は次の命令のアドレス(この場合は0x20)を基準にジャンプする。この一行のおかげで.oファイルをどの順番でリンクしてもエントリが確実に見つけられる */
		LONG(0)			/* 0x20 : heap領域（malloc領域）開始アドレス */
		_ehead = .;
		. = ALIGN(4);
	} > TEXT AT > FILE

	.text : {
		. = ALIGN(4);
		_stext = .; *(.text) *(.text.*)	_etext = .;	/* [.text.*]はコンパイラの -ffunfction-sections、-fdata-sections に対応するためらしい...? */
		. = ALIGN(4);
	} > TEXT AT > FILE

	.data : {
		. = ALIGN(4);
		data_size = .;
		LONG(SIZEOF(.data))
		data_addr = .;
		LONG(LOADADDR(.data))
		_sdata = .; *(.data) _edata = .;
		_sbss = .; /* *(.bss) *(COMMON) */ _ebss = .;
		_srodata = .; *(.rodata*) _erodata = .;
		. = ALIGN(4);
	} > DATA AT > FILE

	/DISCARD/ : { *(.eh_frame) }	/* *(.comment) *(.note.GNU-stack) *(.symtab) *(.strtab) *(.shstrtab) などはここに入れても意味なかった */
}
