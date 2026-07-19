//
// Created by marcel on 08.06.21.
//

#ifndef FICTION_CMD_MAP_HPP
#define FICTION_CMD_MAP_HPP

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>

#include <alice/alice.hpp>

namespace alice
{

/**
 * Performs technology mapping to transform a network into another one using specific gates.
 */
class map_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit map_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the map call. Generates a logic network from another one.
     */
    void execute() override;

  private:
    /**
     * Technology mapping parameters.
     */
    fiction::technology_mapping_params ps{};
};

}  // namespace alice

#endif  // FICTION_CMD_MAP_HPP
