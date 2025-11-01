//
// Created by marcel on 24.10.19.
//

#include "cmd/io/include/tt.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <kitty/constructors.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>

namespace alice
{

tt_command::tt_command(const environment::ptr& e) :
        command(e, "Creates a truth table from the given hex or binary string or from an expression. "
                   "An expression E is a constant 0 or 1, or a variable a, b, ..., p, the negation of an "
                   "expression !E, the conjunction of multiple expressions (E...E), the disjunction of "
                   "multiple expressions {E...E}, the exclusive OR of multiple expressions [E...E], or the "
                   "majority of three expressions <EEE>. Examples are [(ab)(!ac)] to describe if-then-else, "
                   "or !{!a!b} to describe the application of De Morgan's law to (ab). The size of the truth "
                   "table must fit the largest variable in the expression, e.g., if c is the largest "
                   "variable, then the truth table have at least three variables.")
{
    add_option("--table,-t", table, "Truth table (prefix with 0x to parse as hexadecimal)");
    add_option("--expression,-e", expression, "Creates truth table from expression");
    add_option("--random,-r", random_vars, "Creates a random truth table over <INPUT> variables");
}

void tt_command::execute()
{
    if (is_set("table") && is_set("expression"))
    {
        env->out() << "[e] 'table' and 'expression' cannot be set at the same time\n";
        reset_flags();
        return;
    }
    if (is_set("table") && is_set("random"))
    {
        env->out() << "[e] 'table' and 'random' cannot be set at the same time\n";
        reset_flags();
        return;
    }
    if (is_set("expression") && is_set("random"))
    {
        env->out() << "[e] 'expression' and 'random' cannot be set at the same time\n";
        reset_flags();
        return;
    }

    auto& s = store<fiction::truth_table_t>();

    if (is_set("table"))
    {
        if (const auto size = table.size(); size > 2u && table[0] == '0' && table[1] == 'x')
        {
            table = table.substr(2u);
            if (is_hex_string(table))
            {
                const auto num_vars = static_cast<uint32_t>(std::log2(table.size() << 2ul));

                fiction::tt tt(num_vars);
                kitty::create_from_hex_string(tt, table);

                s.extend() = std::make_shared<fiction::tt>(tt);
            }
            else
            {
                env->out() << "[e] " << table << " is not a hex string\n";
            }
        }
        else if (size >= 2u && (size & (size - 1)) == 0)  // size is power of 2 and at least 2
        {
            if (is_bin_string(table))
            {
                const auto num_vars = static_cast<uint32_t>(std::log2(table.size()));

                fiction::tt tt(num_vars);
                kitty::create_from_binary_string(tt, table);

                s.extend() = std::make_shared<fiction::tt>(tt);
            }
            else
            {
                env->out() << "[e] " << table << " is not a binary string\n";
            }
        }
        else
        {
            env->out() << "[e] number of bits is not a power of 2\n";
        }
    }
    else if (is_set("expression"))
    {
        /* find max var */
        uint32_t num_vars{0u};
        for (const auto c : expression)
        {
            if (c >= 'a' && c <= 'p')
            {
                num_vars = std::max<uint32_t>(num_vars, static_cast<uint32_t>(c - 'a') + 1u);
            }
        }

        if (fiction::tt tt(num_vars); kitty::create_from_expression(tt, expression))
        {
            s.extend() = std::make_shared<fiction::tt>(tt);
        }
        else
        {
            env->out() << "[e] expression could not be parsed\n";
        }
    }
    else if (is_set("random"))
    {
        if (random_vars == 0u)
        {
            env->out() << "[e] number of variables must be positive\n";
            reset_flags();
            return;
        }

        fiction::tt tt{random_vars};
        kitty::create_random(tt);

        s.extend() = std::make_shared<fiction::tt>(tt);
    }
    else
    {
        env->out() << "[e] no truth table generation approach specified\n";
    }

    reset_flags();
}

void tt_command::reset_flags()
{
    table       = "";
    expression  = "";
    random_vars = 0;
}

bool tt_command::is_hex_string(const std::string& s) noexcept
{
    return std::all_of(s.begin(), s.end(), [](const unsigned char c) { return std::isxdigit(c) != 0; });
}

bool tt_command::is_bin_string(const std::string& s) noexcept
{
    return std::all_of(s.begin(), s.end(), [](const auto c) { return c == '0' || c == '1'; });
}

}  // namespace alice
