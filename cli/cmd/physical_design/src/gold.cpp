//
// Created by simon on 12.06.2024.
//

#include "cmd/physical_design/include/gold.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>
#include <fiction/utils/network_utils.hpp>

#include <alice/alice.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <variant>

namespace alice
{

gold_command::gold_command(const environment::ptr& e) :
        command(e, "Performs scalable placement and routing of the current logic network in store using the "
                   "Graph-Oriented Layout Design (GOLD) algorithm. GOLD generates close-to-optimal 2DDWave-clocked "
                   "FCN gate-level layouts in reasonable runtime. Its result quality is better than 'ortho' and "
                   "its runtime behavior superior to 'exact' and 'onepass'. Additionally, different cost "
                   "objectives can be specified.")
{
    add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
    add_option("--num_vertex_expansions,-n", ps.num_vertex_expansions, "Number of vertex expansions during search",
               true);
    add_option("--effort_mode,-e", ps.mode,
               "Specify the effort mode of the graph-oriented layout design algorithm. Possible values for the "
               "effort mode:\n"
               " - `0` (high_efficiency): Uses minimal computational resources, resulting in fewer search space graphs "
               "and potentially lower quality solutions.\n"
               " - `1` (high_effort): Uses more computational resources, creating more search space graphs to "
               "improve the likelihood of finding optimal solutions.\n"
               " - `2` (highest_effort): Uses even more computational resources, generating four to five times as many "
               "search space graphs compared to high-effort mode.\n"
               " - `3` (maximum_effort): Uses the maximum computational resources, generating the most search "
               "space graphs to ensure the highest chance of finding the best solution.",
               true)
        ->set_type_name("{high_efficiency=0, high_effort=1, highest_effort=2, maximum_effort=3}");
    add_option("--cost_objective,-c", ps.cost,
               "Specify the cost objective for the graph-oriented layout design algorithm. "
               "Possible values for the cost objective:\n"
               " - `0` (area): Minimize the layout area.\n- `1` (wires): Minimize the number of wire segments.\n"
               " - `2` (crossings): Minimize the number of crossings.\n"
               " - `3` (acp): Minimize the area-crossing product (ACP), balancing area and crossings.",
               true)
        ->set_type_name("{area=0, wires=1, crossings=2, acp=3}");
    add_flag("--return_first,-r", ps.return_first,
             "Terminate on the first found layout; reduces runtime but might sacrifice result quality");
    add_flag("--planar,-p", ps.planar, "Enable planar layout generation");
    add_flag("--multithreading,-m", ps.enable_multithreading, "Enable multithreading (beta feature)");
    add_flag("--verbose,-v", ps.verbose, "Be verbose");
    add_option("--seed,-s", seed,
               "Random seed used for random fanout substitution and random topological ordering in "
               "maximum-effort mode");
    add_flag("--straight_inverters,-i", ps.straight_inverters, "Enforce NOT gates to be routed non-bending only");
    add_option(
        "--tiles_to_skip_between_pis,-g", ps.tiles_to_skip_between_pis,
        "For each primary input (PI) considered during placement, reserve this many empty tiles after the current "
        "frontier to the right of the rightmost occupied tile or below the bottommost occupied tile before "
        "proposing a new PI position. This soft margin can reduce local congestion and increase the probability of "
        "finding a routable layout at the expense of a temporarily larger footprint, which post-layout "
        "optimization may later shrink. Defaults to 0.");
    add_flag("--randomize_tiles_to_skip_between_pis,-j", ps.randomize_tiles_to_skip_between_pis,
             "Randomize the number of skipped tiles for each PI placement. When enabled, each PI will use a "
             "random number of skipped tiles between tiles_to_skip_between_pis-1 and tiles_to_skip_between_pis "
             "(inclusive). When tiles_to_skip_between_pis is 0, only 0 will be used. This can help explore different "
             "placement strategies and potentially find better layouts. Requires a valid seed to be set for "
             "reproducible results.");
}

void gold_command::execute()
{
    // error case: empty logic network store
    if (store<fiction::logic_network_t>().empty())
    {
        env->out() << "[w] no logic network in store\n";
        ps = {};
        return;
    }

    if (ps.num_vertex_expansions == 0)
    {
        env->out() << "[w] the number of vertex expansions has to be at least 1\n";
        ps = {};
        return;
    }

    if (is_set("timeout"))
    {
        // convert timeout entered in seconds to milliseconds
        ps.timeout *= 1000;
    }

    if (is_set("seed"))
    {
        ps.seed = seed;
    }

    graph_oriented_layout_design<fiction::cart_gate_clk_lyt>();

    ps = {};
}

nlohmann::json gold_command::log() const
{
    return nlohmann::json{{"runtime in seconds", mockturtle::to_seconds(st.time_total)},
                          {"number of gates", st.num_gates},
                          {"number of wires", st.num_wires},
                          {"number of crossings", st.num_crossings},
                          {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
}

template <typename Lyt>
void gold_command::graph_oriented_layout_design()
{
    const auto get_name = [](auto&& ntk_ptr) -> std::string { return fiction::get_name(*ntk_ptr); };

    const auto perform_physical_design = [this](auto&& ntk_ptr)
    { return fiction::graph_oriented_layout_design<Lyt>(*ntk_ptr, ps, &st); };

    const auto& ntk_ptr = store<fiction::logic_network_t>().current();

    try
    {
        const auto lyt = std::visit(perform_physical_design, ntk_ptr);

        if (lyt.has_value())
        {
            store<fiction::gate_layout_t>().extend() = std::make_shared<Lyt>(*lyt);
        }
        else
        {
            env->out() << fmt::format("[e] impossible to place and route '{}' within the given parameters\n",
                                      std::visit(get_name, ntk_ptr));
        }
    }
    catch (const fiction::high_degree_fanin_exception& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        env->out() << fmt::format("[e] an error occurred while placing and routing '{}' with the given parameters\n",
                                  std::visit(get_name, ntk_ptr));
    }
}

}  // namespace alice
