//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_CHECK_HPP
#define FICTION_CMD_CHECK_HPP

#include <fiction/algorithms/verification/design_rule_violations.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

namespace alice
{

/**
 * Performs design rule checks on the active gate-level layout. Checks for various design rule validations like crossing
 * gates, non-adjacent connections, wrongly assigned clock zones, etc.
 *
 * See fiction/algorithms/design_rule_violations.hpp for more details.
 */
class check_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit check_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the design rule check call. Generates a report and prints a summary.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the process.
     */
    nlohmann::json log() const override;

  private:
    fiction::gate_level_drv_params ps{};
    fiction::gate_level_drv_stats  pst{};
};

}  // namespace alice

#endif  // FICTION_CMD_CHECK_HPP
