//
// Created by marcel on 14.07.21.
//

#ifndef FICTION_SQD_HPP
#define FICTION_SQD_HPP

#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <filesystem>
#include <ostream>
#include <string>
#include <type_traits>
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

        const auto get_name = [](auto&& lyt) -> std::string { return lyt->get_layout_name(); };

        const auto write_sqd = [this, &get_name](auto&& lyt)
        {
            using Lyt = typename std::decay_t<decltype(lyt)>::element_type;

            if constexpr (std::is_same_v<fiction::technology<Lyt>, fiction::qca_technology> ||
                          std::is_same_v<fiction::technology<Lyt>, fiction::sidb_technology>)
            {
                fiction::write_sqd_layout(*lyt, filename);
            }
            else
            {
                env->out() << fmt::format("[e] {}'s cell technology is not QCA or SiDB but {}", get_name(lyt),
                                          fiction::tech_impl_name<fiction::technology<Lyt>>)
                           << std::endl;
            }
        };

        auto lyt = s.current();

        // error case: do not override directories
        if (std::filesystem::is_directory(filename))
        {
            env->out() << "[e] cannot override a directory" << std::endl;
            return;
        }
        // if filename was not given, use stored layout name
        if (!is_set("filename"))
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

#endif  // FICTION_SQD_HPP
