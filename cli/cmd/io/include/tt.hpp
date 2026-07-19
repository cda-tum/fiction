//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_TT_HPP
#define FICTION_CMD_TT_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Command to create a truth table either from a binary string, a hex string, or an expression.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/algorithms/tt.hpp.
 */
class tt_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit tt_command(const environment::ptr& e);

  protected:
    /**
     * Executes the command: creates a truth table from a binary/hex string, an expression, or randomly.
     */
    void execute() override;

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
    void reset_flags();
    /**
     * Checks whether a passed string contains hex symbols only.
     *
     * @param s String to check.
     * @return True iff s contains hex symbols only.
     */
    static bool is_hex_string(const std::string& s) noexcept;
    /**
     * Checks whether a passed string contains binary symbols only.
     *
     * @param s String to check.
     * @return True iff s contains binary symbols only.
     */
    static bool is_bin_string(const std::string& s) noexcept;
};

}  // namespace alice

#endif  // FICTION_CMD_TT_HPP
