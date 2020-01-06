#include <exception>
#include <iostream>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>

struct empty { };

template <typename T, int N>
  struct bar { };

#if 1
const char* demangle(const char* name)
{
	char buf[1024];
	unsigned int size=1024;
	int status;
	char* res = abi::__cxa_demangle (name,
			0,
			0,
			&status);
    if(res)
        printf("fun name:%s\n", res);
    else
        printf("no demangle fun name:%s\n", name);
	return res;
}
#endif
int main()
{
  int     status;
  char   *realname;


  // typeid
  bar<empty,17>          u;
  const std::type_info  &ti = typeid(u);

  realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
  free(realname);

  demangle("_ZNSt13basic_ostreamIwSt11char_traitsIwEE5flushEv");
  demangle("malloc");
  demangle("pthread_create");
  return 0;
}

