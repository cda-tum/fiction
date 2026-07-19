//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_VERSION_HPP
#define FICTION_CMD_VERSION_HPP

#include <alice/alice.hpp>

namespace alice
{

/**
 * Outputs version and build information.
 */
class version_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit version_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the version print call.
     */
    void execute() override;
};

}  // namespace alice

#endif  // FICTION_CMD_VERSION_HPP
