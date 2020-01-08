
#include "../../gfx.h"

#if GFX_NEED_ROM_ALIGN32_READ
int strcmp_a32 (const char *strR1,const char *str2) {
	size_t offset = ((size_t)strR1 & 0x3);
	const uint32_t *ptr   = (const uint32_t*) (((const uint8_t *)strR1)-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	uint32_t data   = *ptr++ << (offset * 8);

	do {
		if (((char )((data>>24) & 0xFF)) != *str2)
			return 1;
		offset = (offset + 1) & 0x3;
		if (!offset)
			data = *ptr++;
		else
			data <<= 8;
	} while (*str2++);
#else
	uint32_t data   = *ptr++ >> (offset * 8);

	do {
		if (((char )(data & 0xFF)) != *str2)
			return 1;
		offset = (offset + 1) & 0x3;
		if (!offset)
			data = *ptr++;
		else
			data >>= 8;
	} while (*str2++);
#endif
	return 0;
}

int memcpy_a32 (void *dst,const void *src,size_t size) {

	size_t i;
	uint8_t *pdst = (uint8_t *)dst;
	size_t offset = ((size_t)src & 0x3);
	const uint32_t *ptr   = (const uint32_t*)(((const uint8_t*)src)-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	uint32_t data   = *ptr++ << (offset * 8);

	for (i = 0; i < size; ++i) {
		*pdst++ = (data>>24) & 0xFF;
		offset = (offset + 1) & 0x3;
		if (!offset)
			data = *ptr++;
		else
			data <<= 8;
	}
#else
	uint32_t data   = *ptr++ >> (offset * 8);

	for (i = 0; i < size; ++i) {
		*pdst++ = data & 0xFF;
		offset = (offset + 1) & 0x3;
		if (!offset)
			data = *ptr++;
		else
			data >>= 8;
	}
#endif
	return size;
}

#endif

#if GFX_NEED_ROM_ALIGN16_READ
int strcmp_a16 (const char *strA1,const char *str2) {
	size_t offset = ((size_t)strA1 & 0x1);
	const uint16_t *ptr   = (const uint16_t*) (((const uint8_t *)strA1)-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	uint16_t data   = *ptr++ << (offset * 8);

	do {
		if (((char )((data>>8) & 0xFF)) != *str2)
			return 1;
		offset = (offset + 1) & 0x1;
		if (!offset)
			data = *ptr++;
		else
			data <<= 8;
	} while (*str2++);
#else
	uint16_t data   = *ptr++ >> (offset * 8);

	do {
		if (((char )(data & 0xFF)) != *str2)
			return 1;
		offset = (offset + 1) & 0x1;
		if (!offset)
			data = *ptr++;
		else
			data >>= 8;
	} while (*str2++);
#endif
	return 0;
}

int memcpy_a16 (void *dst,const void *src,size_t size) {

	size_t i;
	uint8_t *pdst = (uint8_t *)dst;
	size_t offset = ((size_t)src & 0x1);
	const uint16_t *ptr   = (const uint16_t*)(((const uint8_t*)src)-offset);
#if	GFX_CPU_ENDIAN == GFX_CPU_ENDIAN_BIG
	uint16_t data   = *ptr++ << (offset * 8);

	for (i = 0; i < size; ++i) {
		*pdst++ = (data>>8) & 0xFF;
		offset = (offset + 1) & 0x1;
		if (!offset)
			data = *ptr++;
		else
			data <<= 8;
	}
#else
	uint16_t data   = *ptr++ >> (offset * 8);

	for (i = 0; i < size; ++i) {
		*pdst++ = data & 0xFF;
		offset = (offset + 1) & 0x1;
		if (!offset)
			data = *ptr++;
		else
			data >>= 8;
	}
#endif
	return size;
}

#endif
