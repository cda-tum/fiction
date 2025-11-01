//
// Created by marcel on 24.10.19.
//

#include "cmd/physical_design/include/ortho.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/network_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <variant>

namespace alice
{

ortho_command::ortho_command(const environment::ptr& e) :
        command(e, "Performs scalable placement and routing of the current logic network in store. "
                   "An FCN layout that is not minimal will be found in reasonable runtime.")
{
    add_option("--clock_numbers,-n", num_clock_phases, "Number of clock phases to be used {3 or 4}");
    add_option("--hex", hexagonal_tile_shift,
               "Use hexagonal tiles and specify tile shift. Possible values are 'odd_row', 'even_row', "
               "'odd_column', or 'even_column'");
    add_flag("--verbose,-v", "Be verbose");
}

void ortho_command::execute()
{
    // error case: empty logic network store
    if (store<fiction::logic_network_t>().empty())
    {
        env->out() << "[w] no logic network in store\n";
        ps                   = {};
        hexagonal_tile_shift = "";
        return;
    }
    // error case: phases out of range
    if (num_clock_phases != 3u && num_clock_phases != 4u)
    {
        env->out() << "[e] only 3- and 4-phase clocking schemes are supported\n";
        ps                   = {};
        hexagonal_tile_shift = "";
        return;
    }

    ps.number_of_clock_phases = num_clock_phases == 3 ? fiction::num_clks::THREE : fiction::num_clks::FOUR;

    if (is_set("hex"))
    {
        if (hexagonal_tile_shift == "odd_row")
        {
            orthogonal_physical_design<fiction::hex_odd_row_gate_clk_lyt>();
        }
        else if (hexagonal_tile_shift == "even_row")
        {
            orthogonal_physical_design<fiction::hex_even_row_gate_clk_lyt>();
        }
        else if (hexagonal_tile_shift == "odd_column")
        {
            orthogonal_physical_design<fiction::hex_odd_col_gate_clk_lyt>();
        }
        else if (hexagonal_tile_shift == "even_column")
        {
            orthogonal_physical_design<fiction::hex_even_col_gate_clk_lyt>();
        }
        else
        {
            env->out() << "[e] possible values for the hexagonal tile shift are 'odd_row', 'even_row', "
                          "'odd_column', and 'even_column'\n";
            ps                   = {};
            hexagonal_tile_shift = "";
            return;
        }
    }
    else  // Cartesian layout
    {
        orthogonal_physical_design<fiction::cart_gate_clk_lyt>();
    }

    ps                   = {};
    hexagonal_tile_shift = "";
}

nlohmann::json ortho_command::log() const
{
    return nlohmann::json{{"runtime in seconds", mockturtle::to_seconds(st.time_total)},
                          {"number of gates", st.num_gates},
                          {"number of wires", st.num_wires},
                          {"number of crossings", st.num_crossings},
                          {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
}

template <typename Lyt>
void ortho_command::orthogonal_physical_design()
{
    const auto perform_physical_design = [this](auto&& ntk_ptr) { return fiction::orthogonal<Lyt>(*ntk_ptr, ps, &st); };

    const auto& ntk_ptr = store<fiction::logic_network_t>().current();

    try
    {
        auto lyt = std::visit(perform_physical_design, ntk_ptr);

        store<fiction::gate_layout_t>().extend() = std::make_shared<Lyt>(lyt);

        if (is_set("verbose"))
        {
            st.report(env->out());
        }
    }
    catch (const fiction::high_degree_fanin_exception& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
}

}  // namespace alice
