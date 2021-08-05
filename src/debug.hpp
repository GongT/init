#pragma once

#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <cstdio>
#include <cstdarg>

#define INPUT_END 1
#define OUTPUT_END 0

#define XSTR(x) STR(x)
#define STR(x) #x

static inline void printf_stderr(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

#define ERROR_LOG(p)       \
	std::cerr << p << " "; \
	printf_last_error();   \
	std::cerr << std::endl;

static inline void printf_last_error()
{
	if (errno == 0)
		std::cerr << "no error";
	else
		std::cerr << "(" << errno << ") " << strerror(errno) << ".";
}

static inline void die(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	if (errno == 0)
		printf_stderr(". (error 0)\n");
	else
		printf_stderr(". (error %d %s).\n", errno, strerror(errno));
	va_end(args);
	exit(EXIT_FAILURE);
}
