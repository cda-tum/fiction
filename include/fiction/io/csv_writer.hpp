//
// Created by marcel on 06.05.20.
//

#ifndef FICTION_CSV_WRITER_HPP
#define FICTION_CSV_WRITER_HPP

#include <ostream>
#include <utility>

namespace fiction
{

/**
 * Utility class for writing delimited (e.g. CSV) data into an output stream. It provides a variadic member function,
 * `write_line`, that can take an arbitrary number of arguments and write them to the provided output stream in a line
 * separated by a specified delimiter.
 *
 * The csv_writer follows some behavior principles:
 * - Any standard data type can be written to the output stream.
 * - Data arguments written will be separated by the specified delimiter.
 * - A newline is written at the end of each line.
 * - If `write_line` receives no arguments, it only writes a newline.
 * - The last value written in a line is not followed by a delimiter.
 * - No checks for escape characters are performed.
 *
 * Example usage:
 * \code{.cpp}
 *   std::ofstream file("output.csv");
 *   csv_writer writer(file);
 *   writer.write_line("Name", "Age", "City");
 *   writer.write_line("Alice", 20, "New York");
 * \endcode
 */
class csv_writer
{
  public:
    /**
     * Standard constructor.
     *
     * @param os Output stream to write CSV data into.
     */
    explicit csv_writer(std::ostream& os) : stream{os} {}
    /**
     * Writes a single line of values to the output stream separated by a DELIMITER. No delimiter placed after the last
     * value. Note that no escape checks are performed. Upon receiving no arguments, only a newline is written. This
     * function uses template recursion to process the variadic parameters.
     *
     * @tparam T The type of the first argument.
     * @tparam Ts Types of the rest of the variadic parameter pack.
     * @param arg First argument to write to the stream.
     * @param args Rest of the arguments to write to the stream if any exist.
     */
    template <typename T, typename... Ts>
    void write_line(T&& arg, Ts&&... args)
    {
        if constexpr (sizeof...(args) > 0)
        {
            stream << std::forward<T>(arg) << DELIMITER;
            write_line(std::forward<Ts>(args)...);
        }
        else
        {
            stream << std::forward<T>(arg) << std::endl;
        }
    }
    /**
     * Writes a newline to the output stream. This is the base case of the variadic template function, and is invoked
     * when there are no additional arguments to process.
     *
     * @tparam Ts An empty variadic template argument pack.
     */
    template <typename... Ts>
    void write_line()
    {
        stream << std::endl;
    }

  private:
    /**
     * Output stream to write to.
     */
    std::ostream& stream;
    /**
     * The delimiter to use.
     */
    static constexpr char DELIMITER = ',';
};

}  // namespace fiction

#endif  // FICTION_CSV_WRITER_HPP
