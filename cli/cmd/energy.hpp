//
// Created by marcel on 25.11.19.
//

#ifndef FICTION_ENERGY_HPP
#define FICTION_ENERGY_HPP

#include "fcn_gate_layout.h"

#include <alice/alice.hpp>

namespace alice
{
/**
 * Prints QCA energy dissipation for current fcn_gate_layout. Calculations are based upon
 * 'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata' by Frank Sill Torres, et al.
 * in TCAD 2018.
 */
class energy_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit energy_command(const environment::ptr& e) :
            command(e, "Prints the energy dissipation for the current gate-level layout in store that would "
                       "occur if it was compiled using the QCA-ONE library. Calculations are based upon "
                       "'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata' "
                       "by Frank Sill Torres, et al. in TCAD 2018.")
    {}

  protected:
    /**
     * Function to perform the energy call. Prints estimated energy consumption for QCA-ONE library.
     */
    void execute() override
    {
        // reset energy values
        slow = 0.0, fast = 0.0;

        auto& s = store<fcn_gate_layout_ptr>();

        // error case: empty gate layout store
        if (s.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            return;
        }

        auto fgl = s.current();

        const auto energy = fgl->calculate_energy();
        slow              = energy.first;
        fast              = energy.second;

        env->out() << fmt::format("[i] slow (25 GHz): {} meV, fast (100 GHz): {} meV", slow, fast) << std::endl;
    }

  private:
    /**
     * Slow (25 GHz) and fast (100 GHz) energy dissipation values.
     */
    double slow, fast;
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the energy dissipation.
     */
    nlohmann::json log() const override
    {
        return {{"energy (meV, QCA)", {{"slow (25 GHz)", slow}, {"fast (100 GHz)", fast}}}};
    }
};

ALICE_ADD_COMMAND(energy, "Technology")
}  // namespace alice

#endif  // FICTION_ENERGY_HPP
