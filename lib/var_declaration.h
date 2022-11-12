#ifndef VAR_DECLARATION_H
#define VAR_DECLARATION_H

struct var_declaration
{
    const char  *name_file, *name_func, *name_var;
    unsigned int line;    
};

/*___________________________FUNCTION_DECLARATION___________________________*/

void var_ctor (var_declaration *const var, const char *name_file, const char *name_func, const char *name_var, const unsigned int line);
void var_dtor (var_declaration *const var);
void var_dump (var_declaration *const var);

/*__________________________________________________________________________*/

#endif //VAR_DECLARATION_H