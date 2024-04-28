//
// Created by marcel on 14.07.21.
//

#ifndef FICTION_CMD_SQD_HPP
#define FICTION_CMD_SQD_HPP

#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <filesystem>
#include <ostream>
#include <string>
#include <variant>

namespace alice
{
/**
 * Generates a SiQAD file for the current cell layout in store and writes it to the given path.
 *
 * SiQAD is available at: https://waluslab.ece.ubc.ca/siqad/
 */
class sqd_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit sqd_command(const environment::ptr& e) :
            command(e, "Generates a SiQAD file for the current QCA or SiDB cell layout in store. "
                       "SiQAD can be used to perform physical simulations.")
    {
        add_option("filename", filename, "SQD file name");
    }

  protected:
    /**
     * Function to perform the output call. Generates a SiQAD file.
     */
    void execute() override
    {
        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto write_sqd = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_qca_technology_v<Lyt> || fiction::has_sidb_technology_v<Lyt>)
            {
                fiction::write_sqd_layout(*lyt_ptr, filename);
            }
            else
            {
                env->out() << fmt::format("[e] {}'s cell technology is not QCA or SiDB but {}", get_name(lyt_ptr),
                                          fiction::tech_impl_name<fiction::technology<Lyt>>)
                           << std::endl;
            }
        };

        const auto& lyt = s.current();

        // error case: do not override directories
        if (std::filesystem::is_directory(filename))
        {
            env->out() << "[e] cannot override a directory" << std::endl;
            return;
        }
        // if filename was not given, use stored layout name
        if (filename.empty())
        {
            filename = std::visit(get_name, lyt);
        }
        // add .sqd file extension if necessary
        if (std::filesystem::path(filename).extension() != ".sqd")
        {
            filename += ".sqd";
        }

        try
        {
            std::visit(write_sqd, lyt);
        }
        catch (const std::ofstream::failure& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (...)
        {
            env->out() << "[e] an error occurred while the file was being written; it could be corrupted" << std::endl;
        }
    }

  private:
    /**
     * File name to write the SQD file into.
     */
    std::string filename;
};

ALICE_ADD_COMMAND(sqd, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_SQD_HPP
