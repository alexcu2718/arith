#pragma once // NOLINT

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

// THIS IS MOSTLY A PROOF OF CONCEPT LIBRARY AND TESTING NEW C++ FEATURES.
// TODO investigate smarter pointer funcionality, operator overloading, add
// asserts in.

#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)

namespace cpppointer { // terrible name

using std::bit_cast;

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_floating_point_v<T>)
class Pointer {
  T *_ptr;

public: // for now.
  [[nodiscard]] constexpr Pointer(T *entry) noexcept : _ptr(entry) {};

  [[nodiscard]] constexpr auto is_null(this auto const self) noexcept -> bool {
    return self.get() == nullptr;
  }
  /// Instantiates a null pointer of itself
  [[nodiscard]] static constexpr auto null_ptr() noexcept -> Pointer<T> {
    return {nullptr};
  }

  /// Returns the first pointer as the unaligned head (null if already aligned),
  /// the second pointer starts on an aligned address.
  template <typename H>
    requires(std::is_integral_v<H> && alignof(H) > 0 &&
             IS_POWER_OF_TWO(alignof(H)))
  [[nodiscard]] constexpr auto align_head_body(this auto const self) noexcept
      -> std::pair<Pointer<H>, Pointer<H>> {
    const auto offset = self.template align_to<H>();
    const auto head = self.template cast<H>();
    if (offset == 0) {
      return {Pointer<H>::null_ptr(), head};
    }
    return {head, Pointer<H>{head.byte_add(offset)}};
  }

  [[nodiscard]] constexpr auto byte_add(this auto const self,
                                        size_t amt) noexcept -> Pointer<T> {
    return {bit_cast<T *>(self.address() + amt)};
  }

  [[nodiscard]] constexpr auto byte_sub(this auto const self,
                                        size_t amt) noexcept -> Pointer<T> {
    return {bit_cast<T *>(self.address() - amt)};
  }

  [[nodiscard]] constexpr auto operator*(this auto const self) noexcept -> T {
    return *self._ptr;
  }

  [[nodiscard]] constexpr auto operator+(this auto const self,
                                         size_t amt) noexcept -> Pointer<T> {
    return {self.byte_add(amt)};
  }

  [[nodiscard]] constexpr auto operator!(this auto const self) noexcept
      -> bool {
    return self.is_null();
  }

  [[nodiscard]] constexpr explicit
  operator bool(this auto const self) noexcept {
    return self.is_not_null();
  }
  /// - operator does byte_wise addition, I do not like to use it, but good to
  /// have.
  [[nodiscard]] constexpr auto operator-(this auto const self,
                                         size_t amt) noexcept {
    return self.byte_sub(amt);
  }
  // Unaligned read
  [[nodiscard]] constexpr auto read_unaligned(this auto const self) noexcept
      -> T {
    T value; // dont initialise with padding, this may be optimised away
             // anyway...
    std::memcpy(&value, self.get(), sizeof(T));
    return value;
  }
  /// Aligned read
  [[nodiscard]] constexpr auto read_aligned(this auto const self) noexcept
      -> T {

    return *self._ptr;
  }

  [[nodiscard]] constexpr auto is_not_null(this auto const self) noexcept
      -> bool {
    return !self.is_null();
  }
  /// Check if pointer is aligned to arbitrary POD `M`
  template <typename M>
    requires(sizeof(M) != 0 && alignof(M) != 0 && IS_POWER_OF_TWO(alignof(M)))
  [[nodiscard]] constexpr auto is_aligned_to(this auto const self) noexcept
      -> bool {

    constexpr auto ALIGN = alignof(M);
    return (self.address() % ALIGN) == 0;

    // on 64bit optimised to x % 8 == x & 7, simple bitwise and
    // similar for 32bit, where x % 4 == x & 3 ;
    // x % (2^n) -> x & (2^n - 1)
  }
  /// Check if the pointer is aligned to itself
  [[nodiscard]] constexpr auto is_aligned(this auto const self) noexcept
      -> bool {
    return self.template is_aligned_to<T>();
  }

  /// Returns the value representing this variables stack position.
  [[nodiscard]] constexpr auto address(this auto const self) noexcept
      -> uintptr_t {
    return bit_cast<uintptr_t>(self);
  }

  /// Returns the offset required to meet the alignment of `M`
  template <typename M>
    requires(sizeof(M) != 0 && alignof(M) != 0 && IS_POWER_OF_TWO(alignof(M)))
  [[nodiscard]] constexpr auto align_to(this auto const self) noexcept
      -> size_t {
    constexpr size_t ALIGNMENT = alignof(M);
    constexpr size_t ALIGN_BY = ALIGNMENT - 1;
    const uintptr_t addr = self.address();
    auto val = (addr + ALIGN_BY) & ~ALIGN_BY;
    return val - addr;
  }

  [[nodiscard]] constexpr auto byte_ptr(this auto const self) noexcept
      -> char * {

    return bit_cast<char *>(self);
  }

  [[nodiscard]] constexpr auto strlength(this auto const self) noexcept
      -> size_t {
    const auto *start = self.byte_ptr();
    if (std::is_constant_evaluated()) {
      for (size_t len = 0;; ++len) {
        if (start[len] == 0) {
          return len;
        }
      };
    }

    return std::strlen(start);
  }

  [[nodiscard]] constexpr auto get(this auto const self) noexcept -> T * {
    return self._ptr;
  }

  template <typename V>
  [[nodiscard]] constexpr auto cast(this auto const self) noexcept {
    using Target =
        std::conditional_t<std::is_const_v<T>,
                           const std::remove_pointer_t<std::remove_cvref_t<V>>,
                           std::remove_pointer_t<std::remove_cvref_t<V>>>;
    return Pointer<Target>{bit_cast<Target *>(self)};
  }
};

} // namespace cpppointer
