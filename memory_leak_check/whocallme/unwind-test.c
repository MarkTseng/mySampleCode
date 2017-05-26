#include <unwind.h>
#include <stdio.h>
#include "whocallme.h"
 

static _Unwind_Reason_Code

backtrace_helper (struct _Unwind_Context *ctx, void *a)

{

  void *ip = (void*)_Unwind_GetIP (ctx);

  fprintf (stdout, "   %p\n", ip);
 
  return _URC_NO_REASON;

} 

 

void

print_backtrace (void)

{

  _Unwind_Backtrace (backtrace_helper, NULL);

}

 

int

main (int argc, char **argv)

{

  print_backtrace (); 

  return 0;

}

