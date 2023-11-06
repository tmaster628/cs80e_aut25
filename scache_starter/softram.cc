/* 
 * File: softram.ccc
 * Author: Trip Master
 * --------------------
 * This is the implementation file for the SoftRam class.
 * SoftRam is a helper class that acts as a "RAM" for the SCache
 * class. This implementation is completed for students, who may
 * use it to indiscriminately load and store data from it.
 * 
 * Later implementations will include timing penalties for students
 * to examine the effects of cache locality.
 * 
 * This assignment was written for CS80E.
 * First drafted: 8/10/23
*/


#include <cstddef>
#include <cstring>  // For Memcpy.
#include "softram.hh"

using namespace std;

SoftRam::SoftRam() {};
SoftRam::~SoftRam() {
    /* Free all heap space from the map. */
    for (auto iter = _storage.begin(); iter != _storage.end(); iter++) {
        delete _storage[iter->first];
    }
}


void SoftRam::flush( void *data, addr_t addr) {
    if (_storage.count(addr)) {
        memcpy( _storage[addr], data, LINE_SZ);
    } else {
        _storage[addr] = new unsigned char[LINE_SZ];
        memcpy( _storage[addr], data, LINE_SZ);
    }
}

bool SoftRam::read( addr_t addr, void *buf) {
   if (_storage.count(addr)) {
       memcpy( buf, _storage[addr], LINE_SZ);
        return true;
    } else {
        return false;
    }
    return false;
}

bool SoftRam::contains( addr_t addr) {
    return _storage.count(addr); // returns false on noncontains.
}
