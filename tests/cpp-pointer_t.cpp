#include "cpp-pointer/cpp-pointer.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>

#include <format>
#include <iostream>
#include <string_view>
#include <type_traits>
using namespace cpppointer;

namespace {

int failures = 0;

void expect(bool condition, std::string_view expression, std::string_view file,
            int line) {
  if (!condition) {
    std::cerr << std::format("{}:{} failed: {}\n", file, line, expression);
    ++failures;
  }
}

} // namespace

#define EXPECT(expression)                                                     \
  expect(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

auto main() -> int {
  Pointer<uint32_t> null_pointer{nullptr};

  EXPECT(null_pointer.is_null());
  EXPECT(!null_pointer.is_not_null());
  EXPECT(!null_pointer);
  EXPECT(null_pointer.operator!());
  EXPECT(Pointer<uint32_t>::null_ptr().is_null());

  std::array<char, 8> bytes{1, 2, 3, 4, 5, 6, 7, 8};
  Pointer<char> byte_pointer{bytes.data()};

  EXPECT(byte_pointer.byte_add(3).byte_ptr() == bytes.data() + 3);
  EXPECT(byte_pointer.byte_sub(0).byte_ptr() == bytes.data());
  EXPECT((byte_pointer.byte_add(5)).byte_ptr() == bytes.data() + 5);
  EXPECT((byte_pointer.byte_sub(2)).byte_ptr() == bytes.data() - 2);

  std::array<unsigned char, 16> alignment_bytes{};
  auto *misaligned = alignment_bytes.data() + 1;
  Pointer<uint32_t> alignment_pointer{bit_cast<uint32_t *>(misaligned)};

  EXPECT(!alignment_pointer.is_aligned());
  EXPECT(alignment_pointer.align_to<uint16_t>() == 1);
  EXPECT(alignment_pointer.align_to<uint32_t>() == 3);

  const auto [head, body] = alignment_pointer.align_head_body<uint16_t>();
  EXPECT(head.address() == alignment_pointer.address());
  EXPECT(body.address() ==
         alignment_pointer.address() + alignment_pointer.align_to<uint16_t>());

  uint32_t aligned_value = 0x12345678;
  Pointer<uint32_t> aligned_pointer{&aligned_value};

  std::array<unsigned char, 8> unaligned_bytes{0x78, 0x56, 0x34, 0x12,
                                               0,    0,    0,    0};
  auto *raw = unaligned_bytes.data() + 1;

  Pointer<uint32_t> unaligned_pointer{bit_cast<uint32_t *>(raw)};

  EXPECT(aligned_pointer.read_aligned() == aligned_value);
  EXPECT(!unaligned_pointer.is_aligned());
  EXPECT(unaligned_pointer.read_unaligned() == 0x00123456);

  // char text[] = "pointer";
  // Pointer<char> string_pointer{text};
  // EXPECT(string_pointer.strlength() == 7);

  int value = 42;
  const int const_value = 7;

  Pointer<int> mutable_pointer{&value};
  Pointer<const int> const_pointer{&const_value};

  using MutableCast = decltype(mutable_pointer.cast<short>());
  using ConstCast = decltype(const_pointer.cast<short>());

  static_assert(std::is_same_v<MutableCast, Pointer<short>>);
  static_assert(std::is_same_v<ConstCast, Pointer<const short>>);

  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}