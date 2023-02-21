

typedef struct clisp_positional {
  void **elems;
  uint64_t num_elems;
} clisp_positional;

typedef union clisp_value {
  void *ptr_val;
  void*(*func_val)();
  clisp_positional* positional;
} clisp_value;

typedef struct clisp {
  clisp_type type;
  clisp_value *value;
} clisp;
