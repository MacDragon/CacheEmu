//
//  GenericMemory.hpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

#ifndef GenericMemory_hpp
#define GenericMemory_hpp

#include <stdio.h>

using namespace std;

class GenericMemory {
public:
    GenericMemory(unsigned int memorysize = 0, int accesstime = 0);
    virtual int WriteMemory(const unsigned int address) { return 0; }; // returns cycles spent
    virtual int ReadMemory(const unsigned int address) { return 0; };
    virtual void Reset() { };
    int GetAccesses() const;
    float GetAverageAccessTime() const;
    int GetHits() const;
    int GetMisses() const;
    float GetHitRate() const;
protected:
    int hittime;
    int size;
    int hits;
    int misses;
    int accesses;
    long long cumulativeaccesstime;
    int accesstime;
};

#endif /* GenericMemory_hpp */
