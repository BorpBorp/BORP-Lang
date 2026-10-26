#ifndef BORP_TABLE_H
#define BORP_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "borp_value.h"

typedef struct {
    const char* key;
    BorpValue value;
} BorpTableEntry;

typedef struct BorpTable {
    BorpTableEntry* entries;
    int count;
    int capacity;
} BorpTable;

static inline uint32_t borp_hash_string(const char* key) {
    uint32_t hash = 2166136261u;
    for (int i = 0; key[i] != '\0'; i++) {
        hash ^= (unsigned char)key[i];
        hash *= 16777619u;
    }
    return hash;
}

static inline void borp_table_resize(BorpTable* table, int new_capacity) {
    BorpTableEntry* new_entries = (BorpTableEntry*)malloc(sizeof(BorpTableEntry) * new_capacity);
    for (int i = 0; i < new_capacity; i++) {
        new_entries[i].key = NULL;
    }
    for (int i = 0; i < table->capacity; i++) {
        if (table->entries[i].key != NULL) {
            uint32_t hash = borp_hash_string(table->entries[i].key);
            int idx = hash % new_capacity;
            while (new_entries[idx].key != NULL) {
                idx = (idx + 1) % new_capacity;
            }
            new_entries[idx] = table->entries[i];
        }
    }
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
}

static inline BorpTable* borp_table_create(int capacity) {
    if (capacity < 8) capacity = 8;
    BorpTable* table = (BorpTable*)malloc(sizeof(BorpTable));
    table->count = 0;
    table->capacity = capacity;
    table->entries = (BorpTableEntry*)malloc(sizeof(BorpTableEntry) * capacity);
    for (int i = 0; i < capacity; i++) {
        table->entries[i].key = NULL;
    }
    return table;
}

static inline void borp_table_set(BorpTable* table, const char* key, BorpValue val) {
    if (table->count + 1 > table->capacity * 0.75) {
        borp_table_resize(table, table->capacity * 2);
    }
    uint32_t hash = borp_hash_string(key);
    int index = hash % table->capacity;
    while (true) {
        if (table->entries[index].key == NULL) {
            table->entries[index].key = key;
            table->entries[index].value = val;
            table->count++;
            return;
        } else if (strcmp(table->entries[index].key, key) == 0) {
            table->entries[index].value = val;
            return;
        }
        index = (index + 1) % table->capacity;
    }
}

static inline BorpValue borp_table_get(BorpTable* table, const char* key) {
    if (!table || table->count == 0) return (BorpValue){.type = VAL_NULL};
    uint32_t hash = borp_hash_string(key);
    int index = hash % table->capacity;
    while (true) {
        if (table->entries[index].key == NULL) {
            return (BorpValue){.type = VAL_NULL};
        } else if (strcmp(table->entries[index].key, key) == 0) {
            return table->entries[index].value;
        }
        index = (index + 1) % table->capacity;
    }
}

static inline BorpTable* borp_table_from_entries(int count, const char* keys[], BorpValue values[]) {
    BorpTable* table = borp_table_create(count * 2);
    for (int i = 0; i < count; i++) {
        borp_table_set(table, keys[i], values[i]);
    }
    return table;
}

#endif