#include <string.h>

#ifndef BORP_HELPERS_H
#define BORP_HELPERS_H

static int borp_argc = 0;
static char** borp_argv = NULL;

static inline double borp_arg_count(void) {
    return borp_argc > 1 ? (double)(borp_argc - 1) : 0.0;
}

static inline const char* borp_get_arg(double index) {
    int i = (int)index + 1;
    if (index < 0 || i >= borp_argc) return "";
    return borp_argv[i];
}

static inline void borp_print_val(BorpValue v) {
    switch (v.type) {
        case VAL_NULL:    printf("null\n"); break;
        case VAL_NUMBER:  printf("%g\n", v.as.number); break;
        case VAL_BOOL:    printf("%s\n", v.as.boolean ? "true" : "false"); break;
        case VAL_STRING:  printf("%s\n", v.as.string ? v.as.string : ""); break;
        case VAL_ARRAY:   printf("<array %p>\n", (void*)v.as.borp_array); break;
        case VAL_TABLE:   printf("<table %p>\n", (void*)v.as.borp_table); break;
    }
}

static inline const char* charAt(const char* s, int index) {
    if (!s || index < 0 || index >= (int)strlen(s)) {
        return "";
    }
    static char pool[256][2];
    static bool init = false;
    if (!init) {
        for (int i = 0; i < 256; i++) {
            pool[i][0] = (char)i;
            pool[i][1] = '\0';
        }
        init = true;
    }
    return pool[(unsigned char)s[index]];
}

static inline double borp_length(BorpValue v) {
    switch (v.type) {
        case VAL_STRING:
            return v.as.string ? (double)strlen(v.as.string) : 0.0;
        case VAL_ARRAY:
            if (!v.as.borp_array) {
                fprintf(stderr, "length() called on a null array\n");
                exit(1);
            }
            return (double)v.as.borp_array->length;
        case VAL_TABLE:
            return v.as.borp_table ? (double)v.as.borp_table->count : 0.0;
        default:
            fprintf(stderr, "length() expects a string, table, or array\n");
            exit(1);
    }
}

static inline void borp_fatal(const char* msg) {
    fprintf(stderr, "Runtime error: %s\n", msg);
#ifdef BORP_DEBUG
    *(volatile int*)0 = 0;
#endif
    exit(1);
}

static inline const char* borp_type_name(BorpValueType t) {
    switch (t) {
        case VAL_NULL:   return "null";
        case VAL_NUMBER: return "number";
        case VAL_BOOL:   return "bool";
        case VAL_STRING: return "string";
        case VAL_ARRAY:  return "array";
        case VAL_TABLE:  return "table";
    }
    return "?";
}

static inline void borp_index_fail(const char* op, BorpValue target, BorpValue key) {
    char msg[160];
    snprintf(msg, sizeof msg, "invalid %s: target is %s, key is %s",
             op, borp_type_name(target.type), borp_type_name(key.type));
    borp_fatal(msg);
}

static inline BorpValue borp_index_get(BorpValue target, BorpValue key) {
    if (target.type == VAL_NULL) return borp_val_null();
    if (target.type == VAL_TABLE && key.type == VAL_STRING)
        return borp_table_get(target.as.borp_table, key.as.string);
    if (target.type == VAL_ARRAY && key.type == VAL_NUMBER)
        return borp_array_get(target.as.borp_array, (int)key.as.number);
    borp_index_fail("index read", target, key);
    return borp_val_null();
}

static inline void borp_index_set(BorpValue target, BorpValue key, BorpValue val) {
    if (target.type == VAL_TABLE && key.type == VAL_STRING) {
        borp_table_set(target.as.borp_table, key.as.string, val);
        return;
    }
    if (target.type == VAL_ARRAY && key.type == VAL_NUMBER) {
        borp_array_set(target.as.borp_array, (int)key.as.number, val);
        return;
    }
    borp_index_fail("index write", target, key);
}

static inline char* borp_strdup(const char* s) {
    size_t n = strlen(s);
    char* r = (char*)malloc(n + 1);
    memcpy(r, s, n + 1);
    return r;
}

static inline const char* borp_fmt_num(double n, char* buf, size_t cap) {
    if (n == 0.0) {
        snprintf(buf, cap, "0");
    } else if (n > -1e15 && n < 1e15 && n == (double)(long long)n) {
        snprintf(buf, cap, "%lld", (long long)n);
    } else {
        snprintf(buf, cap, "%.15g", n);
        if (strtod(buf, NULL) != n) snprintf(buf, cap, "%.17g", n);
    }
    return buf;
}

static inline const char* borp_value_to_buf(BorpValue v, char* buf, size_t cap) {
    switch (v.type) {
        case VAL_NULL:   return "null";
        case VAL_NUMBER: return borp_fmt_num(v.as.number, buf, cap);
        case VAL_BOOL:   return v.as.boolean ? "true" : "false";
        case VAL_STRING: return v.as.string ? v.as.string : "";
        case VAL_ARRAY:  snprintf(buf, cap, "<array %p>", (void*)v.as.borp_array); return buf;
        case VAL_TABLE:  snprintf(buf, cap, "<table %p>", (void*)v.as.borp_table); return buf;
    }
    return "";
}

static inline const char* borp_to_text(BorpValue v) {
    if (v.type == VAL_STRING) return v.as.string ? v.as.string : "";
    char buf[64];
    return borp_strdup(borp_value_to_buf(v, buf, sizeof buf));
}

static inline const char* borp_concat(const char* a, const char* b) {
    size_t la = strlen(a), lb = strlen(b);
    char* r = (char*)malloc(la + lb + 1);
    memcpy(r, a, la);
    memcpy(r + la, b, lb + 1);
    return r;
}

static inline const char* borp_num_to_str(double n) {
    char buf[64];
    return borp_strdup(borp_fmt_num(n, buf, sizeof buf));
}

static inline const char* borp_stringify(BorpValue v) {
    char buf[64];
    return borp_strdup(borp_value_to_buf(v, buf, sizeof buf));
}

static inline bool borp_is_numeric(BorpValue v) {
    if (v.type != VAL_STRING || !v.as.string) return false;
    const char* s = v.as.string;
    if (s[0] == '\0') return false;
    bool decimalSeen = false;
    size_t start = 0;
    if (s[0] == '+' || s[0] == '-') {
        start = 1;
        if (s[1] == '\0') return false;
    }
    for (size_t i = start; s[i] != '\0'; i++) {
        if (s[i] == '.') {
            if (decimalSeen) return false;
            decimalSeen = true;
        } else if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

static inline double borp_parse_number(const char* s) {
    char* end;
    double d = strtod(s, &end);
    if (end == s) {
        fprintf(stderr, "Runtime error: toNumber() could not parse '%s'\n", s);
        exit(1);
    }
    return d;
}

static inline const char* borp_substring(const char* s, int start, int len) {
    int size = (int)strlen(s);
    if (start < 0 || start > size || len < 0) borp_fatal("substring() index out of range");
    if (len > size - start) len = size - start;
    char* r = (char*)malloc((size_t)len + 1);
    memcpy(r, s + start, (size_t)len);
    r[len] = '\0';
    return r;
}

static inline double borp_push(BorpArray* arr, BorpValue val) {
    if (!arr) borp_fatal("push() expects an array as its first argument");
    borp_array_push(arr, val);
    return (double)arr->length;
}

static inline BorpValue borp_pop(BorpArray* arr) {
    if (!arr) borp_fatal("pop() expects an array");
    if (arr->length == 0) borp_fatal("pop() called on an empty array");
    return borp_array_pop(arr);
}

static inline const char* borp_join(BorpArray* arr, BorpValue sepVal) {
    if (!arr) borp_fatal("join() expects an array as its first argument");
    char sepBuf[64], tmp[64];
    const char* sep = borp_value_to_buf(sepVal, sepBuf, sizeof sepBuf);
    size_t sepLen = strlen(sep);

    size_t total = 1;
    for (int i = 0; i < arr->length; i++) {
        total += strlen(borp_value_to_buf(arr->data[i], tmp, sizeof tmp));
        if (i > 0) total += sepLen;
    }

    char* out = (char*)malloc(total);
    size_t pos = 0;
    for (int i = 0; i < arr->length; i++) {
        if (i > 0) { memcpy(out + pos, sep, sepLen); pos += sepLen; }
        const char* piece = borp_value_to_buf(arr->data[i], tmp, sizeof tmp);
        size_t n = strlen(piece);
        memcpy(out + pos, piece, n);
        pos += n;
    }
    out[pos] = '\0';
    return out;
}

static inline const char* borp_read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Runtime error: readFile(): could not open file '%s'\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)size + 1);
    size_t got = fread(buf, 1, (size_t)size, f);
    buf[got] = '\0';
    fclose(f);
    return buf;
}

static inline bool borp_write_file(const char* path, BorpValue content) {
    char tmp[64];
    const char* text = borp_value_to_buf(content, tmp, sizeof tmp);
    FILE* f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Runtime error: writeFile(): could not open '%s' for writing\n", path);
        exit(1);
    }
    fputs(text, f);
    fclose(f);
    return true;
}

static inline double borp_run_command(const char* cmd) {
    return (double)system(cmd);
}

#endif