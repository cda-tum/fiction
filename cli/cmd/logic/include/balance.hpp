//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_BALANCE_HPP
#define FICTION_CMD_BALANCE_HPP

#include <fiction/algorithms/network_transformation/network_balancing.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Creates a new technology network of the current logic network in store and performs a path balancing.
 */
class balance_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit balance_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the balancing call. Inserts buffer nodes to equalize path lengths.
     */
    void execute() override;

  private:
    /**
     * Parameters.
     */
    fiction::network_balancing_params ps{};
};

}  // namespace alice

#endif  // FICTION_CMD_BALANCE_HPP
