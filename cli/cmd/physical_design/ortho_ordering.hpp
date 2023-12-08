//
// Created by benjamin on 27.11.23.
//

#ifndef FICTION_CMD_ORTHO_ORDERING_HPP
#define FICTION_CMD_ORTHO_ORDERING_HPP

#include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>
#include <fiction/traits.hpp>

#include <alice/alice.hpp>
#include <mockturtle/views/names_view.hpp>
#include <nlohmann/json.hpp>

#include <memory>

namespace alice
{
/**
 * Executes a modified heuristic physical approach based on \"Scalable Design for Field-coupled Nanocomputing Circuits\"
 * by M. Walter, R. Wille, F. Sill Torres, D. Große, and R. Drechsler. The modified approach was first presented in
 * \"Versatile Signal Distribution Networks for Scalable Placement and Routing of Field-coupled Nanocomputing
 * Technologies\" by M. Walter, B. Hien and R. Wille. See algo/ortho_ordering_network.hpp for more details.
 */
class ortho_ordering_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit ortho_ordering_command(const environment::ptr& e) :
            command(e, "Performs scalable placement and routing of the current logic network in store. "
                       "An FCN layout that is not minimal will be found in reasonable runtime.")
    {
        add_option("--clock_numbers,-n", num_clock_phases, "Number of clock phases to be used {3 or 4}");
        add_option("--hex", hexagonal_tile_shift,
                   "Use hexagonal tiles and specify tile shift. Possible values are 'odd_row', 'even_row', "
                   "'odd_column', or 'even_column'");
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
            ps                   = {};
            hexagonal_tile_shift = "";
            return;
        }
        // error case: phases out of range
        if (num_clock_phases != 3u && num_clock_phases != 4u)
        {
            env->out() << "[e] only 3- and 4-phase clocking schemes are supported" << std::endl;
            ps                   = {};
            hexagonal_tile_shift = "";
            return;
        }

        ps.number_of_clock_phases = num_clock_phases == 3 ? fiction::num_clks::THREE : fiction::num_clks::FOUR;

        if (is_set("hex"))
        {
            if (hexagonal_tile_shift == "odd_row")
            {
                ortho_ordering_physical_design<fiction::hex_odd_row_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "even_row")
            {
                ortho_ordering_physical_design<fiction::hex_even_row_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "odd_column")
            {
                ortho_ordering_physical_design<fiction::hex_odd_col_gate_clk_lyt>();
            }
            else if (hexagonal_tile_shift == "even_column")
            {
                ortho_ordering_physical_design<fiction::hex_even_col_gate_clk_lyt>();
            }
            else
            {
                env->out() << "[e] possible values for the hexagonal tile shift are 'odd_row', 'even_row', "
                              "'odd_column', and 'even_column'"
                           << std::endl;
            }
        }
        else  // Cartesian layout
        {
            ortho_ordering_physical_design<fiction::cart_gate_clk_lyt>();
        }

        ps                   = {};
        hexagonal_tile_shift = "";
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
            {"layout", {{"x-size", st.x_size}, {"y-size", st.y_size}, {"area", st.x_size * st.y_size}}}};
    }

  private:
    /**
     * Tile shift for hexagonal layouts.
     */
    std::string hexagonal_tile_shift{};
    /**
     * Number of clock phases.
     */
    uint8_t num_clock_phases = 4u;
    /**
     * Parameters.
     */
    fiction::orthogonal_physical_design_params ps{};
    /**
     * Statistics.
     */
    fiction::orthogonal_physical_design_stats st{};

    template <typename Lyt>
    void ortho_ordering_physical_design()
    {
        const auto perform_physical_design = [this](auto&& ntk_ptr)
        { return fiction::orthogonal_ordering_network<Lyt>(*ntk_ptr, ps, &st); };

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
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(ortho_ordering, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_ORTHO_ORDERING_HPP
