#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include <cstring>

#define DEBUG

#define POISON INT_MAX

#ifdef DEBUG
typedef struct { 
    const char *file_name;
    const char *func_name;
    int line_num;
}Stack_info;
#endif

typedef struct {
#ifdef DEBUG
    canary_t left_canary;
    int hash_struct;
    int hash_array;
#endif
    elem_t *data;
    size_t size;
    size_t capacity;
#ifdef DEBUG    
    Stack_info stk_inf;
    canary_t right_canary;
#endif
}Stack;

enum errors {
    empty_struct = 1,
    empty_stack = 2,
    invalid_size_or_capacity = 4,
    invalid_capacity = 8,
#ifdef DEBUG
    invalid_left_struct_canary = 16,
    invalid_right_struct_canary = 32,
    invalid_left_arr_canary = 64,
    invalid_right_arr_canary = 128,
    invalid_struct_hash = 256,
    invalid_array_hash = 512
#endif
};

#ifdef DEBUG

int stack_init(Stack *stk, size_t capacity, const char *file_name,
                 const char *func_name, int line_num);

#define STACK_DUMP(stk, errors) stack_dump(stk, #stk, __FILE__, \
                                            __LINE__, __PRETTY_FUNCTION__, errors)

#define STACK_INIT(stk, capacity) stack_init(stk, capacity, __FILE__, \
                                         __PRETTY_FUNCTION__, __LINE__ - 1)

const unsigned long long canary_val = 0xD15C0C001;

int stack_dump(Stack *stk, const char *stk_name, const char *file_name, 
                int line_num, const char *func_name, int errors);

typedef struct {
    errors err_num;
    const char *err_descr;
}Error_info;

#endif

#ifndef DEBUG

int stack_init(Stack *stk, size_t capacity);

#define STACK_INIT(stk, capacity) stack_init(stk, capacity)

#endif

static int stack_realloc(Stack *stk);

const size_t capacity = 25;
const size_t capacity_multiplier = 2;

int stack_destroy(Stack *stk);

int stack_push(Stack *stk, elem_t new_elem);
int stack_pop(Stack *stk, elem_t *ret_value);

int stack_ok(Stack *stk);
