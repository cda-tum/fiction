//
// Created by marcel on 06.02.23.
//

#ifndef FICTION_CMD_VERILOG_HPP
#define FICTION_CMD_VERILOG_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a Verilog file for the current logic network or gate-level layout in store and writes it to the given path.
 */
class verilog_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit verilog_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a Verilog file.
     */
    void execute() override;

  private:
    /**
     * File name to write the Verilog file into.
     */
    std::string filename;

    template <typename NtkOrLytVariant>
    void write_verilog_callback(const NtkOrLytVariant& ntk_or_lyt_variant);
};

}  // namespace alice

#endif  // FICTION_CMD_VERILOG_HPP
