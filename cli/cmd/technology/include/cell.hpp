//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_CELL_HPP
#define FICTION_CMD_CELL_HPP

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_ports.hpp>
#include <fiction/technology/fcn_gate_library.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>

#include <optional>
#include <string>
#include <type_traits>

namespace alice
{

/**
 * Converts a gate-level layout to a cell-level layout. The conversion process allows to choose from supported FCN gate
 * libraries.
 */
class cell_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit cell_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the conversion call. Generates a cell-level layout.
     */
    void execute() override;

  private:
    /**
     * Identifier of gate library to use.
     */
    std::string library = "QCA-ONE";
    /**
     * Helper function to apply a gate library with error handling.
     * Wraps the apply function call in a try/catch block and handles exceptions.
     *
     * @tparam ApplyFunc Type of the apply function.
     * @tparam Source Type of the source gate layout variant.
     * @param apply_func The function that applies the gate library.
     * @param source The source gate layout variant.
     * @return Optional containing the result on success, nullopt on error.
     */
    template <typename ApplyFunc, typename Source>
    auto apply_with_error_handling(ApplyFunc&& apply_func, Source&& source) const
        -> std::optional<std::decay_t<decltype(apply_func(source))>>
    {
        try
        {
            return std::forward<ApplyFunc>(apply_func)(std::forward<Source>(source));
        }
        catch (const fiction::unsupported_gate_type_exception<fiction::offset::ucoord_t>& e)
        {
            env->out() << fmt::format("[e] unsupported gate type at tile position {}\n", e.where());
        }
        catch (
            const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t, fiction::port_position>& e)
        {
            env->out() << fmt::format("[e] unsupported gate orientation at tile position {} with ports {}\n", e.where(),
                                      e.which_ports());
        }
        catch (const fiction::unsupported_gate_orientation_exception<fiction::offset::ucoord_t,
                                                                     fiction::port_direction>& e)
        {
            env->out() << fmt::format("[e] unsupported gate orientation at tile position {} with port directions {}\n",
                                      e.where(), e.which_ports());
        }

        return std::nullopt;
    }
    /**
     * Reset all flags. Necessary for some reason... alice bug?
     */
    void reset_flags();
};

}  // namespace alice

#endif  // FICTION_CMD_CELL_HPP
