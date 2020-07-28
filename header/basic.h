// 汎用的な内容を含んだヘッダファイル

#ifndef _BASIC
#define _BASIC

// データタイプの宣言
#include<datatype.h>

// 各セクションのアドレスを表すシンボル
extern uint8 _shead[], _stext[], _sdata[], _sbss[], _srodata[];
extern uint8 _ehead[], _etext[], _edata[], _ebss[], _erodata[];

// レジスタを格納する構造体
typedef struct{
	uint32 eax, ecx, edx, ebx, esi, edi, esp, ebp, eip;
} REGS;

// 画面情報を示す構造体とその実体(bi)
typedef struct _BOOTINFO {
	uint16 cyls;	// 読み込んだシリンダ数
	uint8 leds;	// キーボードの状態
	uint8 vmode;	// 画面モード
	uint16 scrnx;	// xサイズ
	uint16 scrny;	// yサイズ
	uint8 *vram;	// vramのアドレス
	uint32 pbyte;	// 1ピクセルがバイトか
} BOOTINFO;

extern BOOTINFO *bi;

// GDTとIDT
#define IDT_ADDR 0x002df800
#define GDT_ADDR 0x002e0000

struct GATE_DESCRIPTOR {
	uint16 offset_low;
	uint16 selector;
	uint16 access_right;
	uint16 offset_high;
};

struct SEGMENT_DESCRIPTOR {
	uint16 limit_low;
	uint16 base_low;
	uint8  base_mid;
	uint8  type;
	uint8  limit_high;
	uint8  base_high;
};

// PIC
#define PIC0_IMR 0x0021
#define PIC1_IMR 0x00a1

#define PIC0_ICW1 0x0020
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC0_OCW1 0x0021	//mask port
#define PIC0_OCW2 0x0020
#define PIC1_OCW1 0x00a1
#define PIC1_OCW2 0x00a0
#define PIC1_ICW1 0x00a0
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

// キーボードに関するデータ
#define W_OVR_R 1
#define R_OVR_W 2

#define PORT_KEYDATA		0x0060
#define PORT_KEYSTA		0x0064
#define PORT_KEYCMD		0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE		0x47

struct FIFO32 {
	sint32 *buf;
	uint32 r, w;
	sint32 size;
	sint32 free;
	uint32 flags;
};

// マウス
struct MOUSE_DEC {
	uint8 buf[3], dec_phase;
	sint32 mov_x, mov_y, btn_info;
	uint8 btn_str[4];
};

// メモリ管理
#define FREE_NUM 256

struct FREE_INFO {
	uint32 addr;		//アドレス
	uint32 size;		//サイズ
};

struct MEMMAN {
	struct FREE_INFO free_info[FREE_NUM];
	uint32 alloc_num;	//現在確保している領域の数
	uint32 alloc_size;	//現在確保している領域のサイズ
	uint32 free_num;	//現在開放している領域の数
	uint32 free_size;	//現在開放している領域のサイズ
	uint32 lost_size;	//失ったデータのサイズ
	uint32 losts;		//失ったデータの個数
};

#endif
