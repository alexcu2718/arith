#include "cpp-pointer/cpp-pointer.hpp"
#include <cstddef>
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
void example_usage() {

  Pointer use_this{"hellloooooooo"};
  const bool is_aligned = use_this.is_aligned_to<uint32_t>();
  const size_t offset_to = use_this.align_to<uint32_t>();

  const bool is_aligned_after_offset =
      use_this.byte_add(offset_to).cast<uint32_t>().is_aligned();

  const uint32_t read_u32_from_start_of_aligned_memory =
      use_this.byte_add(offset_to).cast<uint32_t>().read_aligned();

  std::cout << std::format("\n\n\nAlignment offset: {}\n"
                           "Aligned before offset: {}\n"
                           "Pointer address: {}\n"
                           "Aligned after offset: {}\n",
                           offset_to, is_aligned, use_this.address(),
                           is_aligned_after_offset);
}

/*
Example output


Alignment offset: 3
Aligned before offset: false
Pointer address: 94124911207161
Aligned after offset: true


*/

// this is just testing code for really duick things
auto main() -> int {

  example_usage();
  alignas(uint64_t) const auto *hii = "yes";

  auto test = cpppointer::Pointer(hii);
  const auto lol = test.byte_add(2);

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