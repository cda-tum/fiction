//
// Created by marcel on 25.11.19.
//

#include "cmd/technology/include/energy.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/technology/energy_model.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <variant>

namespace alice
{

energy_command::energy_command(const environment::ptr& e) :
        command(e, "Prints the energy dissipation for the current gate-level layout in store that would "
                   "occur if it was compiled using the QCA-ONE library. Calculations are based upon "
                   "'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata' "
                   "by Frank Sill Torres, et al. in TCAD 2018.")
{}

void energy_command::execute()
{
    // reset energy values
    st = {};

    auto& s = store<fiction::gate_layout_t>();

    // error case: empty gate layout store
    if (s.empty())
    {
        env->out() << "[w] no gate layout in store\n";
        return;
    }

    const auto energy = [this](auto&& lyt_ptr) { fiction::qca_energy_dissipation(*lyt_ptr, &st); };

    std::visit(energy, s.current());

    st.report(env->out());
}

nlohmann::json energy_command::log() const
{
    return {{"energy (meV, QCA)", {{"slow (25 GHz)", st.slow}, {"fast (100 GHz)", st.fast}}}};
}

}  // namespace alice
