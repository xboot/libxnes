#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

void hexdump(void (*output)(char), unsigned long base, void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __HEXDUMP_H__ */
