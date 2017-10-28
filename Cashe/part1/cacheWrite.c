/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "setInCache.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and evicts the block at that number
	from the cache. This does not change any of the bits in the cache but 
	checks if data needs to be written to main memory or and then makes 
	calls to the appropriate functions to do so.
*/
void evict(cache_t* cache, uint32_t blockNumber) {
	uint8_t valid = getValid(cache, blockNumber);
	uint8_t dirty = getDirty(cache, blockNumber);
	if (valid && dirty) {
		uint32_t tag = extractTag(cache, blockNumber);
		uint32_t address = extractAddress(cache, tag, blockNumber, 0);
		writeToMem(cache, blockNumber, address);
	}
}

/*
	Takes in a cache, an address, a pointer to data, and a size of data
	and writes the updated data to the cache. If the data block is already
	in the cache it updates the contents and sets the dirty bit. If the
	contents are not in the cache it is written to a new slot and 
	if necessary something is evicted from the cache.
*/
void writeToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize) {
    /* Your Code Here. */
    reportAccess(cache);
    evictionInfo_t* evi = findEviction(cache,address);
    if (evi->match){
        reportHit(cache);
        writeDataToCache(cache, address, data, dataSize, getTag(cache,address), evi);
    }
    else{
        if (validAddresses(address, dataSize)){
            evict(cache, evi->blockNumber);
            uint32_t startAdr = address/cache->blockDataSize* cache->blockDataSize;
            uint8_t* originalMem = readFromMem(cache,startAdr);
            setTag (cache,getTag(cache,startAdr),evi->blockNumber);
            writeDataToCache(cache, startAdr, originalMem, cache->blockDataSize, getTag(cache,startAdr), evi);
            setData(cache,data,evi->blockNumber,dataSize,getOffset(cache,address));
            free(originalMem);
        }
    }
    free(evi);
}

/*
	Takes in a cache, an address to write to, a pointer containing the data
	to write, the size of the data, a tag, and a pointer to an evictionInfo
	struct and writes the data given to the cache based upon the location
	given by the evictionInfo struct.
*/
void writeDataToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize, uint32_t tag, evictionInfo_t* evictionInfo) {
	uint32_t idx = getIndex(cache, address);
	setData(cache, data, evictionInfo->blockNumber, dataSize , getOffset(cache, address));
	setDirty(cache, evictionInfo->blockNumber, 1);
	setValid(cache, evictionInfo->blockNumber, 1);
	setShared(cache, evictionInfo->blockNumber, 0);
	updateLRU(cache, tag, idx, evictionInfo->LRU);
}

/*
	Takes in a cache, an address, and a byte of data and writes the byte
	of data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns -1
	if the address is invalid, otherwise 0.
*/
int writeByte(cache_t* cache, uint32_t address, uint8_t data) {
	/* Your Code Here. */
    uint8_t array[1];
    array[0] = data;
    if (!validAddresses(address, 1) || cache == NULL){
        return -1;
    }
    writeToCache(cache, address, array, 1);
	return 0;
}

/*
	Takes in a cache, an address, and a halfword of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeHalfWord(cache_t* cache, uint32_t address, uint16_t data) {
	/* Your Code Here. */
    if(!((address % 2 == 0) && validAddresses(address,2))){
        return -1;
    }
    if (cache->blockDataSize < 2 || cache == NULL) {
        writeByte(cache, address, (uint8_t) (data >> 8));
        writeByte(cache, address + 1, (uint8_t) (data & UINT8_MAX));
    }
    uint8_t array[2];
    array[0] = (uint8_t) (data >> 8);
    array[1] = (uint8_t) (data & UINT8_MAX);
    writeToCache(cache, address, array, 2);
	return 0;
}

/*
	Takes in a cache, an address, and a word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeWord(cache_t* cache, uint32_t address, uint32_t data) {
	/* Your Code Here. */
    if(!((address % 4 == 0) && validAddresses(address,4))){
        return -1;
    }
    if (cache->blockDataSize < 4 || cache == NULL) {
        writeHalfWord(cache, address, (uint16_t) (data >> 16));
        writeHalfWord(cache, address + 2, (uint16_t) (data & UINT16_MAX));
    }
    uint8_t array[4];
    array[0] = (uint8_t) (data >> 24);
    array[1] = (uint8_t) ((data >> 16) & UINT8_MAX);
    array[2] = (uint8_t) ((data >> 8) & UINT8_MAX);
    array[3] = (uint8_t) (data & UINT8_MAX);
    writeToCache(cache, address, array, 4);
    return 0;
}

/*
	Takes in a cache, an address, and a double word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid address
	was used.
*/
int writeDoubleWord(cache_t* cache, uint32_t address, uint64_t data) {
	/* Your Code Here. */
    if(!((address % 8 == 0) && validAddresses(address,8))){
        return -1;
    }
    if (cache->blockDataSize < 8 || cache == NULL) {
        writeWord(cache, address, (uint32_t) (data >> 32));
        writeWord(cache, address + 4, (uint32_t) (data & UINT32_MAX));
    }
    uint8_t array[8];
    array[0] = (uint8_t) (data >> 56);
    array[1] = (uint8_t) ((data >> 48) & UINT8_MAX);
    array[2] = (uint8_t) ((data >> 40) & UINT8_MAX);
    array[3] = (uint8_t) ((data >> 32) & UINT8_MAX);
    array[4] = (uint8_t) ((data >> 24) & UINT8_MAX);
    array[5] = (uint8_t) ((data >> 16) & UINT8_MAX);
    array[6] = (uint8_t) ((data >> 8) & UINT8_MAX);
    array[7] = (uint8_t) (data & UINT8_MAX);
    writeToCache(cache, address, array, 8);
    return 0;
}

/*
	A function used to write a whole block to a cache without pulling it from
	physical memory. This is useful to transfer information between caches
	without needing to take an intermediate step of going through main memory,
	a primary advantage of MOESI. Takes in a cache to write to, an address
	which is being written to, the block number that the data will be written
	to and an entire block of data from another cache.
*/
void writeWholeBlock(cache_t* cache, uint32_t address, uint32_t evictionBlockNumber, uint8_t* data) {
	uint32_t idx = getIndex(cache, address);
	uint32_t tagVal = getTag(cache, address);
	int oldLRU = getLRU(cache, evictionBlockNumber);
	evict(cache, evictionBlockNumber);
	setValid(cache, evictionBlockNumber, 1);
	setDirty(cache, evictionBlockNumber, 0);
	setTag(cache, tagVal, evictionBlockNumber);
	setData(cache, data, evictionBlockNumber, cache->blockDataSize, 0);
	updateLRU(cache, tagVal, idx, oldLRU);
}
