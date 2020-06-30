#include<basic.h>
#include<asmfunc.h>

#define EFLAGS_AC_BIT           0x00040000
#define CR0_CACHE_DISABLE       0x60000000

#define PAT0 0xaa55aa55
#define PAT1 0x55aa55aa

uint32 memtest_sub(uint32 start, uint32 end){
        uint32 i, *p, old;

        for (i = start; i <= end; i += 0x1000) {
                p = (uint32 *) (i + 0xffc);
                old = *p;			/* いじる前の値を覚えておく */
                *p = PAT0;			/* ためしに書いてみる */
                *p ^= 0xffffffff;		/* そしてそれを反転してみる */
                if (*p != PAT1) {		/* 反転結果になったか？ */
	not_memory:
                        *p = old;
                        break;
                }
                *p ^= 0xffffffff;		/* もう一度反転してみる */
                if (*p != PAT0) {		/* 元に戻ったか？ */
                        goto not_memory;
                }
                *p = old;			/* いじった値を元に戻す */
        }

        return i;
}

uint32 memtest(uint32 start, uint32 end){
        sint8 flg486 = 0;
        uint32 eflg, cr0, i;

	/* 386か、486以降なのかの確認 */
        eflg = io_load_eflags();
        eflg |= EFLAGS_AC_BIT;			/* AC-bit = 1 */
        io_store_eflags(eflg);
        eflg = io_load_eflags();
        if ((eflg & EFLAGS_AC_BIT) != 0) {	/* 386ではAC=1にしても自動で0に戻ってしまう */
                flg486 = 1;
        }
        eflg &= ~EFLAGS_AC_BIT;			/* AC-bit = 0 */
        io_store_eflags(eflg);

        if (flg486 != 0) {
                cr0 = load_cr0();
                cr0 |= CR0_CACHE_DISABLE;	/* キャッシュ禁止 */
                store_cr0(cr0);
        }

        i = memtest_sub(start, end);

        if (flg486 != 0) {
                cr0 = load_cr0();
                cr0 &= ~CR0_CACHE_DISABLE;	/* キャッシュ許可 */
                store_cr0(cr0);
        }

        return i;
}
