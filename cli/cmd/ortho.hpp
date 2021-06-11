//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_ORTHO_HPP
#define FICTION_ORTHO_HPP

#include <fiction/algorithms/orthogonal.hpp>
#include <fiction/traits.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <memory>

namespace alice
{
/**
 * Executes a heuristic physical approach based on "Improved Orthogonal Drawing of 3-graphs" by Therese Biedl, 1996.
 * See algo/orthogonal.h for more details.
 */
class ortho_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit ortho_command(const environment::ptr& e) :
            command(e, "Performs scalable placement and routing of the current logic network in store. "
                       "An FCN layout that is not minimal will be found in reasonable runtime.")
    {
        add_option("--clock_numbers,-n", ps.number_of_clock_phases, "Number of clock phases to be used {3 or 4}", true);
        add_flag("--io_ports,-i", ps.utilize_io_ports, "Use I/O port elements instead of gate pins");
        add_flag("--border_io,-b", ps.route_ios_to_layout_borders,
                 "Enforce primary I/O to be placed at the layout's borders");
    }

  protected:
    /**
     * Function to perform the physical design call. Generates a placed and routed FCN gate layout.
     */
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;
            ps = {};
            return;
        }
        // error case: phases out of range
        if (ps.number_of_clock_phases != 3u && ps.number_of_clock_phases != 4u)
        {
            env->out() << "[e] only 3- and 4-phase clocking schemes are supported" << std::endl;
            ps = {};
            return;
        }
        // if border_ios is set, set io_ports too
        if (ps.route_ios_to_layout_borders)
        {
            ps.utilize_io_ports = true;
        }

        const auto get_name = [](auto&& net) -> std::string { return net->get_network_name(); };

        const auto orthogonal_physical_design = [this](auto&& net)
        { return std::make_shared<fiction::tile_clk_lyt>(fiction::orthogonal<fiction::tile_clk_lyt>(*net, ps)); };

        const auto& net = s.current();

        store<fiction::gate_layout_t>().extend() = std::visit(orthogonal_physical_design, net);

        //        if (auto result = physical_design(); result.success)
        //        {
        //            store<fcn_gate_layout_ptr>().extend() = physical_design.get_layout();
        //            pd_result                             = result.json;
        //        }
        //        else
        //            env->out() << "[e] impossible to place and route " << std::visit(get_name, net) << std::endl;

        ps = {};
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the physical design process.
     */
    nlohmann::json log() const override
    {
        return pd_result;
    }

  private:
    fiction::orthogonal_physical_design_params ps{};
    /**
     * Resulting logging information.
     */
    nlohmann::json pd_result{};
};

ALICE_ADD_COMMAND(ortho, "Physical Design")
}  // namespace alice

#endif  // FICTION_ORTHO_HPP
