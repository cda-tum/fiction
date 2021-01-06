//
// Created by marcel on 01.09.20.
//

#ifndef FICTION_NETWORK_READER_H
#define FICTION_NETWORK_READER_H


#include "logic_network.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <mockturtle/io/verilog_reader.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

class network_reader
{
public:
    /**
     * Standard constructor. Reads and constructs logic networks. May throw.
     *
     * @param filename Path to the file or folder of files to read.
     * @param out Output stream to write status updates into.
     */
    network_reader(const std::string& filename, std::ostream& out);
    /**
     * Getter for networks generated from given files.
     *
     * @param sorted Flag to indicate that the networks are to be returned in ascending order of their vertex count.
     * @return Vector of read and constructed logic networks.
     */
    const std::vector<logic_network_ptr>& get_networks(bool sorted = false);
    
private:
    /**
     * Output stream to which status updates are to be written.
     */
    std::ostream& out;
    /**
     * Vector of logic networks read from files given at construction.
     */
    std::vector<logic_network_ptr> networks;
    /**
     * Actual read function that constructs the logic network from a file.
     *
     * @tparam Reader The functor type which constructs the network, i.e. handles the callbacks.
     * @tparam ReadFun The file parser type.
     * @param file The file name.
     * @param rfun The actual parsing function.
     */
    template<class Reader, class ReadFun>
    void read(const std::string& file, ReadFun rfun) noexcept
    {
        auto name = boost::filesystem::path{file}.stem().string();
        logic_network::mig_nt mig;

        try
        {
            if (lorina::diagnostic_engine diag{};
                    rfun(file, Reader{mig}, &diag) == lorina::return_code::success)
            {
                if (mig.is_combinational())
                    networks.push_back(std::make_shared<logic_network>(std::move(mig), std::move(name)));
                else
                    out << "[e] sequential networks are not yet supported for FCN physical design" << std::endl;
            }
            else
            {
                out << "[e] parsing error in " << file << std::endl;
            }
        }
        catch (...)
        {
            out << "[e] " << file << " contains unsupported features" << std::endl;
        }
    }
};


#endif //FICTION_NETWORK_READER_H
