.. _hexagonalization:

Mapping Cartesian to Hexagonal Layouts
--------------------------------------

This algorithm maps Cartesian 2DDWave-clocked layouts used for Quantum-dot Cellular Automata (QCA) to hexagonal row-clocked layouts,
which are suitable for Silicon Dangling Bonds (SiDBs).

The respective coordinates on the hexagonal grid are calculated as follows:

.. math::

    y_{\mathit{hex}} = x_{\mathit{Cart}} + y_{\mathit{Cart}}

.. math::

    x_{\mathit{hex}} = x_{\mathit{Cart}} + \left \lceil{\left \lfloor{\frac{h}{2}} \right \rfloor} -  \frac{y_{\mathit{hex}}}{2}\right \rceil

.. figure:: /_static/hexagonalization.svg
   :width: 600

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/hexagonalization.hpp``

        .. doxygenstruct:: fiction::hexagonalization_stats
           :members:
        .. doxygenfunction:: fiction::hexagonalization


    .. tab:: Python
        .. autofunction:: mnt.pyfiction.hexagonalization
