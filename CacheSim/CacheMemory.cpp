//
//  CacheMemory.cpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#include <iostream>
#include <cmath>
#include "CacheMemory.hpp"

CacheMemory::CacheMemory(unsigned int memorysize, int blocksize, int associativity, int latency, enum WritePolicy writepolicy, bool writeallocate ) :
    GenericMemory(memorysize, latency),
    blocksize(blocksize),
    setsize(pow(2,associativity-1)),
blocks((2<<memorysize)/(2<<blocksize) / associativity,vector<CacheBlock>(pow(2,associativity-1), {0, false, false, 0,0} ) ), // latency in cycles, sizes in bits. // set size in powers of 2.
    writepolicy(writepolicy),
    fallback(nullptr),
    writeallocate(writeallocate)
{
}

void CacheMemory::SetFallBack(shared_ptr<GenericMemory> nextlevel)
{
    fallback = nextlevel;
}


unsigned int CacheMemory::GetBlock(const unsigned int address) const
{ 
    return address >> blocksize;
}

unsigned int CacheMemory::GetIndex(const unsigned int address) const
{ // shift out non relevant bits. make mask instead?
 //   return ( address << setsize << blocksize ) >> setsize >> ( blocksize * 2);
    return ( address >> blocksize ) % blocks.size(); //blocks.size();
}

unsigned int CacheMemory::GetTag(const unsigned int address) const
{
    int tag = address >> blocksize >> setsize; // blocks.size() >>
    return tag;
}

int CacheMemory::WriteMemory(unsigned int address) //write is always a miss as main memory needs to be updated
{
    ++accesses;
    
    unsigned int penalty = 0;
    // set access still needs to be updated too
    unsigned int cacheset = GetIndex(address);//GetBlock(address);
    unsigned int tag = GetTag(address);
    
    bool hit = false;
    
//    unsigned int block = GetBlock(address);
    
    
    for (auto& block : blocks[cacheset])
        if ( block.valid && block.tag == tag )
        {
            hit = true;
            ++hits;
            block.dirty = true;
            block.accessorder = accesses;
            break; // we've found our hit, no point continuing loop.
        }
    

    if ( ! hit ) // if writeback miss, or writethrough calculate miss penalty and if necessary load cache block.
    {
        ++misses;
        
        if ( writeallocate ) // also penalty
        {
            penalty += LoadCacheBlock(address); // on miss, block is loaded from memory ( LoadCache marks the block as accessed at same time. )
        }  else
        {
            penalty += fallback->WriteMemory(address); // write passed upto next level on a miss
        }
    } else // calculate hit penalty.
        switch ( writepolicy )
         {
           case WriteThrough :
                 penalty = fallback->WriteMemory(address); // next level up needs writing to
                 break;
           case WriteBack :
                 penalty += accesstime;
                 break;
         }
    
    cumulativeaccesstime += penalty;

    return penalty;
}

int CacheMemory::ReadMemory(unsigned int address)
{
    ++accesses;
    
    int penalty = 0;
    bool hit = false;
    
    // check for cache hit
    
    unsigned int cacheset = GetIndex(address);
    unsigned int tag = GetTag(address);
    
    for (auto& block : blocks[cacheset])
        if ( block.valid && block.tag == tag )
        {
            hit = true;
            block.accessorder = accesses;
            break; // we've found our hit, no point continuing loop.
        }
    
    if ( hit )
    {
        // we have a cache hit
        ++hits;
        penalty = accesstime;
    }
    else
    {
        ++misses;
        penalty = LoadCacheBlock(address);
        // pull in new block
    }

    cumulativeaccesstime += penalty;
    
    return penalty;
}
