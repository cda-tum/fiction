//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_QLL_HPP
#define FICTION_CMD_QLL_HPP

#include <alice/alice.hpp>

#include <string>

namespace alice
{

/**
 * Generates a ToPoliNano/MagCAD layout file for the current cell layout in store and writes it to the given path.
 *
 * ToPoliNano and MagCAD are available at: https://topolinano.polito.it/
 */
class qll_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qll_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the output call. Generates a ToPoliNano/MagCAD layout file.
     */
    void execute() override;

  private:
    /**
     * File name to write the QLL file into.
     */
    std::string filename;
};

}  // namespace alice

#endif  // FICTION_CMD_QLL_HPP
