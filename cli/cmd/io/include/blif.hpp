//
// Created by marcel on 22.07.21.
//

#ifndef FICTION_CMD_BLIF_HPP
#define FICTION_CMD_BLIF_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a BLIF file for the current logic network or gate-level layout in store and writes it to the given path.
 */
class blif_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit blif_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a BLIF file.
     */
    void execute() override;

  private:
    /**
     * File name to write the BLIF file into.
     */
    std::string filename;
    /**
     * Callback to write a BLIF file.
     *
     * @tparam NtkOrLytVariant Variant type holding a logic network or gate-level layout.
     * @param ntk_or_lyt_variant Variant holding a logic network or gate-level layout.
     */
    template <typename NtkOrLytVariant>
    void write_blif_callback(const NtkOrLytVariant& ntk_or_lyt_variant);
};

}  // namespace alice

#endif  // FICTION_CMD_BLIF_HPP
