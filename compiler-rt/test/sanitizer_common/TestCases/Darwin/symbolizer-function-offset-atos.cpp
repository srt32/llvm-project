// The no-debug case should cause atos to report the function offset so this should test that path.
// RUN: rm -rf %t-no-debug.dSYM
// RUN: %clangxx %s -g0 -O0 -o %t-no-debug
// RUN: %env_tool_opts=verbosity=2,stack_trace_format='"function_name:%f function_offset:%q"' %run %t-no-debug > %t-no-debug.output 2>&1
// RUN: FileCheck -input-file=%t-no-debug.output %s
// RUN: FileCheck -check-prefix=BADADDR -input-file=%t-no-debug.output %s

// The debug info case should cause atos to not report the function offset so this should test the dladdr() fallback path.
// RUN: %clangxx %s -g -O0 -o %t-with-debug
// RUN: %env_tool_opts=verbosity=2,stack_trace_format='"function_name:%f function_offset:%q"' %run %t-with-debug > %t-with-debug.output 2>&1
// RUN: FileCheck -input-file=%t-with-debug.output %s
// RUN: FileCheck -check-prefix=BADADDR -input-file=%t-with-debug.output %s
#include <sanitizer/common_interface_defs.h>
#include <stdio.h>

void baz() {
  printf("Do stuff in baz\n");
  __sanitizer_print_stack_trace();
}

void bar() {
  printf("Do stuff in bar\n");
  baz();
}

void foo() {
  printf("Do stuff in foo\n");
  bar();
}

int main() {
  printf("Do stuff in main\n");
  foo();
  return 0;
}

// CHECK: Using atos found at:

// CHECK: function_name:baz{{(\(\))?}} function_offset:0x{{[0-9a-f]+}}
// CHECK: function_name:bar{{(\(\))?}} function_offset:0x{{[0-9a-f]+}}
// CHECK: function_name:foo{{(\(\))?}} function_offset:0x{{[0-9a-f]+}}
// CHECK: function_name:main{{(\(\))?}} function_offset:0x{{[0-9a-f]+}}

// BADADDR-NOT: function_name:baz{{(\(\))?}} function_offset:0x0
// BADADDR-NOT: function_name:bar{{(\(\))?}} function_offset:0x0
// BADADDR-NOT: function_name:foo{{(\(\))?}} function_offset:0x0
// BADADDR-NOT: function_name:main{{(\(\))?}} function_offset:0x0
