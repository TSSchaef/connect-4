#include "table.h"

entry_t table[TABLE_SIZE];

void init_table(){
    int i;
    for(i = 0; i < TABLE_SIZE; i++){
        table[i].pos = 0;
        table[i].info = 0;
    }
}

void put(uint64_t key, uint8_t value){
    table[key % TABLE_SIZE].pos = key;
    table[key % TABLE_SIZE].info = value;
}

int8_t get(uint64_t key){
    if(table[key % TABLE_SIZE].pos == key){
        return table[key % TABLE_SIZE].info;
    }
    return 100;
}
