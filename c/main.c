#include<asmfunc.h>
#include<kernel.h>
#include<string.h>

BOOTINFO *bi = (BOOTINFO *) 0x0ff0;

extern uint32 data_size;
extern uint32 data_addr;

struct FIFO32 keyfifo;
struct FIFO32 mousefifo;
struct MEMMAN memman;

void GAFF(){
	// デバッグ用
	uint8 s[512];

	sprintf(s, "boot process is running...");
	putfonts(bi->vram, bi->scrnx, 0, 0, 0xffffff, s, bi->pbyte);

	//check_section_addr();

	memman_init(&memman);
	//memman_free(&memman, 0x700000, 0x1000000);

	// buf
	sint32 keybuf[256];
	sint32 mousebuf[256];

	// IDTとGDTを初期化する
	descriptor_init();

	// PICの初期化
	pic_init();
	_STI

	// fifoの初期化
	fifo32_init(&keyfifo, 256, keybuf);
	fifo32_init(&mousefifo, 256, mousebuf);

	// IRQのマスクを解除する
	_CLI
	io_out8(PIC0_IMR, 0xf9);	/* PIC1とキーボードを許可(11111001) */
	io_out8(PIC1_IMR, 0xef);	/* マウスを許可(11101111) */
	_STI

	// kbcの初期化
	kbc_init();

	// マウス
	uint8 mouse_buf[1024];
	mouse_init(mouse_buf, 0x008484);

	// マウスの初期化
	enable_mouse();

	// メモリーテスト
	uint32 memsize = memtest(0x01000000, 0xbfffffff) / (1024 * 1024);

	// 背景描写
	write_backscr(bi->vram, bi->scrnx, bi->scrny, bi->pbyte);
	sprintf(s, "memsize = %u, vram = %x, cyls = %x, data_size = %x, data_addr = %x", memsize, bi->vram, bi->cyls, data_size, data_addr);
	fillbox(bi->vram, bi->scrnx, 0, 0, 122, 8, 0x008484, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 2, 2, 0x000000, s, bi->pbyte);
	putfonts(bi->vram, bi->scrnx, 0, 0, 0xffffff, s, bi->pbyte);
	fillblock(bi->vram, bi->scrnx, 0, 0, 16, 16, mouse_buf);

	// 一時変数 data
	sint32 data;

	// mouseデコード
	sint8 mouse_num = 0;
	sint8 mouse_dec[3];

	// mouse処理
	sint32 mouse_x = 0, mouse_y = 0;

	// マウスデコード情報
	struct MOUSE_DEC mdec;
	mdec.dec_phase = 0;
	mdec.btn_str[0] = 'l';
	mdec.btn_str[1] = 'c';
	mdec.btn_str[2] = 'r';
	mdec.btn_str[3] = 0;

	for(;;){
		_CLI
		if (fifo32_status(&keyfifo) == 0 && fifo32_status(&mousefifo) == 0){
			_STIHLT
		}
		else if (fifo32_status(&keyfifo) != 0){
			// データ読み出し
			data = fifo32_get(&keyfifo);
			_STI

			// 読み出したデータの表示
			sprintf(s, "key_data = 0x%x", data);
			fillbox(bi->vram, bi->scrnx, 0, 100, 122, 118, 0x008484, bi->pbyte);
			putfonts(bi->vram, bi->scrnx, 2, 102, 0x000000, s, bi->pbyte);
			putfonts(bi->vram, bi->scrnx, 0, 100, 0xffffff, s, bi->pbyte);
		}
		else if (fifo32_status(&mousefifo) != 0){
			// データ読み出し
			data = fifo32_get(&mousefifo);
			_STI

			// データをデコードする
			if(mouse_decode(&mdec, data) != 0){
				// ボタンの状況
				fillbox(bi->vram, bi->scrnx, 0, 200, 26, 218, 0x008484, bi->pbyte);
				putfonts(bi->vram, bi->scrnx, 2, 202, 0x000000, mdec.btn_str, bi->pbyte);
				putfonts(bi->vram, bi->scrnx, 0, 200, 0xffffff, mdec.btn_str, bi->pbyte);

				// マウスを消す
				fillbox(bi->vram, bi->scrnx, mouse_x, mouse_y, mouse_x + 16, mouse_y + 16, 0x008484, bi->pbyte);

				// マウスの移動
				mouse_x += mdec.mov_x;
				mouse_y += mdec.mov_y;

				if(mouse_x < 0){
					mouse_x = 0;
				}
				else if(mouse_x > bi->scrnx - 16){
					mouse_x = bi->scrnx - 16;
				}

				if(mouse_y < 0){
					mouse_y = 0;
				}
				else if(mouse_y > bi->scrny - 16){
					mouse_y = bi->scrny - 16;
				}

				// マウスを書く
				fillblock(bi->vram, bi->scrnx, mouse_x, mouse_y, 16, 16, mouse_buf);
			}
		}
	}
}
