/* 
 * File: cache.ccc
 * Author: Trip Master
 * --------------------
 * This is the implementation file for the SCache class.
 * SCache is a very simple software model of a 1-level MSI
 * cache. The aim of the assignment is for students to encode
 * their understanding of simple MSI cache coherence.
 * 
 * This assignment was written for CS80E.
 * First drafted: 8/10/23
*/

#include "cache.hh"
#include <iostream>

using namespace std;

/* Static redeclarations. */
size_t SCache::CORE_CNTR = 0;
vector<SCache *> SCache::_bus = {};
SoftRam SCache::_ram;

SCache::SCache( const size_t num_lines ) : _CORE_ID(CORE_CNTR++) {
    
    /* Fail loudly if the line size is not a power of 2. */
    if (LINE_SZ % 2) {
        cout << "ERROR: Line_SZ is not a power of 2." << endl;
        assert(0);
    }

    (void) num_lines; // Remove all (void) lines in code before you implement.
    /* TODO: Implement me! */
}

SCache::~SCache() {
    /* TODO: Implement me! */
}

void SCache::put( void* data ) {
    addr_t addr = (addr_t)addr_aligned(data);
    (void) addr; 
    /* TODO: Implement me! */
}

bool SCache::get(addr_t addr, void* buf) {
    addr = (addr_t)addr_aligned(addr); // Byte-align address.
    (void) buf;
    /* TODO: Implement me! */      
    return false;
}

bool SCache::bus_rd(addr_t addr) {
    (void) addr;
    /* TODO: Implement me! */
    return false;
}

void SCache::bus_rdX(addr_t addr) {
    (void) addr;
    /* TODO: Implement me! */
}

/* Provided helper for debugging. Pls ignore. */
vector<Line> SCache::get_debug_info() {
    return this->_cache;
}

/* 
 * Provided Address-extracting helper functions. Please
 * make use of them, but do not modify them! 
 *
 * NOTE: that ulong casting needs to be used instead of addr_t
 * to satisfy the compiler.
*/
unsigned long SCache::addr_to_index(addr_t address) {    
    // Mask off all but the lower 16 bits. Shift out the line size bits.
    return ((unsigned long)address & 0x00000000FFFF) >> LINE_SZ_BITS;
}

unsigned long SCache::addr_to_tag(addr_t address) {
    // All bits except for the lower 16 bits.
    return ((unsigned long)address & 0xFFFFFFFF0000) >> 16;
}

unsigned long SCache::addr_aligned(addr_t address) {
    // Zero out the lower LINE_SZ_BITS bits.
    return (((unsigned long)address >> LINE_SZ_BITS) << LINE_SZ_BITS);
} 