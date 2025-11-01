//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_QCC_HPP
#define FICTION_CMD_QCC_HPP

#include <fiction/io/write_qcc_layout.hpp>

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a ToPoliNano/MagCAD component file for the current cell layout in store and writes it to the given path.
 *
 * ToPoliNano and MagCAD are available at: https://topolinano.polito.it/
 */
class qcc_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qcc_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a ToPoliNano/MagCAD component file.
     */
    void execute() override;

  private:
    /**
     * File name to write the QCC file into.
     */
    std::string filename;

    fiction::write_qcc_layout_params ps{};
};

}  // namespace alice

#endif  // FICTION_CMD_QCC_HPP
