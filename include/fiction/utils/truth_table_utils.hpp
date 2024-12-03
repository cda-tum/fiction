//
// Created by marcel on 05.04.22.
//

#ifndef FICTION_TRUTH_TABLE_UTILS_HPP
#define FICTION_TRUTH_TABLE_UTILS_HPP

#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <bitset>
#include <cstdint>
#include <vector>

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
 * Creates and returns a truth table that implements the conjunction in three variables.
 *
 * @return Conjunction in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_and3_tt() noexcept
{
    constexpr const uint64_t lit = 0x80;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the XOR-AND function (a and (b xor c)) in three variables.
 *
 * @return XOR-AND in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_xor_and_tt() noexcept
{
    constexpr const uint64_t lit = 0x28;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the OR-AND function (a and (b or c)) in three variables.
 *
 * @return OR-AND in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_or_and_tt() noexcept
{
    constexpr const uint64_t lit = 0xa8;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the Onehot function (exactly one of a,b,c) in three variables.
 *
 * @return Onehot in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_onehot_tt() noexcept
{
    constexpr const uint64_t lit = 0x16;

    kitty::dynamic_truth_table table{3};
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
 * Creates and returns a truth table that implements the Gamble function (all or none of a,b,c) in three variables.
 *
 * @return Gamble in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_gamble_tt() noexcept
{
    constexpr const uint64_t lit = 0x81;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the Dot function (a xor (c or a and b)) in three variables.
 *
 * @return Dot function in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_dot_tt() noexcept
{
    constexpr const uint64_t lit = 0x52;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the ITE (MUX) function (if a then b else c) in three variables.
 *
 * @return ITE (MUX) in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_ite_tt() noexcept
{
    constexpr const uint64_t lit = 0xd8;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the AND-XOR function (a xor b and c) in three variables.
 *
 * @return AND-XOR in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_and_xor_tt() noexcept
{
    constexpr const uint64_t lit = 0x6a;

    kitty::dynamic_truth_table table{3};
    kitty::create_from_words(table, &lit, &lit + 1);

    return table;
}
/**
 * Creates and returns a truth table that implements the exclusive disjunction in three variables.
 *
 * @return Exclusive disjunction in three variables.
 */
[[nodiscard]] inline kitty::dynamic_truth_table create_xor3_tt() noexcept
{
    constexpr const uint64_t lit = 0x96;

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
    static constexpr const char* truth_table_string1 = "0110";  // Output 1
    static constexpr const char* truth_table_string2 = "1000";  // Output 2

    kitty::dynamic_truth_table table1{2};  // 2 input variables for Output 1
    kitty::dynamic_truth_table table2{2};  // 2 input variables for Output 2

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}

/**
 * This function calculates the output index for a given input index by evaluating the truth table.
 *
 * @param truth_table The truth table to evaluate.
 * @param current_input_index The index representing the current input pattern.
 * @return The output index derived from the truth table for the given input index.
 */
[[nodiscard]] uint64_t determine_output_index_of_output(const std::vector<kitty::dynamic_truth_table>& truth_table,
                                                        const uint64_t current_input_index) noexcept
{
    std::bitset<64> bits{};
    for (auto i = 0u; i < truth_table.size(); i++)
    {
        bits[i] = kitty::get_bit(truth_table[i], current_input_index);
    }
    return bits.to_ulong();
}

// NOLINTEND(*-pointer-arithmetic)

}  // namespace fiction

#endif  // FICTION_TRUTH_TABLE_UTILS_HPP
