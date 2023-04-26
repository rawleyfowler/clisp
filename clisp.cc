#include <cctype>
#include <cstdint>
#include <vector>
#include <stack>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <malloc.h>
#endif

static const std::string example = "(DEFINE $abc 123)";
static const std::string example2 = "(DEFINE @abc '(1 2 3)";

struct clisp_object;
struct clisp_lambda;

enum clisp_sigil {
  SCALAR = 0,
  VECTOR,
  ASSOC,
  CALLABLE
};

enum clisp_type {
  STRING = 0,
  BOOL,
  FUNCTION,
  FLOAT,
  INTEGER,
  SYMBOL,
  NIL,
  ERROR,
  LIST
};

union clisp_value {
  void *ptr_val;
  void *(*lambda)();
  std::vector<clisp_object*> *pos_val;
};

struct clisp_object {
  const clisp_type type;
  const clisp_value value;

  clisp_object(clisp_type type, clisp_value value):
    type(type),
    value(value) {}
};

static clisp_object clisp_nil = clisp_object(NIL, (clisp_value) { .ptr_val = nullptr });

static clisp_object print(clisp_object& obj) {
  switch(obj.type) {
  case STRING: std::cout << std::string((const char*) obj.value.ptr_val) << std::endl;
  default: std::cout << "NOT IMPLEMENTED" << std::endl;
  }
  return clisp_nil;
}

static clisp_object read(std::string text) {
  std::string::iterator iterator = text.begin();
  clisp_type type = NIL;
  char c;
  while ((c = *iterator)) {
    switch (c) {
    case '(':
      return read(text.substr());
    case '"':
      type = STRING;
    case ' ':
      
    case ')':
      
    default:
      if (std::isdigit(c)) {
        type = INTEGER;
      } else if (c == '.') {
        type = FLOAT;
      } else if (std::isalpha(c)) {
        type = FUNCTION;
      }
    }
    iterator++;
  }
}
