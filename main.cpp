typedef int elem_t;
typedef unsigned long long canary_t;
#define PRINT_TYPE "%d"

#include "stack.h"

int main() {
    elem_t ret_value = 0;
    Stack stk = {};
    STACK_INIT(&stk, capacity);
    for(int i = 0; i < 50; i++)
        stack_push(&stk, i);
    stack_pop(&stk, &ret_value);
    STACK_DUMP(&stk, 0);

    stack_destroy(&stk);
}

#ifdef DEBUG
int stack_init(Stack *stk, size_t capacity, const char *file_name,
                 const char *func_name, int line_num) {
    stk->data = (elem_t *)calloc(capacity * sizeof(elem_t) +
                 2 * sizeof(canary_t), sizeof(char));
    stk->data = (elem_t *)((canary_t *)stk->data + 1);
    stk->size = 0;
    stk->capacity = capacity;
    stk->stk_inf.file_name = file_name;
    stk->stk_inf.func_name = func_name;
    stk->stk_inf.line_num = line_num;
    stk->left_canary = canary_val;
    stk->right_canary = canary_val;
    *((canary_t *)stk->data - 1) = canary_val;
    *((canary_t *)(stk->data + capacity)) = canary_val;

    int errors = stack_ok(stk);

    if(errors) 
        STACK_DUMP(stk, errors);

        return errors;
}

static int stack_realloc(Stack *stk) {
    int errors = stack_ok(stk);
    if(errors) {
        STACK_DUMP(stk, errors);
        return errors;
    }
    stk->data = (elem_t *)((char *)stk->data - sizeof(canary_t));

    if(stk->size == stk->capacity) {
        stk->capacity *= capacity_multiplier;
        stk->data = (elem_t *)realloc(stk->data, 
                    (stk->capacity * sizeof(elem_t) + 2 * sizeof(canary_t)));
        *((canary_t *)((char *)stk->data +
        stk->capacity * sizeof(elem_t) + sizeof(canary_t))) = 
        *((canary_t *)((char *)stk->data +
        stk->capacity/capacity_multiplier * sizeof(elem_t) + sizeof(canary_t)));

    }
    else if(stk->capacity > capacity_multiplier * 2 * stk->size &&
             stk->capacity > capacity) {
        stk->capacity /= capacity_multiplier;
        
        *((canary_t *)((char *)stk->data +
        stk->capacity * sizeof(elem_t) + sizeof(canary_t))) =   
        *((canary_t *)((char *)stk->data +
        stk->capacity * capacity_multiplier * sizeof(elem_t) + sizeof(canary_t)));
        
        stk->data = (elem_t *)realloc(stk->data,
                     (stk->capacity * sizeof(elem_t) + 2 * sizeof(canary_t)));
    }
    
    stk->data = (elem_t *)((char *)stk->data + sizeof(canary_t));

   errors = stack_ok(stk);
    if(errors)
        STACK_DUMP(stk, errors);
    return errors;
}

#endif

int stack_destroy(Stack *stk) {
    free((canary_t *)stk->data - 1);
    stk->size = 0;
    stk->capacity = 0;
    stk = NULL;
    
    return 0;
}

int stack_push(Stack *stk, elem_t new_elem) {
    stack_realloc(stk);
    int errors = stack_ok(stk);
    if(errors) {
#ifdef DEBUG
        STACK_DUMP(stk, errors);
#endif
        return errors;
    }
    stk->data[stk->size] = new_elem;
    ++stk->size;
    errors = stack_ok(stk);
#ifdef DEBUG
    if(errors)
        STACK_DUMP(stk, errors);
#endif
    return errors;
}

int stack_pop(Stack *stk, elem_t *ret_value) {
    int errors = stack_ok(stk);
    if(errors) {
#ifdef DEBUG
        STACK_DUMP(stk, errors);
#endif
        return errors;
    }

    if(!stk->size)
        return -1;
    
    --stk->size;

    *ret_value = stk->data[stk->size];
    stk->data[stk->size] = POISON;

    stack_realloc(stk);
    
   errors = stack_ok(stk);
#ifdef DEBUG
    if(errors)
        STACK_DUMP(stk, errors);
#endif
    return errors;
}

#ifndef DEBUG

int stack_init(Stack *stk, size_t capacity) {
    stk->data = (elem_t *)calloc(capacity, sizeof(elem_t));
    stk->size = 0;
    stk->capacity = capacity;

    return stack_ok(stk);
}

static int stack_realloc(Stack *stk) {
    int error = stack_ok(stk);
    if(error)
        return error;

    if(stk->size == stk->capacity) {
        stk->capacity *= capacity_multiplier;
        stk->data = (elem_t *)realloc(stk->data,
                                     (stk->capacity * sizeof(elem_t)));
    }
    else if(stk->capacity > capacity_multiplier * 2 * stk->size &&
            stk->capacity > capacity) {
        stk->capacity /= capacity_multiplier;
        stk->data = (elem_t *)realloc(stk->data,
                                     (stk->capacity * sizeof(elem_t)));
    }

    return stack_ok(stk);
}
#endif
void set_poison(Stack *stk) {

}
int stack_ok(const Stack *stk) {
    int errors = 0;
    if(!stk)
        errors |= empty_struct;
    if(!stk->data)
        errors |= empty_stack;
    if(stk->size > stk->capacity)
        errors |= invalid_size_or_capacity;
    if(!stk->capacity)
        errors |= invalid_capacity;
#ifdef DEBUG
    if(stk->left_canary != canary_val)
        errors |= invalid_left_struct_canary;
    if(stk->right_canary != canary_val)
        errors |= invalid_right_struct_canary;
    if(*((canary_t *)stk->data - 1) != canary_val)
        errors |= invalid_left_arr_canary;
    if(*((canary_t *)(stk->data + stk->capacity)) != canary_val)
        errors |= invalid_right_arr_canary;
#endif
    
    return errors;
}

#ifdef DEBUG
int stack_dump(const Stack *stk, const char *stk_name, const char *file_name,
                int line_num, const char *func_name, int errors) {
    if(errors & empty_struct)
        printf("ERROR: the structure has pointer on null.\n");
    if(errors & empty_stack)
        printf("ERROR: stack has pointer on null(empty and doesn't have free memory).\n");
    if(errors & invalid_size_or_capacity)
        printf("ERROR: stack capacity lesser than stack size.\n");
    if(errors & invalid_capacity)
        printf("ERROR: stack capacity has invalid number.\n");
    if(errors & invalid_left_struct_canary)
        printf("ERROR: left canary in struct has invalid value\n");
    if(errors & invalid_right_struct_canary)
        printf("ERROR: right canary in struct has invalid value\n");
    if(errors & invalid_left_arr_canary)
        printf("ERROR: left canary in array has invalid value\n");
    if(errors & invalid_right_arr_canary)
        printf("ERROR: right canary in array has invalid value\n");
    printf("Stack[%p] \"%s\" from %s(%d) %s.\n", stk, stk_name,
             stk->stk_inf.func_name, stk->stk_inf.line_num, stk->stk_inf.file_name);
    printf("Called from %s(%d) %s\n", func_name, line_num, file_name);
    printf("{\n");
    printf("left canary in struct %0llX\n", stk->left_canary);
    printf("right canary in struct %0llX\n", stk->right_canary);
    printf("left canary in array %0llX\n", *((canary_t *)stk->data - 1));
    printf("right canary in array %0llX\n",*((canary_t *)(stk->data + stk->capacity)));
    printf("size = %ld\n", stk->size);
    printf("capacity = %ld\n", stk->capacity);
    printf("data[%p]\n", stk->data);
    printf("{\n");
    for(int i = 0; i < stk->capacity; i++) {
        if(i < stk->size && stk->data[i] != POISON) {
            printf("*[%d] = " PRINT_TYPE "\n", i, stk->data[i]);
            continue;
        }
            printf("[%d] = " PRINT_TYPE " (POISON)\n", i, stk->data[i]);
    }
    printf("}\n");    
    return 0;
}
#endif