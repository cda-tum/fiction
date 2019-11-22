//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CELL_H
#define FICTION_CELL_H


#include "qca_one_library.h"
#include "topolinano_library.h"
#include "fcn_cell_layout.h"
#include <alice/alice.hpp>
#include <string>


namespace alice
{
/**
     * Converts an fcn_gate_layout to an fcn_cell_layout. Conversion process allows to choose from supported FCN gate
     * libraries.
     */
    class cell_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit cell_command(const environment::ptr& env)
                :
                command(env,
                        "Compiles the current gate layout in store down to a cell-level layout.\nA gate library must be "
                        "specified in order to instruct the algorithm how to map gate tiles to cell blocks.")
        {
            add_option("--library,-l", library,
                       "Gate library to use for mapping", true)->set_type_name("{QCA-ONE=0, ToPoliNano=1}");
        }

    protected:
        /**
         * Function to perform the conversion call. Generates an fcn_cell_layout.
         */
        void execute() override
        {
            auto& s = store<fcn_gate_layout_ptr>();

            // error case: empty gate layout store
            if (s.empty())
            {
                env->out() << "[w] no gate layout in store" << std::endl;
                reset_flags();
                return;
            }

            auto fgl = s.current();
            fcn_gate_library_ptr lib = nullptr;
            std::string lib_name{};
            try
            {
                if (library == 0u)
                {
                    // QCA-ONE only allows non-shifted layouts
                    if (fgl->is_vertically_shifted())
                    {
                        env->out() << "[e] non-shifted layouts are required for this library" << std::endl;
                        reset_flags();
                        return;
                    }

                    lib = std::make_shared<qca_one_library>(fgl);
                }
                else if (library == 1u)
                {
                    // ToPoliNano only allows vertically shifted layouts
                    if (!fgl->is_vertically_shifted())
                    {
                        env->out() << "[e] vertically shifted layouts are required for this library" << std::endl;
                        reset_flags();
                        return;
                    }
                    // ToPoliNano only allows layouts clocked with a ToPoliNano clocking
                    if (!fgl->is_clocking("TOPOLINANO3") && !fgl->is_clocking("TOPOLINANO4"))
                    {
                        env->out() << "[e] a ToPoliNano clocking is required for this library" << std::endl;
                        reset_flags();
                        return;
                    }

                    lib = std::make_shared<topolinano_library>(fgl);
                }
                    // more libraries go here
                else
                {
                    env->out() << "[e] identifier " << library << " does not refer to a supported gate library"
                               << std::endl;
                    reset_flags();
                    return;
                }

                lib_name = lib->get_name();
            }
            catch (...)
            {
                env->out() << "[e] could not assign directions in " << s.current()->get_name() << " to cell ports"
                           << std::endl;
                reset_flags();
                return;
            }

            try
            {
                auto fcl = std::make_shared<fcn_cell_layout>(std::move(lib));

                // store new layout
                store<fcn_cell_layout_ptr>().extend() = std::move(fcl);
            }
            catch (...)
            {
                env->out() << "[e] mapping " << s.current()->get_name() << " to cell-level using the "
                           << lib_name << " library was not successful" << std::endl;
                reset_flags();
                return;
            }

            reset_flags();
        }

    private:
        /**
         * Identifier of gate library to use.
         */
        unsigned library = 0u;

        /**
         * Reset all flags. Necessary for some reason... alice bug?
         */
        void reset_flags()
        {
            library = 0u;
        }
    };

    ALICE_ADD_COMMAND(cell, "Mapping")
}


#endif //FICTION_CELL_H
