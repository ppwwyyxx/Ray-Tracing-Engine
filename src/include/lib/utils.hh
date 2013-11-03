// File: utils.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include <cstdarg>
#include <cstdlib>
#include <type_traits>
#include <string>
#include <sstream>

std::string TERM_COLOR(int k);

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

typedef double real_t;
const real_t EPS = 1e-6;

inline real_t sqr(real_t x) { return x * x; }

#define BETW(a, b, c) ((a >= b) && (a <= c))
#define REP(x, y) for (auto x = decltype(y){0}; x < (y); x ++)
#define REPL(x, y, z) for (auto x = decltype(z){y}; x < (z); x ++)
#define REPD(x, y, z) for (auto x = decltype(z){y}; x >= (z); x --)

#define P(a) std::cout << (a) << std::endl
#define PP(a) std::cout << #a << " " << (a) << std::endl

template <typename T>
inline void free_2d(T** ptr, int w) {
	if (ptr != nullptr)
		for (int i = 0; i < w; i ++)
			delete[] ptr[i];
	delete[] ptr;
}

template<typename T>
bool update_min(T &dest, const T &val) {
	if (val < dest) { dest = val; return true; }
	return false;
}

template<typename T>
bool update_max(T &dest, const T &val) {
	if (dest < val) { dest = val; return true; }
	return false;
}


void c_printf(const char* col, const char* fmt, ...);

void c_fprintf(const char* col, FILE* fp, const char* fmt, ...);

inline std::string string_format(const char* fmt, ...) {
	int size = 512;
	char* buffer = 0;
	buffer = new char[size];
	va_list ap;
	va_start(ap, fmt);
	int nsize = vsnprintf(buffer, size, fmt, ap);
	if(size <= nsize){ //fail delete buffer and try again
		delete[] buffer;
		buffer = 0;
		buffer = new char[nsize + 1]; //+1 for /0
		nsize = vsnprintf(buffer, size, fmt, ap);
	}
	std::string ret(buffer);
	va_end(ap);
	delete[] buffer;
	return ret;
}

inline void print_progress(int percent) {
	printf("progress: %d %%\r", percent);
	fflush(stdout);
}
