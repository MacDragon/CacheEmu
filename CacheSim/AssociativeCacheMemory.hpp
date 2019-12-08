//
//  AssociativeCacheMemory.hpp
//  CacheSim
//
//  Created by Visa Harvey on 24.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#ifndef AssociativeCacheMemory_hpp
#define AssociativeCacheMemory_hpp

#include <iostream>
#include <stdio.h>
#include "CacheMemory.hpp"

enum ReplacementPolicy { LRU, FIFO, Random, NotSet };

class AssociativeCacheMemory : public CacheMemory {
public:
    AssociativeCacheMemory(unsigned int memorysize = 0 , int blocksize = 0 , int associativity = 0 , int latency = 0, ReplacementPolicy replacepolicy = LRU, WritePolicy writepolicy = WriteBack, bool writeallocate = true );
protected:
    ReplacementPolicy replacepolicy;
    unsigned int LoadCacheBlock(const unsigned int address);
};

#endif /* AssociativeCacheMemory_hpp */
