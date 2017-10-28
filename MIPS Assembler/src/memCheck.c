#include "memCheck.h"

int MEMDBG_AllocatedCount = 0,MEMDBG_FreedCount = 0;
int MEMDBG_AllocatedByte = 0,MEMDBG_FreedByte = 0;
int MEMDBG_AllocatedCountLocal = 0,MEMDBG_FreedCountLocal = 0;
int MEMDBG_AllocatedByteLocal = 0,MEMDBG_FreedByteLocal = 0;
int MEMDBG_LocalOn = 0;
#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef free
#undef free
#endif
void *malloc_debug(size_t sz){
	void *ret = malloc(sz + sizeof(size_t));
	size_t *pSz;
	if(ret == NULL) return NULL;
	pSz = (size_t *)ret;
	*pSz = sz;
	++pSz;
	MEMDBG_AllocatedByte += sz;
	MEMDBG_AllocatedByteLocal += sz;
	MEMDBG_AllocatedCount += 1;
	MEMDBG_AllocatedCountLocal += 1;
	return pSz;
}
void *calloc_debug(size_t n,size_t sz){
    sz *= n;
    void *ret = calloc(1,sz + sizeof(size_t));
    size_t *pSz;
    if(ret == NULL) return NULL;
    pSz = (size_t *)ret;
    *pSz = sz;
    ++pSz;
    MEMDBG_AllocatedByte += sz;
    MEMDBG_AllocatedByteLocal += sz;
    MEMDBG_AllocatedCount += 1;
    MEMDBG_AllocatedCountLocal += 1;
    return pSz;
}

void free_debug(void *p){
	size_t *pSz = (size_t *)p;
	--pSz;
	size_t sz = *pSz;
	free(pSz);
	MEMDBG_FreedByte += sz;
	MEMDBG_FreedCount += 1;
	MEMDBG_FreedByteLocal += sz;
	MEMDBG_FreedCountLocal += 1;
}
void mem_start_region(){
	if(MEMDBG_LocalOn == 1) return;
	MEMDBG_LocalOn = 1;
	MEMDBG_AllocatedCountLocal = 0;
	MEMDBG_FreedCountLocal = 0;
    MEMDBG_AllocatedByteLocal = 0;
	MEMDBG_FreedByteLocal = 0;
}
void mem_end_region(){
	if(MEMDBG_LocalOn == 0) return;
	MEMDBG_LocalOn = 0;
	printf("------------   memcheck local report   ---------------\n\n");
	printf("\tallocated %d times , freed %d times\n",MEMDBG_AllocatedCountLocal,MEMDBG_FreedCountLocal);
	printf("\tallocated %d bytes , freed %d bytes\n\n",MEMDBG_AllocatedByteLocal,MEMDBG_FreedByteLocal);
}
void mem_show_result()
{
	printf("------------   memcheck global report   ---------------\n\n");
	printf("\tallocated %d times , freed %d times\n",MEMDBG_AllocatedCount,MEMDBG_FreedCount);
	printf("\tallocated %d bytes , freed %d bytes\n",MEMDBG_AllocatedByte,MEMDBG_FreedByte);
	printf("\n------------------------------------------------------\n");
}
#ifdef MEMCHECK
#define malloc malloc_debug
#define calloc calloc_debug
#define free free_debug
#endif

