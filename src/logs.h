const char *LOG_FILE_NAME = "log.html";
FILE       *LOG_STREAM    = nullptr;

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
    assert( LOG_STREAM != nullptr);

    setvbuf(LOG_STREAM,   nullptr, _IONBF, 0);

    fprintf(LOG_STREAM, "<pre>\n""\"%s\" OPENING IS OK\n\n", LOG_FILE_NAME);

    atexit(CLOSE_LOG_STREAM);
    return 1;
}

int  _OPEN_CLOSE_LOG_STREAM = OPEN_LOG_STREAM();

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

    for (size_t i = 0; i < sizeof(Stack_elem); ++i)
    {
        current_byte_value = *((const unsigned char *) var + i);

        if (current_byte_value != (unsigned char) POISON_BYTE)
            is_poison = 0;

        fprintf(LOG_STREAM, "<font color=blue>%u</font>", current_byte_value);
    }

    if (is_poison)
        fprintf(LOG_STREAM, " <font color=lightgreen> (POISON) </font>");
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
        fprintf(LOG_STREAM, "<font color=blue>data </font>[<font color=lightgreen>POISON_DATA</font>]\n\n");
        return;
    }

    #ifdef CANARY_PROTECTION

        unsigned left = 0, right = 0;

        StackCheckCanary(stk, &left, &right);

        (left  ==  LEFT_CANARY) ? fprintf(LOG_STREAM, "<font color=blue>  left_canary = %16u </font> <font color=green> (OK) </font>\n",     left) :
                                  fprintf(LOG_STREAM, "<font color=blue>  left_canary = %16u </font> <font color=red> (ERROR) </font>\n",    left) ;

        (right == RIGHT_CANARY) ? fprintf(LOG_STREAM, "<font color=blue> right_canary = %16u </font> <font color=green> (OK) </font>\n",    right) :
                                  fprintf(LOG_STREAM, "<font color=blue> right_canary = %16u </font> <font color=red> (ERROR) </font>\n",   right) ;

    #endif

    #ifdef HASH_PROTECTION

        unsigned good_hash = CheckHash(stk->data, stk->capacity * sizeof(Stack_elem), stk->hash_val);

        if (good_hash)
            fprintf(LOG_STREAM, "<font color=blue> hash_val = %llx </font> <font color=green>  (OK) </font>\n", stk->hash_val);
        else
            fprintf(LOG_STREAM, "<font color=blue> hash_val = %llx </font> <font color=red> (ERROR) </font>\n", stk->hash_val);

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
