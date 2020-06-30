#include<stdarg.h>
#include<basic.h>

uint32 check_param(uint8 c);
uint32 dec2asc(uint8 *str2, uint32 dec, uint32 flags);
uint32 hex2asc(uint8 *str2, uint32 dec, uint32 flags);

//%d = 符号あり10進数    %u = 符号なし10進数    %x = 符号なし16進数小文字    %X = 符号なし16進数大文字
//%m = 符号あり16進数小文字    %M = 符号あり16進数大文字    %c = 文字    %s = 文字列
//使用例 : mysprintf(str, "num = %5ad", num); →  最低5文字でnumの値を符号あり10進数で表示する。5文字に足りない場合はaで埋める。
//%dなどの前に'\\'を入れることで、そのフォーマット指定子を無効化できる

void sprintf (uint8 *str, uint8 *fmt, ...) {
	// 可変長引数
	va_list ap;
	va_start(ap, fmt);

	// fmtのアドレスが変わると後々めんどくさいかもしれないので、コピーして使用する
	uint8 *fmt2 = fmt;

	// 先読みした文字を保存しておく
	uint8 c0, c1, c2, c3;

	// いろんな制御を行うフラグ
	uint32 flags = 0;
	uint32 ignore = 0;	// '\'のあとは1、それ以外は0

	// パラメータを適用する
	while(*fmt2 != '\0'){
		// flag init
		flags = 0;

		// load character
		c0 = *(fmt2 + 0);
		c1 = *(fmt2 + 1);
		c2 = *(fmt2 + 2);
		c3 = *(fmt2 + 3);

		if(ignore == 1 && c0 == '%'){							// パラメータ指定子
			*(--str) = c0;
			str++;
			fmt2++;
		}
		else if(ignore == 0 && c0 == '%'){
			if(check_param(c1)){
				if(c1 == 'd'){
					str += dec2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c1 == 'u'){
					flags |= 0x10000;
					str += dec2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c1 == 'x'){
					flags |= 0x10000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c1 == 'X'){
					flags |= 0x30000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c1 == 'm'){
					str += hex2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c1 == 'M'){
					flags |= 0x20000;
					str += hex2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c1 == 'c'){
					*(str++) = va_arg(ap, uint32);
				}
				else if(c1 == 's'){
					char *temp_s = va_arg(ap, uint8*);
					while(*temp_s != '\0'){
						*(str++) = *temp_s++;
					}
				}
				else {
					*(str++) = c0;
					fmt2++;
				}

				fmt2 += 2;	// 2文字すすめる
			}
			else if(check_param(c2)){
				if(c2 == 'd'){
					flags |= c1 - 0x30;
					str += dec2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c2 == 'u'){
					flags |= c1 - 0x30;
					flags |= 0x10000;
					str += dec2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c2 == 'x'){
					flags |= c1 - 0x30;
					flags |= 0x10000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c2 == 'X'){
					flags |= c1 - 0x30;
					flags |= 0x30000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c2 == 'm'){
					flags |= c1 - 0x30;
					str += hex2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c2 == 'M'){
					flags |= c1 - 0x30;
					flags |= 0x20000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else {
					*(str++) = c0;
					fmt2++;
				}

				fmt2 += 3;	// 3文字すすめる
			}
			else if(check_param(c3)){
				if(c3 == 'd'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					str += dec2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c3 == 'u'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					flags |= 0x10000;
					str += dec2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c3 == 'x'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					flags |= 0x10000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c3 == 'X'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					flags |= 0x30000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else if(c3 == 'm'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					str += hex2asc(str, va_arg(ap, sint32), flags);
				}
				else if(c3 == 'M'){
					flags |= c1 - 0x30;
					flags |= c2 << 8;
					flags |= 0x20000;
					str += hex2asc(str, va_arg(ap, uint32), flags);
				}
				else {
					*(str++) = c0;
					fmt2++;
				}

				fmt2 += 4;	// 4文字すすめる
			}
			else {
				*str++ = c0;
				fmt2++;
			}
		}
		else {
			*str++ = c0;
			fmt2++;
		}

		ignore = 0;
		if(c0 == '\\'){
			ignore = 1;								// '\'かどうかを記録する
		}
	}

	*str = '\0';

	va_end(ap);
	return;
}

// 適切なパラメータ指定子かを調べる
uint32 check_param(uint8 c){
	if(c == 'd' || c == 'u' || c == 'x' || c == 'X' || \
	   c == 'm' || c == 'M' || c == 'c' || c == 's'){
		return 1;
	}

	return 0;
}

// 10進数からascii文字列に変換
// flags(0~7bit : 最低文字数、　8~15bit : 文字数が足りない場合に埋める文字コード、　16bit : 符号あり(0)・符号なし(1)、　17bit : 小文字(0)・大文字(1))
uint32 dec2asc(uint8 *str, uint32 dec, uint32 flags){
	uint32 i;
	uint32 len = 0,total = 0;
	uint8 mem[15];

	if((flags & 0x10000) == 0){
		if((dec & (1 << 31)) != 0){	//もしマイナスならば
			*str++ = '-';		//先頭にマイナスをつける
			dec ^= 0xffffffff;	//二の補数で正の数に直す
			dec += 1;
			total += 1;		//文字列が一文字長くなる。
			if((flags & 0xff) != 0){
				flags--;	//一文字すくなくなる
			}
		}
	}

	while(1){
		mem[len++] = (dec % 10) + 0x30;
		if(dec < 10){
			break;
		}
		dec /= 10;
	}
	total += len;

	if((flags & 0xff) > len){
		for(i = 0;i < ((flags & 0xff) - len);i++){
			*str++ = (flags & 0xff00) ? (flags & 0xff00) >> 8 : 0x20;
		}
		total -= len;
		total += (flags & 0xff);
	}

	while(len){
		*str++ = mem[--len];
	}
	return total;
}
 
// 16進数からascii文字列に変換
// flags(0~7bit : 最低文字数、　8~15bit : 文字数が足りない場合に埋める文字コード、　16bit : 符号あり(0)・符号なし(1)、　17bit : 小文字(0)・大文字(1))
uint32 hex2asc(uint8 *str, uint32 dec, uint32 flags){
	int i;
	int remainder;
	int len = 0,total = 0;
	char mem[15];

	if((flags & 0x10000) == 0){
		if((dec & (1 << 31)) != 0){	//もしマイナスならば
			*str++ = '-';		//先頭にマイナスをつける
			dec ^= 0xffffffff;	//二の補数で正の数に直す
			dec += 1;
			total += 1;		//文字列が一文字長くなる。
			if((flags & 0xff) != 0){
				flags--;	//一文字すくなくなる
			}
		}
	}

	while(1){
		remainder = dec % 16;
		if(remainder < 10){ 		//数字は大文字でも小文字でも変わらない
			mem[len++] = remainder + 0x30;
		}
		else if((flags & 0x20000) == 0){//小文字
			mem[len++] = (remainder - 10) + 0x61;
		}
		else {				//大文字
			mem[len++] = (remainder - 10) + 0x41;
		}
		if(dec < 16){
			break;
		}
		dec /= 16;
	}
	total += len;

	if((flags & 0xff) > len){
		for(i = 0;i < ((flags & 0xff) - len);i++){
			*str++ = (flags & 0xff00) ? (flags & 0xff00) >> 8 : 0x20;
		}
		total -= len;
		total += (flags & 0xff);
	}

	while(len){
		*str++ = mem[--len];
	}
	return total;
}
