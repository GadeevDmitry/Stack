#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../lib/var_declaration.h"

struct Stack
{
    void *data;

    int elem_size;
    int data_size;
    int data_capacity;

    bool is_ctor;

    var_declaration var_info;
};

const Stack default_Stack =
{
    nullptr, // data

          0, // elem_size
          1, // data_size
          4, // data_capacity

       true, // is_ctor

    {
    nullptr,
    nullptr,
    nullptr,
          0
    }        // var_info
};

const Stack *poison_Stack =
{
    nullptr, // data

         -1, // elem_size
         -1, // data_size
         -1, // data_capacity

      false, // is_ctor

    {
    nullptr,
    nullptr,
    nullptr,
          0
    }        // var_info

};

enum STACK_ERRORS
{
    OK                      ,

    NULLPTR_STACK           ,
    NULLPTR_STACK_INFO      ,

    ALREADY_CTORED          ,
    NOT_YET_CTORED          ,

    NEGATIVE_ELEM_SIZE      ,
    NEGATIVE_DATA_SIZE      ,
    NEGATIVE_DATA_CAPCITY   ,

    INVALID_CAPACITY        ,
    INVALID_DATA            ,

    MEMORY_LIMIT_EXCEEDED   ,
};

unsigned int Stack_verify(Stack *const stk)
{
    if (stk            == nullptr) return (1 << NULLPTR_STACK );
    if (stk->is_ctor   == false  ) return (1 << NOT_YET_CTORED);

    unsigned int err = 0;

    if (stk->var_info       == nullptr      ) err = err | (1 << NULLPTR_STACK_INFO   );

    if (stk->elem_size      <              0) err = err | (1 << NEGATIVE_ELEM_SIZE   );
    if (stk->data_size      <              0) err = err | (1 << NEGATIVE_DATA_SIZE   );
    if (stk->data_capacity  <              0) err = err | (1 << NEGATIVE_DATA_CAPCITY);

    if (stk->data_capacity  < stk->data_size) err = err | (1 << INVALID_CAPACITY     );
    if (stk->data           == nullptr      ) err = err | (1 << INVALID_DATA         );

    return err;
}

int  Stack_ctor(Stack *const stk, const int elem_size,  const char *file,
                                                        const char *func,
                                                        const char *name,
                                                        const int   line)
{
    assert(file);
    assert(func);
    assert(name);

   *stk            = default_Stack;
    stk->elem_size = elem_size;
    stk->data      = calloc(stk->data_capacity * elem_size);

    var_ctor(stk->var_info, file, func, name, line);
}

void Stack_dtor(Stack *const stk)
{
    free(stk->data);

    *stk = poison_Stack;
    var_dtor(&stk->var_info);
}

void Stack_realloc(Stack *const stk)
{
    if (stk->data_size == stk->data_capacity)
    {
        int future_capacity = 2 * data_capacity;

        void *temp_data = realloc(stk->data, future_capacity * stk->elem_size);
        if   (temp_data == nullptr) return
    }
}