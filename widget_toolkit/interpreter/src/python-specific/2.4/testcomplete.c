#include <Python.h>
#include <node.h>
#include <errcode.h>
#include <grammar.h>
#include <parsetok.h>
#include <compile.h>

grammar* Py_meta_grammar();

int testcomplete(char *code)

  /* code should end in \n */
  /* return -1 for error, 0 for incomplete, 1 for complete */
{
  node *n;
  perrdetail e;

  n = PyParser_ParseString(code, Py_meta_grammar(), Py_file_input, &e);
  if (n == NULL) {
    if (e.error == E_EOF) 
      return 0;
    return -1;
  }

  PyNode_Free(n);
  return 1;
}
