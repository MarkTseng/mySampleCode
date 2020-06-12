#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <sanitizer/lsan_interface.h>

// linux build command
// g++ -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libgcc -static-libasan -lrt memory_leak.cpp
// g++ -ggdb -fsanitize=leak -fno-omit-frame-pointer -static-libstdc++ -static-libgcc -static-liblsan -lrt memory_leak.cpp
//
// mac osx build command
// g++ -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libgcc memory_leak.cpp
// g++ -ggdb -fsanitize=leak -fno-omit-frame-pointer -static-libstdc++ -static-libgcc memory_leak.cpp

int heap_leak() {
  int* a = new int[10086];
  a[0] = 10086;
  std::cout << __PRETTY_FUNCTION__ << a[0] << std::endl;
  return 0;
}

int heap_use_after_free() {
  int* a = new int[10010];
  a[0] = 10010;
  delete[] a;
  std::cout << __PRETTY_FUNCTION__ << a[0] << std::endl;
  return 0;
}

int stack_buffer_overflow() {
  int a[10];
  std::cout << __PRETTY_FUNCTION__ << a[0] << std::endl;
  return a[10];
}

int heap_buffer_overflow() {
  int* a = new int[10010];
  a[10009] = 10010;
  std::cout << __PRETTY_FUNCTION__ << a[0] << std::endl;
  return a[10010];
}

int g_a[100];
int global_buffer_overflow() {
  std::cout << __PRETTY_FUNCTION__ << g_a[0] << std::endl;
  return g_a[100];
}

#include <vector>
std::vector<int*> g_vec;
int container_deferred() {
  for (int i = 0; i < 1008601; ++i) {
    g_vec.emplace_back(new int(i));
  }
	// global memory leak does not count until the orignal "container" of address
  // actually is replaced
  g_vec.clear();
  g_vec.emplace_back(nullptr); // 4 bytes leak
  g_vec.emplace_back(nullptr); // 8 bytes leak
  return 0;
}

std::unordered_map<std::string, std::function<int()>> g_func = {
  {"global_buffer_overflow", global_buffer_overflow},
  {"heap_leak", heap_leak},
  {"heap_buffer_overflow", heap_buffer_overflow},
  {"heap_use_after_free", heap_use_after_free},
  {"stack_buffer_overflow", stack_buffer_overflow},
  {"container_deferred", container_deferred},
};

int main(int argc, char** argv) {
  if (argc != 2 || (argc == 2 && std::string("-h") == argv[1])) {
    for (auto& i : g_func) {
      std::cout << i.first << std::endl;
    }
    return 1;
  }
  auto func = g_func.find(argv[1]);
  if (func == g_func.end()) {
    std::cerr << "test function `" << argv[1] << "` not found" << std::endl;
  } else {
    func->second();
  }
  int cnt = 5;
  while (cnt--) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    __lsan_do_recoverable_leak_check(); // print to stdout if any leak detects
    // __lsan_do_leak_check(); // print and exit this process
  }
  return 0;
}

