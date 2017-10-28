#ifndef _MEMCHECK_H_
#define _MEMCHECK_H_

#include<stdio.h>
#include<stdlib.h>
#define MEMCHECK
void *malloc_debug(size_t);
void *calloc_debug(size_t,size_t);
void free_debug(void *);
void mem_start_region();
void mem_end_region();
void mem_show_result();

extern int MEMDBG_AllocatedCount,MEMDBG_FreedCount;
extern int MEMDBG_AllocatedByte,MEMDBG_FreedByte;
extern int MEMDBG_AllocatedCountLocal,MEMDBG_FreedCountLocal;
extern int MEMDBG_AllocatedByteLocal,MEMDBG_FreedByteLocal;

#ifdef MEMCHECK
#define malloc malloc_debug
#define calloc calloc_debug
#define free free_debug
#endif

#endif
