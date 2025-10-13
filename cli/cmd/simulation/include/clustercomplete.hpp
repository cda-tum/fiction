#if (FICTION_ALGLIB_ENABLED)

#ifndef FICTION_CMD_CLUSTERCOMPLETE_HPP
#define FICTION_CMD_CLUSTERCOMPLETE_HPP

#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <variant>

namespace alice
{

/**
 * Executes *ClusterComplete* exact simulation for the current SiDB cell-level layout in store.
 */
class clustercomplete_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit clustercomplete_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override;

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{3, -0.32, 5.6, 5.0};
    /**
     * ClusterComplete parameters.
     */
    fiction::clustercomplete_params<> cc_params{};
    /**
     * Type alias for H-Si(100)-2x1 simulation result.
     */
    using sim_result_100 = fiction::sidb_simulation_result<fiction::sidb_100_cell_clk_lyt>;
    /**
     * Type alias for H-Si(111)-1x1 simulation result.
     */
    using sim_result_111 = fiction::sidb_simulation_result<fiction::sidb_111_cell_clk_lyt>;
    /**
     * Simulation result for either the H-Si(100)-2x1 or the H-Si(111)-1x1 surface.
     */
    std::variant<sim_result_100, sim_result_111> sim_result;
    /**
     * Minimum energy.
     */
    double min_energy{std::numeric_limits<double>::infinity()};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    [[nodiscard]] nlohmann::json log() const override;
    /**
     * Resets the parameters to their default values.
     */
    void reset_params();
};

}  // namespace alice

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CMD_CLUSTERCOMPLETE_HPP
