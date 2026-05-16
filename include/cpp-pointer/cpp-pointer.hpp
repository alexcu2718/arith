#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

// TODO investigate smarter pointer funcionality, pointer overloading
namespace cpppointer {
using std::bit_cast;

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_floating_point_v<T>)
class Pointer {
  T *_ptr;

public: // for now.
  [[nodiscard]] explicit constexpr Pointer(T *entry) noexcept : _ptr(entry) {};

  [[nodiscard]] constexpr auto is_null(this auto const self) noexcept -> bool {
    return self.get() == 0;
  }

  [[nodiscard]] constexpr auto byte_add(this auto const self, size_t x) noexcept
      -> T * {
    return bit_cast<T *>(bit_cast<uint8_t *>(self.get()) + x);
  }

  [[nodiscard]] constexpr auto byte_sub(this auto const self, size_t x) noexcept
      -> T * {
    return bit_cast<T *>(bit_cast<uint8_t *>(self.get()) - x);
  }

  [[nodiscard]] constexpr auto operator+(this auto const self,
                                         size_t x) noexcept -> T * {
    return self.byte_add(x);
  }

  [[nodiscard]] constexpr auto operator-(this auto const self, size_t x) {
    return self.byte_sub(x);
  }

  [[nodiscard]] constexpr auto read_unaligned(this auto const self) noexcept -> T {
    T value{};
    std::memcpy(&value, self.get(), sizeof(T));
    return value;
  }

   [[nodiscard]] constexpr auto read(this auto const self ) noexcept ->T{

    return self.get();
   }

  [[nodiscard]] constexpr auto is_not_null(this auto const self) noexcept
      -> bool {
    return !self.is_null();
  }

  template <typename V>
    requires(std::is_integral_v<V>)
  [[nodiscard]] constexpr auto is_aligned_to(this auto const self) noexcept
      -> bool {

    constexpr auto ALIGN = alignof(V);
    return (self.address() % ALIGN) == 0;
  }

  [[nodiscard]] constexpr auto is_aligned(this auto const self) -> bool {
    return self.template is_aligned_to<T>();
  }

  [[nodiscard]] constexpr auto address(this auto const self) noexcept
      -> uintptr_t {
    return bit_cast<uintptr_t>(self.get());
  }

  template <typename M>
  [[nodiscard]] constexpr auto align_to(this auto const self) noexcept
      -> size_t {
    constexpr size_t ALIGNMENT = alignof(M);
    static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                  "alignment must be a power of two");

    constexpr size_t ALIGN_BY = ALIGNMENT - 1;

    const uintptr_t addr = self.address();

    auto val = (addr + ALIGN_BY) & ~ALIGN_BY;

    return val - addr;
  }

  [[nodiscard]] constexpr auto byte_pointer(this auto const self) -> uint8_t * {

    return bit_cast<uint8_t *>(self.get());
  }

  [[nodiscard]] constexpr auto strlength(this auto const self) -> size_t {
    const auto *start = bit_cast<const char *>(self.get());
    if (std::is_constant_evaluated()) {
      const char *end = start;
      while (*end != '\0') {
        ++end;
      }
      return static_cast<size_t>(end - start);
    }
    return std::strlen(start);
  }

  [[nodiscard]] constexpr auto get(this auto const self) noexcept -> T * {
    return self._ptr;
  }

  template <typename V>
  [[nodiscard]] constexpr auto cast(this auto const self) noexcept {
    using U = std::remove_pointer_t<std::remove_cvref_t<V>>;
    using Target =
        std::conditional_t<std::is_const_v<T>, std::add_const_t<U>, U>;
    return Pointer<Target>{bit_cast<Target *>(self._ptr)};
  }

}; // namespace cpppointer

} // namespace cpppointer
