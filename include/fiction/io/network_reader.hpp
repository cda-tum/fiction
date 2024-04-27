//
// Created by marcel on 01.09.20.
//

#ifndef FICTION_NETWORK_READER_HPP
#define FICTION_NETWORK_READER_HPP

#include <lorina/aiger.hpp>
#include <lorina/blif.hpp>
#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/verilog.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/io/blif_reader.hpp>
#include <mockturtle/io/verilog_reader.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace fiction
{
/**
 * Helper class to read directories of mockturtle networks of certain types.
 *
 * @tparam NtkPtr Pointer type to a logic network.
 */
template <typename NtkPtr>
class network_reader
{
  public:
    /**
     * Standard constructor. Reads and constructs logic networks. May throw.
     *
     * @param filename Path to the file or folder of files to read.
     * @param out Output stream to write status updates into.
     */
    network_reader(const std::string_view& filename, std::ostream& o) : out{o}
    {
        constexpr const char* verilog_ext = ".v";
        constexpr const char* aig_ext     = ".aig";
        constexpr const char* blif_ext    = ".blif";

        constexpr const std::array<const char*, 3> extensions{{verilog_ext, aig_ext, blif_ext}};

        // checks for extension validity
        auto is_valid_extension = [&](const auto& p) -> bool
        {
            return std::any_of(extensions.cbegin(), extensions.cend(),
                               [&p](const auto& valid) { return std::filesystem::path(p).extension() == valid; });
        };

        std::vector<std::string> paths{};

        // check for given file's properties
        if (std::filesystem::exists(filename))
        {
            if (std::filesystem::is_regular_file(filename))
            {
                // collect valid files only
                if (is_valid_extension(filename))
                {
                    paths.emplace_back(filename.data());
                }
            }

            else if (std::filesystem::is_directory(filename))
            {
                // iterate over each file in the directory
                for (const auto& file : std::filesystem::directory_iterator(filename))
                {
                    if (std::filesystem::is_regular_file(file))
                    {
                        // collect valid files only
                        if (is_valid_extension(file))
                        {
                            paths.push_back(file.path().string());
                        }
                    }
                }
            }
            else
            {  // existing file but a weird one
                out << "[e] given file name does not point to a regular file" << std::endl;
            }
        }
        else
        {  // path leads nowhere
            out << "[e] given file name does not exist" << std::endl;
        }

        // handle collected files
        for (const auto& p : paths)
        {
            // parse Verilog
            if (std::filesystem::path(p).extension() == verilog_ext)
            {
                read<mockturtle::verilog_reader<Ntk>,
                     lorina::return_code(const std::string&, const lorina::verilog_reader&,
                                         lorina::diagnostic_engine*)>(p, lorina::read_verilog);
            }
            // parse AIGER
            else if (std::filesystem::path(p).extension() == aig_ext)
            {
                read<mockturtle::aiger_reader<Ntk>,
                     lorina::return_code(const std::string&, const lorina::aiger_reader&, lorina::diagnostic_engine*)>(
                    p, lorina::read_aiger);
            }
            // parse BLIF
            else if (std::filesystem::path(p).extension() == blif_ext)
            {
                if constexpr (std::is_same_v<typename Ntk::base_type, mockturtle::aig_network>)
                {
                    out << "[e] AIGs do not support the full feature set of BLIF files" << std::endl;
                }
                else if constexpr (std::is_same_v<typename Ntk::base_type, mockturtle::xag_network>)
                {
                    out << "[e] XAGs do not support the full feature set of BLIF files" << std::endl;
                }
                else if constexpr (std::is_same_v<typename Ntk::base_type, mockturtle::mig_network>)
                {
                    out << "[e] MIGs do not support the full feature set of BLIF files" << std::endl;
                }
                else
                {
                    read<mockturtle::blif_reader<Ntk>,
                         lorina::return_code(const std::string&, const lorina::blif_reader&,
                                             lorina::diagnostic_engine*)>(p, lorina::read_blif);
                }
            }
            // parse ...
            // else if (std::filesystem::path(p).extension() == ...)
        }
    }
    /**
     * Getter for networks generated from given files.
     *
     * @param sorted Flag to indicate that the networks are to be returned in ascending order of their vertex count.
     * @return Vector of read and constructed logic networks.
     */
    const std::vector<NtkPtr>& get_networks(bool sorted = false)
    {
        // sort by network size to make the small ones go first
        if (sorted)
        {
            std::sort(networks.begin(), networks.end(),
                      [](const auto& n1, const auto& n2) { return n1->num_gates() < n2->num_gates(); });
        }

        return networks;
    }

  private:
    /**
     * Output stream to which status updates are to be written.
     */
    std::ostream& out;
    /**
     * Vector of logic networks read from files given at construction.
     */
    std::vector<NtkPtr> networks;

    using Ntk = typename NtkPtr::element_type;
    /**
     * Actual read function that constructs the logic network from a file.
     *
     * @tparam Reader The functor type which constructs the network, i.e. handles the callbacks.
     * @tparam ReadFun The file parser type.
     * @param file The file name.
     * @param rfun The actual parsing function.
     */
    template <class Reader, class ReadFun>
    void read(const std::string_view& file, ReadFun rfun) noexcept
    {
        Ntk ntk{};

        try
        {
            lorina::text_diagnostics client{};
            if (lorina::diagnostic_engine diag{&client};
                rfun(file.data(), Reader{ntk}, &diag) == lorina::return_code::success)
            {
                const auto name = std::filesystem::path{file}.stem().string();

                if constexpr (mockturtle::has_set_network_name_v<Ntk>)
                {
                    ntk.set_network_name(name);
                }

                networks.push_back(std::make_shared<Ntk>(ntk));
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

}  // namespace fiction

#endif  // FICTION_NETWORK_READER_HPP
