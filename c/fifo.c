#include<basic.h>

void fifo32_init(struct FIFO32 *fifo, sint32 size, sint32 *buf){
	fifo->buf = buf;
	fifo->r = 0;
	fifo->w = 0;
	fifo->size = size;
	fifo->free = size;
	fifo->flags = 0;
}

void fifo32_put(struct FIFO32 *fifo, sint32 data){
	fifo->buf[fifo->w++] = data;

	// dataの領域の最後に到達すると最初に戻る
	if(fifo->w == fifo->size){
		fifo->w = 0;
	}

	// データが溢れたかどうかのチェック
	if(--fifo->free < 0){
		fifo->flags |= W_OVR_R;
	}
}

sint32 fifo32_get(struct FIFO32 *fifo){
	sint32 data = fifo->buf[fifo->r++];

	// dataの領域の最後に到達すると最初に戻る
	if(fifo->r == fifo->size){
		fifo->r = 0;
	}

	// 存在しないデータを読んではいないか
	if(++fifo->free > fifo->size){
		fifo->flags |= R_OVR_W;
	}

	return data;
}


uint32 fifo32_status(struct FIFO32 *fifo){
	return 256 - fifo->free;
}
