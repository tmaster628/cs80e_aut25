#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstring>
#include "softram.hh"
#include "cache.hh"

using namespace std;


struct linear_friends1 {
    char s1[2];
    char s2[2];
};

struct linear_friends2 {
    // Looks weird, but because our lower 16 bits are used
    // for indexing / offset, addresses that differ in the
    // 17th bit will appear to have the same index bits!
    char s1[64];
    char difference[1024 * 128 - 64];
    char s2[64];
};

// Do not change this :).
const size_t CACHE_SIZE = LINE_SZ * 1024; 

/**
 * Returns a buf[64] version of the argument string DATA.
 * You will need to FREE() this data once it's done being used.
 * 
 * (You shouldn't need this function, but I left it for convenience).
*/
void *lineify(string data) {
    void *buf = calloc(LINE_SZ, sizeof(char));
    const char *str_d = data.c_str();
    memcpy(buf, str_d, strlen(str_d));
    return buf;
}

// Ignore me.
void test_ram() {
    SoftRam ram;
    void *res = lineify("SoftRam works correctly...");
    ram.flush(res, res);
    char buf[64];
    ram.read(res, buf);
    cout << buf << endl;
    free (res);
}

bool test_scache1() {
    cout << "Running Test 1..." << endl;
    // One write / read from a single cache. No coherence.

    SCache cache(CACHE_SIZE / LINE_SZ);
    // Put some things in the cache.
    string str = "first test string! It's a long one.";
    const char *c = str.c_str();
    cache.put((void *)c);

    vector<Line> cache_contents = cache.get_debug_info();
    if (cache_contents[SCache::addr_to_index((addr_t)c)].state != Line_State::MODIFIED) {
        cout << "ERROR: Line State not set to MODIFIED after a PUT." << endl;
        return false;
    }

    char buf[LINE_SZ];
    cache.get((addr_t)c, buf);
    if(!strcmp(buf, str.c_str())) { // BC strcmp returns 0 on match.
        return true;
    } else {
        cout << "Test 1 Output Mismatch:" << endl;
        cout << "Expected output->: " << str.c_str() << endl;
        cout << "Actual output--->: " << buf << endl;
        return false;
    }
}

bool test_scache2() {
    cout << "Running Test 2..." << endl;
    // Writes / Reads from two caches with no coherence.

    SCache c1(CACHE_SIZE / LINE_SZ);
    SCache c2(CACHE_SIZE / LINE_SZ);

    const char *str1 = "String that belongs in Cache c1.";
    // STDDUP'ing the second string should prevent address collisions.
    char *str2 = strdup("String that belongs in Cache c2.");

    c1.put((void *)str1);
    c2.put((void *)str2);

    char buf1[LINE_SZ];
    char buf2[LINE_SZ];
    c1.get((addr_t)str1, buf1);
    c2.get((addr_t)str2, buf2);

    if(!strcmp(buf1, str1) && !strcmp(buf2, str2)) { // BC strcmp returns 0 on match.
        free(str2);
        return true;
    } else {
        cout << "Test 2 Output Mismatch:" << endl;
        cout << "Expected output (first string)->: " << str1 << endl;
        cout << "Actual output (first string)--->: " << buf1 << endl;
        cout << "Expected output (second string)->: " << str2 << endl;
        cout << "Actual output (second string)--->: " << buf2 << endl;
        free(str2);
        return false;
    }
}

bool test_scache3() {
    cout << "Running Test 3..." << endl;
    // Writes / Reads from mulitple caches with simeple M -> I invalidation.
    // M->I Coherence. Second cache updates data present in the first cache.
    linear_friends1 lf;
    const char *str1 = ":";
    const char *str2 = ">";
    // One character strings here. Any bigger and you run into 
    // byte-alignment issues that are nondeterministic. This
    // still may be nondeterministic (unsure), but false
    // negative rate should be like < 1/30.

    // Strings will occupy the same cache line.
    strcpy(lf.s1, str1);
    strcpy(lf.s2, str2);


    SCache c1(CACHE_SIZE / LINE_SZ);
    SCache c2(CACHE_SIZE / LINE_SZ);
    
    c1.put((void *)&lf.s1);
    c2.put((void *)&lf.s2);
    // Second PUT should invalidate the first put.
    // Attempt to get the first PUT needs to pull 
    // from RAM.

    // Verify RAM has been written to.
    if (!SCache::_ram.contains((addr_t)SCache::addr_aligned(&lf.s1))) {
        cout << "ERROR: Address not found in RAM after invalidation." << endl;
        return false;
    }

    vector<Line> cache_contents1 = c1.get_debug_info();
    vector<Line> cache_contents2 = c2.get_debug_info();
   
    if (cache_contents1[SCache::addr_to_index((addr_t)&lf.s1)].state != Line_State::INVALID) {
        cout << "ERROR: Cache 1 Line State not set to INVALID after an M->I invalidation." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)&lf.s2)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 2 Line State not set to MODIFIED after a PUT." << endl;
        return false;
    }

    char buf1[LINE_SZ];
    char buf2[LINE_SZ];

    // This get tests M->S coherence, since it
    // will demote C2's modified hold over the line.
    c1.get((addr_t)&lf.s1, buf1);
    c2.get((addr_t)&lf.s2, buf2);

    cache_contents1 = c1.get_debug_info();
    cache_contents2 = c2.get_debug_info();
   

    if (cache_contents1[SCache::addr_to_index((addr_t)&lf.s1)].state != Line_State::SHARED) {
        cout << "ERROR: Cache 1 Line State not set to SHARED after a busRD." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)&lf.s2)].state != Line_State::SHARED) {
        cout << "ERROR: Cache 2 Line State not set to SHARED after a M->S Demotion." << endl;
        return false;
    }

    if(!strcmp(buf1, buf2)) { // BC strcmp returns 0 on match.
        return true;
    } else {
        cout << "Test 2 Output Mismatch:" << endl;
        cout << "Expected output (first string)->: " << lf.s2 << endl;
        cout << "Actual output (first string)--->: " << buf1 << endl;
        cout << "Expected output (second string)->: " << lf.s2 << endl;
        cout << "Actual output (second string)--->: " << buf2 << endl;
        return false;
    }
}

bool test_scache4() {
    cout << "Running Test 4..." << endl;
    // S-> M Coherence.
    // 3 Caches share a line in S state. One writes to it again.
    SCache c1(CACHE_SIZE / LINE_SZ);
    SCache c2(CACHE_SIZE / LINE_SZ);
    SCache c3(CACHE_SIZE / LINE_SZ);

    const char *str1 = "Sharing Coherence test!";
    c1.put((void *)str1);

    char buf1[LINE_SZ];
    char buf2[LINE_SZ];
    // Demotion to S state.
    c2.get((addr_t)str1, buf1); // Simple M read, no demote
    c3.get((addr_t)str1, buf2); // M->S demotion from sharing.

    // At this point, all 3 caches should have the line in the S state.

    vector<Line> cache_contents1 = c1.get_debug_info();
    vector<Line> cache_contents2 = c2.get_debug_info();
    vector<Line> cache_contents3 = c3.get_debug_info();
   
    if (cache_contents1[SCache::addr_to_index((addr_t)str1)].state != Line_State::SHARED) {
        cout << "ERROR: Cache 1 Line State not set to SHARED after an M->S Demotion." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)str1)].state != Line_State::SHARED) {
        cout << "ERROR: Cache 2 Line State not set to SHARED after a busRD." << endl;
        return false;
    } else if (cache_contents3[SCache::addr_to_index((addr_t)str1)].state != Line_State::SHARED) {
        cout << "ERROR: Cache 3 Line State not set to SHARED after a busRD." << endl;
        return false;
    }

    // C3 puts at the same address.
    c3.put((void *)str1);

    // Now c3 should have the line in the M state. 2 invalidations
    // should occur, displaying S->I Coherence.

    // Verify RAM has been written to.
    if (!SCache::_ram.contains((addr_t)SCache::addr_aligned((addr_t)str1))) {
        cout << "ERROR: Address not found in RAM after invalidation." << endl;
        return false;
    }


    cache_contents1 = c1.get_debug_info();
    cache_contents2 = c2.get_debug_info();
    cache_contents3 = c3.get_debug_info();

    if (cache_contents1[SCache::addr_to_index((addr_t)str1)].state != Line_State::INVALID) {
        cout << "ERROR: Cache 1 Line State not set to INVALID after a S->I Demotion." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)str1)].state != Line_State::INVALID) {
        cout << "ERROR: Cache 2 Line State not set to INVALID after a S->I Demotion." << endl;
        return false;
    } else if (cache_contents3[SCache::addr_to_index((addr_t)str1)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 3 Line State not set to MODIFIED after a busRDx." << endl;
        return false;
    }

    return true;
}

bool test_scache5() {
    cout << "Running Test 5..." << endl;
    // Edge case test -- Ensure different addresses do not invalidate each other.
    // In other words, index bits match, but tags do not.
    SCache c1(CACHE_SIZE / LINE_SZ);
    SCache c2(CACHE_SIZE / LINE_SZ);

    linear_friends2 lf;
    const char *str1 = "Edge case 1";
    const char *str2 = "Edge case 2";
    strcpy(lf.s1, str1);
    strcpy(lf.s2, str2);
   
    c1.put((void *)&lf.s1);
    c2.put((void *)&lf.s2);
   
    // There should be no invalidation from this.
    // Both lines should be in the MODIFIED state.
    vector<Line> cache_contents1 = c1.get_debug_info();
    vector<Line> cache_contents2 = c2.get_debug_info();

    if (cache_contents1[SCache::addr_to_index((addr_t)&lf.s1)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 1 Line State not set to MODIFIED after a busRDX." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)&lf.s2)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 2 Line State not set to MODIFIED after a busRDX." << endl;
        return false;
    }

    char buf1[LINE_SZ];
    char buf2[LINE_SZ];
    c1.get((addr_t)&lf.s1, buf1);
    c2.get((addr_t)&lf.s2, buf2);

    cache_contents1 = c1.get_debug_info();
    cache_contents2 = c2.get_debug_info();

    if (cache_contents1[SCache::addr_to_index((addr_t)&lf.s1)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 1 Line State should remain MODIFIED, even after a self-read." << endl;
        return false;
    } else if (cache_contents2[SCache::addr_to_index((addr_t)&lf.s2)].state != Line_State::MODIFIED) {
        cout << "ERROR: Cache 2 Line State should remain MODIFIED, even after a self-read." << endl;
        return false;
    }

    if(!strcmp(buf1, str1) && !strcmp(buf2, str2)) { // BC strcmp returns 0 on match.
        return true;
    } else {
        cout << "Test 2 Output Mismatch:" << endl;
        cout << "Expected output (first string)->: " << str1 << endl;
        cout << "Actual output (first string)--->: " << buf1 << endl;
        cout << "Expected output (second string)->: " << str2 << endl;
        cout << "Actual output (second string)--->: " << buf2 << endl;
        return false;
    }
}

int main() {

    /* Feel free to add your own tests / modify this file, but
       be aware that I will be testing your code on the unmodified
       version of this file! */

    vector<bool> tests;

    /* Sanity check for RAM. This should work, so you can comment it out. */
    test_ram(); 

    /* Run the provided tests. Feel free to comment out tests. 
       The harness will stop as soon as ANY test returns false. */
    cout << "Running SCache Tests..." << endl;

    tests.push_back(test_scache1());
    tests.push_back(test_scache2());
    tests.push_back(test_scache3());
    tests.push_back(test_scache4());
    tests.push_back(test_scache5());

    size_t tests_passed = count(tests.begin(), tests.end(), 1); 

    cout << "You passed " << count(tests.begin(), tests.end(), 1) 
         << " out of " << tests.size() << " tests.";

    if (tests_passed == tests.size()) {
        cout << " Wahoo!" << endl;
    } else {
        cout << endl;
    }

    return 0;
}
