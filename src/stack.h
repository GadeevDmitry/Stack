#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>

#define STACK_DUMPING

#ifdef STACK_DUMPING

    /**
    *   @brief Contains information about the Stack variable declaration.
    *
    *   @param  variable_name - name   of the variable
    *   @param  function_name - name   of the function where the variable declarated
    *   @param      file_name - name   of the     file where the variable declarated
    *   @param strings_number - number of the   string where the variable declareted
    */

    typedef struct _StackDeclaration
    {
        const char *variable_name, *function_name, *file_name;
        int string_number;

    } StackDeclaration;

#endif

/**
*   @brief Data structure, which stores the ordered subsequence of "Stack_elem"-type elements,
*   @brief organaized according to the LIFO principle.
*   @brief User can interact with "Stack" only by using operations:
*   @brief   StackCtor() - constructer
*   @brief   StackPush() - add   the element to   the end of "Stack"
*   @brief   StackPop()  - delet the element from the end of "Stack"
*
*   @param     data - pointer to the "Stack" elements store
*   @param     size - number of elements in the "Stack"
*   @param capacity - number of "Stack" elements which may be fit in allocated memory
*   @param  is_Ctor - marker if "Stack" already constructed
*   @param     info - struct which contains information about "Stack" variable declaration (only in STACK_DUMPING mode)
*/

typedef struct _Stack
{
    Stack_elem *data;

    size_t size;
    size_t capacity;

    signed char is_Ctor;

    #ifdef STACK_DUMPING

        StackDeclaration info;

    #endif

} Stack;

/**
*   @brief The enum contains Stack errors.
*
*   @param STACK_OK           - Stack is OK
*   @param STACK_NULLPTR      - pointer to the Stack is nullptr
*   @param STACK_NON_CTOR     - Stack is not     constructed
*   @param STACK_ALREADY_CTOR - Stack is already constructed
*   @param STACK_EMPTY        - Stack is empty
*
*   @param CAPACITY_INVALID - Stack's capacity is invalid(lower than zero or less then     size)
*   @param     SIZE_INVALID - Stack's     size is invalid(lower than zero or more than capacity)
*
*   @param         ACTIVE_POISON_VALUES - poison-values   are active
*   @param NON_ACTIVE_NON_POISON_VALUES - refuse elements are non-poison
*
*   @param MEMORY_LIMIT_EXCEEDED - memory allocation query is failed
*/

typedef enum _StackError
{
    STACK_OK                     = 0,
    STACK_NULLPTR                = 1,
    STACK_NON_CTOR               = 2,
    STACK_ALREADY_CTOR           = 3,
    STACK_EMPTY                  = 4,

    CAPACITY_INVALID             = 5,
        SIZE_INVALID             = 6,

            ACTIVE_POISON_VALUES = 7,
    NON_ACTIVE_NON_POISON_VALUES = 8,

    MEMORY_LIMIT_EXCEEDED        = 9

} StackError;

/**
*   @brief Messages needed to write in log-file in case of errors in "Stack".
*   @brief Index of message is equal to corresponding error-value in the "enum _StackError".
*/

const char *error_message[] = {
    "OK",
    "pointer to the stack is nullptr",
    "stack is not constructed",
    "stack is already constructed",
    "stack is empty",
    "capacity invalid",
    "size invalid",
    "active variables are poisoned",
    "non active variables are non poisoned",
    "memory limit exceeded"
};

/**
*   @brief The enum contains poison-values for Stack's elements.
*
*   @param POISON_DATA       - poison-value for "Stack_elem *data"
*   @param POISON_BYTE       - poison-value for byte of elements of "data"
*   @param POISON_SIZE       - poison-value for "size_t size"
*   @param POISON_CAPACITY   - poison-value for "size_t capacity"
*   @param POISON_NAME       - poison-value for StackDeclaration's elements: variable_name, function_name, file_name
*   @param POISON_STRING     - poison-value fors StackDeclaration's element "string_number"
*/

typedef enum _Poison
{
    POISON_DATA       = 7,
    POISON_BYTE       = unsigned(-345),
    POISON_SIZE       = -1,
    POISON_CAPACITY   = -1,
    POISON_NAME       = 7,
    POISON_STRING     = 0

} Poison;

/*------------------------------LOG_FUNCTIONS---------------------------------*/
/*----------------------------------------------------------------------------*/

const char *log_file_name = "log.txt";
FILE       *log_stream    = nullptr;

#define fop()                                               \
        log_stream = fopen(log_file_name, "a");             \
                                                            \
        if (log_stream == nullptr)                          \
            return;

void log_message(const char *message)
{
    fop();

    fprintf(log_stream, "%s\n", message);
    fclose( log_stream);
}

void log_func_end(const char *function_name, unsigned err)
{
    fop();

    fprintf(log_stream, "%s returns %d\n\n", function_name, err);
    fclose( log_stream);
}

void log_stack_elem(const Stack_elem *var)
{
    fop();

    unsigned char current_byte_value = 0;
    unsigned char is_poison = 1;

    for (int i = 0; i < sizeof(Stack_elem); ++i)
    {
        current_byte_value = *((const unsigned char *) var + i);

        if (current_byte_value != (unsigned char) POISON_BYTE)
            is_poison = 0;

        fprintf(log_stream, "%u", current_byte_value);
    }

    if (is_poison)
        fprintf(log_stream, " (POISON)");

    fclose(log_stream);
}

void log_make_dump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   int         current_line)
{
    fop();

    fprintf(log_stream, "\nERROR occurred at:\n"
                        "FILE: %s\n"
                        "FUNC: %s\n"
                        "LINE: %d\n\n", current_file, current_func, current_line);

    if (stk == nullptr)
    {
        fprintf(log_stream, "Stack[nullptr]\n");
        fclose( log_stream);
        return;
    }

    fprintf(log_stream, "Stack[%p] \"%s\" was constructed at\n"
                            "file: \"%s\"\n"
                            "func: \"%s\"\n"
                            "line: \"%d\"\n"
                            "{\n"
                            "size     = %u\n"
                            "capacity = %u\n"
                            "data[%p]\n",   stk, stk->info.variable_name,
                                            stk->info.file_name, stk->info.function_name, stk->info.string_number,
                                            stk->size, stk->capacity, stk->data);

    if (stk->data == nullptr)
    {
        putc('\n', log_stream);

        fclose(log_stream);
        return;
    }

    putc('\t', log_stream);
    putc('{',  log_stream);
    putc('\n', log_stream);

    for (int data_counter = 0; data_counter < stk->capacity; ++data_counter)
    {
        putc('\t', log_stream);

        if (data_counter < stk->size)
            putc('*', log_stream);
        else
            putc(' ', log_stream);

        fprintf(log_stream, "[%d] = ", data_counter);
        fclose( log_stream);

        log_stack_elem(stk->data + data_counter);

        fop();
        putc('\n', log_stream);
    }
    putc('\t', log_stream);
    putc('}',  log_stream);
    putc('\n', log_stream);
    putc('\n', log_stream);

    fclose(log_stream);
}

void log_dump(Stack *stk, const unsigned err, const char *current_file,
                                              const char *current_func,
                                              int         current_line)
{
    fop();

    fprintf(log_stream, "StackDump(stk = \"%s\", err = %u, current_file = \"%s\"\n"
                        "                                  current_func = \"%s\"\n"
                        "                                  current_line = %d)\n\n",
    stk->info.variable_name, err, current_file, current_func, current_line);

    fclose(log_stream);
}

void log_dumping_ctor(Stack *stk, const int capacity, const char *stk_name,
                                                      const char *stk_func,
                                                      const char *stk_file, const int stk_line)
{
    fop();

    if (stk_name == nullptr)
        stk_name = "nullptr";

    if (stk_func == nullptr)
        stk_func = "nullptr";

    if (stk_file == nullptr)
        stk_file = "nullptr";

    fprintf(log_stream, "(dumping)_StackCtor(stk = %p, capacity = %d, stk_name = \"%s\"\n"
                        "                                             stk_func = \"%s\"\n"
                        "                                             stk_file = \"%s\"\n"
                        "                                             stk_line = %d)\n\n",
    stk, capacity, stk_name, stk_func, stk_file, stk_line);

    fclose(log_stream);
}

void log_verify(Stack *stk)
{
    fop();

    fprintf(log_stream, "StackVerify(stk = %p)\n\n", stk);
    fclose( log_stream);
}

void log_push(Stack *stk, const Stack_elem push_val)
{
    fop();

    fprintf(log_stream, "StackPush(stk = %p, push_val = ", stk);
    fclose( log_stream);

    log_stack_elem(&push_val);

    fop();

    fprintf(log_stream, ")\n\n");
    fclose( log_stream);
}

void log_pop(Stack *stk, Stack_elem *const front_val)
{
    fop();

    fprintf(log_stream, "StackPop(stk = %p, front_val = %p)\n\n", stk, front_val);
    fclose( log_stream);
}

void log_realloc(Stack *stk, const int condition)
{
    fop();

    fprintf(log_stream, "StackRealloc(stk = %p, condition = %d)\n\n", stk, condition);
    fclose( log_stream);
}

void log_dtor(Stack *stk)
{
    fop();

    fprintf(log_stream, "StackDtor(stk = %p)\n\n", stk);
    fclose( log_stream);
}

/*----------------------------------------------------------*/
/*----------------------------------------------------------*/


/*-------------------FUNCTION_DECLARATION------------------*/

unsigned StackVerify(Stack *stk);

unsigned StackPush   (Stack *stk, const Stack_elem push_val);
unsigned StackPop    (Stack *stk, Stack_elem *const front_val = nullptr);
unsigned StackDtor   (Stack *stk);
unsigned StackRealloc(Stack *stk, const int condition);

void make_bit_true(unsigned *const num, const unsigned bit_num);

#ifdef STACK_DUMPING

    void StackDump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   const char *current_line);

    unsigned _StackCtor(Stack *stk, const int capacity, const char *stk_name,
                                                        const char *stk_func,
                                                        const char *stk_file, const int stk_line);

#endif

/*---------------------------------------------------------*/

#ifdef STACK_DUMPING

    #define Stack_assert(stk_ptr, err)                                                          \
                                                                                                \
            if (*err = StackVerify(stk_ptr))                                                    \
            {                                                                                   \
                StackDump(stk_ptr, *err, __FILE__, __PRETTY_FUNCTION__, __LINE__);              \
                                                                                                \
                log_func_end(__PRETTY_FUNCTION__, *err);                                        \
                return *err;                                                                    \
            }

    #define StackCtor(stk_name, capacity)                                                       \
           _StackCtor(stk_name, capacity, #stk_name, __PRETTY_FUNCTION__, __FILE__, __LINE__)

    /**
    *   @brief Prints all information about "Stack" variable in the log-file.
    *
    *   @param          stk [in]          stk - pointer to the "Stack" variable
    *   @param          err [in]          err - bit-mask which encodes the errors from "enum _StackError"
    *   @param current_file [in] current_file - name   of the file, where "StackDump()" called
    *   @param current_func [in] current_func - name   of the func, where "StackDump()" called
    *   @param current_file [in] current_line - number of the line, where "StackDump()" called
    *
    *   @return nothing
    */

    void StackDump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   int         current_line)
    {
        log_dump(stk, err, current_file,
                           current_func,
                           current_line);
        log_message("MESSAGE_ERRORS:");

        int error_numbers = sizeof(error_message) / sizeof(char *);

        for (int i = 0; i <= error_numbers; ++i)
        {
            if (err & (1 << i))
                log_message(error_message[i]);
        }

        log_make_dump(stk, err, current_file,
                                current_func,
                                current_line);

        log_func_end(__PRETTY_FUNCTION__, 0);
    }

    /**
    *   @brief Stack dumping constructor. In adittion to "capacity", it takes "struct _StackDeclaration" elements.
    *   @brief Allocates memory for "Stack_elem *data" and fills them by poison.
    *   @brief The "Stack" must be initialized by nulls before.
    *   @brief Returns the bit-mask which encodes the errors. A set bit means the error.
    *   @brief Names of the erros are in the "enum _StackError".
    *
    *   @param      stk [in][out] stk - pointer to the "Stack"
    *   @param capacity [in] capacity - needed capacity
    *   @param stk_name [in] stk_name - name   of the "Stack" variable
    *   @param stk_func [in] stk_func - name   of the function where the "Stack" variable was declared
    *   @param stk_file [in] stk_file - name   of the     file where the "Stack" variable was declared
    *   @param stk_line [in] stk_line - number of the     line where the "Stack" variable was declared
    *
    *   @return bit-mask which encodes the errors
    *
    *   @note The parameter "stk_name" contains the '&' character before the real name because it obtains
    *   @note by macros #define using the #-operator before "stk"-parameter which is the pointer. To get
    *   @note the pointer user need to use &-operator.
    */

    unsigned _StackCtor(Stack *stk, const int capacity, const char *stk_name,
                                                        const char *stk_func,
                                                        const char *stk_file, const int stk_line)
    {
        log_dumping_ctor(stk, capacity, stk_name,
                                        stk_func,
                                        stk_file,
                                        stk_line);

        unsigned err = 0;

        if (stk == nullptr)
        {
            log_func_end(__PRETTY_FUNCTION__, err);
            return err;
        }

        if (stk->is_Ctor == 1)
            make_bit_true(&err, STACK_ALREADY_CTOR);

        if (err != STACK_OK)
        {
            StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

            log_func_end(__PRETTY_FUNCTION__, err);
            return err;
        }

        stk->is_Ctor = 1;

        stk->info.variable_name = stk_name + 1; // add 1 to skip the '&' character
        stk->info.function_name = stk_func;
        stk->info.file_name     = stk_file;
        stk->info.string_number = stk_line;

        if (capacity)
        {
            stk->capacity = capacity;
            stk->data     = (Stack_elem *) calloc(capacity, sizeof(Stack_elem));

            if (stk->data == nullptr)
            {
                make_bit_true(&err, MEMORY_LIMIT_EXCEEDED);
                StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

                log_func_end(__PRETTY_FUNCTION__, err);
                return err;
            }

            memset(stk->data, POISON_BYTE, sizeof(Stack_elem) * capacity);
        }

        log_func_end(__PRETTY_FUNCTION__, STACK_OK);
        return STACK_OK;
    }
#endif

/**
*   @brief Check if "stk" is invalid. Makes the bit-mask which encodes the errors. A set bit means the error.
*   @brief Names of the erros are in the "enum _StackError".
*
*   @param stk [in] stk - pointer to the "Stack"
*
*   @return bit-mask which encodes the errors
*/

unsigned StackVerify(Stack *stk)
{
    log_verify(stk);

    unsigned err = 0;

    if (stk == nullptr)
    {
        make_bit_true(&err, STACK_NULLPTR);
        return err;
    }

    if (!stk->is_Ctor)
        make_bit_true(&err, STACK_NON_CTOR);

    if (stk->data == (Stack_elem *) POISON_DATA)
        make_bit_true(&err, ACTIVE_POISON_VALUES);

    if (stk->size < 0)
        make_bit_true(&err, SIZE_INVALID);

    if (stk->capacity < 0)
        make_bit_true(&err, CAPACITY_INVALID);

    if (stk->size > stk->capacity)
    {
        make_bit_true(&err,     SIZE_INVALID);
        make_bit_true(&err,  CAPACITY_INVALID);
    }

    if (stk->capacity > 0)
    {
        if (stk->data == nullptr)
            make_bit_true(&err, CAPACITY_INVALID);

        else
        {
            unsigned char *data_check = (unsigned char *) stk->data;

            for (int counter = 0; counter < sizeof(Stack_elem) * stk->size; ++counter)
            {
                if (data_check[counter] == (unsigned char) POISON_BYTE)
                {
                    make_bit_true(&err, ACTIVE_POISON_VALUES);
                    break;
                }
            }

            for (int counter = sizeof(Stack_elem) * stk->size;
                     counter < sizeof(Stack_elem) * stk->capacity ; ++counter)
            {
                if (data_check[counter] != (unsigned char) POISON_BYTE)
                {
                    make_bit_true(&err, NON_ACTIVE_NON_POISON_VALUES);
                    break;
                }
            }
        }
    }

    log_func_end(__PRETTY_FUNCTION__, err);
    return err;
}

/**
*   @brief Makes the bit of the unsigned int true.
*
*   @param     num [in][out] - pointer to the unsigned int
*   @param bit_num [in]      - number of the bit to make true
*
*   @return nothing
*/

void make_bit_true(unsigned *const num, const unsigned bit_num)
{
    *num = (*num) | (1 << bit_num);
}

/**
*   @brief Add the element into the "Stack.data".
*
*   @param      stk [in][out] stk - pointer to the "Stack"
*   @param push_val [in] push_val - value of element to put
*
*   @return bit-mask which encodes the errors from "enum _StackError"
*/

unsigned StackPush(Stack *stk, const Stack_elem push_val)
{
    log_push(stk, push_val);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (stk->size < stk->capacity)
    {
        stk->data[stk->size++] = push_val;

        Stack_assert(stk, &err);

        log_func_end(__PRETTY_FUNCTION__, STACK_OK);
        return STACK_OK;
    }

    if (stk->size == stk->capacity)
    {
        err = StackRealloc(stk, 1);
        if (err)
        {
            log_func_end(__PRETTY_FUNCTION__, err);
            return err;
        }
    }

    stk->data[stk->size++] = push_val;

    Stack_assert(stk, &err);

    log_func_end(__PRETTY_FUNCTION__, STACK_OK);
    return STACK_OK;
}

/**
*   @brief Deletes the front element of the "Stack.data". Puts the front element in variable
*   @brief pointed by "front_val" before deliting.
*   @brief "fron_val" is nullptr default. In this case "StackPop()" doesn't puts the front element.
*
*   @param       stk [in][out]   stk - pointer to the "Stack"
*   @param front_val [out] front_val - pointer to the front element
*
*   @return bit-mask which encodes the errors from "enum _StackError"
*/

unsigned StackPop(Stack *stk, Stack_elem *const front_val)
{
    log_pop(stk, front_val);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (stk->size == 0)
    {
        make_bit_true(&err, STACK_EMPTY);

        #ifdef STACK_DUMPING

            StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

        #endif

        log_func_end(__PRETTY_FUNCTION__, err);
        return err;
    }

    --stk->size;

    if (front_val != nullptr)
        *front_val = stk->data[stk->size];

    memset((char *) stk->data + sizeof(Stack_elem) * stk->size, POISON_BYTE, sizeof(Stack_elem));

    err = StackRealloc(stk, 0);

    log_func_end(__PRETTY_FUNCTION__, err);
    return err;
}

/**
*   @brief Does a "Stack" memory reallocation. Can work in two modes. If "condition" is true, it allocates memory,
*   @brief and else it frees the part of memory.
*   @brief Memory allocation occurs acording the folowing strategy: if "Stack.size" = "Stack.capacity", capacity doubles.
*   @brief Memory    freeing occurs acording the folowing strategy: if "Stack.size" equal or less than a quarter of
*   @brief "Stack.capacity", capacity is halved
*
*   @param       stk [in][out]  stk - pointer to the "Stack"
*   @param condition [in] condition - mode of "StackRealloc()"
*
*   @return bit-mask which encodes the errors from "enum _StackError"
*/

unsigned StackRealloc(Stack *stk, const int condition)
{
    log_realloc(stk, condition);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (condition)
    {
        int future_capacity = 0;

        if (stk->capacity == 0)
            future_capacity = 4; //default elementary capacity
        else
            future_capacity = 2 * stk->capacity;

        stk->data = (Stack_elem *) realloc(stk->data, sizeof(Stack_elem) * future_capacity);

        if (stk->data == nullptr)
        {
            make_bit_true(&err, MEMORY_LIMIT_EXCEEDED);

            #ifdef STACK_DUMPING

                StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

            #endif

            log_func_end(__PRETTY_FUNCTION__, err);
            return err;
        }

        memset((char *)stk->data + sizeof(Stack_elem) * stk->size, POISON_BYTE, sizeof(Stack_elem) * (future_capacity - stk->size));
        stk->capacity = future_capacity;

        Stack_assert(stk, &err);

        log_func_end(__PRETTY_FUNCTION__, STACK_OK);
        return STACK_OK;
    }

    //if (!condition)
    if ((stk->size != 0) && (4 * stk->size <= stk->capacity))
    {
        int future_capacity = 2 * stk->size;

        stk->data = (Stack_elem *) realloc(stk->data, sizeof(Stack_elem) * future_capacity);

        if (stk->data == nullptr)
        {
            make_bit_true(&err, MEMORY_LIMIT_EXCEEDED);

            #ifdef STACK_DUMPING

                StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

            #endif

            log_func_end(__PRETTY_FUNCTION__, err);
            return err;
        }

        stk->capacity = future_capacity;

        Stack_assert(stk, &err);
    }

    log_func_end(__PRETTY_FUNCTION__, STACK_OK);
    return STACK_OK;
}

/**
*   @brief Stack destructor. Frees memory pointed by "Stack.data".
*   @brief Fill all "Stack" elements besides the "Stack.is_Ctor" by poison. "Stack.is_Ctor" becomes equal to zero.
*
*   @param stk [in][out] stk - pointer to the "Stack"
*
*   @return bit-mask which encodes the errors from "enum _StackError"
*/

unsigned StackDtor(Stack *stk)
{
    log_dtor(stk);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (stk->data != nullptr)
    {
        free(stk->data);
        stk->data = (Stack_elem *) POISON_DATA;
    }

    stk->size     = POISON_SIZE;
    stk->capacity = POISON_CAPACITY;
    stk->is_Ctor  = 0;

    #ifdef STACK_DUMPING

        stk->info.variable_name  = stk->info.function_name = stk->info.file_name = (const char *) POISON_NAME;
        stk->info.string_number = POISON_STRING;

    #endif

    log_func_end(__PRETTY_FUNCTION__, STACK_OK);
    return STACK_OK;
}

#endif //STACK
