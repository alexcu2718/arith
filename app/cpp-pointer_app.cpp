#include "cpp-pointer/cpp-pointer.hpp"
#include <iostream>

int main()
{
  int result = cpppointer::add_one(1);
  std::cout << "1 + 1 = " << result << std::endl;
}
