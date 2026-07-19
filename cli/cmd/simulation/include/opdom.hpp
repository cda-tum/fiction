//
// Created by marcel on 12.12.23.
//

#ifndef FICTION_CMD_OPDOM_HPP
#define FICTION_CMD_OPDOM_HPP

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <string>
#include <vector>

namespace alice
{

/**
 * Executes operational domain computation for the current SiDB cell-level layout in store. The operational domain is a
 * set of simulation parameter values for which a given SiDB layout is logically operational. This means that a layout
 * is deemed operational if the layout's ground state corresponds with a given Boolean function at the layout's outputs
 * for all possible input combinations.
 *
 * The computation comes in different flavors:
 * - Grid search: Evaluates all possible parameter combinations within the specified ranges.
 * - Random sampling: Evaluates a specified number of random parameter combinations.
 * - Flood fill: Evaluates a specified number of random parameter combinations and then performs a flood fill to find
 *  the operational domain.
 *  - Contour tracing: Evaluates a specified number of random parameter combinations and then performs contour tracing
 *  to find the edges of the operational domain.
 *
 * The operational domain is written to a CSV file, which can be used for further analysis or visualization.
 *
 * For more information, see algorithms/simulation/sidb/operational_domain.hpp.
 */
class opdom_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit opdom_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the operational domain call.
     */
    void execute() override;

  private:
    /**
     * Default physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters simulation_params{2, -0.32, 5.6, 5.0};
    /**
     * Default value ranges for sweeping.
     */
    std::vector<fiction::operational_domain_value_range> sweep_dimensions{
        {fiction::sweep_parameter::EPSILON_R, 1.0, 10.0, 0.1},
        {fiction::sweep_parameter::LAMBDA_TF, 1.0, 10.0, 0.1},
        {fiction::sweep_parameter::MU_MINUS, -0.50, -0.10, 0.025}};
    /**
     * Operational domain parameters.
     */
    fiction::operational_domain_params params{};
    /**
     * Operational domain stats.
     */
    fiction::operational_domain_stats stats{};
    /**
     * Number of random samples.
     */
    std::size_t num_random_samples{};
    /**
     * User input for the x dimension sweep parameter.
     */
    std::string x_sweep{"epsilon_r"};
    /**
     * User input for the y dimension sweep parameter.
     */
    std::string y_sweep{"lambda_tf"};
    /**
     * User input for the z dimension sweep parameter.
     */
    std::string z_sweep{};
    /**
     * The simulation engine to use.
     */
    std::string sim_engine_str{"QuickExact"};
    /**
     * Cached engine name for logging (preserves the engine name after params reset).
     */
    std::string last_engine_name{};
    /**
     * CSV filename to write the operational domain to.
     */
    std::string filename{};
    /**
     * Flag to omit non-operational samples.
     */
    bool omit_non_operational_samples = false;
    /**
     * The operational domain.
     */
    fiction::operational_domain op_domain{};
    /**
     * Writes the operational domain to the specified CSV file.
     */
    void write_op_domain();
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the operational domain.
     */
    [[nodiscard]] nlohmann::json log() const override;
    /**
     * Resets the parameters to their default values.
     */
    void reset_params();
};

}  // namespace alice

#endif  // FICTION_CMD_OPDOM_HPP
