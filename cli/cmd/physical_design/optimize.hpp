//
// Created by Simon Hofmann on 02.08.23.
//

#ifndef FICTION_CMD_OPTIMIZE_HPP
#define FICTION_CMD_OPTIMIZE_HPP

#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <iostream>
#include <optional>
#include <variant>

namespace alice
{
/**
 * Optimizes a 2DDWave-clocked Cartesian layout.
 */
class optimize_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit optimize_command(const environment::ptr& e) :
            command(e, "Optimizes a 2DDWave-clocked Cartesian layout with respect to area. It achieves this objective "
                       "by strategically repositioning gates within the layout, removing excess wiring, and "
                       "effectively relocating outputs to more favorable positions.")
    {}

  protected:
    /**
     * Statistics.
     */
    fiction::post_layout_optimization_stats st{};

    /**
     * Optimizes a 2DDWave-clocked Cartesian layout.
     */
    void execute() override
    {
        auto& gls = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (gls.empty())
        {
            env->out() << "[w] no gate layout in store" << std::endl;
            return;
        }

        const auto& lyt = gls.current();

        const auto check_clocking_scheme = [](auto&& lyt_ptr)
        { return lyt_ptr->is_clocking_scheme(fiction::clock_name::TWODDWAVE); };

        // error case: layout is not 2DDWave-clocked
        if (const auto is_twoddwave_clocked = std::visit(check_clocking_scheme, lyt); !is_twoddwave_clocked)
        {
            env->out() << "[e] layout has to be 2DDWave-clocked" << std::endl;
            return;
        }

        const auto apply_optimization = [&](auto&& lyt_ptr)
        {
            using Lyt               = typename std::decay_t<decltype(lyt_ptr)>::element_type;
            auto       lyt_copy     = lyt_ptr->clone();
            const auto lyt_copy_ptr = std::make_shared<Lyt>(lyt_copy);

            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                fiction::post_layout_optimization(*lyt_copy_ptr, &st);
                fiction::restore_names(*lyt_ptr, *lyt_copy_ptr);
                gls.extend() = lyt_copy_ptr;
            }
            else
            {
                std::cout << "[e] layout has to be Cartesian" << std::endl;
            }
        };

        try
        {
            std::visit(apply_optimization, lyt);
        }
        catch (...)
        {
            env->out() << "[e] an error occurred while optimizing" << std::endl;
        }
    }
};

ALICE_ADD_COMMAND(optimize, "Physical Design")

}  // namespace alice

#endif  // FICTION_CMD_OPTIMIZE_HPP
