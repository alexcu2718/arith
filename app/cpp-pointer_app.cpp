#include "cpp-pointer/cpp-pointer.hpp"
#include <cstdint>
#include <format>
#include <iostream>
// just an example of deducing this, pretty cool, never used it before.
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

class Another : public Base { // NOLINT
public:
  void show() { std::cout << "This is another class.\n"; } // NOLINT
};

using namespace cpppointer;
// NOLINTBEGIN

// this is just testing code for really duick things
auto main() -> int {
  alignas(uint64_t) const auto *hii = "yes";

  auto test = cpppointer::Pointer(hii);
  const auto *lol = test.byte_add(2);

  const auto test1 = test.align_to<uint8_t>();

  std::cout << std::format("alignment neededdkkkwkkwdkwk {}\n", test1);

  const auto *hekeki = "yeswowowowowow";

  auto h2t = cpppointer::Pointer{hekeki};

  auto is_aligned = h2t.is_aligned();
  std::cout << std::format(" h2t is aligned '{}'\n", is_aligned);

  Base base{};
  base.show();
  Derived derived;
  derived.show();
  Another another;
  another.show();
}
// NOLINTEND