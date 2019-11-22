//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_TT_H
#define FICTION_TT_H


#include <alice/alice.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/constructors.hpp>
#include <cctype>
#include <string>


namespace alice
{
    /**
     * Command to create a truth table either from a binary string, a hex string, or an expression.
     *
     * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/algorithms/tt.hpp.
     */
    class tt_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit tt_command(const environment::ptr& env)
                :
                command(env, "Creates a truth table from the given hex or binary string or from an expression.\n"
                             "An expression E is a constant 0 or 1, or a variable a, b, ..., p, the negation of an "
                             "expression !E, the conjunction\nof multiple expressions (E...E), the disjunction of "
                             "multiple expressions {E...E}, the exclusive OR of multiple\nexpressions [E...E], or the "
                             "majority of three expressions <EEE>. Examples are [(ab)(!ac)] to describe\nif-then-else, "
                             "or !{!a!b} to describe the application of De Morgan’s law to (ab). The size of the truth "
                             "table must fit\nthe largest variable in the expression, e.g., if c is the largest "
                             "variable, then the truth table have at least\nthree variables.")
        {
            add_option("table,--table", table,
                       "Truth table (prefix with 0x to parse as hexadecimal)");
            add_option("--expression,-e", expression,
                       "Creates truth table from expression");
            add_option("--random,-r", random_vars,
                       "Creates a random truth table over <INPUT> variables");
        }

    protected:
        /**
         * Function to perform the read call. Reads Verilog and creates a logic_network.
         */
        void execute() override
        {
            if (is_set("table") && is_set("expression"))
            {
                env->out() << "[w] 'table' and 'expression' cannot be set at the same time" << std::endl;
                reset_flags();
                return;
            }
            if (is_set("table") && is_set("random"))
            {
                env->out() << "[w] 'table' and 'random' cannot be set at the same time" << std::endl;
                reset_flags();
                return;
            }
            if (is_set("expression") && is_set("random"))
            {
                env->out() << "[w] 'expression' and 'random' cannot be set at the same time" << std::endl;
                reset_flags();
                return;
            }

            auto& s = store<kitty::dynamic_truth_table>();

            if (is_set("table"))
            {
                if (auto size = table.size(); size > 2u && table[0] == '0' && table[1] == 'x')
                {
                    table = table.substr(2u);
                    if (is_hex_string(table))
                    {
                        uint32_t num_vars = std::log2(table.size() << 2u);
                        kitty::dynamic_truth_table tt(num_vars);
                        kitty::create_from_hex_string(tt, table);
                        s.extend() = tt;
                    }
                    else
                    {
                        env->out() << "[e] " << table << " is not a hex string" << std::endl;
                    }
                }
                else if ((size & (size - 1)) == 0)  // size is power of 2
                {
                    if (is_bin_string(table))
                    {
                        uint32_t num_vars = std::log2(table.size());
                        kitty::dynamic_truth_table tt(num_vars);
                        kitty::create_from_binary_string(tt, table);
                        s.extend() = tt;
                    }
                    else
                    {
                        env->out() << "[e] " << table << " is not a binary string" << std::endl;
                    }
                }
                else
                {
                    env->out() << "[e] number of bits is not a power of 2" << std::endl;
                }
            }
            else if (is_set("expression"))
            {
                /* find max var */
                uint32_t num_vars{0u};
                for (auto c : expression)
                {
                    if (c >= 'a' && c <= 'p')
                    {
                        num_vars = std::max<uint32_t>(num_vars, c - 'a' + 1u);
                    }
                }
                kitty::dynamic_truth_table tt(num_vars);
                if (kitty::create_from_expression(tt, expression))
                {
                    s.extend() = tt;
                }
            }
            else if (is_set("random"))
            {
                kitty::dynamic_truth_table tt(static_cast<int>(random_vars));
                kitty::create_random(tt);
                s.extend() = tt;
            }
            else
            {
                env->out() << "[w] no truth table generation approach specified" << std::endl;
            }

            reset_flags();
        }

    private:
        /**
         * String representing the truth table.
         */
        std::string table;
        /**
         * String representing the expression.
         */
        std::string expression;
        /**
         * Number of variables to create a random string for.
         */
        unsigned random_vars = 0;

        /**
         * Reset all flags, necessary for some reason... alice bug?
         */
        void reset_flags()
        {
            table = "";
            expression = "";
            random_vars = 0;
        }
        /**
         * Checks whether a passed string contains hex symbols only.
         *
         * @param s String to check.
         * @return True iff s contains hex symbols only.
         */
        bool is_hex_string(const std::string& s) noexcept
        {
            for (const auto& c : s)
            {
                if (!std::isxdigit(c))
                    return false;
            }
            return true;
        }
        /**
         * Checks whether a passed string contains binary symbols only.
         *
         * @param s String to check.
         * @return True iff s contains binary symbols only.
         */
        bool is_bin_string(const std::string& s) noexcept
        {
            for (const auto& c : s)
            {
                if (c != '0' && c != '1')
                    return false;
            }
            return true;
        }
    };

    ALICE_ADD_COMMAND(tt, "I/O")
}


#endif //FICTION_TT_H
