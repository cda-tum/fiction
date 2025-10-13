//
// Created by Simon Hofmann on 27.04.23.
//

#include "cmd/physical_design/include/hex.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <memory>
#include <optional>
#include <type_traits>
#include <variant>

namespace alice
{

hex_command::hex_command(const environment::ptr& e) :
        command(e, "Transforms a 2DDWave-clocked Cartesian layout into a hexagonal one.")
{
    add_flag("--input_pin_extension,-i", "Extend primary input pins to the resulting layout's top row");
    add_flag("--output_pin_extension,-o", "Extend primary output pins to the resulting layout's bottom row");
    add_flag("--planar,-p", "Extend primary input and/or output pins with planar rerouting (i.e., without "
                            "crossings) in the resulting layout");
    add_flag("--verbose,-v", "Be verbose");
}

void hex_command::execute()
{
    auto& gls = store<fiction::gate_layout_t>();

    // error case: empty gate-level layout store
    if (gls.empty())
    {
        env->out() << "[w] no gate layout in store\n";
        ps = {};
        return;
    }

    const auto& lyt = gls.current();

    const auto check_clocking_scheme_2ddwave = [](auto&& lyt_ptr)
    { return lyt_ptr->is_clocking_scheme(fiction::clock_name::TWODDWAVE); };

    // error case: layout is not 2DDWave-clocked
    if (const auto is_2ddwave_clocked = std::visit(check_clocking_scheme_2ddwave, lyt); !is_2ddwave_clocked)
    {
        env->out() << "[e] layout has to be 2DDWave-clocked\n";
        ps = {};
        return;
    }

    if (this->is_set("planar") && !(this->is_set("input_pin_extension") || this->is_set("output_pin_extension")))
    {
        env->out()
            << "[w] planar rerouting is only possible when extending inputs and/or outputs (using -i and/or -o)\n";
    }

    if (this->is_set("input_pin_extension"))
    {
        if (this->is_set("planar"))
        {
            ps.input_pin_extension = fiction::hexagonalization_params::io_pin_extension_mode::EXTEND_PLANAR;
        }
        else
        {
            ps.input_pin_extension = fiction::hexagonalization_params::io_pin_extension_mode::EXTEND;
        }
    }

    if (this->is_set("output_pin_extension"))
    {
        if (this->is_set("planar"))
        {
            ps.output_pin_extension = fiction::hexagonalization_params::io_pin_extension_mode::EXTEND_PLANAR;
        }
        else
        {
            ps.output_pin_extension = fiction::hexagonalization_params::io_pin_extension_mode::EXTEND;
        }
    }

    const auto apply_hexagonalization = [this](auto&& lyt_ptr) -> std::optional<fiction::hex_even_row_gate_clk_lyt>
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::is_cartesian_layout_v<Lyt>)
        {
            return fiction::hexagonalization<fiction::hex_even_row_gate_clk_lyt>(*lyt_ptr, ps, &st);
        }
        else
        {
            ps = {};
            env->out() << "[e] layout has to be Cartesian\n";
        }

        return std::nullopt;
    };

    try
    {
        if (const auto hex_lyt = std::visit(apply_hexagonalization, lyt); hex_lyt.has_value())
        {
            ps = {};
            if (is_set("verbose"))
            {
                st.report(env->out());
            }
            gls.extend() = std::make_shared<fiction::hex_even_row_gate_clk_lyt>(*hex_lyt);
        }
    }
    catch (const fiction::hexagonalization_io_pin_routing_error& e)
    {
        ps = {};
        env->out() << fmt::format("[e] {}\n", e.what());
    }
    catch (...)
    {
        ps = {};
        env->out() << "[e] an error occurred while mapping\n";
    }
}

}  // namespace alice
