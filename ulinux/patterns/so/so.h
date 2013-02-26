#ifndef SO_H
#define SO_H
#define DPRINTF_BUF_SZ 1024
#ifdef SO_C
k_u8 *dprintf_buf;
#else
extern k_u8 *dprintf_buf;
#endif
extern k_i function_1(k_i a,k_i b);
extern k_i function_2(k_i a,k_i b);
#endif
