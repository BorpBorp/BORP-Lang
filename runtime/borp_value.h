#ifndef BORP_VALUE_H
#define BORP_VALUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

struct BorpArray;
struct BorpTable;
struct parentContext;
struct childContext;

typedef enum {
    VAL_NULL,
    VAL_NUMBER,
    VAL_BOOL,
    VAL_STRING,
    VAL_ARRAY,
    VAL_TABLE
} BorpValueType;

typedef struct BorpValue {
    BorpValueType type;
    union {
        double number;
        bool boolean;
        const char* string;
        struct BorpArray* borp_array;
        struct BorpTable* borp_table;
    } as;
} BorpValue;

typedef struct parentContext {

} parentContext;

typedef struct childContext {

} childContext;

static inline BorpValue borp_val_null(void) { 
    return (BorpValue){.type = VAL_NULL}; 
}
static inline BorpValue borp_val_num(double n) { 
    return (BorpValue){.type = VAL_NUMBER, .as = {.number = n}}; 
}
static inline BorpValue borp_val_string(const char* s) { 
    if (!s) return borp_val_null();
    return (BorpValue){.type = VAL_STRING, .as = {.string = s}}; 
}
static inline BorpValue borp_val_boolean(bool b) { 
    return (BorpValue){.type = VAL_BOOL, .as = {.boolean = b}}; 
}
static inline BorpValue borp_val_table(struct BorpTable* t) {
    if (!t) return borp_val_null();
    return (BorpValue){.type = VAL_TABLE, .as = {.borp_table = t}};
}
static inline BorpValue borp_val_array(struct BorpArray* a) {
    if (!a) return borp_val_null();
    return (BorpValue){.type = VAL_ARRAY, .as = {.borp_array = a}};
}

static inline double borp_to_num(BorpValue v) {
    if (v.type == VAL_NUMBER) return v.as.number;
    if (v.type == VAL_BOOL)   return (double)v.as.boolean;
    return 0.0;
}

static inline bool borp_to_bool(BorpValue v) {
    if (v.type == VAL_BOOL)   return v.as.boolean;
    if (v.type == VAL_NUMBER) return v.as.number != 0.0;
    if (v.type == VAL_NULL)   return false;
    return true;
}

static inline const char* borp_to_str(BorpValue v) {
    if (v.type == VAL_STRING) return v.as.string;
    return "";
}

static inline struct BorpArray* borp_to_array(BorpValue v) {
    if (v.type == VAL_ARRAY) return v.as.borp_array;
    return NULL;
}

static inline struct BorpTable* borp_to_table(BorpValue v) {
    if (v.type == VAL_TABLE) return v.as.borp_table;
    return NULL;
}

#endif