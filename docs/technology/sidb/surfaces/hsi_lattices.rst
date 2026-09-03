.. _alternative_hsi_lattices:

H-Si Lattice Orientations
=========================

A collection of the supported H-Si surface lattice orientations for SiDB creation. We currently support
H-Si(100)-2x1 and H-Si(111)-1x1.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/surfaces/lattice_orientations.hpp``

        .. doxygenstruct:: fiction::sidb::surfaces::lattice_100
           :members:
        .. doxygenstruct:: fiction::sidb::surfaces::lattice_111
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.sidb_100_lattice
            :members:
        .. autoclass:: mnt.pyfiction.sidb_111_lattice
            :members:


SiDB Lattice Layout
===================

The SiDB lattice layout can be layered on top of an SiDB cell-level layout. It is used to store and access the
H-Si lattice orientation. We currently support H-Si(100)-2x1 and H-Si(111)-1x1.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/surfaces/lattice.hpp``

        .. doxygenclass:: fiction::sidb::surfaces::lattice
           :members:

    .. tab:: Python
