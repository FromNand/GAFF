#include<basic.h>
#include<asmfunc.h>

void wait_KBC_sendready(void){
	/* bit1はKBCのバッファが空いているかを確認する */
	for(;;){
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0){
			break;
		}
	}
}

void kbc_init(void){
	/* kbcの初期化 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDATA, KBC_MODE);
}
