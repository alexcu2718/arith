#pragma once // NOLINT

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

// Create a converter that enforces saturation on overflow

using std::bit_cast;

static_assert(std::is_same_v<uintptr_t, size_t>,
              "Ok don't know how that's possible nice PDP11 ");

// THIS IS MOSTLY A PROOF OF CONCEPT LIBRARY AND TESTING NEW C++ FEATURES.
// TODO investigate smarter pointer funcionality, operator overloading, add
// asserts in.

// TODO add equivalent functionality for unique_ptr, shared_ptr(non atomic),

/*shared_ptr(atomic) with associated weak references.
 Should be fairly easy to do,
 SKETCH
 single threaded ref counted pointer struct simply keeps 3 variables, a pointer
 to the data, the strong count(size_t) and a weak count(size_t again)

 (I actually
 forget what the c++ standard does, obviously it's compiler dependent to degree
 but this should work) degree)

 equivalent the same for the atomic version of this, just use atomic wrapped
 versions, time  to mess up memory ordering :------D



 Stretch goals: Implement a bump allocator optional template parameter for
 shared/uniques

*/
constexpr auto is_power_of_two(auto num) -> bool {
  return (num & (num - 1)) == 0;
}

namespace cpppointer { // terrible name

template <typename T>
  requires(std::is_pointer_v<T> || std::is_pod_v<T>)
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
             is_power_of_two(alignof(H)))
  [[nodiscard]] constexpr auto align_head_body(this auto const self) noexcept
      -> std::pair<Pointer<H>, Pointer<H>> {
    const size_t offset = self.template align_to<H>();
    const auto head = self.template cast<H>();
    if (offset == 0) {
      return {Pointer<H>::null_ptr(), head};
    }
    return {head, Pointer<H>{head.byte_add(offset)}};
  }
  /// Provides default arithmetic(we do not overload the arithmetic operators
  /// because we deleted them for explicitness.)
  [[nodiscard]] constexpr auto byte_add(this auto const self,
                                        size_t amt) noexcept -> Pointer<T> {
    return {bit_cast<T *>(self.address() + amt)};
  }

  [[nodiscard]] constexpr auto byte_sub(this auto const self,
                                        size_t amt) noexcept -> Pointer<T> {
    return {bit_cast<T *>(self.address() - amt)};
  }

  [[nodiscard]] constexpr auto operator*(this auto const self) noexcept -> T {
    return *self.get();
  }
  // delete some misused operators on pointers.
  [[nodiscard]] constexpr auto operator+() = delete;
  [[nodiscard]] constexpr auto operator*(auto _other) = delete;
  [[nodiscard]] constexpr auto operator-() = delete;
  [[nodiscard]] constexpr auto operator%(auto _other) = delete;

  [[nodiscard]] constexpr auto operator/(auto _other) = delete;

  [[nodiscard]] constexpr auto operator!(this auto const self) noexcept
      -> bool {
    return self.is_null();
  }

  [[nodiscard]] constexpr auto operator==(this auto const self,
                                          auto other) noexcept -> bool {
    return self.get() == other.get();
  }

  [[nodiscard]] constexpr explicit
  operator bool(this auto const self) noexcept {
    return self.is_not_null();
  }

  // Unaligned read
  [[nodiscard]] constexpr auto read_unaligned(this auto const self) noexcept
      -> T {
    T value; // dont initialise, this may be optimised away
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
    requires(sizeof(M) != 0 && alignof(M) != 0 && is_power_of_two(alignof(M)))
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

    return bit_cast<uintptr_t>(self.get());
  }

  /// Returns the offset required to meet the alignment of `M`
  template <typename M>
    requires(sizeof(M) != 0 && alignof(M) != 0 && is_power_of_two(alignof(M)))
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

    return bit_cast<char *>(self.get());
  }

  // [[nodiscard]] constexpr auto strlength(this auto const self) noexcept
  //     -> size_t {
  //   const auto *start = self.byte_ptr();
  //   if (std::is_constant_evaluated()) {
  //     for (size_t len = 0;; ++len) {
  //       if (start[len] == 0) {
  //         return len;
  //       }
  //     };
  //   }

  //   return std::strlen(start);
  // }

  [[nodiscard]] constexpr auto get(this auto const self) noexcept -> T * {
    return self._ptr;
  }

  // forgive me for I have sinned greatly.
#define POINTER_TYPE                                                           \
  std::conditional_t<std::is_const_v<std::remove_pointer_t<T>>,                \
                     const std::remove_cvref_t<V>, std::remove_cvref_t<V>>
  // EW
  template <typename V>
  [[nodiscard]] constexpr auto cast(this auto const self) noexcept

      -> Pointer<POINTER_TYPE> {

    return {bit_cast<POINTER_TYPE *>(self.get())};
  }

#undef POINTER_TYPE
};

// template <typename T> class UniquePtr {

// public:
//   [[nodiscard]] constexpr UniquePtr(Pointer<T> ptr) noexcept

//   }
// };

} // namespace cpppointer
