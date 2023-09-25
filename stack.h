#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>

#define DEBUG

#define POISON INT_MAX
#define STACK_INIT(stk, capacity) stack_init(stk, capacity, __FILE__, \
                                             __PRETTY_FUNCTION__, __LINE__ - 1)

#ifdef DEBUG
#define STACK_DUMP(stk, errors) stack_dump(stk, #stk, __FILE__, \
                                            __LINE__, __func__, errors)

typedef struct { 
    const char *file_name;
    const char *func_name;
    int line_num;
}stack_info;
#endif

typedef struct {
    elem_t *data;
    size_t size;
    size_t capacity;
}Stack;

enum errors {
    empty_struct = 1,
    empty_stack = 2,
    invalid_size_or_capacity = 4,
    invalid_capacity = 8
};

static int stack_realloc(Stack *stk);

const size_t capacity = 25;
const size_t capacity_multiplier = 2;

int stack_init(Stack *stk, size_t capacity, const char *file_name,
                 const char *func_name, int line_num);

int stack_destroy(Stack *stk);

int stack_push(Stack *stk, elem_t new_elem);
int stack_pop(Stack *stk, elem_t *ret_value);

int stack_ok(const Stack *stk);
int stack_dump(const Stack *stk, const char *stk_name, const char *file_name, 
                int line_num, const char *func_name, int errors);