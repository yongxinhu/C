/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "coherenceUtils.h"
#include "coherenceRead.h"
#include "../part1/utils.h"
#include "../part1/setInCache.h"
#include "../part1/getFromCache.h"
#include "../part1/cacheRead.h"
#include "../part1/cacheWrite.h"
#include "../part1/mem.h"
#include "../part2/hitRate.h"


/*
	A function which processes all cache reads for an entire cache system.
	Takes in a cache system, an address, an id for a cache, and a size to read
	and calls the appropriate functions on the cache being selected to read
	the data. Returns the data.
*/
uint8_t* cacheSystemRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size) {
	uint8_t* retVal;
	uint8_t offset;
	uint8_t* transferData;
	evictionInfo_t* dstCacheInfo;
	evictionInfo_t* otherCacheInfo;
	uint32_t evictionBlockNumber;
	cacheNode_t** caches;
	//bool otherCacheContains = false;
	cache_t* dstCache = NULL;
	caches = cacheSystem->caches;
	dstCache = getCacheFromID(cacheSystem, ID); //Selects destination cache pointer from array of caches pointers
		
	dstCacheInfo = findEviction(dstCache, address); //Finds block to evict and potential match
	evictionBlockNumber = dstCacheInfo->blockNumber;
	if (dstCacheInfo->match) {
		/*What do you do if it is in the cache?*/
		/*Your Code Here*/
        retVal = readFromCache(dstCache, address, size);
        addToSnooper(cacheSystem->snooper, address, ID, cacheSystem->blockDataSize);
        free(dstCacheInfo);
		return retVal;
	}
	uint32_t oldAddress = extractAddress(dstCache, extractTag(dstCache, evictionBlockNumber), evictionBlockNumber, 0);
	/*How do you need to update the snooper?*/
	/*How do you need to update states for what is getting evicted (don't worry about evicting this will be handled at a later step when you place data in the cache)?*/
	/*Your Code Here*/
	removeFromSnooper(cacheSystem->snooper, oldAddress, ID, cacheSystem->blockDataSize);
	if (getShared(dstCache, evictionBlockNumber)) {
        int onlyOtherID = returnIDIf1(cacheSystem->snooper, oldAddress, cacheSystem->blockDataSize);
		if (onlyOtherID != -1) {
			cache_t *onlyOtherCache = getCacheFromID(cacheSystem, onlyOtherID);
            updateState(onlyOtherCache,oldAddress,INVALID);
		}
	}
	
	/*Check other caches???*/
	/*Your Code Here*/
	int otherID = returnFirstCacheID(cacheSystem->snooper, address, cacheSystem->blockDataSize);
	if (otherID == -1) {
		/*Check Main memory?*/
        /*Your Code Here*/
        retVal = readFromCache(dstCache, address, size);
		setState(dstCache, dstCacheInfo->blockNumber, EXCLUSIVE);
    } else {
		/*What states need to be updated?*/
        /*Your Code Here*/
		cache_t *otherCache = getCacheFromID(cacheSystem, otherID);
		otherCacheInfo = findEviction(otherCache, address);
		retVal = getData(otherCache, getOffset(otherCache, address), otherCacheInfo->blockNumber, size); 

		uint8_t *tempBlock = fetchBlock(otherCache, otherCacheInfo->blockNumber);
        writeWholeBlock(dstCache, address, evictionBlockNumber, tempBlock);
		free(tempBlock);

		setState(dstCache, dstCacheInfo->blockNumber, SHARED);
        for (int i = 0; i < cacheSystem->size; i++) {
            if (caches[i]->ID != ID) {
                updateState(caches[i]->cache, address, SHARED);
			}
		}
		free(otherCacheInfo);
	}
	
	addToSnooper(cacheSystem->snooper, address, ID, cacheSystem->blockDataSize);
	free(dstCacheInfo);
	return retVal;
}

/*
	A function used to request a byte from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
byteInfo_t cacheSystemByteRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	byteInfo_t retVal = {false, NULL};
	uint8_t* data;
	/* Error Checking??*/
	if (cacheSystem == NULL || !validAddresses(address, 1) || cacheSystem->snooper == NULL)
		return retVal;
	if (getCacheFromID(cacheSystem, ID) == NULL)
		return retVal;
	retVal.success = true;
	data = cacheSystemRead(cacheSystem, address, ID, 1);
	if (data == NULL) {
		return retVal;
	}
	retVal.data = data[0];
	free(data);
	return retVal;
}

/*
	A function used to request a halfword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
halfWordInfo_t cacheSystemHalfWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	byteInfo_t temp;
	halfWordInfo_t retVal;
	uint8_t* data;
	/* Error Checking??*/
	retVal.success = false;
	if (cacheSystem == NULL || !validAddresses(address, 2) || cacheSystem->snooper == NULL)
		return retVal;
	if (address % 2 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return retVal;
	retVal.success = true;
	if (cacheSystem->blockDataSize < 2) {
		temp = cacheSystemByteRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemByteRead(cacheSystem, address + 1, ID);
		retVal.data = (retVal.data << 8) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 2);
	if (data == NULL) {
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	free(data);
	return retVal;
}


/*
	A function used to request a word from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
wordInfo_t cacheSystemWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	halfWordInfo_t temp;
	wordInfo_t retVal;
	uint8_t* data;
	/* Error Checking??*/
	retVal.success = false;
	if (cacheSystem == NULL || !validAddresses(address, 4) || cacheSystem->snooper == NULL)
		return retVal;
	if (address % 4 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return retVal;
	retVal.success = true;
	if (cacheSystem->blockDataSize < 4) {
		temp = cacheSystemHalfWordRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemHalfWordRead(cacheSystem, address + 2, ID);
		retVal.data = (retVal.data << 16) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 4);
	if (data == NULL) {
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	retVal.data = (retVal.data << 8) | data[2];
	retVal.data = (retVal.data << 8) | data[3];
	free(data);
	return retVal;
}

/*
	A function used to request a doubleword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
doubleWordInfo_t cacheSystemDoubleWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	wordInfo_t temp;
	doubleWordInfo_t retVal;
	uint8_t* data;
	/* Error Checking??*/
	retVal.success = false;
	if (cacheSystem == NULL || !validAddresses(address, 8) || cacheSystem->snooper == NULL)
		return retVal;
	if (address % 8 != 0 || getCacheFromID(cacheSystem, ID) == NULL)
		return retVal;
	retVal.success = true;
	if (cacheSystem->blockDataSize < 8) {
		temp = cacheSystemWordRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemWordRead(cacheSystem, address + 4, ID);
		retVal.data = (retVal.data << 32) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 8);
	if (data == NULL) {
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	retVal.data = (retVal.data << 8) | data[2];
	retVal.data = (retVal.data << 8) | data[3];
	retVal.data = (retVal.data << 8) | data[4];
	retVal.data = (retVal.data << 8) | data[5];
	retVal.data = (retVal.data << 8) | data[6];
	retVal.data = (retVal.data << 8) | data[7];
	free(data);
	return retVal;
}
