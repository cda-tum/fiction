//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_CMD_EXACT_HPP
#define FICTION_CMD_EXACT_HPP

#include <fiction/algorithms/physical_design/exact.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <string>

namespace alice
{

/**
 * Executes an exact physical design approach utilizing the SMT solver Z3.
 *
 * See include/fiction/algorithms/physical_design/exact.hpp for more details.
 */
class exact_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit exact_command(const environment::ptr& e);

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
    fiction::exact_physical_design_params ps{};
    /**
     * Statistics.
     */
    fiction::exact_physical_design_stats st{};
    /**
     * Tile shift for hexagonal layouts.
     */
    std::string hexagonal_tile_shift{};

    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags();

    template <typename Lyt>
    void exact_physical_design();
};

}  // namespace alice

#endif  // FICTION_CMD_EXACT_HPP

#endif  // FICTION_Z3_SOLVER
