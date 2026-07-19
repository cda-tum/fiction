//
// Created by marcel on 09.04.20.
//

#if (MUGEN)

#ifndef FICTION_CMD_ONEPASS_HPP
#define FICTION_CMD_ONEPASS_HPP

#include <fiction/algorithms/physical_design/one_pass_synthesis.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

namespace alice
{

/**
 * Executes a SAT-driven topology-based logic re-synthesis, i.e., a one-pass synthesis. Utilizes the synthesis tool
 * Mugen by Winston Haaswijk.
 *
 * See include/fiction/algorithms/physical_design/one_pass_synthesis.hpp for more details.
 */
class onepass_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit onepass_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the physical design call.
     * Given arguments are parsed and a placed and routed FCN gate layout is generated if possible.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the solving process.
     */
    nlohmann::json log() const override;

  private:
    /**
     * Parameters.
     */
    fiction::one_pass_synthesis_params ps{};
    /**
     * Statistics.
     */
    fiction::one_pass_synthesis_stats st{};
    /**
     * Reset flags. Necessary due to an alice bug.
     */
    void reset_flags() noexcept;
};

}  // namespace alice

#endif  // FICTION_CMD_ONEPASS_HPP

#endif  // MUGEN
