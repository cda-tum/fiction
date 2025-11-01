//
// Created by marcel on 06.12.23.
//

#ifndef FICTION_CMD_TEMP_HPP
#define FICTION_CMD_TEMP_HPP

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <string>

namespace alice
{

/**
 * Executes temperature simulation for the current SiDB cell-level layout in store.
 */
class temp_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit temp_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override;

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{2, -0.32, 5.6, 5.0};
    /**
     * Critical temperature parameters.
     */
    fiction::critical_temperature_params params{};
    /**
     * Critical temperature statistics.
     */
    fiction::critical_temperature_stats stats{};
    /**
     * Critical temperature.
     */
    double ct = 0.0;
    /**
     * The simulation engine to use.
     */
    std::string sim_engine_str{"QuickExact"};
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

#endif  // FICTION_CMD_TEMP_HPP
