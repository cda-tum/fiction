//
// Created by marcel on 07.06.23.
//

#ifndef FICTION_CMD_MIGINVOPT_HPP
#define FICTION_CMD_MIGINVOPT_HPP

#include <alice/alice.hpp>

namespace alice
{

/**
 * Optimizes inverter costs of the current MIG in store.
 */
class miginvopt_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit miginvopt_command(const environment::ptr& e);

  protected:
    /**
     * Executes inverter cost optimization on the current MIG and pushes the result to the logic-network store.
     */
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_MIGINVOPT_HPP
