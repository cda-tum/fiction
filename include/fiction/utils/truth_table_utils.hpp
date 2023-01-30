//
// Created by marcel on 05.04.22.
//

#ifndef FICTION_TRUTH_TABLE_UTILS_HPP
#define FICTION_TRUTH_TABLE_UTILS_HPP

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <cstdint>

namespace fiction
{

// NOLINTBEGIN(*-pointer-arithmetic)

/**
 * Creates and returns a truth table that implements the identity function in one variable.
 *
 * @return Identity function in one variable.
 */
inline kitty::dynamic_truth_table create_id_tt() noexcept
{
    constexpr const uint64_t lit = 0x2;

    kitty::dynamic_truth_table table{1};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the negation in one variable.
 *
 * @return Negation in one variable.
 */
inline kitty::dynamic_truth_table create_not_tt() noexcept
{
    constexpr const uint64_t lit = 0x1;

    kitty::dynamic_truth_table table{1};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the conjunction in two variables.
 *
 * @return Conjunction in two variables.
 */
inline kitty::dynamic_truth_table create_and_tt() noexcept
{
    constexpr const uint64_t lit = 0x8;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the disjunction in two variables.
 *
 * @return Disjunction in two variables.
 */
inline kitty::dynamic_truth_table create_or_tt() noexcept
{
    constexpr const uint64_t lit = 0xe;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the negated conjunction in two variables.
 *
 * @return Negated conjunction in two variables.
 */
inline kitty::dynamic_truth_table create_nand_tt() noexcept
{
    constexpr const uint64_t lit = 0x7;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the negated disjunction in two variables.
 *
 * @return Negated disjunction in two variables.
 */
inline kitty::dynamic_truth_table create_nor_tt() noexcept
{
    constexpr const uint64_t lit = 0x1;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the exclusive disjunction in two variables.
 *
 * @return Exclusive disjunction in two variables.
 */
inline kitty::dynamic_truth_table create_xor_tt() noexcept
{
    constexpr const uint64_t lit = 0x6;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the negated exclusive disjunction in two variables.
 *
 * @return Negated exclusive disjunction in two variables.
 */
inline kitty::dynamic_truth_table create_xnor_tt() noexcept
{
    constexpr const uint64_t lit = 0x9;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the majority function in three variables.
 *
 * @return Majority function in three variables.
 */
inline kitty::dynamic_truth_table create_maj_tt() noexcept
{
    constexpr const uint64_t lit = 0xe8;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}

// NOLINTEND

}  // namespace fiction

#endif  // FICTION_TRUTH_TABLE_UTILS_HPP
