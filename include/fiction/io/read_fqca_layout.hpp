//
// Created by marcel on 04.10.21.
//

#ifndef FICTION_READ_FQCA_LAYOUT_HPP
#define FICTION_READ_FQCA_LAYOUT_HPP

#include "../technology/cell_technologies.hpp"
#include "../traits.hpp"

#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <string>
#include <type_traits>
#include <vector>

namespace fiction
{

namespace detail
{
template <typename Lyt>
class read_fqca_layout_impl
{
  public:
    explicit read_fqca_layout_impl(std::istream& s) : lyt{}, is{s} {}

    Lyt run()
    {

        return lyt;
    }

  private:
    Lyt lyt;

    std::istream& is;
};

}  // namespace detail

/**
 *
 * @tparam Lyt
 * @param is
 */
template <typename Lyt>
Lyt read_fqca_layout(std::istream& is)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<Lyt>, qca_technology>, "Lyt must be a QCA layout");

    detail::read_fqca_layout_impl<Lyt> p{is};

    const auto lyt = p.run();

    return lyt;
}

template <typename Lyt>
Lyt read_fqca_layout(const std::string& filename)
{
    std::ifstream is{filename.c_str(), std::ifstream::in};

    if (!is.is_open())
        throw std::ifstream::failure("could not open file");

    const auto lyt = read_fqca_layout<Lyt>(is);
    is.close();

    return lyt;
}

}  // namespace fiction

#endif  // FICTION_READ_FQCA_LAYOUT_HPP
