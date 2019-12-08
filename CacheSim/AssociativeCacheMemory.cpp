//
//  AssociativeCacheMemory.cpp
//  CacheSim
//
//  Created by Visa Harvey on 24.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#include "AssociativeCacheMemory.hpp"

AssociativeCacheMemory::AssociativeCacheMemory(unsigned int memorysize , int blocksize , int associativity , int latency, ReplacementPolicy replacepolicy, WritePolicy writepolicy, bool writeallocate ):
    CacheMemory(memorysize, blocksize, associativity, latency, writepolicy, writeallocate),
    replacepolicy(replacepolicy)
{
}

unsigned int AssociativeCacheMemory::LoadCacheBlock(const unsigned int address)
{
//    cout << "LoadingCacheBlock for " << address << endl;
    unsigned int cacheset = GetIndex(address);
    unsigned int tag = GetTag(address);

    CacheBlock *loadblock = &blocks[cacheset][0];
    
    // first check if there's any unallocated.
    for (auto& block : blocks[cacheset])
        if ( ! block.valid )
        {
            loadblock = &block;
            break; // we've found a free block, use it.
        }
    
    int penalty = 0;
    
    if ( loadblock->valid ) switch ( replacepolicy ) // not found a free block, choose one to replace.
    {
            case LRU :
                for (auto& block : blocks[cacheset])
                    if ( block.valid && block.accessorder < loadblock->accessorder)
                            loadblock = &block;
                break;
            case FIFO :
                for (auto& block : blocks[cacheset])
                    if ( block.valid && block.added < loadblock->added)
                        loadblock = &block;
            break;
            case Random :
                loadblock = &blocks[cacheset][rand()% blocks[cacheset].size()];
            break;
    }
    
    switch ( writepolicy )
    {
        case WriteThrough :
            // nothing to do, memory was already committed during request
            break;
        case WriteBack :
            if ( loadblock->dirty ) // there has been non committed write access to this block, it needs committing.
            {
                penalty += fallback->WriteMemory(address);
            }
            break;
    }
    
    // loadblock is now pointing to the block to replace in cache.
    
    loadblock->valid = true;
    loadblock->tag = tag;
    loadblock->added = accesses;
    loadblock->accessorder = accesses;
    // assume 64byte cache line reads?
    penalty += fallback->ReadMemory(address);
    return penalty;
}
