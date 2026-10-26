#ifndef BORP_ARRAY_H
#define BORP_ARRAY_H

#include <stdlib.h>
#include "borp_table.h"

typedef struct BorpArray {
    struct BorpValue* data;
    int length;
    int capacity;
} BorpArray;

static inline BorpArray* borp_array_create(int capacity) {
    BorpArray* arr = (BorpArray*)malloc(sizeof(BorpArray));
    if (capacity < 4) capacity = 4;
    arr->capacity = capacity;
    arr->data = (struct BorpValue*)malloc(sizeof(struct BorpValue) * capacity);
    arr->length = 0;
    return arr;
}

static inline BorpArray* borp_array_push(BorpArray* arr, struct BorpValue val) {
    if (arr->length >= arr->capacity) {
        arr->capacity = arr->capacity * 2;
        arr->data = (struct BorpValue*)realloc(arr->data, sizeof(struct BorpValue) * arr->capacity);
    }
    arr->data[arr->length] = val;
    arr->length++;
    return arr;
}

static inline struct BorpValue borp_array_get(BorpArray* arr, int index) {
    if (!arr || index < 0 || index >= arr->length) {
        return (struct BorpValue){.type = VAL_NULL};
    }
    return arr->data[index];
}

static inline void borp_array_set(BorpArray* arr, int index, struct BorpValue val) {
    if (!arr || index < 0) return;
    if (index >= arr->capacity) {
        int new_cap = arr->capacity * 2;
        while (index >= new_cap) new_cap *= 2;
        arr->data = (struct BorpValue*)realloc(arr->data, sizeof(struct BorpValue) * new_cap);
        for (int i = arr->capacity; i < new_cap; i++) {
            arr->data[i] = (struct BorpValue){.type = VAL_NULL};
        }
        arr->capacity = new_cap;
    }
    if (index >= arr->length) {
        for (int i = arr->length; i < index; i++) {
            arr->data[i] = (struct BorpValue){.type = VAL_NULL};
        }
        arr->length = index + 1;
    }
    arr->data[index] = val;
}

static inline BorpArray* borp_array_from_elements(int count, struct BorpValue elements[]) {
    BorpArray* arr = borp_array_create(count);
    for (int i = 0; i < count; i++) {
        borp_array_push(arr, elements[i]);
    }
    return arr;
}

static inline struct BorpValue borp_array_pop(BorpArray* arr) {
    if (!arr || arr->length == 0) return (struct BorpValue){.type = VAL_NULL};
    struct BorpValue val = arr->data[arr->length - 1];
    arr->length--;
    return val;
}

#endif