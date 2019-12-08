//
//  CacheMemory.hpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#ifndef CacheMemory_hpp
#define CacheMemory_hpp

#include <vector>
#include <stdio.h>
#include "GenericMemory.hpp"

struct CacheBlock {
    unsigned int tag;
    bool valid;
    bool dirty;
    unsigned int accessorder; // use accesses as cycle counter for LRU
    unsigned int added; // added to use for FIFO
};

enum WritePolicy {WriteThrough, WriteBack };

class CacheMemory : public GenericMemory {
public:
    // direct mapping = set size 1
    
    // index = 2^17/(2^3 ⋅ 2^6) == total size 128k, set size 8, block size 64
    // block offset = blocksizebits.
    // Tag = (addresssizebits − blocksizebits − indexbits) = (32 − 6 − 8) = 18bits in this example
    //
    // total size=block size / set size.
    //Parameters of each cache level (total size, block size, set size, replacement policy (LRU/random), hit
    
    CacheMemory(unsigned int memorysize = 0 , int blocksize = 0, int associativity = 1, int latency = 0, WritePolicy writepolicy = WriteThrough, bool writeallocate = true);// latency in cycles
    
    int WriteMemory(unsigned int address);
    int ReadMemory(unsigned int address);
    void SetFallBack(shared_ptr<GenericMemory> nextlevel);
protected:
    unsigned int GetBlock(const unsigned int address) const;
    unsigned int GetIndex(const unsigned int address) const;
    unsigned int GetTag(const unsigned int address) const;
    virtual unsigned int LoadCacheBlock(const unsigned int address) = 0;
    int blocksize;
    int setsize;
    WritePolicy writepolicy;
    bool writeallocate;
    shared_ptr<GenericMemory> fallback;
    
    vector<vector<CacheBlock> > blocks;
};

#endif /* CacheMemory_hpp */
