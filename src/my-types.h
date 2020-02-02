#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define INPUT_END 1
#define OUTPUT_END 0

#define XSTR(x) STR(x)
#define STR(x) #x

typedef struct ProgramToExecute
{
	const char *title;
	char **command;
	int __command_size;
	int signal;
} program_t;

typedef struct ProgramList
{
	size_t size;
	program_t **head;
} program_list_t;

typedef struct ProcessHandle
{
	const pid_t pid;
	const int fd0;
	const int fd1;
	const program_t *source;
	volatile uint8_t quit;
} process_handle_t;
static inline void printf_stderr(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

static inline void printf_last_error()
{
	if (errno == 0)
		printf_stderr("No last error\n");
	else
		printf_stderr("Last Error: (%d) %s.\n", errno, strerror(errno));
}

static inline void die(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	if (errno == 0)
		printf_stderr(" (0)\n");
	else
		printf_stderr(": (%d) %s.\n", errno, strerror(errno));
	va_end(args);
	exit(EXIT_FAILURE);
}
static inline void *__safe_malloc(size_t size, const char *const debug_title)
{
	void *ret = malloc(size);
	if (ret == NULL)
		die("failed to malloc ( %s )", debug_title);
	errno = 0;
	return ret;
}
static inline void *__safe_realloc(void *ptr, size_t size, const char *const debug_title)
{
	void *ret = realloc(ptr, size);
	if (ret == NULL)
		die("failed to realloc ( %s )", debug_title);
	errno = 0;
	return ret;
}
static inline void *__safe_calloc(size_t count, size_t size, const char *const debug_title)
{
	void *ret = calloc(count, size);
	if (ret == NULL)
		die("failed to calloc ( %s )", debug_title);
	errno = 0;
	return ret;
}

#define smalloc(SIZE_CALC) __safe_malloc(SIZE_CALC, #SIZE_CALC)
#define srealloc(ptr, SIZE_CALC) __safe_realloc(ptr, SIZE_CALC, #SIZE_CALC)
#define scalloc(count, SIZE_CALC) __safe_calloc(count, SIZE_CALC, #count " * " #SIZE_CALC)

#define ALLOC_STRUCT(obj) memcpy(scalloc(1, sizeof(obj)), &obj, sizeof(obj))
