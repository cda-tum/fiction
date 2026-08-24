//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_TECHNOLOGY_QCA_IO_WRITE_QLL_LAYOUT_HPP
#define FICTION_TECHNOLOGY_QCA_IO_WRITE_QLL_LAYOUT_HPP

#include "fiction/technology/inml/io/write_qll_layout.hpp"  // IWYU pragma: export

namespace fiction::qca::io
{

/**
 * Re-exported from `fiction::inml::io`. The QLL format serves QCA, molQCA, and iNML
 * alike; it is defined under `inml/` because ToPoliNano and MagCAD, which read it, are
 * the iNML toolchain. Including either path works.
 */
using inml::io::write_qll_layout;

}  // namespace fiction::qca::io

#endif  // FICTION_TECHNOLOGY_QCA_IO_WRITE_QLL_LAYOUT_HPP
