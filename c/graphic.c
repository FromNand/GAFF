#include<basic.h>

// ascii
extern char hankaku[];

void fillbox(uint8 *buf, uint32 xsize, uint32 x0, uint32 y0, uint32 x1, uint32 y1, uint32 col, uint8 pbyte){
	// ループ変数
	uint32 i, j, k;

	// 掛け算を少なくするための一時変数
	uint8 *buf_temp1, *buf_temp2;

	for(i = y0; i < y1; i++){
		buf_temp1 = buf + (xsize * pbyte * i);
		for(j = x0; j < x1; j++){
			buf_temp2 = buf_temp1 + (pbyte * j);
			for(k = 0; k < pbyte; k++){
				// 下のswitchの省略形
				buf_temp2[k] = (col & (0xff << (8 * k))) >> (8 * k);

				/*switch(k){
					case 0:	// Blue
						vram_temp2[k] = (col & 0x000000ff) >> 0;
						break;
					case 1:	// Green
						vram_temp2[k] = (col & 0x0000ff00) >> 8;
						break;
					case 2: // Red
						vram_temp2[k] = (col & 0x00ff0000) >> 16;
						break;
					case 3: // Ignore
						break;
				}*/
			}
		}
	}
}

void fillblock(uint8 *block, uint32 xsize, uint32 x, uint32 y, uint32 x_len, uint32 y_len, uint8 *buf){
	uint32 i, j, k;
	uint32 temp1, temp2, temp3;

	// 書き込み先バッファの最初の添字
	uint32 first = (y * xsize + x) * bi->pbyte;

	for(i = 0; i < y_len; i++){
		temp1 = first + i * xsize * bi->pbyte;
		for(j = 0; j < x_len; j++){
			temp2 = temp1 + j * bi->pbyte;			// blockの添字用
			temp3 = (x_len * i + j) * bi->pbyte;		// bufの添字用
			for(k = 0; k < bi->pbyte; k++){
				block[temp2 + k] = buf[temp3 + k];
			}
		}
	}
}

void write_backscr(uint8 *buf,  uint32 xsize, uint32 ysize, uint8 pbyte){
	// 割り算を何度もやらなくていいように
	uint32 y_div_10 = ysize / 10;

	// まず背景全体の描写
	fillbox(buf, xsize, 0, 0, xsize, ysize, 0x008484, pbyte);				//背景の青

	// タスクバーの表示(高さの10分の1)
	fillbox(buf, xsize, 0, ysize - y_div_10, xsize, ysize, 0xc6c6c6, pbyte);		//タスクバー自体
	fillbox(buf, xsize, 0, ysize - y_div_10 - 3, xsize, ysize - y_div_10, 0xffffff, pbyte);	//タスクバーの影
}

void putfont(uint8 *buf, uint32 xsize, uint32 x, uint32 y, uint32 col, uint8 c, uint8 pbyte){
	// ループ変数
	uint32 i, j, k;

	// 添字アクセスを減らすための一時変数
	uint8 temp;

	// フォントのデータが格納されている場所
	uint8 *font = &hankaku[16 * c];

	// 実際に書き込み始める場所を計算しておく
	uint8 *buf2 = buf + (xsize * y + x) * pbyte;

	for(i = 0; i < 16; i++){
		temp = font[i];
		for(j = 0; j < 8; j++){
			for(k = 0; k < 3; k++){
				if((temp & (1 << (7 - j))) != 0){
					buf2[(xsize * i + j) * pbyte + k] = (col & (0xff << (8 * k))) >> (8 * k);
				}
				//buf2[(xsize * i + j) * pbyte + k] = (temp & (1 << (7 - j))) ? (col & (0xff << (8 * k))) >> (8 * k) : (0x008484 & (0xff << (8 * k))) >> (8 * k);
			}
		}
	}
}

void putfonts(uint8 *buf, uint32 xsize, uint32 x, uint32 y, uint32 col, uint8 *s, uint8 pbyte){
	uint32 count = 0;

	while(*s != 0){
		putfont(buf, xsize, x + count, y, col, *s++, pbyte);
		count += 8;
	}
}
