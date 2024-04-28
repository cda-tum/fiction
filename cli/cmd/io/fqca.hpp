//
// Created by marcel on 24.09.21.
//

#ifndef FICTION_FQCA_HPP
#define FICTION_FQCA_HPP

#include <fiction/io/write_fqca_layout.hpp>
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
 * Generates a QCA-STACK file for the current cell layout in store and writes it to the given path.
 *
 * QCA-STACK is available at: https://github.com/wlambooy/QCA-STACK
 */
class fqca_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit fqca_command(const environment::ptr& e) :
            command(e, "Generates a QCA-STACK file for the current QCA cell layout in store. "
                       "QCA-STACK can be used to perform discrete simulations and layout stacking.")
    {
        add_option("filename", filename, "FQCA file name");
        add_flag("--via_layers,-v", ps.create_inter_layer_via_cells, "Add additional inter-layer via cells");
    }

  protected:
    /**
     * Function to perform the output call. Generates a QCA-STACK file.
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

        const auto write_fqca = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_qca_technology_v<Lyt>)
            {
                fiction::write_fqca_layout(*lyt_ptr, filename, ps);
            }
            else
            {
                env->out() << fmt::format("[e] {}'s cell technology is not QCA but {}", get_name(lyt_ptr),
                                          fiction::tech_impl_name<fiction::technology<Lyt>>)
                           << std::endl;
            }
        };

        auto lyt = s.current();

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
        if (std::filesystem::path(filename).extension() != ".fqca")
        {
            filename += ".fqca";
        }

        try
        {
            std::visit(write_fqca, lyt);
        }
        catch (const fiction::out_of_cell_names_exception&)
        {
            env->out() << "[e] layout contains more named cells than QCA-STACK's file format supports" << std::endl;
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
     * File name to write the FQCA file into.
     */
    std::string filename;

    fiction::write_fqca_layout_params ps{};
};

ALICE_ADD_COMMAND(fqca, "I/O")

}  // namespace alice

#endif  // FICTION_FQCA_HPP
