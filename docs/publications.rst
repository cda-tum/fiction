.. _publications:

Publications
============

*fiction* is academic software, thus, many of its built-in algorithms have been published as scientific papers.

A brief technical report summarizing the features of *fiction* v0.2.1 is available at `arXiv <https://arxiv.org/abs/1905.02477>`_.

If you use *fiction* in your work, we would appreciate if you cited

    Marcel Walter, Robert Wille, Frank Sill Torres, Daniel Große and Rolf Drechsler: "fiction: An Open Source Framework
    for the Design of Field-coupled Nanocomputing Circuits", 2019, **arXiv:1905.02477**.

.. code-block:: tex

    @misc{fiction,
      author = {Walter, Marcel and Wille, Robert and Sill Torres, Frank and Gro{\ss}e, Daniel and Drechsler, Rolf},
      title = {{fiction: An Open Source Framework for the Design of Field-coupled Nanocomputing Circuits}},
      archivePrefix = {arXiv},
      eprint = {1905.02477},
      note = {arXiv:1905.02477},
      year = {2019},
      month = {May}
    }

and

    Marcel Walter, Jan Drewniok, Simon Hofmann, Benjamin Hien, and Robert Wille: "The Munich Nanotech Toolkit (MNT)",
    IEEE International Conference on Nanotechnology (IEEE NANO), 2024.

.. code-block:: tex

    @inproceedings{mnt,
      author = {Walter, Marcel and Drewniok, Jan and Hofmann, Simon and Hien, Benjamin and Wille, Robert},
      title = {{The Munich Nanotech Toolkit (MNT)}},
      booktitle = {IEEE International Conference on Nanotechnology (IEEE NANO)},
      pages = {454--459},
      year = {2024}
    }

Furthermore, if you use any of the design automation algorithms, please consider citing their respective papers as well.

* :ref:`Exact physical design <exact>` (``exact``):

    .. code-block:: tex

        @inproceedings{walter2018exact,
          title={{An Exact Method for Design Exploration of Quantum-dot Cellular Automata}},
          author={Walter, Marcel and Wille, Robert and Gro{\ss}e, Daniel and Sill Torres, Frank and Drechsler, Rolf},
          booktitle={Design, Automation and Test in Europe Conference \& Exhibition (DATE)},
          pages = {503--508},
          year={2018}
        }

* :ref:`Scalable physical design <ortho>` (``ortho``):

    .. code-block:: tex

        @inproceedings{walter2019ortho,
          title={{Scalable Design for Field-coupled Nanocomputing Circuits}},
          author={Walter, Marcel and Wille, Robert and Sill Torres, Frank and Gro{\ss}e, Daniel and Drechsler, Rolf},
          booktitle={Asia and South Pacific Design Automation Conference (ASP-DAC)},
          pages={197--202},
          year={2019},
          publisher={ACM New York, NY, USA}
        }

* :ref:`One-pass synthesis <onepass>` (``onepass``):

    .. code-block:: tex

        @inproceedings{walter2021onepass,
          title={{One-pass Synthesis for Field-coupled Nanocomputing Technologies}},
          author={Walter, Marcel and Haaswijk, W. and Wille, Robert and Sill Torres, Frank and Drechsler, Rolf},
          booktitle={Asia and South Pacific Design Automation Conference (ASP-DAC)},
          pages={574--580},
          year={2021},
          publisher={ACM New York, NY, USA}
        }

* :ref:`Equivalence checking <equiv>` (``equiv``):

    .. code-block:: tex

        @inproceedings{walter2020equiv,
          title={{Verification for Field-coupled Nanocomputing Circuits}},
          author={Walter, Marcel and Wille, Robert and Sill Torres, Frank and D. Gro{\ss}e and Drechsler, Rolf},
          booktitle={Design Automation Conference (DAC)},
          year={2020}
        }

* :ref:`Multi-path routing <color_routing>` (``color_routing``):

    .. code-block:: tex

        @inproceedings{walter2022colorrouting,
          title={{Efficient Multi-Path Signal Routing for Field-coupled Nanotechnologies}},
          author={Walter, Marcel and Wille, Robert},
          booktitle={International Symposium on Nanoscale Architectures (NANOARCH)},
          year={2022}
        }

    `Multi-path routing experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/color_routing>`_

* :ref:`QuickSim <quicksim>` (``quicksim``):

    .. code-block:: tex

        @inproceedings{drewniok2023quicksim,
          title={{\emph{QuickSim}: Efficient \emph{and} Accurate Physical Simulation of Silicon Dangling Bond Logic}},
          author={Drewniok, Jan and Walter, Marcel and Ng, Samuel Sze Hang and Walus, Konrad and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={817--822},
          year={2023}
        }

    `QuickSim experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/electrostatic_ground_state>`_

* :ref:`QuickExact <quickexact>` (``quickexact``):

    .. code-block:: tex

        @inproceedings{drewniok2024quickexact,
          title={{The Need for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic}},
          author={Drewniok, Jan and Walter, Marcel and Wille, Robert},
          booktitle={Asia and South Pacific Design Automation Conference (ASP-DAC)},
          pages={576--581},
          year={2024}
        }

    `QuickExact experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/electrostatic_ground_state>`_

* :ref:`Critical temperature simulation <critical_temperature>` (``critical_temperature``):

    .. code-block:: tex

        @inproceedings{drewniok2023temperature,
          title={{Temperature Behavior of Silicon Dangling Bond Logic}},
          author={Drewniok, Jan and Walter, Marcel and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={925--930},
          year={2023}
        }

    `Critical temperature experiment scripts <https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/temperature>`_

* :ref:`Operational domain computation <opdom>` (``opdom``):

    .. code-block:: tex

        @inproceedings{walter2023opdom,
          title={{Reducing the Complexity of Operational Domain Computation in Silicon Dangling Bond Logic}},
          author={Walter, Marcel and Drewniok, Jan and Ng, Samuel Sze Hang and Walus, Konrad and Wille, Robert},
          booktitle={International Symposium on Nanoscale Architectures (NANOARCH)},
          year={2023}
        }

    `Operational domain experiment scripts <https://github.com/cda-tum/fiction/tree/main/experiments/operational_domain>`_

* :ref:`SiDB gate design <gate_design>` (``gate_design``):

    .. code-block:: tex

        @inproceedings{drewniok2023gatedesigner,
          title={{Minimal Design of SiDB Gates: An Optimal Basis for Circuits Based on Silicon Dangling Bonds.}},
          author={Drewniok, Jan and Walter, Marcel and Wille, Robert},
          booktitle={International Symposium on Nanoscale Architectures (NANOARCH)},
          year={2023}
        }

* :ref:`Mapping Cartesian to hexagonal layouts <hexagonalization>` (``hexagonalization``):

    .. code-block:: tex

        @inproceedings{hofmann2023hexagonalization,
          title={{Scalable Physical Design for Silicon Dangling Bond Logic: How a 45\textdegree~Turn Prevents the Reinvention of the Wheel}},
          author={Hofmann, Simon and Walter, Marcel and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={872--877},
          year={2023}
        }

    `Hexagonalization experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/hexagonalization>`_

* :ref:`Post-layout optimization <post_layout_optimization>` (``post_layout_optimization``):

    .. code-block:: tex

        @inproceedings{hofmann2023postlayout,
          title={{Post-Layout Optimization for Field-coupled Nanotechnologies}},
          author={Hofmann, Simon and Walter, Marcel and Wille, Robert},
          booktitle={International Symposium on Nanoscale Architectures (NANOARCH)},
          year={2023}
        }

    `Post-layout optimization experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/post_layout_optimization>`_

* :ref:`Wiring reduction <wiring_reduction>` (``wiring_reduction``):

    .. code-block:: tex

        @inproceedings{hofmann2024wiring,
          title = {{Late Breaking Results: Wiring Reduction for Field-coupled Nanotechnologies}},
          author = {Hofmann, Simon and Walter, Marcel and Wille, Robert},
          booktitle={Design Automation Conference (DAC)},
          year = {2024}
        }

    `Wiring reduction experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/wiring_reduction>`_

* :ref:`Graph-oriented layout design <graph_oriented_layout_design>` (``gold``):

    .. code-block:: tex

        @inproceedings{hofmann2024gold,
          title={{A* is Born: Efficient and Scalable Physical Design for Field-coupled Nanocomputing}},
          author={Hofmann, Simon and Walter, Marcel and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={80--85},
          year={2024}
        }

    `Graph-oriented layout design experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/graph_oriented_layout_design>`_

* :ref:`SAT-based clock number assignment <determine_clocking>`:

    .. code-block:: tex

        @inproceedings{walter2024clocknumber,
          title={{Ending the Tyranny of the Clock: SAT-Based Clock Number Assignment for Field-coupled Nanotechnologies}},
          author={Walter, Marcel and Drewniok, Jan and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={68--73},
          year={2024}
        }

    `Clock number assignment experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/clock_number_assignment>`_

* :ref:`On-the-fly Defect-Aware Design of Circuits <on_the_fly_design>`:

    .. code-block:: tex

        @inproceedings{drewniok2024ontheflydesign,
          title={{On-the-fly Defect-Aware Design of Circuits based on Silicon Dangling Bond Logic}},
          author={Drewniok, Jan and Walter, Marcel and Hang Ng, Samuel Sze and Walus, Konrad and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={30--35},
          year={2024}
        }

    `On-the-fly defect-aware design of circuits experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/physical_design_with_on_the_fly_gate_design>`_

* :ref:`Unifying Figures of Merit for Silicon Dangling Bond Logic`:

    .. code-block:: tex

        @inproceedings{drewniok2024figuresofmerit,
          title={{Unifying Figures of Merit: A Versatile Cost Function for Silicon Dangling Bond Logic}},
          author={Drewniok, Jan and Walter, Marcel and Hang Ng, Samuel Sze and Walus, Konrad and Wille, Robert},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={91--96},
          year={2024}
        }

* :ref:`Alternative Silicon Orientations <alternative_hsi_lattices>`:

    .. code-block:: tex

        @inproceedings{ng2024latorientations,
          title={{Unlocking Flexible Silicon Dangling Bond Logic Designs on Alternative Silicon Orientations}},
          author={Ng, Samuel Sze Hang and Drewniok, Jan and Walter, Marcel and Retallick, Jacob and Wille, Robert and Walus, Konrad},
          booktitle={IEEE International Conference on Nanotechnology (IEEE NANO)},
          pages={57--92},
          year={2024}
        }

* :ref:`SiDB Bestagon library <bestagon>`:

    .. code-block:: tex

        @inproceedings{walter2022hexagons,
          title={{Hexagons are the Bestagons: Design Automation for Silicon Dangling Bond Logic}},
          author={Walter, Marcel and Ng, Samuel Sze Hang and Walus, Konrad and Wille, Robert},
          booktitle={Design Automation Conference (DAC)},
          pages = {739--744},
          year={2022}
        }

    `Bestagon experiment script <https://github.com/cda-tum/fiction/tree/main/experiments/bestagon>`_
