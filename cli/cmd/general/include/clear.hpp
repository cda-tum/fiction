//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_CLEAR_HPP
#define FICTION_CMD_CLEAR_HPP

#include <alice/alice.hpp>

namespace alice
{

/**
 * Clears all stores by removing all their entries.
 */
class clear_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit clear_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the clear call. Calls alice's clear function for all stores.
     */
    void execute() override;

  private:
    /**
     * Stores to clear.
     */
    bool cell = false, gate = false, network = false, table = false;
};

}  // namespace alice

#endif  // FICTION_CMD_CLEAR_HPP
