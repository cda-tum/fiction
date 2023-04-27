//
// Created by Simon Hofmann on 27.04.23.
//

#ifndef FICTION_HEX_HPP
#define FICTION_HEX_HPP

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <variant>

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

        if (is_set("gate_layout"))
        {
            auto& gls = store<fiction::gate_layout_t>();

            if (gls.empty())
            {
                env->out() << "[w] no gate layout in store" << std::endl;
                return;
            }
            apply_hexagonalization<>(gls);
        }
        else
        {
            env->out() << "[w] store must be specified" << std::endl;
        }
    }
  private:
    template <typename Lyt>
    void apply_hexagonalization(Lyt)
    {
        const auto apply_mapping = [this](auto&& gls)
        { return fiction::hexagonalization<Lyt>(*gls); };

        const auto& gls = store<fiction::gate_layout_t>().current();
    }
};

ALICE_ADD_COMMAND(hex, "Physical Design")

}  // namespace alice

#endif  // FICTION_HEX_HPP
