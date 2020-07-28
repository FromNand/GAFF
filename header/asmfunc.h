/* complex asm function is here */

#ifndef _ASMFUNC
#define _ASMFUNC

#include<datatype.h>
#include<basic.h>

// HLT
#define	_HLT	__asm__ __volatile__ ("hlt;");

// CLI
#define _CLI	__asm__ __volatile__ ("cli;");

// CLIHLT
#define _CLIHLT	__asm__ __volatile__ ("cli; hlt;");

// STI
#define _STI	__asm__ __volatile__ ("sti;");

// STIHLT
#define _STIHLT	__asm__ __volatile__ ("sti; hlt;");

// call function
uint32 addr_call(uint32 addr);

// check registers
uint32 srch_eax(void);
uint32 srch_ecx(void);
uint32 srch_edx(void);
uint32 srch_ebx(void);
uint32 srch_esi(void);
uint32 srch_edi(void);
uint32 srch_esp(void);
uint32 srch_ebp(void);
uint32 srch_eip(void);
REGS* srch_regs(REGS* regs);

// in function
uint32 io_in8(sint32 port);
uint32 io_in16(sint32 port);
uint32 io_in32(sint32 port);

// out function
void io_out8(sint32 port, sint32 data);
void io_out16(sint32 port, sint32 data);
void io_out32(sint32 port, sint32 data);

// eflags function
sint32 io_load_eflags(void);
void io_store_eflags(sint32 eflags);

// lidtr and lgdtr
void load_idtr(uint32 limit, uint32 addr);
void load_gdtr(int limit, int addr);

// cr0
uint32 load_cr0(void);
void store_cr0(uint32 cr0);

// asm_inthandler
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

#endif
