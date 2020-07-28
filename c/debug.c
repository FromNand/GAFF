#include<datatype.h>
#include<kernel.h>
#include<asmfunc.h>

// 初期値ありのグローバル変数
int global_data = 1;

// 初期値なしのグローバル変数
int global_bss;

// 初期値ありの内部変数
int static_data = 2;

// 初期値ありの内部変数
int static_bss;

void check_section_addr(void){
    // 初期値ありのローカル変数
	int local_data = 3;

    // 初期値なしのローカル変数
	int local_bss;

    // 実際にアドレスを表示してみる
	char s[100];

	sprintf(s, "##### CHECK_SECTION_ADDR #####");
	fillbox(bi->vram, bi->scrnx, 0, 0, 1600, 18, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 2, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 0, 0xffffff, s, bi->pbyte);

	sprintf(s, "global_data = %u, global_bss = %u, static_data = %u, static_bss = %u", global_data, global_bss, static_data, static_bss);
	fillbox(bi->vram, bi->scrnx, 0, 18, 1600, 36, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 20, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 18, 0xffffff, s, bi->pbyte);

	sprintf(s, "&global_data = %x, &global_bss = %x, &static_data = %x, &static_bss = %x", &global_data, &global_bss, &static_data, &static_bss);
	fillbox(bi->vram, bi->scrnx, 0, 36, 1600, 54, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 38, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 36, 0xffffff, s, bi->pbyte);

	sprintf(s, "local_data = %u, local_bss = %u, &local_data = %x, &local_bss = %x", local_data, local_bss, &local_data, &local_bss);
	fillbox(bi->vram, bi->scrnx, 0, 54, 1600, 72, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 56, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 54, 0xffffff, s, bi->pbyte);

	sprintf(s, "sdata = %x, edata = %x, sbss = %x, ebss = %x, srodata = %x, erodata = %x", \
		_sdata, _edata, _sbss, _ebss, _srodata, _erodata);
	fillbox(bi->vram, bi->scrnx, 0, 72, 1600, 90, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 74, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 72, 0xffffff, s, bi->pbyte);

	_CLIHLT

	return;
}

/***** レジスタの値を表示する関数 *****/
void show_regs(REGS* regs){
	char s[30];

	// まず画面をクリアする
	fillbox(bi->vram, bi->scrnx, 0, 0, bi->scrnx, 54, 0x008484, bi->pbyte);

	// レジスタの情報を画面に表示する
	sprintf(s, "eax=%x ecx=%x edx=%x ebx=%x", regs->eax, regs->ecx, regs->edx, regs->ebx);
	putfonts(bi->vram, bi->scrnx, 2, 2, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 0, 0xffffff, s, bi->pbyte);
	sprintf(s, "esi=%x edi=%x esp=%x ebp=%x", regs->esi, regs->edi, regs->esp, regs->ebp);
	putfonts(bi->vram, bi->scrnx, 2, 20, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 18, 0xffffff, s, bi->pbyte);
	sprintf(s, "eip=%x", regs->eip);
	putfonts(bi->vram, bi->scrnx, 2, 38, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 36, 0xffffff, s, bi->pbyte);

	// HLT
	_CLIHLT

	return;
}
