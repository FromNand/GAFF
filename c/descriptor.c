#include<basic.h>
#include<asmfunc.h>

void set_gatedesc(struct GATE_DESCRIPTOR* gd, uint32 offset, uint16 selector, uint16 access_right){
	gd->offset_low = offset & 0xffff;
	gd->offset_high = (offset >> 16) & 0xffff;
	gd->selector = selector;
	gd->access_right = access_right;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR* sd, uint32 limit, uint32 base, uint32 access_right){
	// check Gflag
	if(limit > 0xfffff){
		access_right |= 0x800;
		limit = limit >> 12;
	}

	sd->limit_low = limit & 0xffff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((access_right >> 4) & 0xf0);
	sd->base_low = base & 0xffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->base_high = (base >> 24) & 0xff;
	sd->type = access_right & 0xff;
}

void descriptor_init(void){
	uint32 i;

	// gdt, idt
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR*) IDT_ADDR;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR*) GDT_ADDR;

	// idt initialize
	for(i = 0; i < 256; i++){
		set_gatedesc(idt + i, 0, 0, 0);
	}

	// int20~
	set_gatedesc(idt + 0x21, (uint32) asm_inthandler21, 2 * 8, 0x8e00);
	set_gatedesc(idt + 0x27, (uint32) asm_inthandler27, 2 * 8, 0x8e00);
	set_gatedesc(idt + 0x2c, (uint32) asm_inthandler2c, 2 * 8, 0x8e00);

	load_idtr(0x7ff, IDT_ADDR);

	// gdt initialize
	for(i = 0; i < 8192; i++){
		set_segmdesc(gdt + i, 0, 0, 0);
	}

	// 0-null_segment 1-data_segment 2-code_segment
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x492);
	set_segmdesc(gdt + 2, 0x001fffff, 0x00300000, 0x49a);

	load_gdtr(0xffff, GDT_ADDR);
}
