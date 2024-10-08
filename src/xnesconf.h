#ifndef __XNES_CONF_H__
#define __XNES_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef xnes_malloc
#define xnes_malloc		malloc
#endif

#ifndef xnes_free
#define xnes_free		free
#endif

#ifndef xnes_memcpy
#define xnes_memcpy		memcpy
#endif

#ifndef xnes_memset
#define xnes_memset		memset
#endif

#ifndef xnes_printf
#define xnes_printf		printf
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XNES_CONF_H__ */
