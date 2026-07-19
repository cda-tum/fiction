//
// Created by Simon Hofmann on 27.04.23.
//

#ifndef FICTION_CMD_HEX_HPP
#define FICTION_CMD_HEX_HPP

#include <fiction/algorithms/physical_design/hexagonalization.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Transforms a 2DDWave-clocked Cartesian layout into a hexagonal one.
 */
class hex_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit hex_command(const environment::ptr& e);

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
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_HEX_HPP
