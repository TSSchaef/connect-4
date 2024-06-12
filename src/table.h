#ifndef TABLE
#define TABLE

#include <stdint.h>
#define TABLE_SIZE 1000000

typedef struct entry{
    uint64_t pos;
    uint8_t info;
} entry_t;

void init_table();
void put(uint64_t key, uint8_t value);
int8_t get(uint64_t key);

#endif
