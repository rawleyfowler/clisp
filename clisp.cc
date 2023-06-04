#include <cctype>
#include <cstdint>
#include <vector>
#include <map>
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

enum clisp_sigil
{
  SCALAR = 0,
  VECTOR,
  ASSOC,
  CALLABLE
};

enum clisp_type
{
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

std::ostream &operator<<(std::ostream &l, clisp_type t)
{
  switch (t)
  {
  case STRING:
    l << "string";
    break;
  case BOOL:
    l << "bool";
    break;
  case FUNCTION:
    l << "function";
    break;
  case FLOAT:
    l << "float";
    break;
  case INTEGER:
    l << "integer";
    break;
  case SYMBOL:
    l << "symbol";
    break;
  case NIL:
    l << "nil";
    break;
  case ERROR:
    l << "error";
    break;
  case LIST:
    l << "list";
    break;
  }
  return l;
}

union clisp_value
{
  void *ptr_val;
  clisp_object *(*lambda)(std::vector<clisp_object>);
  std::vector<clisp_object> *pos_val;
};

struct clisp_object
{
  const clisp_type type;
  const clisp_value value;
  clisp_object(clisp_type type, clisp_value value) : type(type),
                                                     value(value) {}
};

static std::map<std::string, clisp_object> func_map;

static void not_implemented()
{
  die_with_error("NOT IMPLEMENTED");
}

static void die_with_error(std::string str)
{
  std::cout << "Died with exn: " << str << std::endl;
  exit(1);
}

static clisp_object clisp_nil = clisp_object(NIL, (clisp_value){.ptr_val = nullptr});

static clisp_object print(clisp_object &obj)
{
  switch (obj.type)
  {
  case STRING:
    std::cout << std::string((const char *)obj.value.ptr_val) << std::endl;
    break;
  default:
    not_implemented();
  }
  return clisp_nil;
}

static clisp_value make_val_from_str(std::string val, clisp_type type)
{
  not_implemented();
  return clisp_nil.value;
}

static clisp_object read(std::string text)
{
  std::string::iterator iterator = text.begin();
  clisp_type type = NIL;
  char c;
  std::string obj;
  while ((c = *iterator))
  {
    switch (c)
    {
    case '(':
      return read(text.substr());
    case '"':
      type = STRING;
    case ' ':
      if (type == FUNCTION)
      {
        std::string args_str;
        while (*iterator++ != ')')
        {
          args_str += *iterator;
        }
        std::vector<clisp_object> args = read_args(args_str);
        auto fun = func_map.at(obj);
        auto ret = fun.value.lambda(args);
        return *ret;
      }
    case ')':
      if (type == NIL && obj != "nil")
      {
        die_with_error("I don't understand you: " + obj);
      }
      return clisp_object(type, clisp_value());
    default:
      if (std::isdigit(c))
      {
        type = INTEGER;
      }
      else if (c == '.')
      {
        type = FLOAT;
      }
      else if (std::isalpha(c))
      {
        type = FUNCTION;
      }
    }
    iterator++;
  }
}

static std::vector<clisp_object> read_args(std::string text)
{
}
