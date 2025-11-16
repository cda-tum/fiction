//
// Pre-compiled headers for the fiction CLI
// This file is automatically included in all CLI command compilation units
//
// Only includes stable, expensive-to-compile, frequently-used headers:
// - Stable: Headers that rarely change
// - Expensive: Template-heavy headers
// - Frequently used: Included in most CLI translation units
//

#ifndef FICTION_PCH_CLI_HPP
#define FICTION_PCH_CLI_HPP

// NOLINTBEGIN(misc-include-cleaner)

// ============================================================================
// Standard Library (stable, frequently used)
// ============================================================================
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

// ============================================================================
// Third-party: Alice Framework (very stable, very expensive)
// Included in ALL CLI command headers - most expensive single header
// ============================================================================
#include <alice/alice.hpp>

// ============================================================================
// Third-party: fmt (stable, used in all commands)
// ============================================================================
#include <fmt/format.h>

// ============================================================================
// Third-party: nlohmann json (stable, used in many commands for logging)
// ============================================================================
#include <nlohmann/json.hpp>

// ============================================================================
// Fiction: Core types and traits (stable, used in almost all commands)
// ============================================================================
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

// ============================================================================
// CLI: Stores (used in 100% of commands, but may change during development)
// ============================================================================
#include "stores.hpp"

// NOLINTEND(misc-include-cleaner)

#endif  // FICTION_PCH_CLI_HPP
