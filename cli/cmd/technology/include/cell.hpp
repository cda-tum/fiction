//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_CELL_HPP
#define FICTION_CMD_CELL_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{
/**
 * Converts a gate-level layout to a cell-level layout. The conversion process allows to choose from supported FCN gate
 * libraries.
 */
class cell_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit cell_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the conversion call. Generates a cell-level layout.
     */
    void execute() override;

  private:
    /**
     * Identifier of gate library to use.
     */
    std::string library = "QCA-ONE";
    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags();
};

}  // namespace alice

#endif  // FICTION_CMD_CELL_HPP
