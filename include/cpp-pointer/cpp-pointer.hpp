#pragma once // NOLINT

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <tuple>
#include <type_traits>

// Create a converter that enforces saturation on overflow

using std::bit_cast;

static_assert(std::is_same_v<uintptr_t, size_t>,
              "Ok don't know how that's possible nice PDP11 ");

template <typename V, typename T>
using Target = std::conditional_t<std::is_const_v<std::remove_pointer_t<T>>,
                                  std::add_const_t<std::remove_cvref_t<V>>,
                                  std::remove_cvref_t<V>>;

template <typename V>
concept CastTarget = std::is_object_v<std::remove_cvref_t<V>> ||
                     std::is_void_v<std::remove_cvref_t<V>>;

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
static constexpr auto is_power_of_two(auto num) -> bool {
  return (num & (num - 1)) == 0;
}

template <typename L>
concept Num = std::is_integral_v<L> && alignof(L) > 0 &&
              is_power_of_two(alignof(L));

namespace cpppointer { // terrible name

template <typename T>
  requires(std::is_pointer_v<T> || std::is_pod_v<T>)
class Pointer {
  T *_ptr;

public: // for now.
  [[nodiscard]] constexpr Pointer() noexcept = default;
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
  template <Num H>
  [[nodiscard]] constexpr auto align_head_body(this auto const self) noexcept
      -> std::tuple<std::optional<Pointer<Target<H, T>>>,
                    std::optional<Pointer<Target<H, T>>>> {
    using HeadT = Target<H, T>;
    const size_t offset = self.template align_to<HeadT>();
    const auto head = self.template cast<HeadT>();
    if (offset == 0) {
      // Already aligned: no head, body starts at head
      return std::make_tuple(std::nullopt,
                             std::make_optional(Pointer<HeadT>{head.get()}));
    }
    // Not aligned: head is first, body is at offset
    return std::make_tuple(
        std::make_optional(Pointer<HeadT>{head.get()}),
        std::make_optional(Pointer<HeadT>{head.byte_add(offset).get()}));
  }
  /// Provides default arithmetic(we do not overload the arithmetic operators
  /// because we deleted them for explicitness.)
  [[nodiscard]] constexpr auto byte_add(this auto const self, auto amt) noexcept
      -> Pointer<T> {
    return {bit_cast<T *>(self.address() + amt)};
  }

  [[nodiscard]] constexpr auto byte_sub(this auto const self, auto amt) noexcept
      -> Pointer<T> {
    return {bit_cast<T *>(self.address() - amt)};
  }

  [[nodiscard]] constexpr auto operator*(this auto const self) noexcept -> T {
    return *self.get();
  }
  // delete some misused operators on pointers.

  auto operator+() = delete;
  auto operator*(auto _other) = delete;
  auto operator-() = delete;
  auto operator%(auto _other) = delete;

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
  template <Num M>
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

  // dumb tests
  // [[nodiscard]] constexpr auto n_strlen() noexcept {

  //   constexpr size_t LOW = ~0UZ / 0xFF;
  //   constexpr size_t HIGH = LOW << 7;

  //   auto start = this->template align_head_body<size_t>();

  //   size_t len{};
  //   if (start.first.is_not_null()) {
  //     auto ptr = start.first.byte_ptr();
  //     while (ptr[len] != '\0') {
  //       len++;
  //     }
  //   }

  //   size_t body_len{};

  //   while (true) {
  //     const size_t num = start.second.byte_add(body_len).read_aligned();
  //     const size_t masked_num = (num - LOW) & ~num & HIGH;
  //     if (masked_num != 0) {
  //       return len + body_len + (std::countr_zero(masked_num) >> 3);
  //     }
  //     body_len += sizeof(size_t);
  //   }
  // }

  /// Returns the value representing this variables stack position.
  [[nodiscard]] constexpr auto address(this const auto self) noexcept
      -> uintptr_t {

    return std::bit_cast<uintptr_t>(self._ptr);
  }

  /// Returns the offset required to meet the alignment of `M`
  template <Num M>
  [[nodiscard]] constexpr auto align_to(this auto const self) noexcept
      -> size_t {
    constexpr size_t ALIGN_BY = alignof(M) - 1;
    const uintptr_t addr = self.address();
    auto val = (addr + ALIGN_BY) & ~ALIGN_BY;
    return val - addr;
  }
  // template <> constexpr auto align_to<0>(this auto const self) { return self;
  // }

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

  template <CastTarget V>
  [[nodiscard]] constexpr auto cast(this auto const self) noexcept
      -> Pointer<::Target<V, T *>> {
    return {bit_cast<::Target<V, T *> *>(self.get())};
  }
};

// template <typename T> class UniquePtr {

// public:
//   [[nodiscard]] constexpr UniquePtr(Pointer<T> ptr) noexcept

//   }
// };

} // namespace cpppointer
