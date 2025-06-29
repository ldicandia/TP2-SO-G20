// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <stdint.h>
#include <stddef.h>

void *memset(void *destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t) c;
	char *dst	= (char *) destination;

	while (length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t) destination % sizeof(uint32_t) == 0 &&
		(uint64_t) source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0) {
		uint32_t *d		  = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *) source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else {
		uint8_t *d		 = (uint8_t *) destination;
		const uint8_t *s = (const uint8_t *) source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

unsigned int log(uint64_t n, int base) {
	unsigned int count = 0;
	while (n /= base)
		count++;
	return count;
}

int itoa(uint64_t n, char *buffer, int base) {
	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	unsigned int len = 0;
	int i			 = 0;
	// Mover a otra funcion si se quiere implementar un itoa que soporte
	// negativos
	/*if (n < 0 && base == 10)
	{
		n = -n;
		buffer[i] = '-';
		len++;
		i++;
	}*/

	len += log(n, base) + 1;
	while (n != 0) {
		int r				  = n % base;
		buffer[len - i++ - 1] = (r > 9) ? (r - 10) + 'A' : r + '0';
		n /= base;
	}
	buffer[i] = '\0';
	return len;
}

int strtoi(char *s, char **end) {
	int num = 0;
	while (*s >= '0' && *s <= '9')
		num = num * 10 + *(s++) - '0';
	*end = s;
	return num;
}

int stringArrayLen(char **array) {
	int len = 0;
	while (*(array++) != NULL) {
		len++;
	}
	return len;
}

int strcpychar(char *dest, const char *origin, char limit) {
	int idx = 0;
	while (origin[idx] != limit && origin[idx] != '\0') {
		dest[idx] = origin[idx];
		idx++;
	}
	dest[idx] = 0;
	return idx;
}

// TODO: Ver de mover a un modulo de strings separado
int strcpy(char *dest, const char *origin) {
	return strcpychar(dest, origin, '\0');
}

int strlen(const char *str) {
	int len = 0;
	while (*(str++) != '\0')
		len++;
	return len;
}
