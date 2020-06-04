//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_QCA_H
#define FICTION_QCA_H


#include "qca_writer.h"
#include "fcn_cell_layout.h"
#include <alice/alice.hpp>
#include <boost/filesystem.hpp>
#include <string>


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
         * @param env alice::environment that specifies stores etc.
         */
        explicit qca_command(const environment::ptr& env)
                :
                command(env, "Generates a QCADesigner file for the current QCA cell layout in store. "
                             "QCADesigner can be used to perform physical simulations.")
        {
            add_option("filename", filename,
                       "QCA file name");
        }

    protected:
        /**
         * Function to perform the output call. Generates a QCADesigner file.
         */
        void execute() override
        {
            auto& s = store<fcn_cell_layout_ptr>();

            // error case: empty cell layout store
            if (s.empty())
            {
                env->out() << "[w] no cell layout in store" << std::endl;
                return;
            }

            auto fcl = s.current();
            if (auto tech = fcl->get_technology(); tech != fcn::technology::QCA)
            {
                env->out() << "[w] " << fcl->get_name() << "'s cell technology is not QCA but " << tech << std::endl;
                return;
            }

            // error case: do not override directories
            if (boost::filesystem::is_directory(filename))
            {
                env->out() << "[e] cannot override a directory" << std::endl;
                return;
            }
            // if filename was empty or not given, use stored layout name
            if (filename.empty())
                filename = fcl->get_name();
            // add .qca file extension if necessary
            if (boost::filesystem::extension(filename) != ".qca")
                filename += ".qca";

            try
            {
                qca::write(std::move(fcl), filename);
            }
            catch (...)
            {
                env->out() << "[e] an error occurred while the file was being written; it could be corrupted" << std::endl;
            }
        }

    private:
        /**
         * File name to write the QCA file into.
         */
        std::string filename;
    };

    ALICE_ADD_COMMAND(qca, "I/O")
}


#endif //FICTION_QCA_H
