//
// Created by marcel on 17.08.26.
//

// clang-format off
// execution_utils.hpp has to stay the first include: the guard must not depend on which standard
// headers the translation unit pulled in before it, and any header sorted above it would mask that
#include <fiction/utils/stl/execution_utils.hpp>
// clang-format on

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>
#include <version>

// FICTION_EXECUTION_POLICY_* expands either to a policy followed by a comma, or to nothing. Feeding
// the expansion to these overloads reports which of the two happened: the two-argument overload only
// wins when the macro contributed a policy. Overload resolution rather than a variadic argument
// count, because MSVC's traditional preprocessor passes __VA_ARGS__ on as a single argument.
constexpr int policy_arity(int) noexcept
{
    return 1;
}
template <typename Policy>
constexpr int policy_arity(const Policy&, int) noexcept
{
    return 2;
}

// the condition the guard is supposed to encode, evaluated here after <version> has defined the
// feature-test macros
#if (defined(__cpp_lib_parallel_algorithm) || defined(__cpp_lib_execution)) && \
    (!defined(__GNUC__) || defined(__clang__) || __GNUC__ > 9)
constexpr int EXPECTED_POLICY_ARITY{2};
#else
constexpr int EXPECTED_POLICY_ARITY{1};
#endif

static_assert(policy_arity(FICTION_EXECUTION_POLICY_SEQ 0) == EXPECTED_POLICY_ARITY,
              "FICTION_EXECUTION_POLICY_SEQ disagrees with the standard library's own feature-test macros");
static_assert(policy_arity(FICTION_EXECUTION_POLICY_PAR 0) == EXPECTED_POLICY_ARITY,
              "FICTION_EXECUTION_POLICY_PAR disagrees with the standard library's own feature-test macros");
static_assert(policy_arity(FICTION_EXECUTION_POLICY_PAR_UNSEQ 0) == EXPECTED_POLICY_ARITY,
              "FICTION_EXECUTION_POLICY_PAR_UNSEQ disagrees with the standard library's own feature-test macros");

TEST_CASE("Execution policy macros are usable as algorithm arguments", "[execution_utils]")
{
    const std::vector<uint64_t> haystack{4, 8, 15, 16, 23, 42};

    CHECK(std::find(FICTION_EXECUTION_POLICY_SEQ haystack.cbegin(), haystack.cend(), uint64_t{15}) ==
          haystack.cbegin() + 2);
    CHECK(std::find(FICTION_EXECUTION_POLICY_PAR haystack.cbegin(), haystack.cend(), uint64_t{108}) == haystack.cend());

    CHECK(std::any_of(FICTION_EXECUTION_POLICY_PAR_UNSEQ haystack.cbegin(), haystack.cend(),
                      [](const auto value) { return value == uint64_t{42}; }));
    CHECK(!std::any_of(FICTION_EXECUTION_POLICY_PAR_UNSEQ haystack.cbegin(), haystack.cend(),
                       [](const auto value) { return value == uint64_t{108}; }));

    const std::vector<std::pair<uint64_t, double>> costs{{0, 2.5}, {1, 0.5}, {2, 1.5}};

    const auto cheapest{std::min_element(FICTION_EXECUTION_POLICY_PAR_UNSEQ costs.cbegin(), costs.cend(),
                                         [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; })};

    CHECK(cheapest->first == 1);
}
