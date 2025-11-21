//
// Created by marcel on 07.06.23.
//

#ifndef FICTION_CMD_MIGINVPROP_HPP
#define FICTION_CMD_MIGINVPROP_HPP

#include <alice/alice.hpp>

namespace alice
{

/**
 * Propagates inverting edges in the current MIG to the primary inputs to reduce inverter cost (may duplicate logic).
 */
class miginvprop_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit miginvprop_command(const environment::ptr& e);

  protected:
    /**
     * Executes inverter propagation on the current MIG and pushes the result to the logic-network store.
     */
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_MIGINVPROP_HPP
