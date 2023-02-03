//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_QCC_HPP
#define FICTION_CMD_QCC_HPP

#include <fiction/io/write_qcc_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
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
 * Generates a ToPoliNano/MagCAD component file for the current cell layout in store and writes it to the given path.
 *
 * ToPoliNano and MagCAD are available at: https://topolinano.polito.it/
 */
class qcc_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qcc_command(const environment::ptr& e) :
            command(e, "Generates a ToPoliNano/MagCAD component file for the current iNML cell layout in store. "
                       "ToPoliNano can reuse the component in hierarchical layouts while MagCAD is able to "
                       "perform physical simulations.")
    {
        add_option("filename", filename, "QCC file name");
        add_flag("-c,--component_name", ps.use_filename_as_component_name,
                 "Use given file name as the component's identifier");
    }

  protected:
    /**
     * Function to perform the output call. Generates a ToPoliNano/MagCAD component file.
     */
    void execute() override
    {
        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;

            ps = {};
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto write_qcc = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_inml_technology_v<Lyt>)
            {
                fiction::write_qcc_layout(*lyt_ptr, filename, ps);
            }
            else
            {
                env->out() << fmt::format("[e] {}'s cell technology is not iNML but {}", get_name(lyt_ptr),
                                          fiction::tech_impl_name<fiction::technology<Lyt>>)
                           << std::endl;
            }
        };

        const auto& lyt = s.current();

        // error case: do not override directories
        if (std::filesystem::is_directory(filename))
        {
            env->out() << "[e] cannot override a directory" << std::endl;

            ps = {};
            return;
        }
        // if filename was not given, use stored layout name
        if (filename.empty())
        {
            filename = std::visit(get_name, lyt);
        }
        // add .qca file extension if necessary
        if (std::filesystem::path(filename).extension() != ".qcc")
        {
            filename += ".qcc";
        }

        try
        {
            std::visit(write_qcc, lyt);
        }
        catch (const std::ofstream::failure& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (const std::invalid_argument& e)
        {
            env->out() << "[e] " << e.what() << std::endl;
        }
        catch (...)
        {
            env->out() << "[e] an error occurred while the file was being written; it could be corrupted" << std::endl;
        }

        ps = {};
    }

  private:
    /**
     * File name to write the QCC file into.
     */
    std::string filename;

    fiction::write_qcc_layout_params ps{};
};

ALICE_ADD_COMMAND(qcc, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_QCC_HPP
