//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_CHECK_HPP
#define FICTION_CMD_CHECK_HPP

#include <fiction/algorithms/design_rule_violations.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <variant>

namespace alice
{
/**
 * Performs design rule checks on the active gate-level layout. Checks for various design rule validations like crossing
 * gates, non-adjacent connections, wrongly assigned clock zones, etc.
 *
 * See fiction/algorithms/design_rule_violations.hpp for more details.
 */
class check_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit check_command(const environment::ptr& e) :
            command(e, "Performs various design rule checks on the current gate-level layout in store. "
                       "A full report can be logged and a summary is printed to standard output.")
    {}

  protected:
    /**
     * Function to perform the design rule check call. Generates a report and prints a summary.
     */
    void execute() override
    {
        auto& s = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (s.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            return;
        }

        ps.out = &env->out();
        pst    = {};

        const auto design_rule_check = [this](auto&& lyt_ptr) { fiction::gate_level_drvs(*lyt_ptr, ps, &pst); };

        std::visit(design_rule_check, s.current());
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the process.
     */
    nlohmann::json log() const override
    {
        return pst.report;
    }

  private:
    fiction::gate_level_drv_params ps{};
    fiction::gate_level_drv_stats  pst{};
};

ALICE_ADD_COMMAND(check, "Verification")

}  // namespace alice

#endif  // FICTION_CMD_CHECK_HPP
