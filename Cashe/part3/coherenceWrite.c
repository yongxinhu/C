/* Summer 2017 */
#include "coherenceUtils.h"
#include "coherenceWrite.h"
#include "../part1/mem.h"
#include "../part1/getFromCache.h"
#include "../part1/setInCache.h"
#include "../part1/cacheWrite.h"
#include "../part1/cacheRead.h"
#include "../part2/hitRate.h"

/*
	A function which processes all cache writes for an entire cache system.
	Takes in a cache system, an address, an id for a cache, an address, a size
	of data, and a pointer to data and calls the appropriate functions on the 
	cache being selected to write to the cache. Returns 0 if the write is
	successful and otherwise returns -1.
*/

void cacheSystemWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size, uint8_t* data) {
    //The skeleton code is terrible!!!! It's in a mass!
    uint8_t* transferData;
    evictionInfo_t* dstCacheInfo;
	uint32_t evictionBlockNumber;
	cacheNode_t** caches;
	uint32_t tagVal;
	cache_t* dstCache = NULL;
    uint8_t counter = 0;
    int otherID;
	caches = cacheSystem->caches;
	dstCache = getCacheFromID(cacheSystem, ID);//Selects destination cache pointer from array of caches pointers
	tagVal = getTag(dstCache, address);
	dstCacheInfo = findEviction(dstCache, address); //Finds block to evict and potential match
	evictionBlockNumber = dstCacheInfo->blockNumber;


	if (dstCacheInfo->match) {
		/*What do you do if it is in the cache?*/
		/*Your Code Here*/
		writeToCache(dstCache, address, data, size);
	}
    else {
		uint32_t oldAddress = extractAddress(dstCache, extractTag(dstCache, evictionBlockNumber), evictionBlockNumber, 0);
		/*How do you need to update the snooper?*/
		/*How do you need to update states for what is getting evicted (don't worry about evicting this will be handled at a later step when you place data in the cache)?*/
		/*Your Code Here*/
        removeFromSnooper(cacheSystem->snooper, oldAddress,
                          ID, cacheSystem->blockDataSize);
        if (getShared(dstCache, evictionBlockNumber)) {
            int onlyOtherID = returnIDIf1(cacheSystem->snooper, oldAddress, cacheSystem->blockDataSize);
            if (onlyOtherID != -1) {
                updateState(getCacheFromID(cacheSystem, onlyOtherID),
                            oldAddress,
                            INVALID);
            }
        }
        otherID = returnFirstCacheID(cacheSystem->snooper,
                                     address, cacheSystem->blockDataSize);
		/*Check other caches???*/
        /*Your Code Here*/
        if (otherID == -1) {
			/*Check Main memory?*/
			/*Your Code Here*/
			writeToCache(dstCache, address, data, size);
        } else {
            /*What states need to be updated?*/
            /*Does anything else need to be editted?*/
            /*Your Code Here*/
            cache_t *otherCache = getCacheFromID(cacheSystem, otherID);
            evictionInfo_t* otherCacheInfo = findEviction(otherCache, address);

            uint8_t *tempBlock = fetchBlock(otherCache, otherCacheInfo->blockNumber);
            writeWholeBlock(dstCache,address, evictionBlockNumber, tempBlock);
            writeDataToCache(dstCache, address, data, size,
                             getTag(dstCache, address), dstCacheInfo);
            free(tempBlock);
            free(otherCacheInfo);
        }
        addToSnooper(cacheSystem->snooper, address,
                     ID, cacheSystem->blockDataSize);
	}
    free(dstCacheInfo);
    setState(dstCache, evictionBlockNumber, MODIFIED);
    for (int i = 0; i < cacheSystem->size; i++) {
        if (caches[i]->ID != ID) {
            updateState(caches[i]->cache, address, MODIFIED);
        }
    }


}
//void cacheSystemWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size, uint8_t* data) {
//	uint8_t* transferData;
//	evictionInfo_t* dstCacheInfo;
//	evictionInfo_t* otherCacheInfo;
//	uint32_t evictionBlockNumber;
//	uint32_t offset;
//	cacheNode_t** caches;
//	uint32_t tagVal;
//	int otherCacheContains = 0;
//	cache_t* dstCache = NULL;
//	uint8_t counter = 0;
//	caches = cacheSystem->caches;
//	dstCache = getCacheFromID(cacheSystem, ID);//Selects destination cache pointer from array of caches pointers
//	tagVal = getTag(dstCache, address);
//	dstCacheInfo = findEviction(dstCache, address); //Finds block to evict and potential match
//	evictionBlockNumber = dstCacheInfo->blockNumber;
//	if (dstCacheInfo->match) {
//		/*What do you do if it is in the cache?*/
//		/*Your Code Here*/
//
//
//
//
//	}
//	else {
//		uint32_t oldAddress = extractAddress(dstCache, extractTag(dstCache, evictionBlockNumber), evictionBlockNumber, 0);
//		/*How do you need to update the snooper?*/
//		/*How do you need to update states for what is getting evicted (don't worry about evicting this will be handled at a later step when you place data in the cache)?*/
//		/*Your Code Here*/
//
//
//
//
//
//		int val = returnFirstCacheID(cacheSystem->snooper, address, cacheSystem->blockDataSize);
//		/*Check other caches???*/
//		/*Your Code Here*/
//
//
//
//
//
//		if (!otherCacheContains) {
//			/*Check Main memory?*/
//			/*Your Code Here*/
//		}
//
//	}
//	addToSnooper(cacheSystem->snooper, address, ID, cacheSystem->blockDataSize);
//	if (otherCacheContains) {
//		/*What states need to be updated?*/
//		/*Does anything else need to be editted?*/
//		/*Your Code Here*/
//	}
//	free(dstCacheInfo);
//}



/*
	A function used to write a byte to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemByteWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t data) {
	/* Error Checking??*/
	if (cacheSystem == NULL || data == NULL || !validAddresses(address, 1) || cacheSystem->snooper == NULL)
		return -1;
	if (getCacheFromID(cacheSystem, ID) == NULL)
		return -1;
	uint8_t array[1];
	array[0] = data;
	cacheSystemWrite(cacheSystem, address, ID, 1, array);
	return 0;
}

/*
	A function used to write a halfword to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemHalfWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint16_t data) {
	/* Error Checking??*/
	if (cacheSystem == NULL || data == NULL || !validAddresses(address, 2) || cacheSystem->snooper == NULL)
		return -1;
	if (address % 2 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return -1;
	if (cacheSystem->blockDataSize < 2) {
		cacheSystemByteWrite(cacheSystem, address, ID, (uint8_t) (data >> 8));
		cacheSystemByteWrite(cacheSystem, address + 1, ID, (uint8_t) (data & UINT8_MAX));
	}
	uint8_t array[2];
	array[0] = (uint8_t) (data >> 8);
	array[1] = (uint8_t) (data & UINT8_MAX);
	cacheSystemWrite(cacheSystem, address, ID, 2, array);
	return 0;
}

/*
	A function used to write a word to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint32_t data) {
	/* Error Checking??*/
	if (cacheSystem == NULL || data == NULL || !validAddresses(address, 4) || cacheSystem->snooper == NULL)
		return -1;
	if (address % 4 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return -1;
	if (cacheSystem->blockDataSize < 4) {
		cacheSystemByteWrite(cacheSystem, address, ID, (uint8_t) (data >> 16));
		cacheSystemByteWrite(cacheSystem, address + 2, ID, (uint8_t) (data & UINT16_MAX));
	}
	uint8_t array[4];
	array[0] = (uint8_t) (data >> 24);
	array[1] = (uint8_t) ((data >> 16) & UINT8_MAX);
	array[2] = (uint8_t) ((data >> 8) & UINT8_MAX);
	array[3] = (uint8_t) (data & UINT8_MAX);
	cacheSystemWrite(cacheSystem, address, ID, 4, array);
	return 0;
}

/*
	A function used to write a doubleword to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemDoubleWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint64_t data) {
	/* Error Checking??*/
	if (cacheSystem == NULL || data == NULL || !validAddresses(address, 8) || cacheSystem->snooper == NULL)
		return -1;
	if (address % 8 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return -1;
	if (cacheSystem->blockDataSize < 8) {
		cacheSystemByteWrite(cacheSystem, address, ID, (uint8_t) (data >> 32));
		cacheSystemByteWrite(cacheSystem, address + 4, ID, (uint8_t) (data & UINT32_MAX));
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
	cacheSystemWrite(cacheSystem, address, ID, 8, array);
	return 0;
}
