//
// Created by Simon Hofmann on 02.08.23.
//

#ifndef FICTION_CMD_OPTIMIZE_HPP
#define FICTION_CMD_OPTIMIZE_HPP

#include <fiction/physical_design/post_layout_optimization.hpp>
#include <fiction/physical_design/wiring_reduction.hpp>

#include <alice/alice.hpp>

#include <cstdint>

namespace alice
{

/**
 * Optimizes a 2DDWave-clocked Cartesian layout.
 */
class optimize_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit optimize_command(const environment::ptr& e);

  protected:
    /**
     * Maximum number of relocations to try for each gate.
     */
    uint64_t max_gate_relocations{0ull};
    /**
     * Post-layout optimization parameters.
     */
    fiction::physical_design::post_layout_optimization_params ps{};
    /**
     * Wiring reduction parameters.
     */
    fiction::physical_design::wiring_reduction_params psw{};
    /**
     * Post-layout optimization statistics.
     */
    fiction::physical_design::post_layout_optimization_stats st{};
    /**
     * Wiring reduction statistics.
     */
    fiction::physical_design::wiring_reduction_stats stw{};

    /**
     * Optimizes a 2DDWave-clocked Cartesian layout.
     */
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_OPTIMIZE_HPP
