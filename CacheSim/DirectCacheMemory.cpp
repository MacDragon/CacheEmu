//
//  DirectCacheMemory.cpp
//  CacheSim
//
//  Created by Visa Harvey on 24.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#include <iostream>
#include "DirectCacheMemory.hpp"

DirectCacheMemory::DirectCacheMemory(unsigned int memorysize, int blocksize, int latency, WritePolicy writethrough, bool writeallocate):
    CacheMemory(memorysize, blocksize, 1, latency, writethrough, writeallocate)
{
    if ( memorysize < blocksize ) throw "Memorysize smaller than blocksize";
//    cout << "Created Cache with " << blocks.size() << " blocks"<< endl;
}

unsigned int DirectCacheMemory::LoadCacheBlock(const unsigned int address)
{
    unsigned int cacheset = GetIndex(address);
    unsigned int tag = GetTag(address);
    unsigned int line = 0; // need to decide which line to load into.
    
    blocks[cacheset][line].valid = true;
    blocks[cacheset][line].tag = tag;
    blocks[cacheset][line].added = accesses;
    // assume 64byte cache line reads?
    int penalty = fallback->ReadMemory(address);
    return penalty;
}
