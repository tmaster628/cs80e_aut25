/* 
 * File: softram.ccc
 * Author: Trip Master
 * --------------------
 * This is the header file for the SoftRam class.
 * SoftRam is a helper class that acts as a "RAM" for the SCache
 * class. This header is completed for students, who may
 * use it to indiscriminately load and store data from it.
 * 
 * This assignment was written for CS80E.
 * First drafted: 8/10/23
*/


#pragma once
#include <unordered_map>
#include <cassert>
#include <cmath>
#include <iostream>

/*
 * The two TYPEDEF statements below create "aliases" for types. 
 *
 *	coreID_t is the type placeholder for the unique SCache identifier.
 *	You won't be needing to use it.
 * 	
 *	addr_t is an alias for void * -> it represents an "address" type.
 *	addr_t is used whenever we're referring to an address. Some functions
 *	may use unsigned long or void * as their input / output types, so
 *	be ready to cast this type if necessary. 
*/
typedef size_t coreID_t;
typedef void * addr_t;

/* 
 * Don't modify these two lines. Although the cache line size is
 * technically parameterizable, doing so would invalidate a lot
 * of the address placement in the testing harness. 
*/
static const size_t LINE_SZ = 64; // In bytes.
const int LINE_SZ_BITS = log2(LINE_SZ);

/*
 * LINE_STATE is a enumerated type that represents the three different
 * states of a cache line in MSI coherence: modified, shared, and invalid.
 * You can access a member of this enum like so:
 * 
 * if ( my_line.state == Line_State::SHARED ) 
*/
enum Line_State { MODIFIED, SHARED, INVALID };

/*
 * The main structure used in this assignment is the LINE struct, representing
 * a single cache line. These structs contain a char buffer for line DATA,
 * a LINE_STATE enum representing the coherence STATE of the line, the 
 * unique coreID of the OWNER of the line (meaning which SCache object this
 * line was accessed from -> NOT whether a line has it in the M state), and
 * the address associated with the line. Note that these addresses need to be
 * LINE_SZ-byte aligned.
*/
struct Line {
    unsigned char data[LINE_SZ];
    Line_State state;
    coreID_t owner;
    addr_t addr;            // Lines store their own address.
};



class SoftRam {
	
  public:
    SoftRam();
	~SoftRam();
	/**
    *  Writes cache line payload to RAM.
   */
	void flush( void *data, addr_t addr); 
	// Reads in entire line to buf.
	// Returns false on spurious read.
	bool read( addr_t addr, void *buf);

	// Wraps around the unordered map contains.
	bool contains( addr_t addr);

	private:	
	// Maps line addresses to LINE_SZ byte buffers.
	std::unordered_map<addr_t, unsigned char *> _storage{};
};



