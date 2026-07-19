//
// Created by simon on 06.10.2023
//

#ifndef FICTION_CMD_FGL_HPP
#define FICTION_CMD_FGL_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a FGL file for the current gate-level layout in store and writes it to the given path.
 */
class fgl_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fgl_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a FGL file.
     */
    void execute() override;

  private:
    /**
     * File name to write the FGL file into.
     */
    std::string filename;
};

}  // namespace alice

#endif  // FICTION_CMD_FGL_HPP
