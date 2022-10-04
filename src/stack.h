/** @file */

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>

#define  STACK_DUMPING
#define CANARY_PROTECTION
#define   HASH_PROTECTION

#ifdef STACK_DUMPING

    /**
    *   @brief Contains information about the Stack variable declaration.
    *
    *   @param  variable_name - name   of the variable
    *   @param  function_name - name   of the function where the variable declarated
    *   @param      file_name - name   of the     file where the variable declarated
    *   @param strings_number - number of the   string where the variable declareted
    */

    typedef struct _VarDeclaration
    {
        const char *variable_name, *function_name, *file_name;
        int string_number;

    } VarDeclaration;

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

    #ifdef HASH_PROTECTION

        unsigned long long hash_val;

    #endif

    #ifdef STACK_DUMPING

        VarDeclaration info;

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
*   @param CAPACITY_INVALID   - Stack's capacity is invalid(lower than zero or less then     size)
*   @param SIZE_INVALID       - Stack's     size is invalid(lower than zero or more than capacity)
*
*   @param ACTIVE_POISON_VALUES         - poison-values   are active
*   @param NON_ACTIVE_NON_POISON_VALUES - refuse elements are non-poison
*
*   @param MEMORY_LIMIT_EXCEEDED        - memory allocation query is failed
*
*   @param CANARY_PROTECTION_FAILED     - canary protection is failed
*   @param HASH_PROTECTION_FAILED       - hash   protection is failed
*/

typedef enum _StackError
{
    STACK_OK                     = 0,
    STACK_NULLPTR                = 1,
    STACK_NON_CTOR               = 2,
    STACK_ALREADY_CTOR           = 3,
    STACK_EMPTY                  = 4,

    CAPACITY_INVALID             = 5,
    SIZE_INVALID                 = 6,

    ACTIVE_POISON_VALUES         = 7,
    NON_ACTIVE_NON_POISON_VALUES = 8,

    MEMORY_LIMIT_EXCEEDED        = 9,

    CANARY_PROTECTION_FAILED     = 10,
    HASH_PROTECTION_FAILED       = 11

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
    "canary protection failed",              // 10
    "hash   protection failed"               // 11
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

/**
*   @brief The enum contains any protection constants.
*
*   @param LEFT_CANARY  - value for the  left canary protection
*   @param RIGHT_CANARY - value for the right canary protection
*
*   @param HASH_START   - begining value of the hash
*/

typedef enum _Protection
{
    LEFT_CANARY  = 0xBAADF00D,
    RIGHT_CANARY = 0xDEADBEEF,
    HASH_START   = 0xFEEDFACE

} Protection;

/*---------------------------------------------FUNCTIONS_DECLARATION--------------------------------------------------*/

static unsigned StackVerify(Stack *stk);

static unsigned StackPush   (Stack *stk, const Stack_elem push_val);
static unsigned StackPop    (Stack *stk, Stack_elem *const front_val = nullptr);
static unsigned StackDtor   (Stack *stk);
static unsigned StackRealloc(Stack *stk, const int condition);

static unsigned  PoisonCheck(void *_verifiable_elem, const size_t elem_size, const unsigned char poison_val,
                                                                      const unsigned char mode);
static void FillPoison(void *_fillable_elem, const size_t elem_size, const unsigned left,
                                                              const unsigned right, const unsigned char poison_val);
static void make_bit_true(unsigned *const num, const unsigned bit_num);

#ifdef STACK_DUMPING

    static void StackDump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   const char *current_line);

    static unsigned _StackCtor(Stack *stk, int capacity, const char *stk_name,
                                                  const char *stk_func,
                                                  const char *stk_file, const int stk_line);

#endif

#ifdef CANARY_PROTECTION

    static unsigned StackCheckCanary(Stack *stk, unsigned *const left_canary  = nullptr,
                                          unsigned *const right_canary = nullptr);

#endif

#ifdef HASH_PROTECTION

    static unsigned long long get_hash(void *_data_store, const size_t elem_size);
    static unsigned CheckHash(void *_data_store, const size_t elem_size, unsigned long long hash_val);

#endif

/*---------------------------------------LOG_FUNCTIONS_DECLARATION----------------------------------------------------*/

/**
*   @brief enum contains HTML-COLORS
*
*   @param YELLOW       - "Gold"
*   @param RED          - "DarkRed"
*   @param GREEN        - "LimeGreen"
*   @param BLUE         - "MediumBlue"
*   @param POISON_COLOR - "Olive"
*   @param USUAL        - ""
*/

enum COLOR
{
    YELLOW,
    RED,
    GREEN,
    BLUE,
    POISON_COLOR,
    USUAL
};

const char *COLOR_NAMES[] = 
{
    "Gold",
    "DarkRed",
    "LimeGreen",
    "MediumBlue",
    "Olive",
    ""
};

const char *LOG_FILE_NAME = "log.html";
FILE       *LOG_STREAM    = nullptr;

char *TAB_SHIFT = nullptr;
int   TAB_NUM = 0;

/**
*   @brief Closes log-file. Called by using atexit().
*
*   @return 1 if closing is OK. Does abort() if an ERROR found.
*/

void CLOSE_LOG_STREAM()
{
    assert(LOG_STREAM != nullptr);

    fprintf(LOG_STREAM, "\"%s\" CLOSING IS OK\n\n", LOG_FILE_NAME);
    fclose( LOG_STREAM);
}

/**
*   @brief Opens log-file. Ckecks if opening is OK and in this case prints message in the log-file.
*   @brief Uses atexit() to call CLOSE_LOG_STREAM() after program end.
*
*   @return 1 if checking is OK. Does abort() if an ERROR found.
*/

int OPEN_LOG_STREAM()
{
    LOG_STREAM = fopen(LOG_FILE_NAME, "w");

    TAB_SHIFT  = (char *) calloc(1, 100);
    TAB_NUM    = 0;

    assert(LOG_STREAM != nullptr);
    assert(TAB_SHIFT  != nullptr);

    setvbuf(LOG_STREAM,   nullptr, _IONBF, 0);

    fprintf(LOG_STREAM, "<pre>\n""\"%s\" OPENING IS OK\n\n", LOG_FILE_NAME);

    atexit(CLOSE_LOG_STREAM);
    return 1;
}

int  _OPEN_CLOSE_LOG_STREAM = OPEN_LOG_STREAM();

void log_message(COLOR col, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    fprintf (LOG_STREAM, "<font color=%s>", COLOR_NAMES[col]);
    vfprintf(LOG_STREAM, fmt, ap);
    fprintf (LOG_STREAM, "</font>");
}

void log_func_end(const char *function_name, unsigned err)
{
    TAB_SHIFT[--TAB_NUM] = '\0';

    log_message(USUAL, "%s returns %d\n\n%s", function_name, err, TAB_SHIFT);
}

void log_stack_elem(const Stack_elem *var)
{
    unsigned char current_byte_value = 0;
    unsigned char is_poison = 1;

    for (size_t i = 0; i < sizeof(Stack_elem); ++i)
    {
        current_byte_value = *((const unsigned char *) var + i);

        if (current_byte_value != (unsigned char) POISON_BYTE)
            is_poison = 0;

        log_message(BLUE, "%x", current_byte_value);
    }

    if (is_poison)
        log_message(POISON_COLOR, "(POISON)");
}

void log_make_dump(Stack *stk, const char *current_file,
                               const char *current_func,
                               int         current_line)
{
    log_message(BLUE, "\n%sERROR occurred at:\n"
                      "             %sFILE: %s\n"
                      "             %sFUNC: %s\n"
                      "             %sLINE: %d\n\n%s", TAB_SHIFT, TAB_SHIFT, current_file, TAB_SHIFT, current_func, TAB_SHIFT, current_line, TAB_SHIFT);    

    if (stk == nullptr)
    {
        fprintf(LOG_STREAM, "Stack[nullptr]\n%s", TAB_SHIFT);
        return;
    }

    if (stk->info.variable_name == nullptr) stk->info.variable_name = "nullptr";
    if (stk->info.file_name     == nullptr) stk->info.file_name     = "nullptr";
    if (stk->info.function_name == nullptr) stk->info.function_name = "nullptr";

    if (stk->info.variable_name == (const char *) POISON_NAME) stk->info.variable_name = "POISON_NAME";
    if (stk->info.file_name     == (const char *) POISON_NAME) stk->info.file_name     = "POISON_NAME";
    if (stk->info.function_name == (const char *) POISON_NAME) stk->info.function_name = "POISON_NAME";

    log_message(BLUE, "Stack[%p] \"%s\" was constructed at\n%s"
                      "file: \"%s\"\n%s"
                      "func: \"%s\"\n%s"
                      "line: \"%d\"\n%s"
                      "{\n%s"
                      "\tsize     = %u\n%s"
                      "\tcapacity = %u\n%s", stk, stk->info.variable_name, TAB_SHIFT,
                                               stk->info.file_name, TAB_SHIFT, stk->info.function_name, TAB_SHIFT, stk->info.string_number, TAB_SHIFT,
                                               TAB_SHIFT, stk->size, TAB_SHIFT, stk->capacity, TAB_SHIFT);
    if (stk->data == nullptr)
    {
        log_message(BLUE, "\tdata[nullptr]\n%s}\n%s", TAB_SHIFT, TAB_SHIFT);
        return;
    }

    if (stk->data == (Stack_elem *) POISON_DATA)
    {
        log_message(BLUE, "\tdata");
        log_message(POISON_COLOR, "[POISON_DATA]");
        log_message(BLUE, "\n%s}\n%s", TAB_SHIFT, TAB_SHIFT);
        return;
    }

    #ifdef CANARY_PROTECTION

        unsigned left = 0, right = 0;

        StackCheckCanary(stk, &left, &right);

        if (left == LEFT_CANARY)
        {
            log_message(BLUE,  "\tleft_canary  = %16u", left);
            log_message(GREEN, "(OK)\n%s",         TAB_SHIFT);
        }
        else
        {
            log_message(BLUE,  "\tleft_canary  = %16u", left);
            log_message(RED,   "(ERROR)\n%s",      TAB_SHIFT);
        }

        if (right == RIGHT_CANARY)
        {
            log_message(BLUE,  "\tright_canary = %16u", right);
            log_message(GREEN, "(OK)\n%s",          TAB_SHIFT);
        }
        else
        {
            log_message(BLUE, "\tright_canary = %16u", right);
            log_message(RED,  "(ERROR)\n%s",       TAB_SHIFT);
        }

    #endif

    #ifdef HASH_PROTECTION

        unsigned good_hash = CheckHash(stk->data, stk->capacity * sizeof(Stack_elem), stk->hash_val);

        if (good_hash)
        {
            log_message(BLUE,  "\thash_val = %llx", stk->hash_val);
            log_message(GREEN, "(OK)\n%s",            TAB_SHIFT);
        }
        else
        {
            log_message(BLUE, "\thash_val = %llx", stk->hash_val);
            log_message(RED,  "(ERROR)\n%s",         TAB_SHIFT);
        }

    #endif

    log_message(BLUE, "\tdata[%p]\n%s\t{\n%s", stk->data, TAB_SHIFT, TAB_SHIFT);

    for (size_t data_counter = 0; data_counter < stk->capacity; ++data_counter)
    {
        putc('\t', LOG_STREAM);

        (data_counter < stk->size) ? putc('*', LOG_STREAM) : putc(' ', LOG_STREAM);

        log_message(BLUE, "[%d] = ", data_counter);

        log_stack_elem(stk->data + data_counter);

        fprintf(LOG_STREAM, "\n%s", TAB_SHIFT);
    }
    log_message(BLUE, "\t}\n%s}\n%s", TAB_SHIFT, TAB_SHIFT);
}

void log_dumping_ctor(Stack *stk, const int capacity, const char *stk_name,
                                                      const char *stk_func,
                                                      const char *stk_file, const int stk_line)
{
    if (stk_name == nullptr) stk_name = "nullptr";
    if (stk_func == nullptr) stk_func = "nullptr";
    if (stk_file == nullptr) stk_file = "nullptr";

    log_message(USUAL, "(dumping)_StackCtor(stk = %p, capacity = %d,\n%s"
                       "stk_name = \"%s\"\n%s"
                       "stk_func = \"%s\"\n%s"
                       "stk_file = \"%s\"\n%s"
                       "stk_line = %d)\n\n%s\t",
                                            stk,      capacity,       TAB_SHIFT,
                        stk_name, TAB_SHIFT,
                        stk_func, TAB_SHIFT,
                        stk_file, TAB_SHIFT,
                        stk_line, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';
}

void log_push(Stack *stk, const Stack_elem push_val)
{
    fprintf(LOG_STREAM, "StackPush(stk = %p, push_val = ", stk);

    log_stack_elem(&push_val);

    fprintf(LOG_STREAM, ")\n\n%s\t", TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';
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

static unsigned PoisonCheck(void *_verifiable_elem, const size_t elem_size, const unsigned char poison_val,
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

static void FillPoison(void *_fillable_elem, const size_t elem_size, const unsigned left,
                                                              const unsigned right, const unsigned char poison_val)
{
    fprintf(LOG_STREAM, "FillPoison(_fillable_elem = %p, elem_size = %lu,\n%s"
                        "                                                left  = %u,\n%s"
                        "                                                right = %u, poison_val = %u)\n\n%s",
                                    _fillable_elem,      elem_size, TAB_SHIFT,
                                                                         left, TAB_SHIFT,
                                                                         right,      poison_val, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';

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

    static unsigned StackCheckCanary(Stack *stk, unsigned *const left_canary, unsigned *const right_canary)
    {
        assert(stk != nullptr);

        unsigned  left = *(unsigned *) ((unsigned *)stk->data - 1);
        unsigned right = *(unsigned *) (stk->data + stk->capacity);

        unsigned ret = (left == (unsigned) LEFT_CANARY) && (right == (unsigned) RIGHT_CANARY);

        if ( left_canary) * left_canary = left ;
        if (right_canary) *right_canary = right;

        return ret;
    }

#endif

#ifdef HASH_PROTECTION

    /**
    *   @brief Counts the hash_value for the variable of any type.
    *
    *   @param _data_store [in] _data_store - pointer to the first byte of the variable to hash
    *   @param   elem_size [in]   elem_size - size (in bytes)           of the variable to hash
    *
    *   @return hash_value
    */

    static unsigned long long get_hash(void *_data_store, const size_t elem_size)
    {
        assert(_data_store != nullptr);

        unsigned char *data_store = (unsigned char *) _data_store;
        unsigned long long hash_ret = HASH_START;

        for (size_t counter = 0; counter < elem_size; ++counter)
        {
            hash_ret = ((hash_ret << 5) + hash_ret) + data_store[counter];
        }

        return hash_ret;
    }

    /**
    *   @brief Checks if the "hash_val" of any variable still equal to original hash.
    *
    *   @param _data_store [in] _data_store - pointer to the first byte of the variable to check
    *   @param   elem_size [in]   elem_size - size (in bytes)           of the variable to check
    *   @param    hash_val [in]    hash_val - hash value to compare with
    *
    *   @return true-value if the "hash_val" is right and false-value else
    */

    static unsigned CheckHash(void *_data_store, const size_t elem_size, const unsigned long long hash_val)
    {
        assert(_data_store != nullptr);

        unsigned long long     old_hash = hash_val;
        unsigned long long current_hash = get_hash(_data_store, elem_size);

        unsigned ret = (old_hash == current_hash);

        return ret;
    }

#endif

#ifdef STACK_DUMPING

    #define Stack_assert(stk_ptr, err)                                                          \
            if ((*err = StackVerify(stk_ptr)))                                                  \
            {                                                                                   \
                StackDump(stk_ptr, *err, __FILE__, __PRETTY_FUNCTION__, __LINE__);              \
                                                                                                \
                log_func_end(__PRETTY_FUNCTION__, *err);                                        \
                return *err;                                                                    \
            }
#else

    #define Stack_assert(stk_ptr, err)                                                          \
            if ((*err = StackVerify(stk_ptr)))                                                  \
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

    static void StackDump(Stack *stk, const unsigned err, const char *current_file,
                                                   const char *current_func,
                                                   int         current_line)
    {
        fprintf(LOG_STREAM, "StackDump(stk = %p, err = %u,\n%s"
                            "                              current_file = \"%s\"\n%s"
                            "                              current_func = \"%s\"\n%s"
                            "                              current_line = %d)\n\n%s",
                                       stk,      err, TAB_SHIFT,
                                                           current_file, TAB_SHIFT,
                                                           current_func, TAB_SHIFT,
                                                           current_line, TAB_SHIFT);
        TAB_SHIFT[TAB_NUM++] = '\t';
        
        err == 0 ? log_message(GREEN, "NO_ERRORS\n%s", TAB_SHIFT) : log_message(RED, "MESSAGE_ERRORS\n%s", TAB_SHIFT);

        int error_numbers = sizeof(error_message) / sizeof(char *);

        for (int i = 0; i <= error_numbers; ++i)
        {
            if (err & (1 << i))
                log_message(RED, error_message[i]);
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

    static unsigned _StackCtor(Stack *stk, int capacity, const char *stk_name,
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

        #ifdef HASH_PROTECTION

            stk->hash_val = get_hash(stk->data, stk->capacity * sizeof(Stack_elem));

        #endif

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

static unsigned StackVerify(Stack *stk)
{
    fprintf(LOG_STREAM, "StackVerify(stk = %p)\n\n%s",
                                     stk, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';

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

    #ifdef HASH_PROTECTION

        if (!CheckHash(stk->data, stk->capacity * sizeof(Stack_elem), stk->hash_val))
            make_bit_true(&err, HASH_PROTECTION_FAILED);

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

static void make_bit_true(unsigned *const num, const unsigned bit_num)
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

static unsigned StackPush(Stack *stk, const Stack_elem push_val)
{
    log_push(stk, push_val);

    unsigned err = 0;
    Stack_assert(stk, &err);

    if (stk->size < stk->capacity)
    {
        stk->data[stk->size++] = push_val;

        #ifdef HASH_PROTECTION

            stk->hash_val = get_hash(stk->data, stk->capacity * sizeof(Stack_elem));

        #endif

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

    #ifdef HASH_PROTECTION

        stk->hash_val = get_hash(stk->data, stk->capacity * sizeof(Stack_elem));

    #endif

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

static unsigned StackPop(Stack *stk, Stack_elem *const front_val)
{
    fprintf(LOG_STREAM, "StackPop(stk = %p, front_val = %p)\n\n%s",
                                  stk,      front_val, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';

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

    #ifdef HASH_PROTECTION

        stk->hash_val = get_hash(stk->data, stk->capacity * sizeof(Stack_elem));

    #endif

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

static unsigned StackRealloc(Stack *stk, const int condition)
{
    fprintf(LOG_STREAM, "StackRealloc(stk = %p, condition = %d)\n\n%s",
                                      stk,      condition, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';

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

    if (future_capacity == 0)
        return STACK_OK;

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

        *(int *) (stk->data + future_capacity) = RIGHT_CANARY;

    #else

        stk->data = temp_data_store;

    #endif

    stk->capacity = future_capacity;

    FillPoison(stk->data, sizeof(Stack_elem), stk->size, stk->capacity, (unsigned char) POISON_BYTE);

    #ifdef HASH_PROTECTION

        stk->hash_val = get_hash(stk->data, stk->capacity * sizeof(Stack_elem));

    #endif

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

static unsigned StackDtor(Stack *stk)
{
    fprintf(LOG_STREAM, "StackDtor(stk = %p)\n\n%s",
                                   stk, TAB_SHIFT);
    TAB_SHIFT[TAB_NUM++] = '\t';

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

    #ifdef HASH_PROTECTION

        stk->hash_val = 0;

    #endif

    log_func_end(__PRETTY_FUNCTION__, STACK_OK);
    return STACK_OK;
}

#endif //STACK
