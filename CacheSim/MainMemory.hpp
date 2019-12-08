//
//  MainMemory.hpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#ifndef MainMemory_hpp
#define MainMemory_hpp

#include <stdio.h>
#include "GenericMemory.hpp"

class MainMemory : public GenericMemory {
public:
    MainMemory(int memorysize, int latency );// latency in cycles
    
    int WriteMemory(const unsigned int address);
    int ReadMemory(const unsigned int address);
};

#endif /* MainMemory_hpp */
