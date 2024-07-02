//
// Created by marcel on 08.06.21.
//

#ifndef FICTION_CMD_MAP_HPP
#define FICTION_CMD_MAP_HPP

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <variant>

namespace alice
{
/**
 * Performs technology mapping to transform a network into another one using specific gates.
 */
class map_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit map_command(const environment::ptr& e) :
            command(e, "Performs technology mapping to transform a network into another one using specific gates.")
    {
        add_flag("--and,-a", ps.and2, "Enable the use of AND gates");
        add_flag("--nand", ps.nand2, "Enable the use of NAND gates");
        add_flag("--or,-o", ps.or2, "Enable the use of OR gates");
        add_flag("--nor", ps.nor2, "Enable the use of NOR gates");
        add_flag("--xor,-x", ps.xor2, "Enable the use of XOR gates");
        add_flag("--xnor", ps.xnor2, "Enable the use of XNOR gates");
        add_flag("--lt", ps.lt2, "Enable the use of LT gates");
        add_flag("--gt", ps.gt2, "Enable the use of GT gates");
        add_flag("--le", ps.le2, "Enable the use of LE gates");
        add_flag("--ge", ps.ge2, "Enable the use of GE gates");
        add_flag("--inv,-i", ps.inv, "Enable the use of NOT gates");

        add_flag("--maj,-m", ps.maj3, "Enable the use of MAJ gates");
        add_flag("--dot,-d", ps.dot, "Enable the use of DOT gates");
        add_flag("--and3", ps.and3, "Enable the use of AND3 gates");
        add_flag("--xor_and", ps.xor_and, "Enable the use of XOR-AND gates");
        add_flag("--or_and", ps.or_and, "Enable the use of OR-AND gates");
        add_flag("--onehot", ps.onehot, "Enable the use of ONEHOT gates");
        add_flag("--gamble", ps.gamble, "Enable the use of GAMBLE gates");
        add_flag("--mux", ps.mux, "Enable the use of MUX gates");
        add_flag("--and_xor", ps.and_xor, "Enable the use of AND-XOR gates");

        add_flag("--all2", "Enable the use of all supported 2-input gates + inverters");
        add_flag("--all3", "Enable the use of all supported 3-input gates + inverters");
        add_flag("--all", "Enable the use of all supported gates");

        add_flag("--decay", ps.decay, "Enforce the application of at least one constant input to three-input gates");
        add_flag("--logic_sharing,-s", ps.mapper_params.enable_logic_sharing, "Enable logic sharing optimization");
        add_flag("--verbose,-v", ps.mapper_params.verbose, "Be verbose");
    }

  protected:
    /**
     * Function to perform the map call. Generates a logic network from another one.
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

        if (static_cast<int>(is_set("all2")) + static_cast<int>(is_set("all3")) + static_cast<int>(is_set("all")) > 1)
        {
            env->out() << "[w] only one of '--all2', '--all3', or '--all' must be set" << std::endl;
            ps = {};
            return;
        }

        if (is_set("all2"))
        {
            ps = fiction::all_standard_2_input_functions();
        }
        else if (is_set("all3"))
        {
            ps = fiction::all_standard_3_input_functions();
        }
        else if (is_set("all"))
        {
            ps = fiction::all_supported_standard_functions();
        }

        const std::array gate_flags{is_set("and"),    is_set("nand"),   is_set("or"),      is_set("nor"),
                                    is_set("xor"),    is_set("xnor"),   is_set("inv"),     is_set("maj"),
                                    is_set("dot"),    is_set("and3"),   is_set("xor_and"), is_set("or_and"),
                                    is_set("onehot"), is_set("gamble"), is_set("mux"),     is_set("and_xor"),
                                    is_set("all2"),   is_set("all3"),   is_set("all")};

        if (std::none_of(gate_flags.cbegin(), gate_flags.cend(), [](const auto f) { return f; }))
        {
            env->out() << "[e] no gates specified; cannot perform technology mapping" << std::endl;
            ps = {};
            return;
        }

        const auto perform_mapping = [this, &s](auto&& ntk_ptr)
        {
            fiction::technology_mapping_stats st{};

            const auto mapped_ntk = fiction::technology_mapping(*ntk_ptr, ps, &st);

            if (st.mapper_stats.mapping_error)
            {
                env->out() << "[e] an error occurred in mockturtle's technology mapper" << std::endl;
                return;
            }

            s.extend() = std::make_shared<fiction::tec_nt>(mapped_ntk);
        };

        std::visit(perform_mapping, s.current());

        ps = {};
    }

  private:
    /**
     * Technology mapping parameters.
     */
    fiction::technology_mapping_params ps{};
};

ALICE_ADD_COMMAND(map, "Logic")

}  // namespace alice

#endif  // FICTION_CMD_MAP_HPP
