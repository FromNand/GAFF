##### 24/32bit色モード(http://oswiki.osask.jp/?VESA) #####
.equ VESAMODE1,	0x11f		# 1600 x 1200
.equ VESAMODE2, 0x11b		# 1280 x 1024
.equ VESAMODE3,	0x118		# 1024 x  768
.equ VESAMODE4,	0x115		#  800 x  600
.equ VESAMODE5,	0x112		#  640 x  480
.equ VESAMODE6,	0x10f		#  320 x  200


##### データの転送についてのアドレス #####
.equ BOTPAK,	0x00300000
.equ DSKCAC,	0x00100000
.equ DISK_DATA,	0x00008000


##### 画面の情報を記憶する #####
.equ CYLS,	0x0ff0
.equ LEDS,	0x0ff2
.equ VMODE,	0x0ff3
.equ SCRNX,	0x0ff4
.equ SCRNY,	0x0ff6
.equ VRAM,	0x0ff8
.equ PBYTE,	0x0ffc


##### まだ、32bitモードに移行していないため #####
.code16


##### head.lsでセクションを利用するため #####
.text


##### vesaの8:8:8(24bit)のグラフィックモードに対応しているかを大きい画面から順番に調べる #####
check_vesa32:

# screen mode 0x11f, 1600 * 1200
#	movw $VESAMODE1, %ax
#	pushw %ax
#	call set_screen
#	cmpl $0, %eax			# 0 - success, else - fault
#	je keystatus

# screen mode 0x11b, 1280 * 1024
#	movw $VESAMODE2, %ax
#	pushw %ax
#	call set_screen
#	cmpl $0, %eax			# 0 - success, else - fault
#	je keystatus

# screen mode 0x118, 1024 * 768
	movw $VESAMODE3, %ax
	pushw %ax
	call set_screen
	cmpl $0, %eax			# 0 - success, else - fault
	je keystatus

# screen mode 0x115, 800 * 600
	movw $VESAMODE4, %ax
	pushw %ax
	call set_screen
	cmpl $0, %eax			# 0 - success, else - fault
	je keystatus

# screen mode 0x112, 640 * 480
	movw $VESAMODE5, %ax
	pushw %ax
	call set_screen
	cmpl $0, %eax			# 0 - success, else - fault
	je keystatus

# screen mode 0x10f, 320 * 200
	movw $VESAMODE6, %ax
	pushw %ax
	call set_screen
	cmpl $0, %eax			# 0 - success, else - fault
	je keystatus


##### VESAの24/32bitモードがすべて失敗したので、320 * 200モードに変更する #####
##### C言語側で対応させられてない #####
#	movb $0x13, %al			# VGAグラフィックス、320 * 200の8bitカラー
#	movb $0x00, %ah
#	int $0x10
#	movb $8, (VMODE)
#	movw $320, (SCRNX)
#	movw $200, (SCRNY)
#	movl $0x000a0000,(VRAM)


#### VESAの高画質モードも320 * 200モードもすべて失敗した #####
	movw $vesa_fault_msg, %si
	call putloop
vesa_fault_loop:
	hlt
	jmp vesa_fault_loop


##### キーボードの状態を取得 #####
keystatus:
	movb $0x02, %ah
	int $0x16
	movb %al, (LEDS)


##### 32ビットプロテクトモードへ移行 #####
	# PICへの割り込み禁止
	cli				# PIC設定中はcliしておく
	movb $0xff, %al
	outb %al, $0x21
	nop				# 連続してout命令を実行するとバグる機種があるらしい(from 自作本)
	outb %al, $0xa1
	sti				# PIC設定中に受け付けた割り込みがもしあったら、それを32bitモードに入る前に処理させてあげるためらしい
	nop				# stiとcliが続くのは気持ち悪いので
	cli				# CPUレベルでも割り込み禁止

	# A20信号線をON
	call waitkbdout
	movb $0xd1, %al
	outb %al, $0x64
	call waitkbdout
	movb $0xdf, %al
	outb %al, $0x60			# enable A20
	call waitkbdout

.arch i486				# 32bitネイティブコード
	lgdtl (GDTR0)			# 暫定GDTを設定
	movl %cr0, %eax
	andl $0x7fffffff, %eax		# bit31を0に(ページング禁止)
	orl $0x00000001, %eax		# bit0を1に(プロテクトモード移行)
	movl %eax, %cr0
	jmp pipelineflash		# 動作モードがリアルモード(16bit)からプロテクテッドモード(32bit)に変化して、機械語の意味も変わったのでパイプラインをフラッシュする
pipelineflash:
	movw $1*8, %ax			# 読み書き可能セグメント32bit
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss


##### bootpackの転送 #####
	movl $bootpack, %esi
	movl $BOTPAK, %edi
	movl $512*0x400/4, %ecx
	call memcpy


##### iplで読み込んだデータ(0x8000~)を0x100000番地に転送する #####
	movl $DISK_DATA, %esi		# 0x8000
	movl $DSKCAC, %edi		# 0x100000
	movl $0x22000, %ecx		# (0x90000 - 0x8000) / 4 = 0x22000
	call memcpy


##### start bootpack #####
	movl $BOTPAK, %ebx
	movl 16(%ebx), %ecx
	addl $3, %ecx
	shrl $2, %ecx
	jz skip
	movl 20(%ebx), %esi
	addl %ebx, %esi
	movl 12(%ebx), %edi
	call memcpy
skip:
	movl 8(%ebx), %esp
	ljmpl $2*8, $0x0000001b


##### waitkbdout #####
waitkbdout:
	inb $0x64, %al
	andb $0x02, %al
	jnz waitkbdout
	ret


##### memcpy #####
memcpy:
	movl %ds:(%esi), %eax
	addl $4, %esi
	movl %eax, %ds:(%edi)
	addl $4, %edi
	loop memcpy
	ret


########## VESAの設定を行う関数(set_screen) ##########
# VBE存在確認(ax=0x4f00)
set_screen:
	movw $0x9000, %ax
	movw %ax, %es
	movw $0, %di
	movw $0x4f00, %ax
	int $0x10
	movw $msg1, %si
	cmpw $0x004f, %ax		# ax=4f02 bx=mode int 0x10 の戻り値では、al=0x4f(この関数が存在する), ah=0(正常終了)の２つを満たすべき
	jne not_exist

# VBEのバージョンチェック(VESAのバージョン 1.02なら0x0102)
	movw %es:4(%di), %ax		# es:di+4の部分にはVESAのバージョン情報が入っている
	movw $msg2, %si
	cmpw $0x0200, %ax		# ver2.00より古い場合は諦める。とりあえず。
	jb not_exist

# 画面モードを得る(ax=0x4f01)
	movw 2(%esp), %cx		# (%esp)にはcallによる戻り値が入っている
	movw $0x4f01, %ax
	int $0x10
	movw $msg3, %si
	cmpw $0x004f, %ax		# ax=4f02 bx=mode int 0x10 の戻り値では、al=0x4f(この関数が存在する), ah=0(正常終了)の２つを満たすべき
	jne not_exist

# 画面モード情報の確認
	movb $24, %al
	cmpb %al, %es:0x19(%di)		# 1ピクセルが何ビットか（要するに色数）
	je ok_pbyte
	movb $32, %al
	cmpb %al, %es:0x19(%di)		# 1ピクセルが何ビットか（要するに色数）
	je ok_pbyte
	movw $msg4, %si			# qemu - 24bit, my machine - 32bit
	jmp not_exist
ok_pbyte:
	shrb $3, %al			# ビットをバイト単位に直す
	movb %al, (PBYTE)		# 1ピクセルが何ビットか（要するに色数）
	movw $msg5, %si
	cmpb $6, %es:0x1b(%di)		# メモリモデル（0x04=256パレット、0x06=ダイレクトカラー）
	jne not_exist
	movw $msg6, %si
	movw %es:0x00(%di), %ax
	andw 0x0080, %ax
	jz not_exist

# 画面モードの切り替え(ax=0x4f02)
	movw 2(%esp), %ax
	addw $0x4000, %ax
	movw %ax, %bx
	movw $0x4f02, %ax
	int $0x10
	movw $msg7, %si
	cmpw $0x004f, %ax		# ax=4f02 bx=mode int 0x10 の戻り値では、al=0x4f(この関数が存在する), ah=0(正常終了)の２つを満たすべき
	jne not_exist			# 参照(http://oswiki.osask.jp/?VESA)
	movw %es:0x00(%di), %ax
	movw %ax, (VMODE)
	movw %es:0x12(%di), %ax
	movw %ax, (SCRNX)
	movw %es:0x14(%di), %ax
	movw %ax, (SCRNY)
	movl %es:0x28(%di), %eax
	movl %eax, (VRAM)
	xorl %eax, %eax			# 正常終了0
	ret $2


##### 見つけたはずのVESAのモードに適切に移動することができなかった...  #####
not_exist:
	call putloop
	movl $1, %eax
	ret $2


##### 文字列表示関数 #####
putloop:
	movb (%si), %al
	addw $1, %si
	cmpb $0, %al 
	je putloop_fin
	movb $0x0e, %ah
	movw $0, %bx
	int $0x10
	jmp putloop
putloop_fin:
	ret


##### データが並びます #####
.data


##### エラーメッセージリスト #####

msg1:
	.string	"In head.s: error1.\r\n"

msg2:
	.string	"In head.s: error2.\r\n"

msg3:
	.string	"In head.s: error3.\r\n"

msg4:
	.string	"In head.s: error4.\r\n"

msg5:
	.string	"In head.s: error5.\r\n"

msg6:
	.string	"In head.s: error6.\r\n"

msg7:
	.string	"In head.s: error7.\r\n"

vesa_fault_msg:
	.string	"vesa's 24/32mode doesn't exist.\r\n"   


##### とりあえずのGDTを設定する #####
.align 8

GDT0:
.skip 8, 0x00					# NULLセグメント
	.word 0xffff, 0x0000, 0x9200, 0x00cf	# サイズ0xfffff * 4K, ベースアドレス0x0, 読み書き可能の32bitデータセグメント
	.word 0xffff, 0x0000, 0x9a30, 0x0047	# サイズ0x7ffff, ベースアドレス0x300000,  読み書き実行可能の32bitコードセグメント
	.word 0x0000				# なんかよくわからん
GDTR0:
	.word 8*3-1
	.int GDT0

.align 8


##### bootpack start from here... #####
bootpack:					#os.lsの最初にある定数から始まり、c言語で書いたプログラムへと続く

