
#ifndef _MICO_REDIRECV_HEADERS
#define _MICO_REDIRECV_HEADERS

#define _STDLIB // stdlib.h
#define _STRING // string.h
#define _STDDEF
#define _STDIO
#define _CTYPE
#define _LIMITS
#define _STDARG

#define NULL 0
#define ULONG_MAX 0xFFFFFFFFul

typedef int         ssize_t; 
typedef unsigned int size_t;
typedef unsigned int ptrdiff_t;

void * memset(void * _D, int _C, unsigned int _N);
void * memmove(void * _D, const void * _S, unsigned int _N);
void * memcpy(void * _D, const void * _S, unsigned int _N);
char *strrchr(char *_S, int _C);
char *strstr(char *_S, char *_P);
char *strchr(const char *_S, int _C);
void *memchr(void *_S, int _C, size_t _N);
char *strpbrk(char *_S, const char *_P);
int memcmp(const void *s1, const void *s2, size_t n);


void* malloc(size_t size); // malloc
void* realloc(void* pv, size_t size); // realloc
void free(void* pv);     //free
void* calloc(int a, int b);     // calloc

char *   strdup(const char *);

#endif