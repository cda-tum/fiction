//
// Created by marcel on 01.09.20.
//

#include "network_reader.h"

network_reader::network_reader(const std::string& filename, std::ostream& o)
        :
        out{o}
{
    // checks for extension validity
    auto is_valid_extension = [](const auto& _f)->bool
    {
        const std::vector<std::string> extensions{".v", ".aig"};
        return std::any_of(extensions.cbegin(), extensions.cend(),
                           [&_f](const auto& _e) { return boost::filesystem::extension(_f) == _e; });
    };

    std::vector<std::string> paths{};

    // check for for given file's properties
    if (boost::filesystem::exists(filename))
    {
        if (boost::filesystem::is_regular_file(filename))
        {
            // collect valid files only
            if (is_valid_extension(filename))
                paths.push_back(filename);
        }

        else if (boost::filesystem::is_directory(filename))
        {
            // iterate over each file in the directory
            for (auto& file : boost::make_iterator_range(boost::filesystem::directory_iterator(filename), {}))
            {
                if (boost::filesystem::is_regular_file(file))
                {
                    // collect valid files only
                    if (is_valid_extension(file))
                        paths.push_back(file.path().string());
                }
            }
        }
        else // existing file but a weird one
            out << "[e] given file name does not point to a regular file" << std::endl;
    }
    else // path leads nowhere
        out << "[e] given file name does not exist" << std::endl;

    // handle collected files
    for (const auto& f : paths)
    {
        // parse Verilog
        if (boost::filesystem::extension(f) == ".v")
        {
            read<mockturtle::verilog_reader<logic_network::mig_nt>,
                 lorina::return_code(const std::string&,
                                     const lorina::verilog_reader&,
                                     lorina::diagnostic_engine*)>(f, lorina::read_verilog);
        }
            // parse Aiger
        else if (boost::filesystem::extension(f) == ".aig")
        {
            read<mockturtle::aiger_reader<logic_network::mig_nt>,
                 lorina::return_code(const std::string&,
                                     const lorina::aiger_reader&,
                                     lorina::diagnostic_engine*)>(f, lorina::read_aiger);
        }
        // parse ...
        // else if (boost::filesystem::extension(f) == ...)
    }
}

const std::vector<logic_network_ptr>& network_reader::get_networks(bool sorted)
{
    // sort by network size to make the small ones go first
    if (sorted)
    {
        std::sort(networks.begin(), networks.end(), [](auto n1, auto n2)
        {
            return n1->vertex_count() >= n2->vertex_count();
        });
    }

    return networks;
}
