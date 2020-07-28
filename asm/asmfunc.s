##### プロテクテッド(32bit)の機械語を生成する #####
.code32


##### 外に公開するグローバル関数 #####
.globl addr_call
.globl io_in8, io_in16, io_in32
.globl io_out8, io_out16, io_out32
.globl io_load_eflags, io_store_eflags
.globl load_idtr, load_gdtr
.globl load_cr0, store_cr0
.globl asm_inthandler21, asm_inthandler27, asm_inthandler2c

.extern inthandler21, inthandler27, inthandler2c

.text

# uint32 addr_call(uint32 addr);
addr_call:
	movl 4(%esp), %eax
	subl $0x300000, %eax
	call *%eax
	ret

# int io_in8(int port);
io_in8:
	movl 4(%esp), %edx
	xorl %eax, %eax
	inb %dx, %al
	ret

# int io_in16(int port);
io_in16:
	movl 4(%esp), %edx
	xorl %eax, %eax
	inw %dx, %ax
	ret

# int io_in32(int port);
io_in32:
	movl 4(%esp), %edx
	inl %dx, %eax
	ret

# void io_out8(int port, int data);
io_out8:
	movl 4(%esp), %edx
	movb 8(%esp), %al
	outb %al, %dx
	ret

# void io_out16(int port, int data);
io_out16:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outw %ax, %dx
	ret

# void io_out32(int port, int data);
io_out32:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outl %eax, %dx
	ret

# int io_load_eflags(void);
io_load_eflags:
	pushf
	popl %eax
	ret

# void io_store_eflags(int eflags);
io_store_eflags:
	pushl 4(%esp)
	popf
	ret

# void load_idtr(uint32 limit, uint32 addr);
load_idtr:
	movw 4(%esp), %ax
	movw %ax, 6(%esp)
	lidt 6(%esp)
	ret

# void load_gdtr(int limit, int addr);
load_gdtr:
	movw 4(%esp), %ax
	movw %ax, 6(%esp)
	lgdt 6(%esp)
	ret

# uint32 load_cr0(void);
load_cr0:
	movl %cr0, %eax
	ret

# void store_cr0(uint32 cr0);
store_cr0:
	movl 4(%esp), %eax
	movl %eax, %cr0
	ret

#マクロを定義している
.macro asm_inthandler   c_inthandler
    pushw %es
    pushw %ds
    pushal
    movl %esp, %eax
    pushl %eax
    movw %ss, %ax
    movw %ax, %ds
    movw %ax, %es
    call \c_inthandler
    popl %eax
    popal
    popw %ds
    popw %es
    iretl
.endm

#asm_inthandler20:
#    asm_inthandler inthandler20

asm_inthandler21:
    asm_inthandler inthandler21

asm_inthandler27:
    asm_inthandler inthandler27

asm_inthandler2c:
    asm_inthandler inthandler2c
