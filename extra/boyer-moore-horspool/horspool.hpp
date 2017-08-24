
#ifndef HORSPOOL_HPP
#define HORSPOOL_HPP

#include <vector>
#include <cstring>
#include <climits>
#include <cstdint>

uint32_t search_horspool(const unsigned char *haystack, uint32_t haystack_length,
                         const unsigned char *needle, const uint32_t needle_length);


#endif // HORSPOOL_HPP
