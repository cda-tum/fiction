//
// Created by simon on 12.06.2024.
//

#ifndef FICTION_CMD_GOLD_HPP
#define FICTION_CMD_GOLD_HPP

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/traits.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <memory>

namespace alice
{
/**
 *
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
            command(e, "Performs scalable placement and routing of the current logic network in store. "
                       "An FCN layout that is not minimal will be found in reasonable runtime.")
    {
        add_option("--timeout,-t", timeout, "Timeout (in ms)");
        add_flag("--high_effort,-e", "High effort");
        add_flag("--return_first,-r", "Return first found layout");
        add_flag("--verbose,-v", "Be verbose");
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

        if (is_set("return_first"))
        {
            ps.return_first = true;
        }
        if (is_set("high_effort"))
        {
            ps.high_effort = true;
        }
        if (is_set("timeout"))
        {
            ps.timeout = timeout;
        }
        if (is_set("verbose"))
        {
            ps.verbose = true;
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
        return nlohmann::json{{"runtime in seconds", mockturtle::to_seconds(st.time_total)}};
    }

  private:
    /**
     * Timeout.
     */
    uint64_t timeout = 0;
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
        const auto perform_physical_design = [this](auto&& ntk_ptr)
        { return fiction::graph_oriented_layout_design<Lyt>(*ntk_ptr, ps, &st); };

        const auto& ntk_ptr = store<fiction::logic_network_t>().current();

        try
        {
            auto lyt = std::visit(perform_physical_design, ntk_ptr);

            store<fiction::gate_layout_t>().extend() = std::make_shared<Lyt>(lyt);
        }
        catch (const fiction::high_degree_fanin_exception& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(gold, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_GELS_HPP
