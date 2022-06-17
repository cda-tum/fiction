//
// Created by marcel on 02.06.22.
//

#include "pybind11/pybind11.h"
#include "pyfiction/algorithms/physical_design/exact.hpp"
#include "pyfiction/algorithms/physical_design/orthogonal.hpp"
#include "pyfiction/layouts/cartesian_layout.hpp"
#include "pyfiction/layouts/cell_level_layout.hpp"
#include "pyfiction/layouts/clocked_layout.hpp"
#include "pyfiction/layouts/coordinates.hpp"
#include "pyfiction/layouts/gate_level_layout.hpp"
#include "pyfiction/layouts/hexagonal_layout.hpp"
#include "pyfiction/networks/logic_network.hpp"

PYBIND11_MODULE(pyfiction, m)
{
    // docstring
    m.doc() = "Python interface for the fiction framework for Field-coupled Nanotechnologies";

    /**
     * Layouts
     */
    pyfiction::coordinates(m);
    pyfiction::cartesian_layout(m);
    pyfiction::hexagonal_layout(m);
    pyfiction::clocked_cartesian_layout(m);
    pyfiction::gate_level_clocked_cartesian_layout(m);
    pyfiction::cell_level_clocked_cartesian_layout(m);
    /**
     * Networks
     */
    pyfiction::logic_network(m);
    /**
     * Algorithms
     */
    pyfiction::exact(m);
    pyfiction::orthogonal(m);
}
