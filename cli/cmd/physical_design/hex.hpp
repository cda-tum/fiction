//
// Created by Simon Hofmann on 27.04.23.
//

#ifndef FICTION_CMD_HEX_HPP
#define FICTION_CMD_HEX_HPP

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <optional>

namespace alice
{
/**
 * Transforms a 2DDWave-clocked Cartesian layout into a hexagonal one.
 */
class hex_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit hex_command(const environment::ptr& e) :
            command(e, "Transforms a 2DDWave-clocked Cartesian layout into a hexagonal one.")
    {
        add_option("--input_mode,-i", ps.input_mode,
                   "Specifies how primary inputs should be handled in the hexagonalization process:\n"
                   " - `0` (none): Do not extend primary inputs to the top row (default).\n"
                   " - `1 (extend): Extend primary inputs to the top row.\n"
                   " - `2` (extend_planar): Extend primary inputs to the top row with planar rerouting (i.e., without "
                   "crossings).",
                   true)
            ->set_type_name("{none=0, extend=1, extend_planar=2}");
        add_flag("--verbose,-v", "Be verbose");
    }

  protected:
    /**
     * Parameters.
     */
    fiction::hexagonalization_params ps{};
    /**
     * Statistics.
     */
    fiction::hexagonalization_stats st{};

    /**
     * Function to transform a 2DDWave-clocked Cartesian layout into a hexagonal one.
     */
    void execute() override
    {
        auto& gls = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (gls.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            ps = {};
            return;
        }

        const auto& lyt = gls.current();

        const auto check_clocking_scheme = [](auto&& lyt_ptr)
        { return lyt_ptr->is_clocking_scheme(fiction::clock_name::TWODDWAVE); };

        // error case: layout is not 2DDWave-clocked
        if (const auto is_twoddwave_clocked = std::visit(check_clocking_scheme, lyt); !is_twoddwave_clocked)
        {
            env->out() << "[e] layout has to be 2DDWave-clocked" << std::endl;
            ps = {};
            return;
        }

        const auto apply_hexagonalization = [&](auto&& lyt_ptr) -> std::optional<fiction::hex_even_row_gate_clk_lyt>
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                return fiction::hexagonalization<fiction::hex_even_row_gate_clk_lyt>(*lyt_ptr, ps, &st);
            }
            else
            {
                ps = {};
                std::cout << "[e] layout has to be Cartesian" << std::endl;
            }

            return std::nullopt;
        };

        try
        {
            if (const auto hex_lyt = std::visit(apply_hexagonalization, lyt); hex_lyt.has_value())
            {
                if (is_set("verbose"))
                {
                    st.report(env->out());
                }
                gls.extend() = std::make_shared<fiction::hex_even_row_gate_clk_lyt>(*hex_lyt);
            }
        }
        catch (const fiction::hexagonalization_route_inputs_error& e)
        {
            ps = {};
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (...)
        {
            ps = {};
            env->out() << "[e] an error occurred while mapping" << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(hex, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_HEX_HPP
