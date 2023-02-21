#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#include <malloc.h>
#endif

#define true  1
#define false 0

char *example = "(DEFINE $abc 123)";
char *example2 = "(DEFINE @abc '(1 2 3)";

typedef unsigned char bool;

typedef enum {
  STRING = 0,
  BOOL,
  FUNCTION,
  FLOAT,
  INTEGER,
  SYMBOL,
  NIL,
  LIST
} clisp_type;

typedef struct clisp_positional {
  void** elems;
  uint64_t num_elems;
} clisp_positional;

typedef union clisp_value {
  void* ptr_val;
  void*(*func_val)();
  clisp_positional* positional_val;
} clisp_value;

typedef struct clisp {
  clisp_type type;
  clisp_value *value;
} clisp;

static clisp* clisp_make(clisp_type type, void* data, uint64_t num_elems) {
  clisp *t = malloc(sizeof(clisp));
  t->type = type;
  t->value = malloc(sizeof(clisp_value));
  if (type == FUNCTION || type == SYMBOL) {
    t->value->func_val = data;
  } else if(type == LIST || type == STRING) {
    t->value->positional_val = malloc(sizeof(clisp_positional));
    t->value->positional_val->elems = data;
    t->value->positional_val->num_elems = num_elems;
  } else {
    t->value->ptr_val = data;
  }

  return t;
}

static clisp_value nil = (clisp_value){.ptr_val = NULL};
static clisp *clisp_nil_ptr = &((clisp){.type = NIL, .value = &nil});
static inline clisp *clisp_nil(void) { return clisp_nil_ptr; }

static uint64_t num_symbols;
static clisp **clisp_symbols;
static clisp *clisp_add_symbol(char *name, void*(*func)()) {
  num_symbols++;
  clisp* t = clisp_make(SYMBOL, func, 0);
  clisp_symbols = realloc(clisp_symbols, num_symbols);
  clisp_symbols[num_symbols - 1] = t;
}

static void clisp_free_positional(clisp_positional *positional) {
  // Because the elements are clisp's, we probably dont want to just nuke them with the list.
  free(positional->elems);
}
static void clisp_free_value(clisp_value *value) {
  if (value->positional_val) {
    clisp_free_positional(value->positional_val);
  } else if (value->ptr_val) {
    free(value->ptr_val);
  } else {
    free(value->func_val);
  }

  free(value);
}
static void clisp_free(clisp* clisp) {
  clisp_free_value(clisp->value);
  free(clisp);
}

clisp* clisp_eval_func(clisp *func, clisp **args) {
  return (clisp*) func->value->func_val(args);
}

clisp* clisp_eval_func_noargs(clisp* func) {
  // TODO: Errors
  if (func->type != FUNCTION)
    return clisp_nil();

  return (clisp*) func->value->func_val();
}

clisp* clisp_eval_list(clisp* list) {
  // TODO: Errors
  if (list->type != LIST)
    return clisp_nil();

  uint64_t num_elems = list->value->positional_val->num_elems;
  if (!num_elems) {
    return clisp_nil();
  } else if(num_elems == 1) {
    return clisp_eval_func_noargs((clisp*) list->value->positional_val->elems[0]);
  } else {
    clisp_type type = ((clisp *) list->value->positional_val->elems[0])->type;
    // TODO: Errors
    if (type != SYMBOL && type != FUNCTION)
      return clisp_nil();

    clisp* params[list->value->positional_val->num_elems - 1];
    for (size_t i = 1; i < list->value->positional_val->num_elems; i++)
      params[i] = list->value->positional_val->elems[i];
    return clisp_eval_func(list->value->positional_val->elems[0], params);
  }
}

clisp* clisp_eval(clisp* value) {
  switch (value->type) {
  case SYMBOL: {
    return clisp_eval_func_noargs(clisp_lookup_symbol(value));
  }
  case LIST:
    return clisp_eval_list(value);
  default:
    return value;
  }
}

inline bool clisp_truthy(clisp* x) {
  switch(x->type) {
  case FUNCTION:
    return x->value->func_val != NULL;
  case BOOL:
    return *((bool*) x->value->ptr_val);
  case INTEGER:
    return *((int*) x->value->ptr_val);
  case FLOAT:
    return *((float*) x->value->ptr_val);
  case LIST:
    return x->value->positional_val->num_elems;
  case STRING:
    return x->value->positional_val->num_elems;
  default:
    return x->value->ptr_val != NULL;
  }
}
inline bool clisp_falsy(clisp *x) { return !clisp_truthy(x); }

clisp* clisp_update_value(clisp *x, void* data, clisp_type type, uint64_t elems) {
  clisp_free_value(x->value);

  x->type = type;
  if (type == LIST || type == STRING) {
    clisp_positional* positional = malloc(sizeof(clisp_positional));
    positional->num_elems = elems;
    positional->elems     = data;

    x->value = malloc(sizeof(clisp_value));
    x->value->positional_val = positional;
  } else if (type == FUNCTION) {
    x->value->func_val = data;
  } else {
    x->value->ptr_val = data;
  }

  return x;
}

clisp* clisp_if(clisp *value, clisp *t, clisp *f) {
  if (clisp_truthy(value))
    return t;
  else
    return f;
}

clisp* clisp_unless(clisp *value, clisp *t, clisp *f) {
  if (clisp_falsy(value)) {
    return t;
  } else {
    return f;
  }
}

clisp* clisp_defun(char *name, void*(*func)()) {
  clisp* symbol = clisp_add_symbol(name, func);
}

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}

