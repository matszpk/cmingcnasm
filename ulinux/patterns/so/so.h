#ifndef SO_H
#define SO_H
#define DPRINTF_BUF_SZ 1024
#ifdef SO_C
u8 *dprintf_buf;
#else
extern u8 *dprintf_buf;
#endif
extern i function_1(i a,i b);
extern i function_2(i a,i b);
#endif
