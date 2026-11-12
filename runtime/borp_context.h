#ifndef BORP_CONTEXT_H
#define BORP_CONTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "borp_value.h"

typedef struct parentContext parentContext;

struct parentContext {
    parentContext *parent;
    size_t count;
    int ref_count;
    BorpValue variables[];
};

static inline parentContext *parent_context_create(parentContext *parent, size_t var_count) {
    size_t total_size = sizeof(parentContext) + (sizeof(BorpValue) * var_count);
    parentContext *ctx = (parentContext *)malloc(total_size);
    if (!ctx) return NULL;

    ctx->parent = parent;
    ctx->count = var_count;
    ctx->ref_count = 1;

    if (parent) {
        parent->ref_count++;
    }

    for (size_t i = 0; i < var_count; i++) {
        ctx->variables[i] = borp_val_null();
    }

    return ctx;
}

static inline BorpValue borp_context_get(parentContext *ctx, size_t depth, size_t index) {
    while (depth > 0 && ctx) {
        ctx = ctx->parent;
        depth--;
    }
    if (!ctx || index >= ctx->count) {
        return borp_val_null();
    }
    return ctx->variables[index];
}

static inline void borp_context_set(parentContext *ctx, size_t depth, size_t index, BorpValue val) {
    while (depth > 0 && ctx) {
        ctx = ctx->parent;
        depth--;
    }
    if (ctx && index < ctx->count) {
        ctx->variables[index] = val;
    }
}

static inline void parent_context_release(parentContext *ctx) {
    if (!ctx) return;

    ctx->ref_count--;
    if (ctx->ref_count <= 0) {
        parentContext *parent = ctx->parent;
        free(ctx);
        if (parent) {
            parent_context_release(parent);
        }
    }
}

#endif