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
    {}

  protected:
    /**
     * Function to transform a 2DDWave-clocked Cartesian layout into a hexagonal one.
     */
    void execute() override
    {
        hexagonalization<fiction::cart_gate_clk_lyt>();
    }
  private:
    template <typename Lyt>
    void hexagonalization()
    {
        auto& gls = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (gls.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            return;
        }

        const auto& lyt_ptr = gls.current();

        //if (lyt_ptr.is_clocking_scheme(fiction::clock_name::TWODDWAVE))
        //{
        //    env->out() << "[e] Layout has to be 2DDWave-clocked" << std::endl;
        //    return;
        //}

        const auto apply_hexagonalization = [](auto&& lyt_ptr) { return fiction::hexagonalization(*lyt_ptr); };

        const auto lyt = std::visit(apply_hexagonalization, lyt_ptr);
        gls.extend()   = std::make_shared<fiction::hex_even_row_gate_clk_lyt>(lyt);

    }
};

ALICE_ADD_COMMAND(hex, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_HEX_HPP
