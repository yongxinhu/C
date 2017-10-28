/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "setInCache.h"
#include "cacheRead.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and fetches that block of data, 
	returning it in a uint8_t* pointer.
*/
uint8_t* fetchBlock(cache_t* cache, uint32_t blockNumber) {
	uint64_t location = getDataLocation(cache, blockNumber, 0);
	uint32_t length = cache->blockDataSize;
	uint8_t* data = malloc(sizeof(uint8_t) << log_2(length));
	if (data == NULL) {
		allocationFailed();
	}
	int shiftAmount = location & 7;
	uint64_t byteLoc = location >> 3;
	if (shiftAmount == 0) {
		for (uint32_t i = 0; i < length; i++) {
			data[i] = cache->contents[byteLoc + i];
		}
	} else {
		length = length << 3;
		data[0] = cache->contents[byteLoc] << shiftAmount;
		length -= (8 - shiftAmount);
		int displacement = 1;
		while (length > 7) {
			data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
			data[displacement] = cache->contents[byteLoc + displacement] << shiftAmount;
			displacement++;
			length -= 8;
		}
		data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
	}
	return data;
}

/*
	Takes in a cache, an address, and a dataSize and reads from the cache at
	that address the number of bytes indicated by the size. If the data block 
	is already in the cache it retrieves the contents. If the contents are not
	in the cache it is read into a new slot and if necessary something is 
	evicted.
*/
uint8_t* readFromCache(cache_t* cache, uint32_t address, uint32_t dataSize) {
	/* Your Code Here. */
    uint8_t* ret;
    reportAccess(cache);
    evictionInfo_t* evi = findEviction(cache, address);
    if (evi -> match){
        reportHit(cache);
        ret = getData(cache, getOffset(cache, address), evi->blockNumber, dataSize);
        updateLRU(cache, getTag(cache,address), getIndex(cache, address), evi -> LRU);
    }
    else{
        if (validAddresses(address, dataSize)){
            evict(cache, evi->blockNumber);
            uint32_t startAdr = address/cache->blockDataSize* cache->blockDataSize;
            uint8_t* fetchedBlock = readFromMem(cache,startAdr);
            setValid(cache, evi->blockNumber,1);
            setDirty(cache, evi->blockNumber,0);
            setShared(cache,evi->blockNumber, 0);
            setTag(cache, getTag(cache,address), evi->blockNumber);
            setData(cache, fetchedBlock, evi->blockNumber, cache->blockDataSize, 0);
            ret = getData(cache, getOffset(cache,address), evi->blockNumber, dataSize);
            free (fetchedBlock);
            updateLRU(cache, getTag(cache,address), getIndex(cache, address), evi -> LRU);
        }
    }
    free(evi);
	return ret;
}

/*
	Takes in a cache and an address and fetches a byte of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
byteInfo_t readByte(cache_t* cache, uint32_t address) {
	byteInfo_t retVal;
	/* Your Code Here. */
    uint8_t* data;
    if(validAddresses(address,1) && cache){
        retVal.success = true;
        data = readFromCache(cache, address, 1);
        retVal.data = data[0];
    }
    else{
        retVal.success = false;
        return retVal;
    }
    free(data);
	return retVal;
}

/*
	Takes in a cache and an address and fetches a halfword of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
halfWordInfo_t readHalfWord(cache_t* cache, uint32_t address) {
    byteInfo_t temp;
    halfWordInfo_t retVal;
    uint8_t* data;
	/* Your Code Here. */
    if((address % 2 == 0) && validAddresses(address,2) && cache){
        retVal.success = true;
        if (cache->blockDataSize<2){
            temp = readByte(cache,address);
            retVal.data = temp.data;
            temp = readByte(cache,address+1);
            retVal.data = (retVal.data << 8)|temp.data;
            return retVal;
        }
        data = readFromCache(cache, address, 2);
        retVal.data = data[0];
        retVal.data = (retVal.data << 8) | data[1];
        
    }
    else{
        retVal.success = false;
        return retVal;
    }
    free(data);
	return retVal;
}

/*
	Takes in a cache and an address and fetches a word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
wordInfo_t readWord(cache_t* cache, uint32_t address) {
	wordInfo_t retVal;
	/* Your Code Here. */
    
    halfWordInfo_t temp;
    uint8_t* data;
    /* Your Code Here. */
    if((address % 4 == 0) && validAddresses(address,4) && cache){
        retVal.success = true;
        if (cache->blockDataSize<4){
            temp = readHalfWord(cache,address);
            retVal.data = temp.data;
            temp = readHalfWord(cache,address+2);
            retVal.data = (retVal.data << 16)|temp.data;
            return retVal;
        }
        data = readFromCache(cache, address, 4);
        retVal.data = data[0];
        retVal.data = (retVal.data << 8) | data[1];
        retVal.data = (retVal.data << 8) | data[2];
        retVal.data = (retVal.data << 8) | data[3];
        
    }
    else{
        retVal.success = false;
        return retVal;
    }
    free(data);
    return retVal;
}

/*
	Takes in a cache and an address and fetches a double word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
doubleWordInfo_t readDoubleWord(cache_t* cache, uint32_t address) {
	doubleWordInfo_t retVal;
	/* Your Code Here. */
    wordInfo_t temp;
    uint8_t* data;
    /* Your Code Here. */
    if((address % 8 == 0) && validAddresses(address,8) && cache){
        retVal.success = true;
        if (cache->blockDataSize<8){
            temp = readWord(cache,address);
            retVal.data = temp.data;
            temp = readWord(cache,address+4);
            retVal.data = (retVal.data << 32)|temp.data;
            return retVal;
        }
        data = readFromCache(cache, address, 8);
        retVal.data = data[0];
        retVal.data = (retVal.data << 8) | data[1];
        retVal.data = (retVal.data << 8) | data[2];
        retVal.data = (retVal.data << 8) | data[3];
        retVal.data = (retVal.data << 8) | data[4];
        retVal.data = (retVal.data << 8) | data[5];
        retVal.data = (retVal.data << 8) | data[6];
        retVal.data = (retVal.data << 8) | data[7];
    }
    else{
        retVal.success = false;
        return retVal;
    }
    free(data);
    return retVal;
}
