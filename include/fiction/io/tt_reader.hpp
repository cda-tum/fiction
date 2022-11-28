//
// Created by marcel on 05.05.20.
//

#ifndef FICTION_TT_READER_HPP
#define FICTION_TT_READER_HPP

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

namespace fiction
{

/**
 * Simple reader to parse truth tables from files. The files must be organized as follows: Each line consists of a
 * truth table in hexadecimal form plus its corresponding Boolean expression. The two are separated by a space.
 *
 * This format is used by, e.g., Alan Mishchenko for his DSD functions: https://people.eecs.berkeley.edu/~alanmi/temp5/
 */
template <typename TT = kitty::dynamic_truth_table>
class tt_reader
{
  public:
    /**
     * Standard constructor. Reads the given stream into a vector of lines.
     *
     * @param stream Stream to parse.
     */
    explicit tt_reader(std::istream& stream)
    {
        read(stream);
    }
    /**
     * Standard constructor. Reads the given file into a vector of lines.
     *
     * @param filename File to parse.
     */
    explicit tt_reader(const std::string& filename)
    {
        std::ifstream file{filename, std::ios::in};
        if (file.is_open())
        {
            read(file);
        }
    }
    /**
     * Returns the next truth table parsed from the file or std::nullopt if no further truth tables are available.
     *
     * @return Next truth table from the file.
     */
    std::optional<TT> next()
    {
        if (pos >= elements.size() - 1)
        {
            return std::nullopt;
        }

        // remove the 0x prefix
        auto table = elements[pos].substr(2u);
        // determine number of truth table variables
        auto num_vars = static_cast<uint32_t>(std::log2(table.size() << 2ul));
        // create truth table
        TT tt{num_vars};
        kitty::create_from_hex_string(tt, table);

        // TODO change pos to an iterator
        pos += 2;

        return tt;
    }

  private:
    /**
     * Reads hexadecimal truth table representations as well as Boolean expressions from the given stream.
     *
     * @param in_stream Stream of truth table representations.
     */
    void read(std::istream& in_stream)
    {
        // read the file into a vector of strings
        elements = std::vector<std::string>{std::istream_iterator<std::string>{in_stream},
                                            std::istream_iterator<std::string>{}};
        // TODO fill the vector with an std::copy_if instead. increase a counter and only copy if counter is even
    }
    /**
     * Contains the hexadecimal truth tables and their Boolean expressions in string representation. The two are
     * alternating.
     */
    std::vector<std::string> elements;
    /**
     * Current position in the vector.
     */
    std::size_t pos = 0ul;
};

}  // namespace fiction

#endif  // FICTION_TT_READER_HPP
