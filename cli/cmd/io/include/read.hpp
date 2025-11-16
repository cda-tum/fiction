//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_READ_HPP
#define FICTION_CMD_READ_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Shorthand for all read commands. Chooses the proper function by the file ending.
 *
 * Currently parses Verilog, AIGER, and BLIF using the lorina parsers.
 *
 * For more information see: https://github.com/hriener/lorina
 *
 * Parses FGL, SQD, and FQCA via custom reader functions.
 */
class read_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit read_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the read call. Reads a network or layout from a file.
     */
    void execute() override;

  private:
    /**
     * Verilog filename.
     */
    std::string filename;
    /**
     * Gate-level layout topology.
     */
    std::string topology;
    /**
     * Flag to indicate that files should be sorted by file size.
     */
    bool sort = false;
    /**
     * Identifier of H-Si lattice orientation.
     */
    std::string orientation{"100"};
};

}  // namespace alice

#endif  // FICTION_CMD_READ_HPP
