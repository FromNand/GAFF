# temp1を使用する関数の場合、0x10000バイト以上のデータは転送できません(した場合は.data領域が破壊される)

.code32

# globl function
.globl memcpy, memmove, strcpy, strncpy
.globl strcat, strncat
.globl memcmp, strcmp, strncmp
.globl memchr, strchr, strcspn, strpbrk, strrchr, strspn, strstr, strdiv
.globl memset, strlen

.text

##### void *memcpy(void *buf1, const void *buf2, uint32 size); #####
##### buf1 と buf2 の領域が重なっている場合は未定義の動作 #####

memcpy:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
memcpy_4byte:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	movl 0x10(%ebp), %ecx
	shrl $2, %ecx
	rep movsl
memcpy_1byte:
	movl 0x10(%ebp), %ecx
	andl $0x3, %ecx
	rep movsb
memcpy_end:
	movl 0x8(%ebp), %eax
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### void *memmove(void *buf1, const void *buf2, uint32 size); #####
##### 実質memcpyだが、buf1とbuf2の領域が重なっている場合も正常に動作する #####

memmove:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
memmove_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	movl 0x10(%ebp), %ecx
	# buf1がbuf2よりも低位(等しい場合を含む)のアドレスに存在したらただmemcpyする
	cmpl %esi, %edi
	# buf1がbuf2よりも高位のアドレスに存在したら、temp1にbuf2の内容をコピーしてからtemp1の内容をbuf1にコピーする
	jle memmove_call_memcpy2
memmove_cpysrc_to_temp1:
	movl $temp1, %edi
	pushl %ecx
	pushl %esi
	pushl %edi
	call memcpy
memmove_call_memcpy1:
	movl $temp1, %esi
	movl 0x8(%ebp), %edi
	movl 0x10(%ebp), %ecx
	pushl %ecx
	pushl %esi
	pushl %edi
	call memcpy
	jmp memmove_end
memmove_call_memcpy2:
	pushl %ecx
	pushl %esi
	pushl %edi
	call memcpy
memmove_end:
	movl 0x8(%ebp), %eax
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### sint8 *strcpy(sint8 *s1, const sint8 *s2); #####
##### buf1 に buf2('\0'を含む)をコピーします #####

strcpy:
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
strcpy_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
strcpy_loop:
	lodsb
	stosb
	cmpb $0, %al
	je strcpy_end
	jmp strcpy_loop
strcpy_end:
	movl 8(%ebp), %eax
	popl %edi
	popl %esi
	leave
	ret

##### sint8 *strncpy(sint8 *s1, const sint8 *s2, uint32 size); #####
##### buf1 に buf2を最大size文字までコピーします('\0'が見つかると即終了) #####

strncpy:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
strncpy_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	movl 0x10(%ebp), %ecx
strncpy_loop:
	lodsb
	stosb
	cmpb $0, %al
	je strcpy_end
	loop strncpy_loop
strncpy_end:
	movl 8(%ebp), %eax
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### sint8 *strcat(sint8 *s1, const sint8 *s2); #####
##### buf1 に buf2を連結します('\0'を含む) #####

strcat:
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
strcat_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	xorl %eax, %eax
strcat_search_loop:
	scasb
	jne strcat_search_loop
	decl %edi
strcat_send_loop:
	lodsb
	stosb
	cmpb $0, %al
	jne strcat_send_loop
strcat_end:
	movl 8(%ebp), %eax
	popl %edi
	popl %esi
	leave
	ret

##### sint8 *strncat(sint8 *s1, const sint8 *s2, uint32 size); #####
##### buf1 に buf2をsize文字だけ連結します('\0'が見つかると即終了) #####

strncat:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
strncat_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	movl 0x10(%ebp), %ecx
	# scasbはds:(%edi)とalの値を比較する
	xorl %eax, %eax
strncat_search_loop:
	scasb
	jne strncat_search_loop
	decl %edi
strncat_send_loop:
	cmpl $0, %ecx
	je strncat_end
	decl %ecx
	lodsb
	stosb
	cmpb $0, %al
	jne strncat_send_loop
strncat_end:
	movl 8(%ebp), %eax
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### sint32 memcmp(const void *buf1, const void *buf2, uint32 size); #####
##### buf1 と buf2をsizeバイトだけ比較します(s1 > s2なら正、s1 == s2なら0、s1 < s2なら負の値) #####

memcmp:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
memcmp_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %esi
	movl 0x10(%ebp), %ecx
	# loope命令の性質上、なる文字までを調べるために+1しておく
	incl %ecx
memcmp_loop:
	cmpsb
	loope memcmp_loop
	# このxorlはfailureでもsuccessでも共通なのでここで書いておく
	xorl %eax, %eax
	cmpl $0, %ecx
	je memcmp_end
memcmp_dif:
	movb -1(%esi), %cl
	movb -1(%edi), %al
	subb %cl, %al
memcmp_end:
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### sint32 strcmp(const sint8 *s1, const sint8 *s2); #####
##### buf1 と buf2を比較します(s1 > s2なら正、s1 == s2なら0、s1 < s2なら負の値) #####

strcmp:
	pushl %ebp
	movl %esp, %ebp
	pushl %edi
	pushl %esi
	movl 0x8(%ebp), %esi
	movl 0xc(%ebp), %edi
	xorl %eax, %eax
strcmp_loop:
	lodsb
	scasb
	jne strcmp_dif
strcmp_same:
	#文字コードが0で、２つの文字がひとしいなら２つの文字列は等しい。
	cmpb $0, %al
	jne strcmp_loop
	jmp strcmp_end
strcmp_dif:
	# 今までの関数と違って、esiにs1がediにs2が入っていることに注意！
	subb -1(%edi), %al
strcmp_end:
	popl %esi
	popl %edi
	leave
	ret

##### sint32 strncmp(const sint8 *s1, const sint8 *s2, uint32 size); #####
##### buf1 と buf2をsize文字分だけ比較します(s1 > s2なら正、s1 == s2なら0、s1 < s2なら負の値) #####

strncmp:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edi
	pushl %esi
strncmp_start:
	movl 0x8(%ebp), %esi
	movl 0xc(%ebp), %edi
	movl 0x10(%ebp), %ecx
	xorl %eax, %eax
strncmp_loop:
	lodsb
	scasb
	jne strncmp_dif
strncmp_same:
	# 文字コードが0で、２つの文字がひとしいなら２つの文字列は等しい。
	cmpb $0, %al
	loopne strncmp_loop
	xorl %eax, %eax
	jmp strncmp_end
strncmp_dif:
	# 今までの関数と違って、esiにs1がediにs2が入っていることに注意！
	subb -1(%edi), %al
strncmp_end:
	popl %esi
	popl %edi
	popl %ecx
	leave
	ret

##### void *memchr(const void *s, sint32 c, uint32 size); #####
##### s が指すオブジェクトの先頭の size 文字の中で c が最初に現れる位置を探します #####

memchr:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %esi
memchr_start:
	movl 0x8(%ebp), %esi
	movl 0xc(%ebp), %edx
	movl 0x10(%ebp), %ecx
memchr_loop:
	cmpl $0, %ecx
	je memchr_not_found
	decl %ecx
	lodsb
	cmpb %al, %dl
	je memchr_found
	jmp memchr_loop
memchr_found:
	leal -1(%esi), %eax
	jmp memchr_end
memchr_not_found:
	xorl %eax, %eax
memchr_end:
	popl %esi
	popl %edx
	popl %ecx
	leave
	ret

##### sint8 *strchr(const sint8 *s, sint32 c); #####
##### s が指す文字列の中で c が最初に現れる位置を探します(なる文字が見つかると終了) #####

strchr:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
strchr_start:
	movl 0x8(%ebp), %esi
	movl 0xc(%ebp), %ecx
strchr_loop:
	lodsb
	cmpb $0, %al
	je strchr_not_found
	cmpb %cl, %al
	je strchr_found
	jmp strchr_loop
strchr_found:
	leal -1(%esi), %eax
	jmp strchr_end
strchr_not_found:
	xorl %eax, %eax
strchr_end:
	popl %esi
	popl %ecx
	leave
	ret

##### uint32 strcspn(const sint8 *s1, const sint8 *s2); #####
##### s1 が指す文字列の中で s2 が指す文字列の中の文字以外から構成される先頭部分の最大の長さを計算します #####
##### 見つからなかったときは s1 のなる文字の部分を指すオフセットが返り値として返ることになります #####

strcspn:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
strcspn_start:
	movl 0x8(%ebp), %edi
	xorl %ecx, %ecx
strcspn_check_s1:
	movb (%edi), %dl
	cmpb $0, %dl
	je strcspn_end
	incl %edi
	movl 0xc(%ebp), %esi
strcspn_check_s2:
	lodsb
	cmpb $0, %al
	je strcspn_check_s2_fin
	cmpb %dl, %al
	je strcspn_end
	jmp strcspn_check_s2
strcspn_check_s2_fin:
	incl %ecx
	jmp strcspn_check_s1
strcspn_end:
	movl %ecx, %eax
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	leave
	ret

##### sint8 *strpbrk(const sint8 *s1, const sint8 *s2); #####
##### s1 が指す文字列の中で s2 が指す文字列中のいずれかの文字が最初に現れる位置を探します #####

strpbrk:
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	pushl %esi
	pushl %edi
strpbrk_start:
	movl 0x8(%ebp), %edi
strpbrk_check_s1:
	movb (%edi), %dl
	cmpb $0, %dl
	je strpbrk_not_found
	incl %edi
	movl 0xc(%ebp), %esi
strpbrk_check_s2:
	lodsb
	cmpb $0, %al
	je strpbrk_check_s1
	cmpb %dl, %al
	je strpbrk_found
	jmp strpbrk_check_s2
strpbrk_found:
	leal -1(%edi), %eax
	jmp strpbrk_end
strpbrk_not_found:
	xorl %eax, %eax
strpbrk_end:
	popl %edi
	popl %esi
	popl %edx
	leave
	ret

##### sint8 *strrchr(const sint8 *s, sint32 c); #####
##### s が指す文字列の中で c が最後に現れる位置を探します #####

strrchr:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
strrchr_start:
	movl 0x8(%ebp), %esi
	# ediには文字列のはじめのアドレスを覚えておいてもらう(ただし、lodsbの性質上さらに-1しておく必要がある)
	movl %esi, %edi
	decl %edi
	movl 0xc(%ebp), %ecx
strrchr_cursor_to_end:
	lodsb
	cmpb $0, %al
	je strrchr_search
	jmp strrchr_cursor_to_end
strrchr_search:
	# 2文字読みすぎているので巻き戻す
	subl $2, %esi
	# 次は逆に読んでいくので、dfをセットする
	pushf
	std
strrchr_loop:
	lodsb
	cmpb %cl, %al
	je strrchr_found
	cmpl %esi, %edi
	je strrchr_not_found
	jmp strrchr_loop
strrchr_found:
	leal 1(%esi), %eax
	jmp strrchr_end
strrchr_not_found:
	xorl %eax, %eax
strrchr_end:
	popf
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### uint32 strspn(const sint8 *s1, const sint8 *s2); #####
##### s1が指す文字列の中で，s2中の文字だけを含む先頭部分の最大の長さを計算します #####

strspn:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
strspn_start:
	movl 0x8(%ebp), %edi
	xorl %ecx, %ecx
strspn_loop1:
	movb (%edi), %dl
	incl %edi
	cmpb $0, %dl
	je strspn_end
	# 検索する文字列(s2)の最初にesiを合わせる
	movl 0xc(%ebp), %esi
strspn_loop2:
	lodsb
	cmpb $0, %al
	je strspn_end
	cmpb %dl, %al
	je strspn_found
	jmp strspn_loop2
strspn_found:
	incl %ecx
	jmp strspn_loop1
strspn_end:
	movl %ecx, %eax
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	leave
	ret

##### sint8 *strstr(const sint8 *s1, const sint8 *s2); #####
##### s1が指す文字列の最初から，s2が指す文字列の文字の並びを検索する関数です #####
	
strstr:
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
strstr_start:
	movl 0x8(%ebp), %edi
strstr_loop1:
	movb (%edi), %al
	cmpb $0, %al
	je strstr_not_found
	movl 0xc(%ebp), %esi
	# s2のはじめがなる文字の場合は0を返す
	cmpb $0, (%esi)
	je strstr_not_found
	pushl %edi
strstr_loop2:
	lodsb
	cmpb $0, %al
	je strstr_found
	scasb
	jne strstr_not_eq
	jmp strstr_loop2
strstr_not_eq:
	popl %edi
	incl %edi
	jmp strstr_loop1
strstr_found:
	popl %eax
	jmp strstr_end
strstr_not_found:
	xorl %eax, %eax
strstr_end:
	popl %edi
	popl %esi
	leave
	ret

##### sint8 *strdiv(const sint8 *s1, sint8 *s2, sint8 *s3, sint32 c); #####
##### s1をcで区切った文字列の前半をs2、後半をs3に入れます #####
	
strdiv:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
	pushl %edi
strdiv_start:
	movl 0x8(%ebp), %esi
	# はじめはs2に入れていく
	movl 0xc(%ebp), %edi
	movl 0x14(%ebp), %ecx
strdiv_copy1:
	lodsb
	cmpb $0, %al
	je strdiv_failure
	cmpb %cl, %al
	je strdiv_copy1_end
	stosb
	jmp strdiv_copy1
strdiv_copy1_end:
	movb $0, %al
	stosb
	movl 0x10(%ebp), %edi
strdiv_copy2:
	lodsb
	stosb
	cmpb $0, %al
	je strdiv_success
	jmp strdiv_copy2
strdiv_success:
	movl 0x8(%ebp), %eax
	jmp strdiv_end
strdiv_failure:
	# 区切り文字が見つからなかった場合でもなる文字だけは入れておく
	stosb
	xorl %eax, %eax
strdiv_end:
	popl %edi
	popl %esi
	popl %ecx
	leave
	ret

##### void *memset(void *s, sint32 c, uint32 size); #####
##### s が指すオブジェクトの先頭から size 文字分 c をセットします #####

memset:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edi
memset_start:
	movl 0x8(%ebp), %edi
	movl 0xc(%ebp), %eax
	movl 0x10(%ebp), %ecx
memset_loop:
	stosb
	loop memset_loop
memset_end:
	movl 0x8(%ebp), %eax
	popl %edi
	popl %ecx
	leave
	ret

##### uint32 strlen(const uint8 *s); #####
##### 文字列sの長さを返します #####

strlen:
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %esi
strlen_start:
	movl 0x8(%ebp), %esi
	xorl %ecx, %ecx
strlen_loop:
	lodsb
	cmpb $0, %al
	je strlen_end
	incl %ecx
	jmp strlen_loop
strlen_end:
	movl %ecx, %eax
	popl %esi
	popl %ecx
	leave
	ret

##### ここからは.dataセクション #####

.bss

##### memmoveなどで使用する一時記憶領域 #####

temp1:					# 0x600000から並べられているみたい
	.skip 65536
