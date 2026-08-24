//
// Pre-compiled header for the fiction test suite
// Included automatically in every test translation unit when FICTION_ENABLE_PCH is on
//
// Only headers that are stable, expensive, and pulled in by a large share of the 118 test
// translation units belong here. `fiction/types.hpp` alone instantiates every preset layout type and
// is included by 62 of them.
//

#ifndef FICTION_PCH_TEST_HPP
#define FICTION_PCH_TEST_HPP

// NOLINTBEGIN(misc-include-cleaner)

// ============================================================================
// Catch2 (stable, included by every test)
// ============================================================================
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// ============================================================================
// fiction: type presets and traits (the two heaviest shared headers)
// ============================================================================
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/fcn/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

// ============================================================================
// Standard Library (stable, frequently used)
// ============================================================================
#include <algorithm>
#include <cstdint>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// NOLINTEND(misc-include-cleaner)

#endif  // FICTION_PCH_TEST_HPP
