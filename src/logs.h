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