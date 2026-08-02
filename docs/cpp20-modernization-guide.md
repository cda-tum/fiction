# C++20 Modernization Guide

Canonical reference: <https://en.cppreference.com/w/cpp/20.html>

**Out of scope for this migration pass — do not touch:** Concepts (incl. standard library
concepts such as `std::integral`, `std::same_as` used as constraints, and user-defined `concept`
declarations), Modules, Coroutines.

---

## Language

### Three-way comparison (`operator<=>`)

```cpp
// before
bool operator==(const P& o) const { return x == o.x && y == o.y; }
bool operator<(const P& o) const { return std::tie(x, y) < std::tie(o.x, o.y); }

// after
auto operator<=>(const P&) const = default;   // also gives ==, !=, <, <=, >, >=
```

Use `= default` when member-wise lexicographic comparison is correct; write it by hand only for
custom ordering. If only equality is needed, `bool operator==(const T&) const = default;` alone
still synthesizes `!=`.
<https://en.cppreference.com/w/cpp/language/default_comparisons.html>

### Designated initializers

```cpp
struct point { int x{}; int y{}; int z{}; };
point p{.x = 1, .z = 3};   // y value-initialized
```

Order must match declaration order; no reordering/skipping-then-filling.
<https://en.cppreference.com/w/cpp/language/aggregate_initialization.html#Designated_initializers>

### `constinit`

```cpp
constinit int counter = compute_at_compile_time();   // must be constant-initialized, may still mutate
```

Use for globals/statics where constant initialization must be guaranteed (avoids static-init-order
fiasco) but the variable is not `const`.
<https://en.cppreference.com/w/cpp/language/constinit.html>

### `consteval`

```cpp
consteval int square(int n) { return n * n; }
```

**Caveat:** every call site must be constant-evaluable — a `consteval` function cannot be called
with a purely runtime argument, even indirectly through a non-consteval wrapper. Only apply where
this is guaranteed for _all_ current and future callers; prefer `constexpr` when in doubt.
<https://en.cppreference.com/w/cpp/language/consteval.html>

### Extended `constexpr`

- Virtual functions may be `constexpr`.
- `try`/`catch` allowed in `constexpr` functions (the `throw` just can't execute in a constant
  evaluation).
- Many more `<algorithm>`/`<numeric>` functions are now `constexpr` (e.g. `std::sort`,
  `std::find`, `std::accumulate`, `std::for_each`) — enables compile-time use of existing
  algorithm calls with no code change beyond adding `constexpr` to the enclosing function.

```cpp
constexpr auto sorted(std::array<int, 4> a) { std::sort(a.begin(), a.end()); return a; }
static_assert(sorted({4,2,3,1})[0] == 1);
```

<https://en.cppreference.com/w/cpp/language/constexpr.html>

### Range-`for` with init-statement

```cpp
for (auto it = m.find(k); auto& [key, val] : it->second) { ... }
// common case: scope a lookup/lock to the loop
for (std::lock_guard lk{mtx}; auto& x : shared_vec) { ... }
```

<https://en.cppreference.com/w/cpp/language/range-for.html>

### Aggregate init improvements

```cpp
struct S { int a = 1; int b; };   // default member initializer now allowed with aggregate init
S s1{.b = 2};                     // a == 1, b == 2

struct T { int a; int b; };
T t(1, 2);                        // parenthesized aggregate init (was brace-only pre-C++20)
```

<https://en.cppreference.com/w/cpp/language/aggregate_initialization.html>

### Template lambdas with explicit template parameter list

```cpp
auto f = []<typename T>(std::vector<T>& v) { return v.size(); };
```

Use when the generic `auto` parameter lambda needs to name `T` in its body (e.g. `T{}`,
`sizeof(T)`, nested type aliases).
<https://en.cppreference.com/w/cpp/language/lambda.html>

### Lambda capture `[=, this]`

```cpp
// implicit capture of `this` via [=] is deprecated in C++20
auto f = [=, this] { return member_ + x; };   // explicit, no warning
auto g = [=] { return member_; };             // deprecated: use [=, this] instead
```

Stateless/capture-less lambdas are now default-constructible and assignable.
<https://en.cppreference.com/w/cpp/language/lambda.html>

### Abbreviated function templates (`auto` parameters)

```cpp
// before
template <typename T> void f(T x);
// after — only when it genuinely simplifies the signature (no explicit template<> needed)
void f(auto x);
```

Prefer only where the template parameter is never named elsewhere in the signature/body; otherwise
keep the explicit `template<typename T>` form.
<https://en.cppreference.com/w/cpp/language/function_template.html#Abbreviated_function_template>

### `using enum`

```cpp
enum class color { red, green, blue };
using enum color;
color c = green;   // instead of color::green
```

<https://en.cppreference.com/w/cpp/language/enum.html#using-enum>

---

## Attributes

### `[[likely]]` / `[[unlikely]]`

```cpp
if (error) [[unlikely]] { handle_error(); }
else [[likely]] { fast_path(); }
```

<https://en.cppreference.com/w/cpp/language/attributes/likely.html>

### `[[no_unique_address]]`

```cpp
struct empty_alloc {};
struct box { [[no_unique_address]] empty_alloc a{}; int* data; };  // a adds no size
```

<https://en.cppreference.com/w/cpp/language/attributes/no_unique_address.html>

---

## Library

### `std::jthread` + `std::stop_token` (`<thread>`, `<stop_token>`)

```cpp
// before
std::thread t([]{ while (running) { work(); } });
running = false;  // separate atomic flag needed
t.join();         // must remember to join, or std::terminate on destruction

// after
std::jthread t([](std::stop_token st) { while (!st.stop_requested()) { work(); } });
t.request_stop(); // cooperative cancellation, no separate flag needed
// automatically joins on destruction — no explicit join() required
```

Use when a thread needs cooperative cancellation and/or automatic join-on-destruction (RAII).
For fire-and-forget threads with no cancellation needs, a plain `std::jthread` (auto-join only,
ignore the `stop_token` parameter) is still a strict improvement over `std::thread`.
<https://en.cppreference.com/w/cpp/thread/jthread.html>,
<https://en.cppreference.com/w/cpp/thread/stop_token.html>

### `std::span` (`<span>`)

```cpp
void process(std::span<const double> data);   // non-owning view, replaces (ptr,len) or (begin,end) pairs
process(vec);        // from std::vector
process(arr);        // from std::array / C array
```

<https://en.cppreference.com/w/cpp/container/span.html>

### `<bit>` header

```cpp
#include <bit>
auto bits = std::bit_cast<std::uint64_t>(some_double);  // safe reinterpret, no UB, no memcpy boilerplate
std::popcount(0b1011u);        // 3
std::countl_zero(x);           // leading zero bits
std::countr_zero(x);           // trailing zero bits
std::has_single_bit(x);        // is power of two
std::bit_ceil(x);              // next power of two >= x
std::bit_floor(x);             // largest power of two <= x
```

<https://en.cppreference.com/w/cpp/header/bit.html>

### `std::to_array` (`<array>`)

```cpp
auto a = std::to_array({1, 2, 3});          // std::array<int, 3>, deduces type & size
auto b = std::to_array<double>({1, 2, 3});  // explicit element type
```

<https://en.cppreference.com/w/cpp/container/array/to_array.html>

### Uniform container erasure

```cpp
// before
v.erase(std::remove(v.begin(), v.end(), val), v.end());
std::remove_if(...);   // + erase
for (auto it = m.begin(); it != m.end(); ) { if (pred(*it)) it = m.erase(it); else ++it; }

// after
std::erase(v, val);
std::erase_if(v, pred);
std::erase_if(m, pred);   // works uniformly for vector, map, set, unordered_*, list, deque
```

<https://en.cppreference.com/w/cpp/container/vector/erase2.html>

### `std::ssize` (`<iterator>`)

```cpp
for (auto i = std::ssize(v) - 1; i >= 0; --i) { ... }  // signed size, avoids signed/unsigned compare bugs
```

<https://en.cppreference.com/w/cpp/iterator/size.html>

### `std::midpoint`, `std::lerp` (`<numeric>`)

```cpp
auto m = std::midpoint(a, b);     // no overflow, works for ints and pointers
auto l = std::lerp(a, b, 0.5);    // linear interpolation, correctly rounded
```

<https://en.cppreference.com/w/cpp/numeric/midpoint.html>,
<https://en.cppreference.com/w/cpp/numeric/lerp.html>

### `std::source_location` (`<source_location>`)

```cpp
void log(std::string_view msg, std::source_location loc = std::source_location::current()) {
    std::cerr << loc.file_name() << ':' << loc.line() << ": " << msg << '\n';
}
```

Changes the effective call-site signature/ABI-visible default arg — use only in logging/assertion
helpers, not as a general parameter-passing pattern.
<https://en.cppreference.com/w/cpp/utility/source_location.html>

### `std::ranges` algorithms and `std::views` (`<ranges>`, constrained `<algorithm>`)

```cpp
// before
std::sort(v.begin(), v.end());
std::vector<int> out;
std::copy_if(v.begin(), v.end(), std::back_inserter(out), pred);

// after
std::ranges::sort(v);
std::ranges::copy_if(v, std::back_inserter(out), pred);

// views: lazy, composable, no intermediate containers
for (int x : v | std::views::filter(pred) | std::views::transform(f)) { ... }
```

High-value, low-risk: replace `algo(c.begin(), c.end(), ...)` with `std::ranges::algo(c, ...)`
wherever a full-container call is being made.
<https://en.cppreference.com/w/cpp/ranges.html>, <https://en.cppreference.com/w/cpp/algorithm/ranges.html>

### `std::format` (`<format>`)

```cpp
std::string s = std::format("{} gates at ({}, {})", type, x, y);
```

Requires full standard library support (`<format>`) across the whole CI compiler matrix; older
GCC/Clang may lack it. Consider only if already available in the codebase's toolchain baseline —
otherwise skip and leave existing formatting (`fmt`/`iostream`/`printf`) as is.
<https://en.cppreference.com/w/cpp/utility/format/format.html>
