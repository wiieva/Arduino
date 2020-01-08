
#ifndef _GMISC_ROM_H
#define _GMISC_ROM_H

#include "../../gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GFX_ROM_DATA_ATTR
#define GFX_ROM_DATA_ATTR
#endif


#if GFX_NEED_ROM_ALIGN32_READ
	int strcmp_a32 (const char *strR1,const char *str2);
	int memcpy_a32 (void *dst,const void *src,size_t size);
	uint8_t static GFXINLINE gfxROMread8(const uint8_t *a) {
		size_t offset = ((size_t)(a) & 0x3);
		const uint32_t *ptr   = (const uint32_t*) (((const uint8_t *)(a))-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
		offset = 3 - offset;
#endif
		return *ptr >> (offset * 8);
	}
	uint16_t static GFXINLINE gfxROMread16(const uint16_t *a) {
		size_t offset = ((size_t)(a) & 0x2);
		const uint32_t *ptr   = (const uint32_t*) (((const uint8_t *)(a))-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
		offset = 2 - offset;
#endif
		return *ptr >> (offset * 8);
	}
	#define gfxROMstrcmp(a,b)   strcmp_a32 ((a),(b))
	#define gfxROMmemcpy(a,b,c) memcpy_a32 ((a),(b),(c))

#elif GFX_NEED_ROM_ALIGN16_READ
	int strcmp_a16 (const char *strR1,const char *str2);
	int memcpy_a16 (void *dst,const void *src,size_t size);
	#define gfxROMstrcmp(a,b)   strcmp_a16 ((a),(b))
	#define gfxROMmemcpy(a,b,c) memcpy_a16 ((a),(b),(c))
	uint8_t static GFXINLINE gfxROMread8(const uint8_t *a) {
		size_t offset = ((size_t)(a) & 0x1); 
		const uint16_t *ptr   = (const uint16_t*) (((const uint8_t *)(a))-offset); 
#if GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
		offset = 1 - offset;
#endif
		return *ptr >> (offset * 8);
	}
	#define gfxROMread16(a)     *((uint16_t*)a)
#else
	#define gfxROMstrcmp(a,b)   strcmp  ((a),(b))
	#define gfxROMmemcpy(a,b,c) memcpy  ((a),(b),(c))
	#define gfxROMread8(a)      *((uint8_t*)a)
	#define gfxROMread16(a)     *((uint16_t*)a)
#endif

#ifdef __cplusplus
}
#endif

#endif
