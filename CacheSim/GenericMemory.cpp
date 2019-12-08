//
//  GenericMemory.cpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#include "GenericMemory.hpp"

GenericMemory::GenericMemory(unsigned int memorysize, int accesstime) : size(memorysize), accesstime(accesstime)
{
    hits = 0;
    misses = 0;
    hittime = 0;
//    int hittime;
//    int size;
    accesses = 0;
    cumulativeaccesstime = 0;
}

int GenericMemory::GetAccesses() const
{
    return accesses;
}

float GenericMemory::GetAverageAccessTime() const
{
    return cumulativeaccesstime / float(accesses);
}

int GenericMemory::GetHits() const
{
    return hits;
}

int GenericMemory::GetMisses() const
{
    return misses;
}

float GenericMemory::GetHitRate() const
{
    if ( hits == 0 ) return 0;
    return float(hits)/(hits+misses);
}
