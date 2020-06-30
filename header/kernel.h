#ifndef _KERNEL
#define _KERNEL

#include<datatype.h>
#include<basic.h>

// graphic.c
void fillbox(uint8 *buf, uint32 xsize, uint32 x0, uint32 y0, uint32 x1, uint32 y1, uint32 col, uint8 pbyte);
void fillblock(uint8 *block, uint32 xsize, uint32 x, uint32 y, uint32 x_len, uint32 y_len, uint8 *buf);
void write_backscr(uint8 *buf,  uint32 xsize, uint32 ysize, uint8 pbyte);
void putfont(uint8 *buf, uint32 xsize, uint32 x, uint32 y, uint32 col, uint8 c, uint8 pbyte);
void putfonts(uint8 *buf, uint32 xsize, uint32 x, uint32 y, uint32 col, uint8 *s, uint8 pbyte);

// mysprintf.c
void sprintf (uint8 *str, uint8 *fmt, ...);

// mouse.c
void mouse_init(uint8 *buf, uint32 backcol);
void enable_mouse(void);
sint32 mouse_decode(struct MOUSE_DEC *mdec, uint8 data);

// descriptor.c
void set_gatedesc(struct GATE_DESCRIPTOR* gd, uint32 offset, uint16 selector, uint16 access_right);
void set_segmdesc(struct SEGMENT_DESCRIPTOR* sd, uint32 limit, uint32 base, uint32 access_right);
void descriptor_init(void);

// int.c
void pic_init(void);
void inthandler21(uint32 *esp);
void inthandler27(uint32 *esp);
void inthandler2c(uint32 *esp);

// fifo.c
void fifo32_init(struct FIFO32 *fifo, sint32 size, sint32 *buf);
void fifo32_put(struct FIFO32 *fifo, sint32 data);
sint32 fifo32_get(struct FIFO32 *fifo);
uint32 fifo32_status(struct FIFO32 *fifo);

// kbc.c
void wait_KBC_sendready(void);
void kbc_init(void);

// memtest.c
uint32 memtest_sub(uint32 start, uint32 end);
uint32 memtest(uint32 start, uint32 end);

// memman.c

// メモリ管理関数の初期化を行う
void memman_init(struct MEMMAN *memman);
// 空きサイズの合計を返す
uint32 memman_total(struct MEMMAN *memman);
// メモリを確保する
uint32 memman_alloc(struct MEMMAN *memman, uint32 size);
// メモリを開放する
sint32 memman_free(struct MEMMAN *memman, uint32 addr, uint32 size);

/***** debug.c *****/
void check_section_addr(void);
void show_regs(REGS* regs);

#endif
