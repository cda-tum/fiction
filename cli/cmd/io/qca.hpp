//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_QCA_HPP
#define FICTION_CMD_QCA_HPP

#include <fiction/io/write_qca_layout.hpp>
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
 * Generates a QCADesigner file for the current cell layout in store and writes it to the given path.
 *
 * QCADesigner is available at: https://waluslab.ece.ubc.ca/qcadesigner/
 */
class qca_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit qca_command(const environment::ptr& e) :
            command(e, "Generates a QCADesigner file for the current QCA cell layout in store. "
                       "QCADesigner can be used to perform physical simulations.")
    {
        add_option("filename", filename, "QCA file name");
        add_flag("--no_via_layers,-v", "Do not insert additional inter-layer via cells");
    }

  protected:
    /**
     * Function to perform the output call. Generates a QCADesigner file.
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

        if (is_set("no_via_layers"))
        {
            ps.create_inter_layer_via_cells = false;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto write_qca = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_qca_technology_v<Lyt>)
            {
                fiction::write_qca_layout(*lyt_ptr, filename, ps);
            }
            else
            {
                env->out() << fmt::format("[e] {}'s cell technology is not QCA but {}", get_name(lyt_ptr),
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
        if (std::filesystem::path(filename).extension() != ".qca")
        {
            filename += ".qca";
        }

        try
        {
            std::visit(write_qca, lyt);
        }
        catch (const std::ofstream::failure& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
        catch (...)
        {
            env->out() << "[e] an error occurred while the file was being written; it could be corrupted" << std::endl;
        }

        ps = {};
    }

  private:
    /**
     * File name to write the QCA file into.
     */
    std::string filename;

    fiction::write_qca_layout_params ps{};
};

ALICE_ADD_COMMAND(qca, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_QCA_HPP
