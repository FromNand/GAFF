#include<basic.h>
#include<kernel.h>
#include<asmfunc.h>

void mouse_init(uint8 *buf, uint32 backcol){
	static uint8 cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};

	uint32 i, j, k;
	uint32 temp1 = 0, temp2 = 0;
	uint32 col = 0;

	for(i = 0; i < 16; i++){
		temp1 = (16 * i) * bi->pbyte;
		for(j = 0; j < 16; j++){
			temp2 = temp1 + j * bi->pbyte;
			if(cursor[i][j] == '*'){
				col = 0;		//黒
			}
			else if(cursor[i][j] == 'O'){
				col = 0xffffff;		//白
			}
			else if(cursor[i][j] == '.'){
				col = backcol;		//背景色
			}

			for(k = 0; k < bi->pbyte; k++){
				buf[temp2 + k] = (col & (0xff << (8 * k))) >> (8 * k);
			}
		}
	}
}

#define KEYCMD_SENDTO_MOUSE	0xd4
#define MOUSECMD_ENABLE		0xf4

void enable_mouse(void){
	/* マウス有効化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDATA, MOUSECMD_ENABLE);
}

/*struct MOUSE_DEC {
	uint8 buf[3], dec_phase;
	sint32 mov_x, mov_y, btn_info;
	uint8 btn_str[4];
};*/

sint32 mouse_decode(struct MOUSE_DEC *mdec, uint8 data){
	if(mdec->dec_phase == 0){
		if(data == 0xfa){
			mdec->dec_phase++;
		}
		return 0;
	}
	else if(mdec->dec_phase == 1){
		mdec->buf[0] = data;
		mdec->dec_phase++;
		return 0;
	}
	else if(mdec->dec_phase == 2){
		mdec->buf[1] = data;
		mdec->dec_phase++;
		return 0;
	}
	else if(mdec->dec_phase == 3){
		mdec->buf[2] = data;
		mdec->dec_phase = 1;

		// マウスのボタンが押されているか
		mdec->btn_info = mdec->buf[0] & 0x7;

		// ボタンの文字列をいじる
		if((mdec->btn_info & 0x1) != 0){
			mdec->btn_str[0] = 'L';
		}
		else {
			mdec->btn_str[0] = 'l';
		}

		if((mdec->btn_info & 0x2) != 0){
			mdec->btn_str[2] = 'R';
		}
		else {
			mdec->btn_str[2] = 'r';
		}

		if((mdec->btn_info & 0x4) != 0){
			mdec->btn_str[1] = 'C';
		}
		else {
			mdec->btn_str[1] = 'c';
		}

		// マウスの移動
		mdec->mov_x = mdec->buf[1];
		mdec->mov_y = mdec->buf[2];

		if((mdec->buf[0] & 0x10) != 0){
			mdec->mov_x |= 0xffffff00;
		}
		if((mdec->buf[0] & 0x20) != 0){
			mdec->mov_y |= 0xffffff00;
		}

		mdec->mov_y = -mdec->mov_y;

		return 1;
	}

	return 0;
}
