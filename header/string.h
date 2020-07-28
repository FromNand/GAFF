#ifndef _STRING
#define _STRING

#include<datatype.h>

// NULL
#define NULL ((void*)0)

// コピー関数
//void *memcpy(void *buf1, const void *buf2, uint32 size);
//void *memmove(void *buf1, const void *buf2, uint32 size);
void *memcpy(uint8 *buf1, const uint8 *buf2, uint32 size);
void *memmove(uint8 *buf1, const uint8 *buf2, uint32 size);
sint8 *strcpy(sint8 *s1, const sint8 *s2);
sint8 *strncpy(sint8 *s1, const sint8 *s2, uint32 size);

// 連結関数
sint8 *strcat(sint8 *s1, const sint8 *s2);
sint8 *strncat(sint8 *s1, const sint8 *s2, uint32 size);

// 比較関数
sint32 memcmp(const void *buf1, const void *buf2, uint32 size);
sint32 strcmp(const sint8 *s1, const sint8 *s2);
sint32 strncmp(const sint8 *s1, const sint8 *s2, uint32 size);

// 探索関数(strdivは自分で考えついた)
void *memchr(const void *s, sint32 c, uint32 size);
sint8 *strchr(const sint8 *s, sint32 c);
uint32 strcspn(const sint8 *s1, const sint8 *s2);
sint8 *strpbrk(const sint8 *s1, const sint8 *s2);
sint8 *strrchr(const sint8 *s, sint32 c);
uint32 strspn(const sint8 *s1, const sint8 *s2);
sint8 *strstr(const sint8 *s1, const sint8 *s2);
sint8 *strdiv(const sint8 *s1, sint8 *s2, sint8 *s3, sint32 c);

// その他の関数
void *memset(void *s, sint32 c, uint32 size);
uint32 strlen(const uint8 *s);

#endif
