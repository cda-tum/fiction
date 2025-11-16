//
// Created by marcel on 25.11.19.
//

#ifndef FICTION_CMD_ENERGY_HPP
#define FICTION_CMD_ENERGY_HPP

#include <fiction/technology/energy_model.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

namespace alice
{

/**
 * Prints QCA energy dissipation for current gate-level layout. Calculations are based upon
 * 'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata' by Frank Sill Torres, et al.
 * in TCAD 2018.
 */
class energy_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit energy_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the energy call. Prints estimated energy consumption for QCA-ONE library.
     */
    void execute() override;

  private:
    /**
     * Slow (25 GHz) and fast (100 GHz) energy dissipation values.
     */
    fiction::energy_dissipation_stats st{};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the energy dissipation.
     */
    nlohmann::json log() const override;
};

}  // namespace alice

#endif  // FICTION_CMD_ENERGY_HPP
