/** @file */

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "flags.h"

#define  STACK_DUMPING
#define CANARY_PROTECTION

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
*
*   @param CANARY_PROTECTION_FAILED - canary protection is failed
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

    MEMORY_LIMIT_EXCEEDED        = 9,

    CANARY_PROTECTION_FAILED     = 10

} StackError;

/**
*   @brief Messages needed to write in log-file in case of errors in "Stack".
*   @brief Index of message is equal to corresponding error-value in the "enum _StackError".
*/

const char *error_message[] =
{
    "OK",                                    // 0
    "pointer to the stack is nullptr",       // 1
    "stack is not constructed",              // 2
    "stack is already constructed",          // 3
    "stack is empty",                        // 4
    "capacity invalid",                      // 5
    "size invalid",                          // 6
    "active variables are poisoned",         // 7
    "non active variables are non poisoned", // 8
    "memory limit exceeded",                 // 9
    "canary protection failed"               // 10
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
*
*   @param  LEFT_CANARY      - value for the  left canary protection
*   @param RIGHT_CANARY      - value for the right canary protection
*/

typedef enum _Poison
{
    POISON_DATA       = 7,
    POISON_BYTE       = unsigned(-345),
    POISON_SIZE       = -1,
    POISON_CAPACITY   = -1,
    POISON_NAME       = 7,
    POISON_STRING     = 0,
     LEFT_CANARY      = 0xBAADF00D,
    RIGHT_CANARY      = 0xDEADBEEF

} Poison;

/*----------------------------------------------FUNCTION_DECLARATION--------------------------------------------------*/

unsigned StackVerify(Stack *stk);

unsigned StackPush   (Stack *stk, const Stack_elem push_val);
unsigned StackPop    (Stack *stk, Stack_elem *const front_val = nullptr);
unsigned StackDtor   (Stack *stk);
unsigned StackRealloc(Stack *stk, const int condition);

unsigned  PoisonCheck(void *_verifiable_elem, const size_t elem_size, const unsigned char poison_val,
                                                                      const unsigned char mode);
void FillPoison(void *_fillable_elem, const size_t elem_size, const unsigned left,
                                                              const unsigned right, const unsigned char poison_val);
void make_bit_true(unsigned *const num, const unsigned bit_num);

#ifdef STACK_DUMPING

    void StackDump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   const char *current_line);

    unsigned _StackCtor(Stack *stk, int capacity, const char *stk_name,
                                                  const char *stk_func,
                                                  const char *stk_file, const int stk_line);

#endif

#ifdef CANARY_PROTECTION

    unsigned StackCheckCanary(Stack *stk, unsigned *const left_canary  = nullptr,
                                          unsigned *const right_canary = nullptr);

#endif

/*--------------------------------------------------LOG_FUNCTIONS----------------------------------------------------*/

const char *LOG_FILE_NAME = "log.html";
FILE       *LOG_STREAM    = nullptr;

/**
*   @brief Opens log-file. Ckecks if opening is OK and in this case prints message in the log-file.
*
*   @return 1 if checking is OK. Does abort() if an ERROR found.
*/

int OPEN_LOG_STREAM()
{
    LOG_STREAM = fopen(LOG_FILE_NAME, "w");
    assert( LOG_STREAM != nullptr);

    setvbuf(LOG_STREAM,   nullptr, _IONBF, 0);

    fprintf(LOG_STREAM, "<pre>\n""\"%s\" OPENING IS OK\n\n", LOG_FILE_NAME);
    return 1;
}

/**
*   @brief Closes log-file. Called using atexit().
*
*   @return 1 if closing is OK. Does abort() if an ERROR found.
*/

void CLOSE_LOG_STREAM()
{
    assert(LOG_STREAM != nullptr);

    fprintf(LOG_STREAM, "\"%s\" CLOSING IS OK\n\n", LOG_FILE_NAME);
    fclose( LOG_STREAM);
}

int  _OPEN_LOG_STREAM = OPEN_LOG_STREAM();
int _CLOSE_LOG_STREAM = atexit(CLOSE_LOG_STREAM);

void log_message(const char *message)
{
    fprintf(LOG_STREAM, "<font color=red> %s </font>\n", message);
}

void log_func_end(const char *function_name, unsigned err)
{
    fprintf(LOG_STREAM, "%s returns %d\n\n", function_name, err);
}

void log_stack_elem(const Stack_elem *var)
{
    unsigned char current_byte_value = 0;
    unsigned char is_poison = 1;

    for (int i = 0; i < sizeof(Stack_elem); ++i)
    {
        current_byte_value = *((const unsigned char *) var + i);

        if (current_byte_value != (unsigned char) POISON_BYTE)
            is_poison = 0;

        fprintf(LOG_STREAM, "<font color=blue>%u</font>", current_byte_value);
    }

    if (is_poison)
        fprintf(LOG_STREAM, " <font color=yellow> (POISON) </font>");
}

void log_make_dump(Stack *stk, const char *current_file,
                               const char *current_func,
                               int         current_line)
{
    fprintf(LOG_STREAM, "\n<font color=blue> ERROR occurred at:\n"
                        "FILE: %s\n"
                        "FUNC: %s\n"
                        "LINE: %d </font>\n\n", current_file, current_func, current_line);

    if (stk == nullptr)
    {
        fprintf(LOG_STREAM, "Stack[nullptr]\n");
        return;
    }

    if (stk->info.variable_name == nullptr) stk->info.variable_name = "nullptr";
    if (stk->info.file_name     == nullptr) stk->info.file_name     = "nullptr";
    if (stk->info.function_name == nullptr) stk->info.function_name = "nullptr";

    if (stk->info.variable_name == (const char *) POISON_NAME) stk->info.variable_name = "POISON_NAME";
    if (stk->info.file_name     == (const char *) POISON_NAME) stk->info.file_name     = "POISON_NAME";
    if (stk->info.function_name == (const char *) POISON_NAME) stk->info.function_name = "POISON_NAME";

    fprintf(LOG_STREAM, "<font color=blue> Stack[%p] \"%s\" was constructed at\n"
                            "file: \"%s\"\n"
                            "func: \"%s\"\n"
                            "line: \"%d\"\n"
                            "{\n"
                            "size     = %u\n"
                            "capacity = %u\n</font>", stk, stk->info.variable_name,
                                               stk->info.file_name, stk->info.function_name, stk->info.string_number,
                                               stk->size, stk->capacity);
    if (stk->data == nullptr)
    {
        fprintf(LOG_STREAM, "<font color=blue>data[nullptr]\n\n </font>");
        return;
    }

    if (stk->data == (Stack_elem *) POISON_DATA)
    {
        fprintf(LOG_STREAM, "<font color=blue>data </font>[<font color=yellow>POISON_DATA</font>]\n\n");
        return;
    }

    #ifdef CANARY_PROTECTION

        unsigned left = 0, right = 0;

        StackCheckCanary(stk, &left, &right);

        (left  ==  LEFT_CANARY) ? fprintf(LOG_STREAM, "<font color=blue> left_canary  = %u </font> <font color=green> (OK) </font>\n",     left) :
                                  fprintf(LOG_STREAM, "<font color=blue> left_canary  = %u </font> <font color=red> (ERROR) </font>\n",    left) ;

        (right == RIGHT_CANARY) ? fprintf(LOG_STREAM, "<font color=blue> right_canary = %u </font> <font color=green> (OK) </font>\n",    right) :
                                  fprintf(LOG_STREAM, "<font color=blue> right_canary = %u </font> <font color=red> (ERROR) </font>\n",   right) ;

    #endif

    fprintf(LOG_STREAM, "<font color=blue>data[%p]\n\t{\n</font>", stk->data);

    for (size_t data_counter = 0; data_counter < stk->capacity; ++data_counter)
    {
        putc('\t', LOG_STREAM);

        (data_counter < stk->size) ? putc('*', LOG_STREAM) : putc(' ', LOG_STREAM);

        fprintf(LOG_STREAM, "<font color=blue>[%d] = </font>", data_counter);

        log_stack_elem(stk->data + data_counter);

        putc('\n', LOG_STREAM);
    }
    fprintf(LOG_STREAM, "\t}\n\n");
}

void log_dumping_ctor(Stack *stk, const int capacity, const char *stk_name,
                                                      const char *stk_func,
                                                      const char *stk_file, const int stk_line)
{
    if (stk_name == nullptr) stk_name = "nullptr";
    if (stk_func == nullptr) stk_func = "nullptr";
    if (stk_file == nullptr) stk_file = "nullptr";

    fprintf(LOG_STREAM, "(dumping)_StackCtor(stk = %p, capacity = %d,\n"
                        "                                             stk_name = \"%s\"\n"
                        "                                             stk_func = \"%s\"\n"
                        "                                             stk_file = \"%s\"\n"
                        "                                             stk_line = %d)\n\n",
                                             stk,      capacity,      stk_name,
                                                                      stk_func,
                                                                      stk_file,
                                                                      stk_line);
}

void log_push(Stack *stk, const Stack_elem push_val)
{
    fprintf(LOG_STREAM, "StackPush(stk = %p, push_val = ", stk);

    log_stack_elem(&push_val);

    fprintf(LOG_STREAM, ")\n\n");
}

/*--------------------------------------------------------------------------------------------------------------------*/

/**
*   @brief Works in 2 modes.
*   @brief Checks if the variable is filled by "poison_val"     in the first  mode.
*   @brief Checks if the variable doesn't have any poison bytes in the second mode.
*
*   @param _verifiable_elem [in] _verifiable_elem - pointer         to the variable to check
*   @param        elem_size [in]        elem_size - size (in bytes) of the variable to check
*   @param       poison_val [in]       poison_val - poison value to compare with
*   @param             mode [in]             mode - mode of "PoisonCheck()"
*
*   @return In the first  mode: 1 if all bytes are     equal to "poison_value" and 0 else
*   @return In the zero   mode: 1 if all bytes are not equal to "poison_value" and 0 else
*/

unsigned PoisonCheck(void *_verifiable_elem, const size_t elem_size, const unsigned char poison_val,
                                                                const unsigned char mode)
{
    assert(_verifiable_elem != nullptr);

    const unsigned char* verifiable_elem = (const unsigned char *) _verifiable_elem;

    for (size_t counter = 0; counter < elem_size; ++counter)
    {
        unsigned char checking_res = mode ? verifiable_elem[counter] == poison_val :
                                            verifiable_elem[counter] != poison_val ;
        if (!checking_res)
        {
            log_func_end(__PRETTY_FUNCTION__, 0);
            return 0;
        }
    }
    return 1;
}

/**
*   @brief Fills the segment [l, r) of the array "_fillable_elem" by "poison_val".
*
*   @param _fillable_elem [in] _fillable_elem - pointer to the first byte of the array to fill
*   @param      elem_size [in]      elem_size - size (in bytes) of the array's elements
*   @param           left [in]           left - index of the filling segment beginning
*   @param          right [in]          right - index of the filling segment ending
*   @param     poison_val [in]     poison_val - value to fill in
*
*   @return nothing
*/

void FillPoison(void *_fillable_elem, const size_t elem_size, const unsigned left,
                                                              const unsigned right, const unsigned char poison_val)
{
    fprintf(LOG_STREAM, "FillPoison(_fillable_elem = %p, elem_size = %u,\n"
                        "                                                left  = %u,\n"
                        "                                                right = %u, poison_val = %u)\n\n",
                                    _fillable_elem,      elem_size,      left,
                                                                         right,      poison_val);
    assert(_fillable_elem != nullptr);

    char *fillable_elem = (char *) _fillable_elem;

    memset(fillable_elem + elem_size * left, poison_val, elem_size * (right - left));

    log_func_end(__PRETTY_FUNCTION__, 0);
}

#ifdef CANARY_PROTECTION

    /**
    *   @brief Checks if canary protection of the "stack" is OK. Puts values of canaries in the "left_canary" and
    *   @brief the "right_canary".
    *
    *   @param          stk [in]           stk - pointer to the "stack" to check the canary protection
    *   @param  left_canary [out]  left_canary - pointer to the  left_canary value. In case of nullptr, value doesn't put
    *   @param right_canary [out] right_canary - pointer to the right_canary value. In case of nullptr, value doesn't put
    *
    *   @return 0 if check failed and 0 else
    */

    unsigned StackCheckCanary(Stack *stk, unsigned *const left_canary, unsigned *const right_canary)
    {
        fprintf(LOG_STREAM, "CheckCanary(stk = %p)\n\n",
                                         stk);

        unsigned  left = *(unsigned *) ((unsigned *)stk->data - 1);
        unsigned right = *(unsigned *) (stk->data + stk->capacity);

        unsigned ret = (left == (unsigned) LEFT_CANARY) && (right == (unsigned) RIGHT_CANARY);

        if ( left_canary) * left_canary = left ;
        if (right_canary) *right_canary = right;

        log_func_end(__PRETTY_FUNCTION__, ret);
        return ret;
    }

#endif

#ifdef STACK_DUMPING

    #define Stack_assert(stk_ptr, err)                                                          \
            if ((*err = StackVerify(stk_ptr)))                                                    \
            {                                                                                   \
                StackDump(stk_ptr, *err, __FILE__, __PRETTY_FUNCTION__, __LINE__);              \
                                                                                                \
                log_func_end(__PRETTY_FUNCTION__, *err);                                        \
                return *err;                                                                    \
            }
#else

    #define Stack_assert(stk_ptr, err)                                                          \
            if ((*err = StackVerify(stk_ptr)))                                                    \
            {                                                                                   \
                log_func_end(__PRETTY_FUNCTION__, *err);                                        \
                return *err;                                                                    \
            }

#endif

#ifdef STACK_DUMPING

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
        fprintf(LOG_STREAM, "StackDump(stk = %p, err = %u,\n"
                            "                              current_file = \"%s\"\n"
                            "                              current_func = \"%s\"\n"
                            "                              current_line = %d)\n\n",
                                       stk,      err,      current_file,
                                                           current_func,
                                                           current_line);
        log_message("MESSAGE_ERRORS:");

        int error_numbers = sizeof(error_message) / sizeof(char *);

        for (int i = 0; i <= error_numbers; ++i)
        {
            if (err & (1 << i))
                log_message(error_message[i]);
        }

        log_make_dump(stk, current_file,
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

    unsigned _StackCtor(Stack *stk, int capacity, const char *stk_name,
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
        stk->size    = 0;

        stk->info.variable_name = stk_name + 1; // add 1 to skip the '&' character
        stk->info.function_name = stk_func;
        stk->info.file_name     = stk_file;
        stk->info.string_number = stk_line;

        capacity = (capacity < 0) ? 0 : capacity;

        #ifdef CANARY_PROTECTION

            unsigned *temp_data_store = (unsigned *) calloc(1, capacity * sizeof(Stack_elem) + 8);

            if (temp_data_store == nullptr)
            {
                stk->data        = nullptr;

                make_bit_true(&err, MEMORY_LIMIT_EXCEEDED);
                StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

                log_func_end(__PRETTY_FUNCTION__, err);
                return err;
            }
            else
            {
                stk->data        = (Stack_elem *) (temp_data_store + 1);
                stk->capacity    = capacity;

                *temp_data_store = (unsigned)  LEFT_CANARY;
                 temp_data_store = (unsigned *) (stk->data + stk->capacity);
                *temp_data_store = (unsigned) RIGHT_CANARY;
            }

        #else

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
            }
        #endif

        if (stk->capacity)
            FillPoison(stk->data, sizeof(Stack_elem), 0, capacity, (unsigned char) POISON_BYTE);

        Stack_assert(stk, &err);

        log_func_end(__PRETTY_FUNCTION__, STACK_OK);
        return (unsigned) STACK_OK;
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
    fprintf(LOG_STREAM, "StackVerify(stk = %p)\n\n",
                                     stk);

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

    if ((int) stk->size < 0)
        make_bit_true(&err, SIZE_INVALID);

    if ((int) stk->capacity < 0)
        make_bit_true(&err, CAPACITY_INVALID);

    if (stk->size > stk->capacity)
    {
        make_bit_true(&err,     SIZE_INVALID);
        make_bit_true(&err,  CAPACITY_INVALID);
    }

    if ((int) stk->capacity > 0)
    {
        if (stk->data == nullptr)
            make_bit_true(&err, CAPACITY_INVALID);

        else if (stk->data != (Stack_elem *) POISON_DATA)
        {
            for (size_t counter = 0; counter < stk->size; ++counter)
            {
                if (!PoisonCheck(stk->data + counter, sizeof(Stack_elem), (unsigned char) POISON_BYTE,
                                                                          (unsigned char) 0))
                {
                    make_bit_true(&err, ACTIVE_POISON_VALUES);
                    break;
                }
            }

            for (size_t counter = stk->size; counter < stk->capacity; ++counter)
            {
                if (!PoisonCheck(stk->data + counter, sizeof(Stack_elem), (unsigned char) POISON_BYTE,
                                                                          (unsigned char) 1))
                {
                    make_bit_true(&err, NON_ACTIVE_NON_POISON_VALUES);
                    break;
                }
            }
        }
    }
    if (stk->data == (Stack_elem *) POISON_DATA || stk->data == nullptr)
    {
        log_func_end(__PRETTY_FUNCTION__, err);
        return err;
    }

    #ifdef CANARY_PROTECTION

        if (!StackCheckCanary(stk))
            make_bit_true(&err, CANARY_PROTECTION_FAILED);

    #endif

    log_func_end(__PRETTY_FUNCTION__, err);
    return err;
}

/**
*   @brief Makes the bit of the unsigned int true.
*
*   @param     num [in][out] num - pointer to the unsigned int
*   @param bit_num [in]  bit_num - number of the bit to make true
*
*   @return nothing
*/

void make_bit_true(unsigned *const num, const unsigned bit_num)
{
    assert (num);

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
    fprintf(LOG_STREAM, "StackPop(stk = %p, front_val = %p)\n\n",
                                  stk,      front_val);

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

    FillPoison(stk->data, sizeof(Stack_elem), stk->size, stk->size + 1, (unsigned char) POISON_BYTE);

    Stack_assert(stk, &err);

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
    fprintf(LOG_STREAM, "StackRealloc(stk = %p, condition = %d)\n\n",
                                      stk,      condition);

    unsigned err = 0;
    Stack_assert(stk, &err);

    int future_capacity = 0;

    if (condition)
    {
            future_capacity = 2 *  stk->capacity;
        if (future_capacity < 4) future_capacity = 4; //default elementary capacity
    }
    else
    {
        if ((stk->size != 0) && (stk->capacity >= 4 * stk->size)) future_capacity = 2 * stk->size;
    }

    #ifdef CANARY_PROTECTION

        int *temp_data_store = (int *) realloc((unsigned *) (stk->data) - 1, 8 + sizeof(Stack_elem) * future_capacity);

    #else

        Stack_elem *temp_data_store = (Stack_elem *) realloc(stk->data, sizeof(Stack_elem) * future_capacity);

    #endif

    if (temp_data_store == nullptr)
    {
        make_bit_true(&err, MEMORY_LIMIT_EXCEEDED);

        #ifdef STACK_DUMPING

            StackDump(stk, err, __FILE__, __PRETTY_FUNCTION__, __LINE__);

        #endif

        log_func_end(__PRETTY_FUNCTION__, err);
        return err;
    }


    #ifdef CANARY_PROTECTION

        stk->data = (Stack_elem *) (temp_data_store + 1);

        *(int *) (stk->data + stk->capacity) = RIGHT_CANARY;

    #else

        stk->data = temp_data_store;

    #endif

    stk->capacity = future_capacity;

    FillPoison(stk->data, sizeof(Stack_elem), stk->size, stk->capacity, (unsigned char) POISON_BYTE);

    Stack_assert(stk, &err);

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
    fprintf(LOG_STREAM, "StackDtor(stk = %p)\n\n",
                                   stk);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (stk->data != nullptr)
    {
        #ifdef CANARY_PROTECTION

            free((unsigned *) stk->data - 1);

        #else

            free(stk->data);

        #endif

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
