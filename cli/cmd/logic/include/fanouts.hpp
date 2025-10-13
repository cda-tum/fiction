//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_FANOUTS_HPP
#define FICTION_CMD_FANOUTS_HPP

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>

#include <alice/alice.hpp>

#include <cstdint>

namespace alice
{

/**
 * Substitutes multi-outputs of gate vertices and replaces them with fan-out ones.
 */
class fanouts_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fanouts_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the fanouts call. Substitutes high-degree outputs and replaces them by fan-out vertices.
     */
    void execute() override;

  private:
    /**
     * Random seed used for random fanout substitution
     */
    uint32_t seed{0u};
    /**
     * Parameters.
     */
    fiction::fanout_substitution_params ps{};
};

}  // namespace alice

#endif  // FICTION_CMD_FANOUTS_HPP
