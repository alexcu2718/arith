#include "cpp-pointer/cpp-pointer.hpp"
#include <cstring>
#include <format>
#include <iostream>

class Base { // NOLINT
public:
  void show(this auto _self) {
    (void)_self;
    std::cout << "This is base class.\n";
  }
};

class Derived : public Base { // NOLINT
public:
  void show() { std::cout << "This is derived class.\n"; } // NOLINT
};

class Another : public Base {// NOLINT
public:
  void show() { std::cout << "This is another class.\n"; } // NOLINT
};

using namespace cpppointer;

int main() {
  alignas(uint64_t) const auto *hi = "yes";

  const auto *kl = "lelelelelellelelele";

  auto test = cpppointer::Pointer(hi);
  const auto *lol = test.byte_add(2);

  const auto test1 = test.align_to<uint8_t>();

  std::cout << std::format("alignment neededdkkkwkkwdkwk {}\n", test1);

  const auto *hekeki = "yeswowowowowow";

  auto h2t = cpppointer::Pointer{hekeki};

  auto is_aligned = h2t.is_aligned();

  auto lentest = h2t.strlength();
  auto reallen = std::strlen(hekeki);
  std::cout << std::format(" std strlen test is {} and real strleb is {}",
                           lentest, reallen);

  auto val = h2t.cast<uint64_t>();

  std::cout << std::format("test is {}  meanwhile add 3 is {}\n\n", test.get(),
                           lol);

  Base base{};
  base.show();
  Derived d;
  d.show();
  Another a;
  a.show();
}
