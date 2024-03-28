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
[[nodiscard]] inline kitty::dynamic_truth_table create_id_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_not_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_and_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_or_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_nand_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_nor_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_xor_tt() noexcept
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
[[nodiscard]] inline kitty::dynamic_truth_table create_xnor_tt() noexcept
{
    constexpr const uint64_t lit = 0x9;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the less-than function in two variables.
 *
 * @return Less-than function in two variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_lt_tt() noexcept
{
    constexpr const uint64_t lit = 0x2;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the greater-than function in two variables.
 *
 * @return Greater-than function in two variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_gt_tt() noexcept
{
    constexpr const uint64_t lit = 0x4;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the less-than-or-equal function in two variables.
 *
 * @return Less-than-or-equal function in two variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_le_tt() noexcept
{
    constexpr const uint64_t lit = 0x11;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the greater-than-or-equal function in two variables.
 *
 * @return Greater-than-or-equal function in two variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_ge_tt() noexcept
{
    constexpr const uint64_t lit = 0x13;

    kitty::dynamic_truth_table table{2};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the majority function in three variables.
 *
 * @return Majority function in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_maj_tt() noexcept
{
    constexpr const uint64_t lit = 0xe8;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a vector of truth tables for a double wire multi-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a double wire multi-output function in two variables. The function returns a vector containing
 * two truth tables.
 *
 * @return Vector of truth tables, each representing an output of the double wire function.
 */
[[nodiscard]] inline std::vector<kitty::dynamic_truth_table> create_double_wire_tt() noexcept
{
    static constexpr const char* truth_table_string1 = "1100";  // Output 1
    static constexpr const char* truth_table_string2 = "1010";  // Output 2

    kitty::dynamic_truth_table table1{2};  // 2 input variables for Output 1
    kitty::dynamic_truth_table table2{2};  // 2 input variables for Output 2

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}
/**
 * Creates and returns a vector of truth tables for a crossing wire multi-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a crossing wire multi-output function in two variables. The function returns a vector containing
 * two truth tables.
 *
 * @return Vector of truth tables, each representing an output of the crossing wire function.
 */
[[nodiscard]] inline std::vector<kitty::dynamic_truth_table> create_crossing_wire_tt() noexcept
{
    static constexpr const char* truth_table_string1 = "1010";  // Output 1
    static constexpr const char* truth_table_string2 = "1100";  // Output 2

    kitty::dynamic_truth_table table1{2};  // 2 input variables for Output 1
    kitty::dynamic_truth_table table2{2};  // 2 input variables for Output 2

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}
/**
 * Creates and returns a vector of truth tables for a multi-output function with two variables.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a multi-output function in two variables.
 *
 * @return Vector of truth tables, each representing an output of the identity function.
 */
[[nodiscard]] inline std::vector<kitty::dynamic_truth_table> create_fan_out_tt() noexcept
{
    static constexpr const char* truth_table_string = "10";  // Output 1

    kitty::dynamic_truth_table table{1};

    kitty::create_from_binary_string(table, truth_table_string);

    return std::vector<kitty::dynamic_truth_table>{table, table};
}
/**
 * Creates and returns a vector of truth tables for a half adder multi-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a half adder multi-output function in two variables. The function returns a vector containing
 * two truth tables.
 *
 * @return Vector of truth tables, each representing an output of the half adder function.
 */
[[nodiscard]] inline std::vector<kitty::dynamic_truth_table> create_half_adder_tt() noexcept
{
    static constexpr const char* truth_table_string1 = "1000";  // Output 1
    static constexpr const char* truth_table_string2 = "0110";  // Output 2

    kitty::dynamic_truth_table table1{2};  // 2 input variables for Output 1
    kitty::dynamic_truth_table table2{2};  // 2 input variables for Output 2

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}

// NOLINTEND(*-pointer-arithmetic)

}  // namespace fiction

#endif  // FICTION_TRUTH_TABLE_UTILS_HPP
