//
//  main.cpp
//  CacheSim
//
//  Created by Visa Harvey on 23.11.2019.
//  Copyright © 2019 Visa Harvey. All rights reserved.
//

// The program must ask user to enter the following information:
// Number of data cache levels
// Parameters of each cache level (total size, block size, set size, replacement policy (LRU/random), hit time)
// Main memory access time

//The program must have the following commands:
//  Setup caches

//  Read and process trace file from the disk

//  Print a report of each memory hierarchy level:
//      average access time
//      number of hits and misses o hit ratio

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cmath>
//#include <stdio.h>
#include <filesystem>
#include <regex>

#include "GenericMemory.hpp"
#include "MainMemory.hpp"
#include "DirectCacheMemory.hpp"
#include "AssociativeCacheMemory.hpp"

using namespace std;

//#define quickdebug

#define PowerException 2 // input errors.
#define RangeException 1

#define SetupMemory 1 // menu key definitions.
#define DoTrace 2
#define ResetStats 3
#define Report 4
#define Quit 'Q'


int Powerof2(unsigned int n) // returns which power of 2 number is.
{
    return (int)log2(n); ;
}

bool IsPowerOfTwo(unsigned int x) // checks if a number is a power of 2.
{
    return x > 0 && (x & (x - 1)) == 0;
}

int readint(string prompt, bool pow2check = true, int max = 1024) // reads in integer, with optional power of 2 check and maximum range.
{
    string input;
    int result = 0;
    bool intok = false;
    do {
        cout << prompt;
        getline(cin, input);
        if ( input == "" ) {
            cout << "Exiting input." << endl;
            return -1;
        };
        try {
            result = stoi(input);
            if ( pow2check ) if ( !IsPowerOfTwo(result) ) throw PowerException;
            if ( result < 1 && result > max ) throw RangeException;
            intok = true; // all checks passed, allow proceeding out of loop.
        }
        catch(int i) // if there was any issue converting string to valid number, process exception.
        {
            switch ( i )
            {
                case PowerException:
                    cout << "Number entered not a valid power of 2, please try again." << endl;
                    break;
                case RangeException:
                    cout << "Input out of acceptable range." << endl;
                default:
                    cout << "Unknown Exception thrown." << endl;
            }
        }
        catch(...) {
            cout << "Invalid number entered, please try again." << endl;
        }

    } while ( !intok );
    return result;
    
}

shared_ptr<CacheMemory> SetupCache(int level) // setup a cache level, takes level as input for display purposes.
{
#ifdef quickdebug
    switch ( level ){
        case 3 : return make_shared<DirectCacheMemory>(20, 6, 20, WriteBack, false);
        case 2 : return make_shared<AssociativeCacheMemory>(14, 6, 2, 10, LRU, WriteBack, false);
        case 1 : return make_shared<AssociativeCacheMemory>(10, 6, 2, 2, LRU, WriteBack, false);
        default: return nullptr;
    }
#endif
    
    cout << endl << "Please enter details for Lv" << level << " Cache. ( blank value to quit )" << endl;
    
    int memsize;
    int blocksize;
    int associativity;
    int latency;
    ReplacementPolicy reppol;
    //writeallocate
  
    memsize = readint("Cache Size ( kB ):");
    if ( memsize < 0 ) return nullptr;
    memsize = Powerof2(memsize*1024); // make memsize in kB
    
    do {
        blocksize = readint("Block Size ( B ):");
        if ( blocksize < 0 ) return nullptr;
        blocksize = Powerof2(blocksize);
        if ( blocksize >= memsize )
        {
            cout << "Block size must be smaller than memory size" << endl;
            blocksize = 0;
        }
    } while ( blocksize == 0 );

    associativity = readint("Set Associativity Size ( set size ):");
    if ( associativity < 0 ) return nullptr;
    
    latency = readint("Cache Latency ( cycles ):", false);
    if ( latency < 0 ) return nullptr;
    
    cout << endl;
    
    if ( associativity > 1 )
    {
        // get LRU policy
        reppol = NotSet;
        
        do {
            int choice;
            
            cout << "Replacement Policy." << endl;
            cout << "  1: LRU" << endl;
            cout << "  2: FIFO" << endl;
            cout << "  3: Random" << endl << endl;
            
            choice = readint("Choice:", false, 3);
            switch ( choice ) {
                case -1: return nullptr; // nothing was chosen, quit setup.
                case 1: reppol = LRU; break;
                case 2: reppol = FIFO; break;
                case 3: reppol = Random; break;
                default : break;
            }
        } while ( reppol == NotSet );
           
        return shared_ptr<AssociativeCacheMemory>(new AssociativeCacheMemory(memsize, blocksize, associativity, latency, reppol, WriteBack, false));
    } else
        return shared_ptr<DirectCacheMemory>(new DirectCacheMemory(memsize, blocksize, latency, WriteBack, false));
}


void DisplayLevel(shared_ptr<GenericMemory> memory, int level)
{
    cout << "Statistics for CacheLv" << level << endl;
    cout << "No of accesses: " << memory->GetAccesses() << endl;
    cout << "No of hits: " << memory->GetHits() << endl;
    cout << "No of misses: " << memory->GetMisses() << endl;
    cout << "AMAT: " << memory->GetAverageAccessTime() << endl;
    cout << "Hitratio: " << memory->GetHitRate() << endl;
    cout << endl;
}

int testcode(shared_ptr<GenericMemory> &memory)
{ // some initial test code that was used to validate behaviour before file reading was implemented.
    int cycles = 0;
    
    cycles += memory->ReadMemory(0);
    cycles += memory->WriteMemory(0);
    cycles += memory->ReadMemory(63);
    cycles += memory->ReadMemory(64);
    cycles += memory->ReadMemory(256);
    cycles += memory->ReadMemory(256);
    cycles += memory->ReadMemory(64*16*2);
    cycles += memory->ReadMemory(64*16*2+63);
    cycles += memory->ReadMemory(0);
    cycles += memory->ReadMemory(63);
    cycles += memory->ReadMemory(16);

//    cycles += memory->ReadMemory(1);
    
//    cycles += 4; // non memory cycles added.
    
//    cycles += memory->ReadMemory(255);
//    cycles += memory->ReadMemory(256);
//    cycles += memory->ReadMemory(4);
//    cycles += memory->ReadMemory(5);
    
    return cycles;
}


long ExecuteTrace(shared_ptr<GenericMemory> &memory )
{
    long executioncycles = 0;    string line;
 
    ifstream inputFile;
    
    string filename;
    
    bool fileok = false;
    do {
        cout << "Current folder: " << filesystem::current_path() << endl;
        
        
        for(auto& p: filesystem::directory_iterator(filesystem::current_path() ))
        {
            string file_name = p.path().filename();
            if ( file_name.find(".trace") == 0 )
            {
               cout << file_name  << endl;
            }
        }
        
    /*    for(auto& p: filesystem::directory_iterator(filesystem::current_path()))
            cout << p.path() << '\n'; */
        
        
        cout << "Please enter filename to trace: ";
        getline(cin, filename);
        if ( filename == "" ) {
            cout << "Exiting input." << endl;
            return -1;
        };
        inputFile.open(filename);
        
        if (!inputFile)
        {
          cout << "Error opening file, try again." << endl;
        } else fileok = true;

    } while ( !fileok );
    
    //  string filename("/Users/visa/Code/CacheEmu/CacheSim/CacheSimulator/small.trace");
    //   string filename2("/Users/visa/Code/CacheEmu/CacheSim/CacheSimulator/gcc.trace");

    cout << endl << "Executing trace: " << filename << endl << endl;

    while (getline(inputFile, line)) { // reads line from input file
        char op;
        unsigned int addr;
        unsigned int cyc;
        int result = sscanf(line.c_str(), "%c %x %d", &op, &addr, &cyc);
        if ( result == 3 )
        { // successfully read instruction.
            //          cout << line << " %c:" << op << " %x:" << addr << " %d:" << cyc << endl;
            executioncycles += cyc;
            switch ( op ) {
                case 'l' : executioncycles += memory->ReadMemory(addr); break;
                case 's' : executioncycles += memory->WriteMemory(addr); break;
                default : cout << "Unknown instruction." << endl;
            }
        } else
        {
          cout << "Error interpreting line:" << line << endl;
        }
        line.erase();  // clears input string.
    }
    return executioncycles;
}

int DoMenu( deque<shared_ptr<GenericMemory> > & MemoryStruct, long cycles ){
    
    bool menuok = false;
    int choice = 0;
    int maxchoice = SetupMemory;


    do {
        cout << "Main Menu:" << endl;
        cout << endl;
        cout << "   " << SetupMemory << ": Setup Caches" << endl;
        
        if ( ! MemoryStruct.empty() )
        {
            maxchoice = DoTrace;
            cout << "   " << DoTrace << ": Run Trace" << endl;
            if ( cycles > 0 ) {
                cout << "   " << ResetStats << ": Reset stats" << endl;
                cout << "   " << Report << ": Display Report" << endl;
                maxchoice = Report;
            }
        }
        cout << "   " << Quit << ": Quit" << endl;
        cout << endl << "Enter option:";
        
        string input;
        
        getline(cin, input);
        
        if ( input.length() == 1 )
        {
            if ( toupper(input[0]) == 'Q') {
                choice = 0;
                menuok = true;
            }
            else
            {
                try {
                    choice = stoi(input);
                    if ( choice > 0 && choice <= maxchoice )
                    {
                        menuok = true;
                    }
                }
                catch ( ... )
                {
                    cout << "Invalid choice." << endl << endl;
                }
            }
        }
     
    } while ( !menuok );
    return choice;
    
}


int SetUpMemory( deque<shared_ptr<GenericMemory> > & MemoryStruct )
{

    int mainlatency;
    
    if ( !MemoryStruct.empty() ) MemoryStruct.clear(); // resetting cache setup, clear everything if something already in place.
    
    mainlatency = readint("Enter main memory access time(cycles): ", false);
 
    if (mainlatency == -1 )
    {
        cout << "No value entered, returning." << endl;
        return 1;
    }
    
    cout << endl;
    
    
    MemoryStruct.push_back(make_shared<MainMemory>(1024, mainlatency));

    // shared/weak pointers
    
    int levels = readint("Cache levels:", false);
    if ( levels < 0 ) return 1;
    
    for ( int i = levels; i>0;--i)
    {
        auto tempcache = SetupCache(i);
        if ( tempcache == nullptr )
        {
            cout << "No Cache details given, Exiting." << endl;
            MemoryStruct.clear();
            return 1;
        }
        tempcache->SetFallBack(MemoryStruct.front());
        MemoryStruct.push_front(tempcache);
    }
    
    return 0;
}

void PrintReport(deque<shared_ptr<GenericMemory> > & MemoryStruct, long cycles)
{
    int i = 1;
    for ( auto & element : MemoryStruct )
    {
        if ( ! ( element == MemoryStruct.back() ) )
            DisplayLevel(element, i);
        else {
            cout << "No of accesses to main memory: " << element->GetHits() << endl;
            cout << "AMAT of main memory: " << element->GetAverageAccessTime() << endl;
        }
        ++i;
    };
    
    cout << endl << "Total Execution cycles: " << cycles << endl;
}

void ResetCounts(deque<shared_ptr<GenericMemory> > & MemoryStruct)
{
    for ( auto & element : MemoryStruct )
        if ( ! ( element == MemoryStruct.back() ) )
            element->Reset();
}

int main(int argc, const char * argv[]) {
    
    srand((unsigned int)time(NULL));
    
    deque<shared_ptr<GenericMemory> > MemoryStruct;
    
    long cycles = 0;
    
    int menuoption = 0;
    
    do {
        menuoption = DoMenu(MemoryStruct, cycles);
        
        switch ( menuoption ) {
            case SetupMemory : SetUpMemory(MemoryStruct); break;
            case DoTrace : cycles = ExecuteTrace(MemoryStruct.front()); break;
            case ResetStats : cycles = 0; ResetCounts(MemoryStruct); break;
            case Report : PrintReport(MemoryStruct, cycles ); break;
        }
            
    } while ( menuoption != 0 );
    
    return 0;
}
