//
//  DirectCacheMemory.hpp
//  CacheSim
//
//  Created by Visa Harvey on 24.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#ifndef DirectCacheMemory_hpp
#define DirectCacheMemory_hpp

#include <stdio.h>
#include "CacheMemory.hpp"

class DirectCacheMemory : public CacheMemory {
public:
    // direct mapping = set size 1
    
    // index = 2^17/(2^3 ⋅ 2^6) == total size 128k, set size 8, block size 64
    // block offset = blocksizebits.
    // Tag = (addresssizebits − blocksizebits − indexbits) = (32 − 6 − 8) = 18bits in this example
    //
    // total size=block size / set size.
    //Parameters of each cache level (total size, block size, set size, replacement policy (LRU/random), hit
    
    DirectCacheMemory(unsigned int memorysize = 0, int blocksize = 0 , int latency = 0, WritePolicy writepolicy = WriteThrough, bool writeallocate = true );// latency in cycles
    
//    int WriteMemory(unsigned int address);
//    int ReadMemory(unsigned int address);
protected:
    unsigned int LoadCacheBlock(const unsigned int address);
};

#endif /* DirectCacheMemory_hpp */
