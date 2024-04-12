SiDB Lattice Layout
===================

The SiDB lattice layout can be layered on top of an SiDB cell-level layout. It is used to store and access the H-Si lattice orientation (H-Si(100)-2x1 surface or H-Si(111)-1x1 surface).

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb_lattice.hpp``

        .. doxygenclass:: fiction::sidb_lattice
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_lattice_100
            :members:
        .. autoclass:: mnt.pyfiction.sidb_lattice_111
            :members:
