//
//  MainMemory.cpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#include "MainMemory.hpp"


MainMemory::MainMemory(int memorysize, int latency ): GenericMemory(memorysize, latency)// latency in cycles
{
}

int MainMemory::WriteMemory(const unsigned int address)
{
    ++accesses;
    ++hits;
    cumulativeaccesstime += accesstime;
    return accesstime;
}

int MainMemory::ReadMemory(const unsigned int address)
{
    ++accesses;
    ++hits;
    cumulativeaccesstime += accesstime;
    return accesstime;
}


