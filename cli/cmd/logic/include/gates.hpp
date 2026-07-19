//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_GATES_HPP
#define FICTION_CMD_GATES_HPP

#include <alice/alice.hpp>

namespace alice
{

/**
 * Prints numbers of gates for a current logic network.
 */
class gates_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit gates_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the gates call. Outputs gates counts for each node type.
     */
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_GATES_HPP
