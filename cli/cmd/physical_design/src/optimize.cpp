//
// Created by Simon Hofmann on 02.08.23.
//

#include "cmd/physical_design/include/optimize.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>
#include <fiction/algorithms/physical_design/wiring_reduction.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <optional>
#include <type_traits>
#include <variant>

namespace alice
{

optimize_command::optimize_command(const environment::ptr& e) :
        command(e, "Optimizes a 2DDWave-clocked Cartesian layout with respect to area. It achieves this objective "
                   "by strategically repositioning gates within the layout, removing excess wiring, and "
                   "effectively relocating outputs to more favorable positions.")
{
    add_flag("--wiring_reduction_only,-w",
             "Do not attempt gate repositioning, but apply wiring reduction "
             "exclusively (recommended for logic functions with >200 gates due to scalability reasons).");
    add_option("--max_gate_relocations,-m", max_gate_relocations,
               "Specify the maximum number of relocations to try for each gate (defaults "
               "to the number of tiles in the layout).");
    add_flag("--planar_optimization,-p", ps.planar_optimization,
             "During optimization, only relocate gates if the new wiring contains no crossings. For planar "
             "layouts, the resulting layout will also be planar. If the layout already contains crossings, the "
             "optimized layout will have the same number of crossings or less.");
    add_flag("--verbose,-v", "Be verbose");
    add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
}

void optimize_command::execute()
{
    auto& gls = store<fiction::gate_layout_t>();

    // error case: empty gate-level layout store
    if (gls.empty())
    {
        env->out() << "[w] no gate layout in store\n";
        ps  = {};
        psw = {};
        st  = {};
        stw = {};
        return;
    }

    const auto& lyt = gls.current();

    const auto is_twoddwave_clocked = [](auto&& lyt_ptr) -> bool
    { return lyt_ptr->is_clocking_scheme(fiction::clock_name::TWODDWAVE); };

    // error case: layout is not 2DDWave-clocked
    if (!std::visit(is_twoddwave_clocked, lyt))
    {
        env->out() << "[e] layout has to be 2DDWave-clocked\n";
        ps  = {};
        psw = {};
        st  = {};
        stw = {};
        return;
    }

    if (is_set("timeout"))
    {
        // convert timeout entered in seconds to milliseconds
        ps.timeout *= 1000;
        psw.timeout = ps.timeout;
    }

    const auto apply_optimization = [&](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        auto lyt_copy = lyt_ptr->clone();

        if constexpr (fiction::is_cartesian_layout_v<Lyt>)
        {
            if (is_set("wiring_reduction_only"))
            {
                fiction::wiring_reduction(lyt_copy, psw, &stw);
                if (is_set("verbose"))
                {
                    stw.report(env->out());
                }
            }
            else
            {
                fiction::post_layout_optimization(lyt_copy, ps, &st);
                if (is_set("verbose"))
                {
                    st.report(env->out());
                }
            }

            fiction::restore_names(*lyt_ptr, lyt_copy);

            gls.extend() = std::make_shared<Lyt>(lyt_copy);
        }
        else
        {
            env->out() << "[e] layout has to be Cartesian\n";
        }
    };

    if (is_set("max_gate_relocations"))
    {
        ps.max_gate_relocations = max_gate_relocations;
    }

    std::visit(apply_optimization, lyt);

    ps  = {};
    psw = {};
    st  = {};
    stw = {};
}

}  // namespace alice
