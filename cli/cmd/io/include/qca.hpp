//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_QCA_HPP
#define FICTION_CMD_QCA_HPP

#include <fiction/io/write_qca_layout.hpp>

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a QCADesigner file for the current cell layout in store and writes it to the given path.
 *
 * QCADesigner is available at: https://waluslab.ece.ubc.ca/qcadesigner/
 */
class qca_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qca_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a QCADesigner file.
     */
    void execute() override;

  private:
    /**
     * File name to write the QCA file into.
     */
    std::string filename;

    fiction::write_qca_layout_params ps{};
};

}  // namespace alice

#endif  // FICTION_CMD_QCA_HPP
