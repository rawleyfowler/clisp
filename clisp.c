#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

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

typedef struct {
  void** elems;
  uint64_t num_elems;
} clisp_positional;

typedef union {
  void* ptr_val;
  void*(*func_val)();
  clisp_positional* positional;
} clisp_value;

typedef struct {
  clisp_type type;
  clisp_value *value;
} clisp;

static clisp_value nil = (clisp_value) { .ptr_val = NULL };
static clisp *clisp_nil_ptr = &((clisp){.type = NIL, .value = &nil});
static clisp* clisp_nil(void) { return clisp_nil_ptr; }

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

  uint64_t num_elems = list->value->positional->num_elems;
  if (!num_elems) {
    return clisp_nil();
  } else if(num_elems == 1) {
    return clisp_eval_func_noargs((clisp*) list->value->positional->elems[0]);
  } else {
    clisp* params[list->value->positional->num_elems - 1];
    for (size_t i = 1; i < list->value->positional->num_elems; i++)
      params[i] = list->value->positional->elems[i];
    return clisp_eval_func(list->value->positional->elems[0], params);
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
    return x->value->positional->num_elems;
  case STRING:
    return x->value->positional->num_elems;
  case NIL:
    return false;
  default:
    return x->value->ptr_val != NULL;
  }
}
inline bool clisp_falsy(clisp* x) { return !clisp_truthy(x); }

clisp* clisp_if(clisp* value, clisp* t, clisp* f) {
  if (clisp_truthy(value))
    return t;
  else
    return f;
}

int main(int argc, char** argv) {
  return EXIT_SUCCESS;
}
