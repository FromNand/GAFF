##### レジスタの値をデバッグするときに使用する関数群 #####
##### 基本的にこれらの関数を呼び出す直前の値を格納するようにしてある #####

.code32

.global srch_eax, srch_ecx, srch_edx, srch_ebx, srch_esi, srch_edi, srch_esp, srch_ebp
.global srch_eip
.global srch_regs

.text

##### 汎用レジスタ #####

# uint32 srch_eax(void);
srch_eax:
    ret

# uint32 srch_ecx(void);
srch_ecx:
    movl %ecx, %eax
    ret

# uint32 srch_edx(void);
srch_edx:
    movl %edx, %eax
    ret

# uint32 srch_ebx(void);
srch_ebx:
    movl %ebx, %eax
    ret

# uint32 srch_esi(void);
srch_esi:
    movl %esi, %eax
    ret

# uint32 srch_edi(void);
srch_edi:
    movl %edi, %eax
    ret

# uint32 srch_esp(void);
srch_esp:
    leal -0x4(%esp), %eax
    ret

# uint32 srch_ebp(void);
srch_ebp:
    movl %ebp, %eax
    ret

##### 特殊レジスタ #####

# uint32 srch_eip(void);
srch_eip:
    movl (%esp), %eax
    subl $5, %eax
    ret

##### 全部のレジスタを一気に調べる #####
##### デバッグしたので、多分正しい値が出力されていると思うが、一応デバッグ方法 #####
##### eax, ecx, edx, ebx, esi, ediは関数の最初で値を書き換えてみる #####
##### eipは4(%ebp)を書き換えると実行継続不可になる #####
##### espはeipのすぐ上にあるはずなので、ebp+8のアドレス #####
##### ebpだけはちょっと面倒で、この関数の直前に他の関数で書き換えないといけない #####

# REGS* srch_regs(REGS*);
srch_regs:
	pushl %ebp
	movl %esp, %ebp
	pushl %eax		# 構造体のアドレスをeaxに入れるため、退避する
	movl 8(%ebp), %eax	# REGS構造体のアドレスをeaxに入れる
	movl %ecx, 4(%eax)
	movl %edx, 8(%eax)
	movl %ebx, 12(%eax)
	movl %esi, 16(%eax)
	movl %edi, 20(%eax)
	leal 8(%ebp), %ecx	# ecxはすでに保存したので破壊しても良い。呼び出し前のespはebpに保存されているが、リターンアドレスとpush %ebpの分だけ足しておく。
	movl %ecx, 24(%eax)
	movl (%ebp), %ecx	# ecxはすでに保存したので破壊しても良い
	movl %ecx, 28(%eax)	# (%ebp)にはもとのebpの値が入っている
	movl 4(%ebp), %ecx	# 4(%ebp)にはリターンアドレス(関数呼び出し前のeip)が入っている
	movl %ecx, 32(%eax)	# eipを保存する
	movl %eax, %ecx		# REGS構造体のアドレスをecxに入れる
	popl %eax		# eaxを復旧する
	movl %eax, (%ecx)	# eaxを保存する
	movl %ecx, %eax		# REGS構造体のアドレスを返り値にする
	leave
	ret
