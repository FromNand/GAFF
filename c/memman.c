#include<basic.h>

#define FREE_NUM 256

/*struct FREE_INFO {
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
};*/

// メモリ管理関数の初期化を行う
void memman_init(struct MEMMAN *memman){
	memman->alloc_num = 0;
	memman->alloc_size = 0;
	memman->free_num = 0;
	memman->free_size = 0;
	memman->lost_size = 0;
	memman->losts = 0;
}

// 空きサイズの合計を返す
uint32 memman_total(struct MEMMAN *memman){
	uint32 i, size = 0;

	for(i = 0; i < memman->free_num; i++){
		size += memman->free_info[i].size;
	}

	return size;
}

// メモリを確保する
uint32 memman_alloc(struct MEMMAN *memman, uint32 size){
	uint32 i;
	uint32 alloc_addr;

	// free_numは空き領域の数
	for(i = 0; i < memman->free_num; i++){
		// もし見つかったら...
		if(memman->free_info[i].size >= size){
			memman->alloc_num++;				// 確保領域のカウントを1進める
			memman->alloc_size += size;			// 確保領域の合計にsize足す
			alloc_addr = memman->free_info[i].addr;		// 確保されたアドレスの先頭を記憶する
			memman->free_info[i].addr += size;		// 先頭からsizeバイト分だけ確保されているので、addrにsize分だけ加算する
			memman->free_info[i].size -= size;		// size分だけ領域を減らす

			// もし、上の処理でfree側のサイズが0になったら前に詰める
			if(memman->free_info[i].size == 0){
				memman->free_num--;			// 空き領域の数を一つ減らす
				memman->free_size -= size;		// 空き領域の合計サイズがsize分だけ減る
				for(; i < memman->free_num; i++){
					memman->free_info[i] = memman->free_info[i+1];
				}
			}

			return alloc_addr;				// 確保した領域の先頭のアドレスを返す
		}
	}

	return 0;	// 確保できなかった...
}

// メモリを開放する
sint32 memman_free(struct MEMMAN *memman, uint32 addr, uint32 size){
	uint32 i, j;

	// アドレスは順番に並んでいたほうがいいので
	for(i = 0; i < memman->free_num; i++){
		if(memman->free_info[i].addr > addr){
			break;
		}
	}

	// 今、memman->free_info[i-1].addr < addr < memman->free_info[i].addr の状態になっている(ただし、0byteの領域がないのなら)

	// 前と繋げられるかどうか
	if(i > 0){
		// 前と繋げられる
		if(memman->free_info[i-1].addr + memman->free_info[i-1].size == addr){
			memman->free_size += size;		// 開放領域のサイズがsize分だけ増える
			memman->free_info[i-1].size += size;	// 前の領域につなげてしまう

			// 後ろもある
			if(i < memman->free_num){
				// 後ろも繋げられる
				if(addr + size == memman->free_info[i].addr){
					memman->free_info[i-1].size += memman->free_info[i].size;	// i-1の領域にiの領域もつなげてしまう
					memman->free_num--;						// 空き領域の数が一つ減る
					for(; i < memman->free_num; i++){
						memman->free_info[i] = memman->free_info[i+1];		// iの領域がなくなるので、ずらす
					}
				}
			}

			return 0;	// 成功
		}
	}

	// 前とは繋げられなかったが、後ろとは繋げられるか
	if(i < memman->free_num){
		// 後ろと繋げられる
		if(addr + size == memman->free_info[i].addr){
			memman->free_size += size;		// 空き領域の合計サイズにsize足す
			memman->free_info[i].addr = addr;	// 後ろとまとめる
			memman->free_info[i].size += size;	// 後ろのサイズにsize足す
			return 0;				// 成功
		}
	}

	// 前とも後ろともまとめられない
	if(memman->free_num < FREE_NUM){	// 空き領域の最大値を超えていないなら
		memman->free_num++;		// 空き領域に1を足す
		memman->free_size += size;	// 空き領域のサイズにsizeを加算する

		// 空き領域を後ろにずらして隙間を作る
		for(j = memman->free_num; j > i; j--){
			memman->free_info[j] = memman->free_info[j-1];
		}

		// 間に新しい空き領域を差し込む
		memman->free_info[i].addr = addr;
		memman->free_info[i].size = size;

		return 0;	// 成功	
	}

	// うまく差し込めなかった	
	memman->losts++;
	memman->lost_size += size;
	return -1;
}
