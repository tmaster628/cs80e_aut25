/* 
 * File: cache.ccc
 * Author: Trip Master
 * --------------------
 * This is the header file for the SCache class.
 * SCache is a very simple software model of a 1-level MSI
 * cache. The aim of the assignment is for students to encode
 * their understanding of simple MSI cache coherence.
 * 
 * This assignment was written for CS80E.
 * First drafted: 8/10/23
*/


#pragma once
#include <cstring>
#include "softram.hh"
#include <algorithm>
#include <vector>

class SBus;

class SCache {
  public:
    
    /*
     * Static counter that keeps track of how many SCache objects have
     * existed in the lifetime of the current program.
    */ 
    static size_t CORE_CNTR;

    /**
     * Constructor for the SCache. Accepts a NUM_LINES referring to
     * the number of cache lines it can hold. 
    */
    SCache( const size_t num_lines);

    /**
     * Destructor for the SCache. Frees any allocated memory in the cache
     * and removes the cache from the _BUS vector.
    */
    ~SCache();

    /**
     * PUT inserts the provided DATA into an SCache line.
     * PUT must handle any conflict misses and update the coherence 
     * metadata accordingly.
     * 
     * Hint:" Your SCache should determine whether the data being written to
     * already exists in someone's SCache. If it does, this requires more 
     * coherence traffic than if the data does not exist anywhere. 
     * 
     * You may assume that DATA will at most refer to LINE_SZ bytes, and that
     * the data will always start at address 0 relative to the start of the
     * line.
     * 
    */
    void put( void* data );

    /**
     * Retrieves the requested cache line from the SCache (or RAM) and writes
     * it into BUF. Returns FALSE if the line has never been written to and 
     * writes zeroes into BUF.
    */
    bool get(addr_t addr, void* buf);


    /* The following functions are STATIC for testing purposes. */

    /**
     * Aligns a given ADDRESS by LINE_SZ bytes.
    */
    static unsigned long addr_aligned(addr_t address);

    /**
     * Given an ADDRESS, returns the INDEX into the cache that the
     * address corresponds to. Note that the ADDRESS should be LINE_SZ-byte
     * aligned first.
    */
    static unsigned long addr_to_index(addr_t address);    

    /**
     * Given an ADDRESS, returns the TAG bits (non index / offset) bits.
     * This address need not be byte-aligned because the lower 16 bits
     * will be masked off.
    */
    static unsigned long addr_to_tag(addr_t address);

    /**
     * This function is included for testing purposes. Do not modify.
    */
    std::vector<Line> get_debug_info();

    /*
     * Represents the RAM abstraction in our SCache. The SoftRam class
     * has been implemented for you, and you do not need to modify it.
     * _RAM can be used to FLUSH and READ data, and it also has a CONTAINS
     * method.
     *  
     * Although this should be private, it is made public for testing
     * pusposes.
     */
    static SoftRam _ram;


  private:

    /**
     * bus_rd handles the coherence traffic for a READ or GET request from
     * an SCache. This helper function attempts to add a line to the 
     * cache in the "S" state. This line's data should come from either 
     * another cache or RAM (if no caches have it).
     * 
     * This method should also handle any invalidation / writebacks if a
     * line in the "M" state needs to be demoted.
     * 
     * This function returns true UNLESS the line could not be found in cache
     * OR RAM. 
    */
    bool bus_rd(addr_t addr);


    /**
     * bus_rdX handles the coherence traffic for a WRITE or PUT request from
     * an SCache. This (optional but recommended) helper function attempts
     * to add a line to the current cache in the "M" state. This line's data
     * should come from either another cache or RAM (if no caches have it).
     * 
     * This method should also handle any invalidation / writebacks if a
     * line in the "M" state needs to be demoted.
     * 
     * This function returns true UNLESS the line could not be found in cache
     * OR RAM. 
     * 
    */
  	void bus_rdX(addr_t addr);
	

    /* 
     * The unique identifier for the Core (specifically the SCache).
     * Useful so that we don't examine ourselves when looping through
     * the BUS.
    */
    const coreID_t _CORE_ID;
    
    /* Our underlying cache structure. Stores a collection of Line structs. */
    std::vector<Line> _cache{};

    /* The _BUS represents the interconnect between all SCache objects. The
     * bus contains all active SCache objects and is used to enable
     * coherence management. 
    */
    static std::vector<SCache *> _bus;
    
};