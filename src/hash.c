#include <stdint.h>


uint32_t hash(const char* str) {
    uint32_t hash = 5381;
    uint32_t i = 0;
    char c = str[i];
    while (c != '\0') {
        hash = hash * 33 + c;
        i++;
        c = str[i];
    }
    return hash;
}