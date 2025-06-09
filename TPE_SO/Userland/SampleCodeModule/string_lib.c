// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include <string_lib.h>
#include <stddef.h>

int strchr(const char *str, char c) {
	while (*str) {
		if (*str == c) {
			return 1;
		}
		str++;
	}
	return 0;
}

char *strchrAndReturn(const char *str, char c) {
	while (*str) {
		if (*str == c) {
			return (char *) str;
		}
		str++;
	}
	return NULL;
}

char *strtok(char *str, const char *delim) {
	static char *last = NULL;
	if (str == NULL) {
		str = last;
	}
	if (str == NULL) {
		return NULL;
	}
	while (*str && strchr(delim, *str)) {
		str++;
	}
	if (*str == '\0') {
		last = NULL;
		return NULL;
	}
	char *start = str;
	while (*str && !strchr(delim, *str)) {
		str++;
	}
	if (*str) {
		*str++ = '\0';
	}
	last = str;
	return start;
}

int strlen(const char *str) {
	int len = 0;
	while (str[len] != '\0') {
		len++;
	}
	return len;
}

int atoi(const char *str) {
	int res	 = 0;
	int sign = 1;

	if (*str == '-') {
		sign = -1;
		str++;
	}

	while (*str >= '0' && *str <= '9') {
		res = res * 10 + (*str - '0');
		str++;
	}

	return sign * res;
}

