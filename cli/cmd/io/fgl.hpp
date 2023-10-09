//
// Created by simon on 06.10.2023
//

#ifndef FICTION_CMD_FGL_HPP
#define FICTION_CMD_FGL_HPP

#include <fiction/io/write_fgl_layout.hpp>
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
 * Generates a FGL file for the current gate-level layout in store and writes it to the given path.
 */
class fgl_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fgl_command(const environment::ptr& e) :
            command(e, "Generates a FGL file for the current gate-level layout in store.")
    {
        add_option("filename", filename, "FGL file name");
    }

  protected:
    /**
     * Function to perform the output call. Generates a FGL file.
     */
    void execute() override
    {
        auto& s = store<fiction::gate_layout_t>();

        // error case: empty gate-level layout store
        if (s.empty())
        {
            env->out() << "[w] no gate-level layout in store" << std::endl;
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto write_fgl = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::is_gate_level_layout_v<Lyt>)
            {
                fiction::write_fgl_layout(*lyt_ptr, filename);
            }
            else
            {
                env->out() << fmt::format("[e] {} is not a gate-level layout", get_name(lyt_ptr)) << std::endl;
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
        // add .fgl file extension if necessary
        if (std::filesystem::path(filename).extension() != ".fgl")
        {
            filename += ".fgl";
        }

        try
        {
            std::visit(write_fgl, lyt);
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
     * File name to write the FGL file into.
     */
    std::string filename;
};

ALICE_ADD_COMMAND(fgl, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_FGL_HPP
