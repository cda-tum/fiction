//
// Created by simon on 12.06.2024.
//

#ifndef FICTION_CMD_GOLD_HPP
#define FICTION_CMD_GOLD_HPP

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <variant>

namespace alice
{
/**
 * Executes a physical design approach utilizing A* path finding in a search space graph.
 * See algorithms/physical_design/graph_oriented_layout_search.hpp for more details.
 */
class gold_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit gold_command(const environment::ptr& e) :
            command(e, "Performs scalable placement and routing of the current logic network in store using the "
                       "Graph-Oriented Layout Design (GOLD) algorithm. GOLD generates close-to-optimal 2DDWave-clocked "
                       "FCN gate-level layouts in reasonable runtime. Its result quality is better than 'ortho' and "
                       "its runtime behavior superior to 'exact' and 'onepass'. Additionally, different cost "
                       "objectives can be specified.")
    {
        add_option("--timeout,-t", ps.timeout, "Timeout in seconds");
        add_option("--num_vertex_expansions,-n", ps.num_vertex_expansions, "Number of vertex expansions during search",
                   true);
        add_option(
            "--effort_mode,-e", ps.mode,
            "Specify the effort mode of the graph-oriented layout design algorithm. Possible values for the "
            "effort mode:\n"
            " - `0` (high_efficiency): Uses minimal computational resources, resulting in fewer search space graphs "
            "and potentially lower quality solutions.\n"
            " - `1` (high_effort): Uses more computational resources, creating more search space graphs to "
            "improve the likelihood of finding optimal solutions.\n"
            " - `2` (highest_effort): Uses the maximum computational resources, generating the most search "
            "space graphs to ensure the highest chance of finding the best solution.",
            true)
            ->set_type_name("{high_efficiency=0, high_effort=1, highest_effort=2}");
        add_option("--cost_objective,-c", ps.cost,
                   "Specify the cost objective for the graph-oriented layout design algorithm."
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
    }

  protected:
    /**
     * Function to perform the physical design call. Generates a placed and routed FCN gate layout.
     */
    void execute() override
    {
        // error case: empty logic network store
        if (store<fiction::logic_network_t>().empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;
            ps = {};
            return;
        }

        if (ps.num_vertex_expansions == 0)
        {
            env->out() << "[w] the number of vertex expansions has to be at least 1" << std::endl;
            ps = {};
            return;
        }

        if (is_set("timeout"))
        {
            // convert timeout entered in seconds to milliseconds
            ps.timeout *= 1000;
        }

        graph_oriented_layout_design<fiction::cart_gate_clk_lyt>();

        ps = {};
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the physical design process.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json{
            {"runtime in seconds", mockturtle::to_seconds(st.time_total)},
            {"number of gates", st.num_gates},
            {"number of wires", st.num_wires},
            {"number of crossings", st.num_crossings},
            {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
    }

  private:
    /**
     * Parameters.
     */
    fiction::graph_oriented_layout_design_params ps{};
    /**
     * Statistics.
     */
    fiction::graph_oriented_layout_design_stats st{};

    template <typename Lyt>
    void graph_oriented_layout_design()
    {
        const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

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
                env->out() << fmt::format("[e] impossible to place and route '{}' within the given parameters",
                                          std::visit(get_name, ntk_ptr))
                           << std::endl;
            }
        }
        catch (const fiction::high_degree_fanin_exception& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (...)
        {
            env->out() << fmt::format("[e] an error occurred while placing and routing '{}' with the given parameters",
                                      std::visit(get_name, ntk_ptr))
                       << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(gold, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_GOLD_HPP
