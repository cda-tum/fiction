/*
  This file contains docstrings for use in the Python bindings.
  Do not edit! They were automatically extracted by pybind11_mkdoc.
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_fiction_a_star =
R"doc(The A* path finding algorithm for shortest loop-less paths between a
given source and target coordinate in a layout. This function
automatically detects whether the given layout implements a clocking
interface (see `clocked_layout`) and respects the underlying
information flow imposed by `layout`'s clocking scheme.

A* is an extension of Dijkstra's algorithm for shortest paths but
offers better average complexity. It uses a heuristic distance
function that estimates the remaining costs towards the target in
every step. Thus, this heuristic function should neither be complex to
calculate nor overestimating the remaining costs. Common heuristics to
be used are the Manhattan and the Euclidean distance functions. See
`distance_functor` for implementations.

If the given layout implements the obstruction interface (see
`obstruction_layout`), paths will not be routed via obstructed
coordinates and connections.

If the given layout is a gate-level layout and implements the
obstruction interface (see `obstruction_layout`), paths may contain
wire crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

In certain cases it might be desirable to determine regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:

```
{.cpp}
using clk_lyt = clocked_layout<cartesian_layout<>>;
using path = layout_coordinate_path<cartesian_layout<>>;
clk_lyt layout = ...;
auto shortest_path = a_star<path>(static_cast<cartesian_layout<>>(layout), {source, target});
```

A* was introduced in \"A Formal Basis for the Heuristic Determination
of Minimum Cost Paths\" by Peter E. Hart, Nils J. Nilsson, and Bertram
Raphael in IEEE Transactions on Systems Science and Cybernetics 1968,
Volume 4, Issue 2.

This implementation is based on the pseudocode from
https://en.wikipedia.org/wiki/A_star_search_algorithm.

Template parameter ``Path``:
    Type of the returned path.

Template parameter ``Lyt``:
    Type of the layout to perform path finding on.

Template parameter ``Dist``:
    Distance value type to be used in the heuristic estimation
    function.

Template parameter ``Cost``:
    Cost value type to be used when determining moving cost between
    coordinates.

Parameter ``layout``:
    The layout in which the shortest path between a source and target
    coordinate is to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``dist_fn``:
    A distance functor that implements the desired heuristic
    estimation function.

Parameter ``cost_fn``:
    A cost functor that implements the desired cost function.

Parameter ``params``:
    Parameters.

Returns:
    The shortest loop-less path in `layout` from `objective.source` to
    `objective.target`.)doc";

static const char *__doc_fiction_a_star_distance =
R"doc(A distance function that does not approximate but compute the actual
minimum path length on the given layout via A* traversal. Naturally,
this function cannot be evaluated in :math:`\mathcal{O}(1)`, but has
the polynomial complexity of A*.

If no path between `source` and `target` exists in `layout`, the
returned distance is `std::numeric_limits<Dist>::infinity()` if that
value is supported by `Dist`, or `std::numeric_limits<Dist>::max()`,
otherwise.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.

Parameter ``layout``:
    The layout in which the distance between `source` and `target` is
    to be determined.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Minimum path length between `source` and `target` in `layout`.)doc";

static const char *__doc_fiction_a_star_distance_functor =
R"doc(A pre-defined distance functor that uses the A* distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.)doc";

static const char *__doc_fiction_a_star_distance_functor_a_star_distance_functor = R"doc()doc";

static const char *__doc_fiction_a_star_params = R"doc(Parameters for the A* algorithm.)doc";

static const char *__doc_fiction_a_star_params_crossings =
R"doc(Allow paths to cross over obstructed tiles if they are occupied by
wire segments.)doc";

static const char *__doc_fiction_all_2_input_functions =
R"doc(Auxiliary function to create technology mapping parameters for AND,
OR, NAND, NOR, XOR, XNOR, LE, GE, LT, GT, and NOT gates.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_all_coordinates_in_spanned_area =
R"doc(Generates a vector of all coordinates within an area spanned by two
coordinates.

This function calculates and returns a list of all coordinates within
a rectangular area defined by two corner coordinates, inclusive of the
boundaries. The coordinates are generated in a top-to-bottom, left-to-
right order, covering the entire area between the two specified
corners.

Template parameter ``CoordinateType``:
    Coordinate Type.

Parameter ``cell_first_corner``:
    The cell defining the first corner of the area.

Parameter ``cell_second_corner``:
    The cell defining the second corner of the area.

Returns:
    A vector containing all cells within the specified area.)doc";

static const char *__doc_fiction_all_incoming_edge_paths =
R"doc(Returns a vector of all possible paths to reach the given node from
the primary inputs within the given network.

Each node without predecessors is considered a terminal and a path is
defined as a sequence of edges.

Parameter ``n``:
    Node whose paths are desired.

Returns:
    A vector of all possible edge paths leading from terminals to `v`.)doc";

static const char *__doc_fiction_all_standard_2_input_functions =
R"doc(Auxiliary function to create technology mapping parameters for AND,
OR, NAND, NOR, XOR, XNOR, and NOT gates.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_all_standard_3_input_functions =
R"doc(Auxiliary function to create technology mapping parameters for AND3,
XOR_AND, OR_AND, ONEHOT, MAJ3, GAMBLE, DOT, MUX, and AND_XOR gates.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_all_supported_functions =
R"doc(Auxiliary function to create technology mapping parameters for all
supported functions.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_all_supported_standard_functions =
R"doc(Auxiliary function to create technology mapping parameters for all
supported standard functions.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_and_or_not =
R"doc(Auxiliary function to create technology mapping parameters for AND,
OR, and NOT gates.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_and_or_not_maj =
R"doc(Auxiliary function to create technology mapping parameters for AND,
OR, NOT, and MAJ gates.

Returns:
    Technology mapping parameters.)doc";

static const char *__doc_fiction_apply_gate_library =
R"doc(Applies a gate library to a given gate-level layout and, thereby,
creates and returns a cell-level layout. The gate library type should
provide all functions specified in fcn_gate_library. It is, thus,
easiest to extend fcn_gate_library to implement a new gate library.
Examples are `qca_one_library`, `inml_topolinano_library`, and
`sidb_bestagon_library`.

May pass through, and thereby throw, an
`unsupported_gate_type_exception` or an
`unsupported_gate_orientation_exception`.

Template parameter ``CellLyt``:
    Type of the returned cell-level layout.

Template parameter ``GateLibrary``:
    Type of the gate library to apply.

Template parameter ``GateLyt``:
    Type of the gate-level layout to apply the library to.

Parameter ``lyt``:
    The gate-level layout.

Returns:
    A cell-level layout that implements `lyt`'s gate types with
    building blocks defined in `GateLibrary`.)doc";

static const char *__doc_fiction_apply_gate_library_to_defective_surface =
R"doc(Applies a gate library to a given gate-level layout and maps the SiDB
and defect locations onto a defect surface. The gate library type
should provide all functions specified in fcn_gate_library. It is,
thus, easiest to extend fcn_gate_library to implement a new gate
library. Examples are `qca_one_library`, `inml_topolinano_library`,
and `sidb_bestagon_library`.

May pass through, and thereby throw, an
`unsupported_gate_type_exception` or an
`unsupported_gate_orientation_exception`.

Template parameter ``CellLyt``:
    Type of the returned cell-level layout.

Template parameter ``GateLibrary``:
    Type of the gate library to apply.

Template parameter ``GateLyt``:
    Type of the gate-level layout to apply the library to.

Parameter ``lyt``:
    The gate-level layout.

Returns:
    A cell-level layout that implements `lyt`'s gate types with
    building blocks defined in `GateLibrary`.)doc";

static const char *__doc_fiction_apply_parameterized_gate_library =
R"doc(Applies a parameterized gate library to a given gate-level layout and,
thereby, creates and returns a cell-level layout.

May pass through, and thereby throw, an
`unsupported_gate_type_exception`, an
`unsupported_gate_orientation_exception` and any further custom
exceptions of the gate libraries.

Template parameter ``CellLyt``:
    Type of the returned cell-level layout.

Template parameter ``GateLibrary``:
    Type of the gate library to apply.

Template parameter ``GateLyt``:
    Type of the gate-level layout to apply the library to.

Template parameter ``Params``:
    Type of the parameter used for SiDB on-the-fly gate library.

Parameter ``lyt``:
    The gate-level layout.

Parameter ``params``:
    Parameter for the gate library.

Returns:
    A cell-level layout that implements `lyt`'s gate types with
    building blocks defined in `GateLibrary`.)doc";

static const char *__doc_fiction_apply_parameterized_gate_library_to_defective_surface =
R"doc(Applies a defect-aware parameterized gate library to a given gate-
level layout and, thereby, creates and returns a cell-level layout.

May pass through, and thereby throw, an
`unsupported_gate_type_exception`, an
`unsupported_gate_orientation_exception` and any further custom
exceptions of the gate libraries.

Template parameter ``DefectLyt``:
    Type of the returned cell-level layout.

Template parameter ``GateLibrary``:
    Type of the gate library to apply.

Template parameter ``GateLyt``:
    Type of the gate-level layout to apply the library to.

Template parameter ``Params``:
    Type of the parameter used for SiDB on-the-fly gate library.

Parameter ``lyt``:
    The gate-level layout.

Parameter ``params``:
    Parameter for the gate library.

Parameter ``defect_lyt``:
    Defect surface.

Returns:
    A cell-level layout that implements `lyt`'s gate types with
    building blocks defined in `GateLibrary`.)doc";

static const char *__doc_fiction_are_cell_layouts_identical =
R"doc(This function checks whether the given layouts `first_lyt` and
`second_lyt` are identical by comparing various properties such as the
number of cells, the types of cells, defects (if applicable), and
charge states (if applicable). The comparison is done in a detailed
manner depending on the specific layout type.

@Note The aspect ratios of the cell-level layouts are not compared.

Template parameter ``Lyt``:
    The layout type. Must be a cell-level layout.

Parameter ``first_lyt``:
    The first layout to compare.

Parameter ``second_lyt``:
    The second layout to compare.

Returns:
    `true` if the layouts are identical, `false` otherwise.)doc";

static const char *__doc_fiction_area =
R"doc(Computes the area of a given coordinate assuming its origin is (0, 0,
0). Calculates :math:`(|x| + 1) \cdot (|y| + 1)` by default. The
exception is SiQAD coordinates, for which it computes :math:`(|x| + 1)
\cdot (2 \cdot |y| + |z| + 1)`.

Template parameter ``CoordinateType``:
    Coordinate type.

Parameter ``coord``:
    Coordinate.

Returns:
    Area of coord.)doc";

static const char *__doc_fiction_area_2 =
R"doc(Computes real-world area requirements in nm² of a given cell-level
layout. For this purpose, each cell position in the layout is assigned
a vertical and horizontal size. Additionally, a spacing between cell
positions in horizontal and vertical direction is taken into account.

Template parameter ``Lyt``:
    Cell-level layout type.

Template parameter ``AreaType``:
    Type for area representation.

Parameter ``lyt``:
    The cell-level layout whose area is desired.

Parameter ``ps``:
    Area parameters.

Parameter ``pst``:
    Area statistics.

Returns:
    Area requirements in nm².)doc";

static const char *__doc_fiction_area_3 =
R"doc(Computes real-world area requirements in nm² of the bounding box of a
given cell-level layout. For this purpose, each cell position in the
layout is assigned a vertical and horizontal size. Additionally, a
spacing between cell positions in horizontal and vertical direction is
taken into account.

Template parameter ``Lyt``:
    Cell-level layout type.

Template parameter ``AreaType``:
    Type for area representation.

Parameter ``bb``:
    The bounding box of the cell-level layout whose area is desired.

Parameter ``ps``:
    Area parameters.

Parameter ``pst``:
    Area statistics.

Returns:
    Area requirements in nm².)doc";

static const char *__doc_fiction_area_params = R"doc()doc";

static const char *__doc_fiction_area_params_height = R"doc()doc";

static const char *__doc_fiction_area_params_hspace = R"doc(Horizontal and vertical spacing between cells.)doc";

static const char *__doc_fiction_area_params_vspace = R"doc()doc";

static const char *__doc_fiction_area_params_width = R"doc(Width and height of each cell.)doc";

static const char *__doc_fiction_area_stats = R"doc()doc";

static const char *__doc_fiction_area_stats_area = R"doc(Area requirements in nm².)doc";

static const char *__doc_fiction_area_stats_report = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator =
R"doc(An iterator type that iterates over increasingly larger 2D aspect
ratios via factorization, starting from a number of faces :math:`n`.
After iterating over all possible factorizations of n, the next step
increases :math:`n` and continues with the factorization. Thereby, a
sequence of aspect ratios starting from :math:`n = 4` faces looks like
this: :math:`1 \times 4, 4 \times 1, 2 \times 2, 1 \times 5, 5 \times
1, 1 \times 6, 6 \times 1, 2 \times 3, 3 \times 2, \dots`

Template parameter ``AspectRatio``:
    Aspect ratio type.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_aspect_ratio_iterator =
R"doc(Standard constructor. Takes a starting value and computes an initial
factorization. The value `n` represents the amount of faces in the
desired aspect ratios. For example, :math:`n = 1` will yield aspect
ratios with exactly :math:`1` face, i.e. :math:`1 \times 1` which is
equal to `ucoord_t{0, 0}`. If :math:`n = 2`, the aspect ratios
:math:`1 \times 2` and :math:`2 \times 1` will result, which are equal
to `ucoord_t{0, 1}` and `ucoord_t{1, 0}`. Both examples with
`AspectRatio == offset::ucoord_t`.

Parameter ``n``:
    Starting value of the aspect ratio iteration.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_factorize =
R"doc(Factorizes the current `num` into all possible factors :math:`(x, y)`
with :math:`x \cdot y = num`. The result is stored as a vector of
`AspectRatio` objects in the attribute factors.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_factors = R"doc(Factors of num.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_it = R"doc(Iterator pointing to current factor.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_next =
R"doc(Computes the next possible `num` where a factorization :math:`(x, y)`
with :math:`x \cdot y = num` exists.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_num = R"doc(Number to factorize into dimensions.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_eq = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_eq_2 = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_inc =
R"doc(Lets the iterator point to the next dimension of the current
factorization. If there are no next factors, `num` is incremented and
the next factors are computed.

Prefix version.

Returns:
    Reference to this.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_inc_2 =
R"doc(Creates a new iterator that points to the next dimension of the
current factorization. If there are no next factors, `num` is
incremented and the next factors are computed.

Postfix version. Less performance than the prefix version due to copy
construction.

Returns:
    Resulting iterator.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_le = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_le_2 = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_lt = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_lt_2 = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_mul = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_ne = R"doc()doc";

static const char *__doc_fiction_aspect_ratio_iterator_operator_ne_2 = R"doc()doc";

static const char *__doc_fiction_bancs_clocking =
R"doc(Returns the BANCS clocking as defined in \"BANCS: Bidirectional
Alternating Nanomagnetic Clocking Scheme\" by Ruan Evangelista
Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI
2018.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    BANCS clocking scheme.)doc";

static const char *__doc_fiction_band_bending_resilience =
R"doc(Calculates the band bending resilience. This is the minimum
electrostatic potential required to induce a charge change in an SiDB
layout among all possible input combinations which was proposed in
\"Unifying Figures of Merit: A Versatile Cost Function for Silicon
Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K.
Walus, and R. Wille in IEEE NANO 2024
(https://ieeexplore.ieee.org/abstract/document/10628671).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout for which the band bending resilience is calculated.

Parameter ``spec``:
    Expected Boolean function of the layout, provided as a multi-
    output truth table.

Parameter ``params``:
    Parameters for assessing physical population stability.

Parameter ``transition_type``:
    The optional type of charge transition to consider. This can be
    used if one is only interested in a specific type of charge
    transition.

Returns:
    The minimum potential (in V) required for charge change across all
    input combinations.)doc";

static const char *__doc_fiction_band_bending_resilience_params =
R"doc(This struct stores the parameters required to simulate the band
bending resilience of an SiDB layout)doc";

static const char *__doc_fiction_band_bending_resilience_params_assess_population_stability_params = R"doc(Parameters for the assessing physical population stability simulation)doc";

static const char *__doc_fiction_band_bending_resilience_params_bdl_iterator_params = R"doc(Parameters for the input BDL iterator.)doc";

static const char *__doc_fiction_bdl_input_iterator =
R"doc(Iterator that iterates over all possible input states of a BDL layout.
There are :math:`2^n` possible input states for an :math:`n`-input BDL
layout, each with a unique input index. The input index is interpreted
as a binary number, where the :math:`i`-th bit represents the input
state of the :math:`i`-th input BDL pair. If the bit is `1`, the lower
BDL dot is set and the upper BDL dot removed. If the bit is `0`, the
upper BDL dot is removed and the lower BDL dot set. The iterator
creates and stores a deep-copy of the given layout. The state
enumeration wraps around, i.e., after the last possible input state,
the first input state is set again.

The iterator satisfies the requirements of
`LegacyRandomAccessIterator` and can be used in iterator-based `for`
loops.

Template parameter ``Lyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_bdl_input_iterator_bdl_input_iterator =
R"doc(Standard constructor. It alters the layout to set the first input
state, which assigns binary `0` to all input BDL pairs.

Parameter ``lyt``:
    The SiDB BDL layout to iterate over.

Parameter ``ps``:
    Parameters for the BDL input iterator.)doc";

static const char *__doc_fiction_bdl_input_iterator_bdl_input_iterator_2 =
R"doc(Constructor with pre-detected input wires and directions. It alters
the layout to set the first input state, which assigns binary `0` to
all input BDL pairs.

Parameter ``lyt``:
    The SiDB BDL layout to iterate over.

Parameter ``ps``:
    Parameters for the BDL input iterator.

Parameter ``input_wires``:
    Pre-detected input BDL wires.)doc";

static const char *__doc_fiction_bdl_input_iterator_current_input_index =
R"doc(The current input index. There are :math:`2^n` possible input states
for an :math:`n`-input BDL layout.)doc";

static const char *__doc_fiction_bdl_input_iterator_determine_last_bdl_for_each_wire =
R"doc(This function iterates through each wire in `input_bdl_wires`,
identifies the first BDL pair of type `INPUT`, and then finds the BDL
pair within the same wire that has the maximum distance from the
starting pair. The resulting last BDL pairs are stored in
`last_bdl_for_each_wire`.

@note Assumes that `input_bdl_wires` and `last_bdl_for_each_wire` are
accessible within the scope.)doc";

static const char *__doc_fiction_bdl_input_iterator_get_current_input_index =
R"doc(Returns the current input index.

Returns:
    The current input index.)doc";

static const char *__doc_fiction_bdl_input_iterator_input_bdl_wires = R"doc(The detected input BDL wires.)doc";

static const char *__doc_fiction_bdl_input_iterator_input_pairs = R"doc(The detected input BDL pairs.)doc";

static const char *__doc_fiction_bdl_input_iterator_last_bdl_for_each_wire = R"doc(Last BDL pairs for each BDL wire.)doc";

static const char *__doc_fiction_bdl_input_iterator_layout = R"doc(The layout to iterate over.)doc";

static const char *__doc_fiction_bdl_input_iterator_num_input_pairs =
R"doc(Returns the total number of input BDL pairs of the given SiDB gate
layout.

Returns:
    The number of input BDL pairs.)doc";

static const char *__doc_fiction_bdl_input_iterator_num_inputs = R"doc(The amount of input BDL pairs.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_add =
R"doc(Addition operator. Computes the input state of the current iterator
plus the given integer.

Parameter ``m``:
    The amount of input states to skip.

Returns:
    The input state of the current iterator plus the given integer.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_array =
R"doc(Subscript operator. Computes the input state of the current iterator
plus the given integer.

Parameter ``m``:
    The amount of input states to skip.

Returns:
    The input state of the current iterator plus the given integer.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_assign =
R"doc(Assignment operator. Sets the input state to the given integer.

Parameter ``m``:
    The input state to set.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_dec =
R"doc(Prefix decrement operator. Sets the previous input state.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_dec_2 =
R"doc(Postfix decrement operator. Sets the previous input state.

Returns:
    Copy of `this` before decrementing.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_eq =
R"doc(Equality operator. Compares the current input index with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is equal to `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_ge =
R"doc(Greater-or-equal-than operator. Compares the current input index with
the given integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is greater than or equal to `m`,
    `false` otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_gt =
R"doc(Greater-than operator. Compares the current input index with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is greater than `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_iadd =
R"doc(Addition assignment operator. Sets a next input state.

Parameter ``m``:
    The amount of input states to skip.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_inc =
R"doc(Prefix increment operator. Sets the next input state.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_inc_2 =
R"doc(Postfix increment operator. Sets the next input state.

Returns:
    Copy of `this` before incrementing.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_isub =
R"doc(Subtraction assignment operator. Sets a previous input state.

Parameter ``m``:
    The amount of input states to skip.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_le =
R"doc(Less-or-equal-than operator. Compares the current input index with the
given integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is less than or equal to `m`,
    `false` otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_lt =
R"doc(Less-than operator. Compares the current input index with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is less than `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_mul =
R"doc(Dereference operator. Returns a reference to the layout with the
current input state.

Returns:
    Reference to the current layout.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_ne =
R"doc(Inequality operator. Compares the current input index with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current input index is not equal to `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_sub =
R"doc(Subtraction operator. Computes the input state of the current iterator
minus the given integer.

Parameter ``m``:
    The amount of input states to skip.

Returns:
    The input state of the current iterator minus the given integer.)doc";

static const char *__doc_fiction_bdl_input_iterator_operator_sub_2 =
R"doc(Subtraction operator. Computes the difference between the current
input index and the given iterator ones.

Parameter ``other``:
    Iterator to compute the difference with.

Returns:
    The difference between the current input index and the given
    iterator ones.)doc";

static const char *__doc_fiction_bdl_input_iterator_params = R"doc(Parameters for the BDL input iterator.)doc";

static const char *__doc_fiction_bdl_input_iterator_params_2 = R"doc(Parameters for the BDL input iterator.)doc";

static const char *__doc_fiction_bdl_input_iterator_params_bdl_wire_params = R"doc(Parameters to detect BDL wires.)doc";

static const char *__doc_fiction_bdl_input_iterator_params_input_bdl_config =
R"doc(The `input_bdl_config` member allows selection between different modes
for handling input BDLs.)doc";

static const char *__doc_fiction_bdl_input_iterator_params_input_bdl_configuration =
R"doc(This enum defines how BDL inputs are manipulated within the algorithm.
According to existing literature, there are two main approaches for
handling BDL inputs:

1. Distance Encoding, in which an input of `1` is generated by placing
a perturber closer to the target, whereas a `0` is produced by
positioning the perturber farther away (as described in
https://dl.acm.org/doi/10.1145/3489517.3530525). 2. Absence Encoding,
in which a perturber is used to set the input to `1`, while the
absence of a perturber represents a `0` (as described in
https://ieeexplore.ieee.org/abstract/document/8963859).)doc";

static const char *__doc_fiction_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED =
R"doc(A perturber is used to set the input to `1`, while the absence of a
perturber represents a `0` (as described in
https://ieeexplore.ieee.org/abstract/document/8963859).)doc";

static const char *__doc_fiction_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED =
R"doc(An input of `1` is generated by placing a perturber closer to the BDL
wire, whereas a `0` is produced by positioning the perturber farther
away (as described in https://dl.acm.org/doi/10.1145/3489517.3530525).)doc";

static const char *__doc_fiction_bdl_input_iterator_set_all_inputs =
R"doc(Sets all input cells of the layout according to the current input
index. The input index is interpreted as a binary number, where the
:math:`i`-th bit represents the input state of the :math:`i`-th input
BDL pair. If the bit is `1`, the lower BDL dot is set and the upper
BDL dot removed. If the bit is `0`, the upper BDL dot is removed and
the lower BDL dot set.)doc";

static const char *__doc_fiction_bdl_pair =
R"doc(A Binary-dot Logic (BDL) pair is a pair of SiDBs that are close to
each other and, thus, most likely share a charge.

Template parameter ``CellType``:
    Cell type.)doc";

static const char *__doc_fiction_bdl_pair_bdl_pair = R"doc(Standard constructor for empty BDL pairs.)doc";

static const char *__doc_fiction_bdl_pair_bdl_pair_2 =
R"doc(Constructor for BDL pairs.

Parameter ``t``:
    Type of the SiDBs in the pair.

Parameter ``u``:
    The upper SiDB of the pair.

Parameter ``l``:
    The lower SiDB of the pair.)doc";

static const char *__doc_fiction_bdl_pair_equal_ignore_type =
R"doc(Equality check ignoring type.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if the `upper` and `lower` attributes are equal, otherwise
    `false`.)doc";

static const char *__doc_fiction_bdl_pair_has_same_x_coordinate =
R"doc(Checks if the `upper` and `lower` SiDBs in this BDL pair have the same
x-coordinate as the corresponding SiDBs in the other BDL pair.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if both the `upper` and `lower` SiDBs in this pair have the
    same x-coordinate as the corresponding SiDBs in the other pair,
    otherwise `false`.)doc";

static const char *__doc_fiction_bdl_pair_has_same_y_coordinate =
R"doc(Checks if the `upper` and `lower` SiDBs in this BDL pair have the same
y-coordinate as the corresponding SiDBs in the other BDL pair.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if both the `upper` and `lower` SiDBs in this pair have the
    same y-coordinate as the corresponding SiDBs in the other pair,
    otherwise `false`.)doc";

static const char *__doc_fiction_bdl_pair_lower =
R"doc(The lower SiDB of the pair. Upper and lower are defined relative to
each other via the `operator<` overload.)doc";

static const char *__doc_fiction_bdl_pair_not_equal_ignore_type =
R"doc(Inequality check ignoring type.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if the `upper` and `lower` attributes are not equal,
    otherwise `false`.)doc";

static const char *__doc_fiction_bdl_pair_operator_eq =
R"doc(Equality operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is equal to the other, `false` otherwise.)doc";

static const char *__doc_fiction_bdl_pair_operator_ge =
R"doc(Greater-than-or-equal-to operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is greater than or equal to the other,
    otherwise `false`.)doc";

static const char *__doc_fiction_bdl_pair_operator_gt =
R"doc(Greater than operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is greater than the other, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_pair_operator_le =
R"doc(Less than or equal to operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is less than or equal to the other,
    `false` otherwise.)doc";

static const char *__doc_fiction_bdl_pair_operator_lt =
R"doc(Less than operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is less than the other, `false` otherwise.)doc";

static const char *__doc_fiction_bdl_pair_operator_ne =
R"doc(Inequality operator.

Parameter ``other``:
    The other BDL pair to compare with.

Returns:
    `true` if this BDL pair is not equal to the other, `false`
    otherwise.)doc";

static const char *__doc_fiction_bdl_pair_type =
R"doc(The type of the SiDBs in the pair. BDL SiDBs must be of the same type.
They can either be normal, input, or output SiDBs.)doc";

static const char *__doc_fiction_bdl_pair_upper =
R"doc(The upper SiDB of the pair. Upper and lower are defined relative to
each other via the `operator<` overload.)doc";

static const char *__doc_fiction_bdl_wire =
R"doc(This struct encapsulates a vector of `bdl_pair` objects, representing
the pairs of SiDBs in the BDL wire.

Template parameter ``Lyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_bdl_wire_add_bdl_pair =
R"doc(Add a BDL pair to the wire.

Parameter ``pair``:
    The BDL pair to add.)doc";

static const char *__doc_fiction_bdl_wire_bdl_wire = R"doc(Default constructor for an empty BDL wire.)doc";

static const char *__doc_fiction_bdl_wire_bdl_wire_2 =
R"doc(Constructor to initialize the BDL wire with a given vector of BDL
pairs.

Also updates the start and end BDL pairs based on the given vector.

Parameter ``p``:
    The vector of BDL pairs to initialize the wire with.)doc";

static const char *__doc_fiction_bdl_wire_bdl_wire_3 =
R"doc(Copy constructor.

Creates a new `bdl_wire` object as a copy of another `bdl_wire`
object.

Parameter ``other``:
    The `bdl_wire` object to copy from.)doc";

static const char *__doc_fiction_bdl_wire_bdl_wire_4 =
R"doc(Move constructor.

Transfers ownership of the BDL pairs, port, and start/end pairs from
another `bdl_wire` object.

Parameter ``other``:
    The `bdl_wire` object to move from.)doc";

static const char *__doc_fiction_bdl_wire_erase_bdl_pair =
R"doc(Erase a specific BDL pair from the wire.

Parameter ``pair``:
    The BDL pair to remove. The pair is compared using the equality
    operator (operator==).)doc";

static const char *__doc_fiction_bdl_wire_find_bdl_pair_by_type =
R"doc(Find the first Binary-dot Logic (BDL) pair of a specified type in the
wire.

Parameter ``t``:
    Type of BDL pair to search for
    (`sidb_technology::cell_type::INPUT`,
    `sidb_technology::cell_type::OUTPUT`, etc.).

Returns:
    Optional containing the first BDL pair with the specified type
    `t`, or `std::nullopt` if no such BDL pair is found.)doc";

static const char *__doc_fiction_bdl_wire_first_bdl_pair = R"doc(First BDL pair of the wire.)doc";

static const char *__doc_fiction_bdl_wire_last_bdl_pair = R"doc(Last BDL pair of the wire.)doc";

static const char *__doc_fiction_bdl_wire_operator_assign =
R"doc(Move assignment operator.

Transfers ownership of the BDL pairs, port, and start/end pairs from
another `bdl_wire` object.

Parameter ``other``:
    The `bdl_wire` object to move from.

Returns:
    A reference to the updated object.)doc";

static const char *__doc_fiction_bdl_wire_operator_assign_2 =
R"doc(Copy assignment operator.

Copies the content of another `bdl_wire` object, including start and
end pairs.

Parameter ``other``:
    The `bdl_wire` object to copy from.

Returns:
    A reference to the updated object.)doc";

static const char *__doc_fiction_bdl_wire_pairs = R"doc(Vector of BDL pairs representing the wire.)doc";

static const char *__doc_fiction_bdl_wire_port = R"doc(Port of the BDL wire.)doc";

static const char *__doc_fiction_bdl_wire_selection = R"doc(An enumeration of the selection of different types of wires.)doc";

static const char *__doc_fiction_bdl_wire_selection_ALL = R"doc(Select all BDL wires.)doc";

static const char *__doc_fiction_bdl_wire_selection_INPUT = R"doc(Select only BDL wires that start with input cells.)doc";

static const char *__doc_fiction_bdl_wire_selection_OUTPUT = R"doc(Select only BDL wires that end with output cells.)doc";

static const char *__doc_fiction_bdl_wire_update_direction = R"doc(Update the port of the wire based on the current BDL pairs.)doc";

static const char *__doc_fiction_binomial_coefficient =
R"doc(Calculates the binomial coefficient :math:`\binom{n}{k}`.

Parameter ``n``:
    The total number of items.

Parameter ``k``:
    The number of items to choose from n.

Returns:
    The binomial coefficient :math:`\binom{n}{k}`.)doc";

static const char *__doc_fiction_bound_direction =
R"doc(The electrostatic potential bounds required for the *Ground State
Space* algorithm. As the domain in which our potential bounds live are
simply the real numbers, we may think of the lower bound and upper
bound domains to be separate partial order structures on the real
number line, inverse to each other. The *Ground State Space* algorithm
requires the properties of a lower semi-lattice for these domains, ie.
all finite meets must exist. This is implemented for the lower and
upper bound respectively simply by taking a minimum or a maximum. One
may think of meets as follows, which is very relevant to intention of
their application: a meet, or greatest lower bound, is the *maximal
information* common to a set (of potential bounds). This semantic
operation is essential to the *Ground State Space* algorithm, which
thus envelops without loss of accuracy.)doc";

static const char *__doc_fiction_bound_direction_LOWER =
R"doc(Lower bounds live in the set :math:`(-\infty\f, \infty]` and are
ordered by >.)doc";

static const char *__doc_fiction_bound_direction_UPPER =
R"doc(Upper bounds live in the set :math:`[-\infty, \infty)` and are ordered
by <.)doc";

static const char *__doc_fiction_bounding_box_2d =
R"doc(A 2D bounding box object that computes a minimum-sized box around all
non-empty coordinates in a given layout. Layouts can be of arbitrary
size and, thus, may be larger than their contained elements.
Sometimes, it might be necessary to know exactly which space the
associated layout internals occupy. A bounding box computes
coordinates that span a minimum-sized rectangle that encloses all non-
empty layout coordinates.

The bounding box does not automatically updated when the layout
changes. Call `update_bounding_box()` to recompute it.

Template parameter ``Lyt``:
    Gate-level or cell-level layout type.)doc";

static const char *__doc_fiction_bounding_box_2d_bounding_box_2d =
R"doc(Standard constructor that computes an initial bounding box.

Parameter ``lyt``:
    Gate-level or cell-level layout whose bounding box is desired.)doc";

static const char *__doc_fiction_bounding_box_2d_get_max =
R"doc(Returns the maximum corner of the bounding box.

In a `cartesian_layout<offset::ucoord_t>` object, this location
represents the most south-eastern coordinate of the bounding box
enclosing every non-empty coordinate.

Returns:
    The maximum enclosing coordinate in the associated layout.)doc";

static const char *__doc_fiction_bounding_box_2d_get_min =
R"doc(Returns the minimum corner of the bounding box.

In a `cartesian_layout<offset::ucoord_t>` object, this location
represents the most north-western coordinate of the bounding box
enclosing every non-empty coordinate.

Returns:
    The minimum enclosing coordinate in the associated layout.)doc";

static const char *__doc_fiction_bounding_box_2d_get_x_size =
R"doc(Returns the horizontal size of the bounding box in layout coordinates.

Returns:
    Bounding box size along the x-axis.)doc";

static const char *__doc_fiction_bounding_box_2d_get_y_size =
R"doc(Returns the vertical size of the bounding box in layout coordinates.

Returns:
    Bounding box size along the y-axis.)doc";

static const char *__doc_fiction_bounding_box_2d_is_empty_coordinate =
R"doc(Checks if a given coordinate is empty in the layout.

Parameter ``c``:
    The coordinate to check.

Returns:
    True if the coordinate is empty, false otherwise.)doc";

static const char *__doc_fiction_bounding_box_2d_layout = R"doc(The layout whose bounding box is being computed.)doc";

static const char *__doc_fiction_bounding_box_2d_max = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_min = R"doc(The minimum and maximum coordinates of the bounding box.)doc";

static const char *__doc_fiction_bounding_box_2d_update_bounding_box =
R"doc(The bounding box is not automatically updated when the layout changes.
This function recomputes the bounding box.)doc";

static const char *__doc_fiction_bounding_box_2d_x_size = R"doc(The horizontal size of the bounding box in layout coordinates.)doc";

static const char *__doc_fiction_bounding_box_2d_y_size = R"doc(The vertical size of the bounding box in layout coordinates.)doc";

static const char *__doc_fiction_branching_signal_container =
R"doc(A container class to help identify layout locations of branching nodes
like fanouts. When a node from a network is to placed in a layout,
fetching the node's fanins and looking for their locations in the
layout does not work properly when branching nodes like fanouts are
involved that got extended by wire nodes. This container solves that
issue.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Template parameter ``fanout_size``:
    Maximum fanout size possible in the layout and/or the network.)doc";

static const char *__doc_fiction_branching_signal_container_branches = R"doc(Storage for all branches.)doc";

static const char *__doc_fiction_branching_signal_container_branching_signal = R"doc(Branch type.)doc";

static const char *__doc_fiction_branching_signal_container_branching_signal_branching_signal = R"doc()doc";

static const char *__doc_fiction_branching_signal_container_branching_signal_lyt_signal = R"doc()doc";

static const char *__doc_fiction_branching_signal_container_branching_signal_ntk_node = R"doc()doc";

static const char *__doc_fiction_branching_signal_container_operator_array =
R"doc(Accesses the branching container to find the location of a given node
`n`. Returns the signal to that location if it was already stored or
the default signal, otherwise.

Parameter ``n``:
    Node whose branching position is desired.

Returns:
    Signal to `n`'s layout location or the default signal if it wasn't
    found.)doc";

static const char *__doc_fiction_branching_signal_container_update_branch =
R"doc(Updates the given node's branch by another layout signal, thereby,
creating a new branch or updating the position of an existing one,
e.g., if further wire segments were moving the head of the branch.

Parameter ``ntk_node``:
    Node whose branch is to be updated.

Parameter ``lyt_signal``:
    New signal pointing to the end of the branch.)doc";

static const char *__doc_fiction_calculate_boltzmann_factor =
R"doc(This function computes the Boltzmann factor for a given energy, the
minimal energy, and the temperature.

Parameter ``energy``:
    Boltzmann factor for the given energy is calculated.

Parameter ``min_energy``:
    Minimum energy of the system.

Parameter ``temperature``:
    Temperature of the system (unit: K).

Returns:
    Boltzmann factor.)doc";

static const char *__doc_fiction_calculate_defect_clearance =
R"doc(Computes the defect clearance for a given SiDB layout based on a
defect influence domain. The defect clearance is the maximum distance
at which a defect can influence the layout. It calculates the minimum
distance from each SiDB to any influential defect position.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    SiDB layout for which the defect clearance is computed.

Parameter ``defect_inf_domain``:
    Defect influence domain of the given SiDB layout.

Returns:
    Defect clearance.)doc";

static const char *__doc_fiction_calculate_energy_and_state_type_with_kinks_accepted =
R"doc(This function takes in an SiDB energy distribution. For each charge
distribution, the state type is determined (i.e. erroneous,
transparent) while kinks are accepted, meaning a state with kinks is
considered transparent.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``energy_distribution``:
    Energy distribution.

Parameter ``valid_charge_distributions``:
    Physically valid charge distributions.

Parameter ``output_bdl_pairs``:
    Output BDL pairs.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``input_index``:
    The index of the current input configuration.

Returns:
    Electrostatic potential energy of all charge distributions with
    state type.)doc";

static const char *__doc_fiction_calculate_energy_and_state_type_with_kinks_rejected =
R"doc(This function takes in an SiDB energy distribution. For each charge
distribution, the state type is determined (i.e. erroneous,
transparent) while kinks are rejected, meaning a state with kinks is
considered erroneous.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``energy_distribution``:
    Energy distribution.

Parameter ``valid_charge_distributions``:
    Physically valid charge distributions.

Parameter ``output_bdl_pairs``:
    Output BDL pairs.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``input_index``:
    The index of the current input configuration.

Parameter ``input_bdl_wires``:
    Input BDL wires.

Parameter ``output_bdl_wires``:
    Output BDL wires.

Returns:
    Electrostatic potential energy of all charge distributions with
    state type.)doc";

static const char *__doc_fiction_calculate_energy_distribution =
R"doc(This function takes in a vector of `charge_distribution_surface`
objects and returns a map containing the system energy and the number
of occurrences of that energy in the input vector. To compare two
energy values for equality, the comparison uses a tolerance specified
by `constants::ERROR_MARGIN`.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``charge_distributions``:
    A vector of `charge_distribution_surface` objects for which
    statistics are to be computed.

Returns:
    A map containing the system energy as the key and the number of
    occurrences of that energy in the input vector as the value.)doc";

static const char *__doc_fiction_can_positive_charges_occur =
R"doc(This algorithm determines if positively charged SiDBs can occur in a
given SiDB cell-level layout due to strong electrostatic interaction.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to be analyzed.

Parameter ``sim_params``:
    Physical parameters used to determine whether positively charged
    SiDBs can occur.)doc";

static const char *__doc_fiction_cartesian_combinations =
R"doc(This function computes the Cartesian product of a list of vectors.
Each vector in the input list represents a dimension, and the function
produces all possible combinations where each combination consists of
one element from each dimension vector.

Template parameter ``VectorDataType``:
    The type of elements in the vectors.

Parameter ``sets``:
    The sets to compute the Cartesian product for. In this
    implementation, a vector of vectors is utilized for efficiency.
    Each inner vector represents one dimension. The function generates
    combinations using one element from each dimension vector.

Returns:
    A vector of vectors, where each inner vector represents a
    combination of elements, one from each dimension. The total number
    of combinations is the product of the sizes of the input vectors.)doc";

static const char *__doc_fiction_cartesian_layout =
R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian grid. Its faces are organized in the following way:

\verbatim +-------+-------+-------+-------+ | | | | | | (0,0) | (1,0)
| (2,0) | (3,0) | | | | | | +-------+-------+-------+-------+ | | | |
| | (0,1) | (1,1) | (2,1) | (3,1) | | | | | |
+-------+-------+-------+-------+ | | | | | | (0,2) | (1,2) | (2,2) |
(3,2) | | | | | | +-------+-------+-------+-------+ \endverbatim

Template parameter ``OffsetCoordinateType``:
    The coordinate implementation to be used.)doc";

static const char *__doc_fiction_cartesian_layout_above =
R"doc(Returns the coordinate that is directly above a given coordinate `c`,
i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
is already at maximum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose above counterpart is desired.

Returns:
    Coordinate directly above `c`.)doc";

static const char *__doc_fiction_cartesian_layout_adjacent_coordinates =
R"doc(Returns a container that contains all coordinates that are adjacent to
a given one. Thereby, only cardinal directions are being considered,
i.e., the container contains all coordinates `ac` for which
`is_adjacent(c, ac)` returns `true`.

Coordinates that are outside of the layout bounds are not considered.
Thereby, the size of the returned container is at most 4.

Parameter ``c``:
    Coordinate whose adjacent ones are desired.

Returns:
    A container that contains all of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_adjacent_opposite_coordinates =
R"doc(Returns a container that contains all coordinates pairs of opposing
adjacent coordinates with respect to a given one. In this Cartesian
layout, the container will contain (`north(c)`, `south(c)`) and
(`east(c)`, `west(c)`).

This function comes in handy when straight lines on the layout are to
be examined.

Coordinates outside of the layout bounds are not being considered.

Parameter ``c``:
    Coordinate whose opposite ones are desired.

Returns:
    A container that contains pairs of `c`'s opposing coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_area =
R"doc(Returns the layout's number of faces depending on the coordinate type.

Returns:
    Area of layout.)doc";

static const char *__doc_fiction_cartesian_layout_below =
R"doc(Returns the coordinate that is directly below a given coordinate `c`,
i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
is already at minimum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose below counterpart is desired.

Returns:
    Coordinate directly below `c`.)doc";

static const char *__doc_fiction_cartesian_layout_cartesian_layout =
R"doc(Standard constructor. The given aspect ratio points to the highest
possible coordinate in the layout. That means in the ASCII layout
above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the layout has
exactly one coordinate.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

static const char *__doc_fiction_cartesian_layout_cartesian_layout_2 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another cartesian_layout.)doc";

static const char *__doc_fiction_cartesian_layout_cartesian_layout_storage = R"doc()doc";

static const char *__doc_fiction_cartesian_layout_cartesian_layout_storage_cartesian_layout_storage = R"doc()doc";

static const char *__doc_fiction_cartesian_layout_cartesian_layout_storage_dimension = R"doc()doc";

static const char *__doc_fiction_cartesian_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_cartesian_layout_coord =
R"doc(Creates and returns a coordinate in the layout from the given x-, y-,
and z-values.

@note This function is equivalent to calling `OffsetCoordinateType(x,
y, z)`.

Template parameter ``X``:
    x-type.

Template parameter ``Y``:
    y-type.

Template parameter ``Z``:
    z-type.

Parameter ``x``:
    x-value.

Parameter ``y``:
    y-value.

Parameter ``z``:
    z-value.

Returns:
    A coordinate in the layout of type `OffsetCoordinateType`.)doc";

static const char *__doc_fiction_cartesian_layout_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout between
`start` and `stop`. If no values are provided, all coordinates in the
layout will be included. The returned iterator range points to the
first and last coordinate, respectively. The range object can be used
within a for-each loop. Incrementing the iterator is equivalent to
nested for loops in the order z, y, x. Consequently, the iteration
will happen inside out, i.e., x will be iterated first, then y, then
z.

Parameter ``start``:
    First coordinate to include in the range of all coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all
    coordinates.

Returns:
    An iterator range from `start` to `stop`. If they are not
    provided, the first/last coordinate is used as a default.)doc";

static const char *__doc_fiction_cartesian_layout_east =
R"doc(Returns the coordinate that is directly adjacent in eastern direction
of a given coordinate `c`, i.e., the face whose x-dimension is higher
by 1. If `c`'s x-dimension is already at maximum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose eastern counterpart is desired.

Returns:
    Coordinate adjacent and east of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_eastern_border_of =
R"doc(Returns the coordinate with the same y and z values as a given
coordinate but that is located at the layout's eastern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The eastern border equivalent of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_foreach_adjacent_coordinate =
R"doc(Applies a function to all coordinates adjacent to a given one.
Thereby, only cardinal directions are being considered, i.e., the
function is applied to all coordinates `ac` for which `is_adjacent(c,
ac)` returns `true`.

Coordinates that are outside of the layout bounds are not considered.
Thereby, at most 4 coordinates are touched.

Template parameter ``Fn``:
    Functor type.

Parameter ``c``:
    Coordinate whose adjacent ones are desired.

Parameter ``fn``:
    Functor to apply to each of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_foreach_adjacent_opposite_coordinates =
R"doc(Applies a function to all opposing coordinate pairs adjacent to a
given one. In this Cartesian layout, the function will be applied to
(`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).

Template parameter ``Fn``:
    Functor type.

Parameter ``c``:
    Coordinate whose opposite adjacent ones are desired.

Parameter ``fn``:
    Functor to apply to each of `c`'s opposite adjacent coordinate
    pairs.)doc";

static const char *__doc_fiction_cartesian_layout_foreach_coordinate =
R"doc(Applies a function to all coordinates accessible in the layout between
`start` and `stop`. The iteration order is the same as for the
coordinates function.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``fn``:
    Functor to apply to each coordinate in the range.

Parameter ``start``:
    First coordinate to include in the range of all coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all
    coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_foreach_ground_coordinate =
R"doc(Applies a function to all coordinates accessible in the layout's
ground layer between `start` and `stop`. The iteration order is the
same as for the ground_coordinates function.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``fn``:
    Functor to apply to each coordinate in the range.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all ground
    coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_ground_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout's ground
layer between `start` and `stop`. The iteration order is the same as
for the coordinates function but without the z dimension.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all ground
    coordinates.

Returns:
    An iterator range from `start` to `stop`. If they are not
    provided, the first/last coordinate in the ground layer is used as
    a default.)doc";

static const char *__doc_fiction_cartesian_layout_initialize_dimension = R"doc()doc";

static const char *__doc_fiction_cartesian_layout_is_above =
R"doc(Returns `true` iff coordinate `c2` is directly above coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly above `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_adjacent_elevation_of =
R"doc(Similar to `is_adjacent_of` but also considers `c1`'s elevation, i.e.,
if `c2` is adjacent to `above(c1)` or `below(c1)`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is either directly north, east, south, or west of
    `c1` or `c1`'s elevations.)doc";

static const char *__doc_fiction_cartesian_layout_is_adjacent_of =
R"doc(Returns `true` iff coordinate `c2` is either directly north, east,
south, or west of coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is either directly north, east, south, or west of
    `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_at_any_border =
R"doc(Returns whether the given coordinate is located at any of the layout's
borders where x or y are either minimal or maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at any of the layout's borders.)doc";

static const char *__doc_fiction_cartesian_layout_is_at_eastern_border =
R"doc(Returns whether the given coordinate is located at the layout's
eastern border where x is maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's northern border.)doc";

static const char *__doc_fiction_cartesian_layout_is_at_northern_border =
R"doc(Returns whether the given coordinate is located at the layout's
northern border where y is minimal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's northern border.)doc";

static const char *__doc_fiction_cartesian_layout_is_at_southern_border =
R"doc(Returns whether the given coordinate is located at the layout's
southern border where y is maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's southern border.)doc";

static const char *__doc_fiction_cartesian_layout_is_at_western_border =
R"doc(Returns whether the given coordinate is located at the layout's
western border where x is minimal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's western border.)doc";

static const char *__doc_fiction_cartesian_layout_is_below =
R"doc(Returns `true` iff coordinate `c2` is directly below coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly below `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_crossing_layer =
R"doc(Returns whether the given coordinate is located in a crossing layer
where z is not minimal.

Parameter ``c``:
    Coordinate to check for elevation.

Returns:
    `true` iff `c` is in a crossing layer.)doc";

static const char *__doc_fiction_cartesian_layout_is_east_of =
R"doc(Returns `true` iff coordinate `c2` is directly east of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly east of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_eastwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere east of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere east of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_ground_layer =
R"doc(Returns whether the given coordinate is located in the ground layer
where z is minimal.

Parameter ``c``:
    Coordinate to check for elevation.

Returns:
    `true` iff `c` is in ground layer.)doc";

static const char *__doc_fiction_cartesian_layout_is_north_of =
R"doc(Returns `true` iff coordinate `c2` is directly north of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly north of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_northwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere north of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere north of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_south_of =
R"doc(Returns `true` iff coordinate `c2` is directly south of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly south of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_southwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere south of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere south of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_west_of =
R"doc(Returns `true` iff coordinate `c2` is directly west of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly west of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_westwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere west of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere west of `c1`.)doc";

static const char *__doc_fiction_cartesian_layout_is_within_bounds =
R"doc(Returns whether the given coordinate is located within the layout
bounds.

Parameter ``c``:
    Coordinate to check for boundary.

Returns:
    `true` iff `c` is located within the layout bounds.)doc";

static const char *__doc_fiction_cartesian_layout_north =
R"doc(Returns the coordinate that is directly adjacent in northern direction
of a given coordinate `c`, i.e., the face whose y-dimension is lower
by 1. If `c`'s y-dimension is already at minimum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose northern counterpart is desired.

Returns:
    Coordinate adjacent and north of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_north_east =
R"doc(Returns the coordinate that is located in north-eastern direction of a
given coordinate `c`, i.e., the face whose x-dimension is higher by 1
and whose y-dimension is lower by 1. If `c`'s x-dimension is already
at maximum or `c`'s y-dimension is already at minimum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose north-eastern counterpart is desired.

Returns:
    Coordinate directly north-eastern of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_north_west =
R"doc(Returns the coordinate that is located in north-western direction of a
given coordinate `c`, i.e., the face whose x-dimension and y-dimension
are lower by 1. If `c`'s x-dimension or y-dimension are already at
minimum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose north-western counterpart is desired.

Returns:
    Coordinate directly north-western of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_northern_border_of =
R"doc(Returns the coordinate with the same x and z values as a given
coordinate but that is located at the layout's northern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The northern border equivalent of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_resize =
R"doc(Updates the layout's dimensions, effectively resizing it.

Parameter ``ar``:
    New aspect ratio.)doc";

static const char *__doc_fiction_cartesian_layout_south =
R"doc(Returns the coordinate that is directly adjacent in southern direction
of a given coordinate `c`, i.e., the face whose y-dimension is higher
by 1. If `c`'s y-dimension is already at maximum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose southern counterpart is desired.

Returns:
    Coordinate adjacent and south of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_south_east =
R"doc(Returns the coordinate that is located in south-eastern direction of a
given coordinate `c`, i.e., the face whose x-dimension and y-dimension
are higher by 1. If `c`'s x-dimension or y-dimension are already at
maximum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose south-eastern counterpart is desired.

Returns:
    Coordinate directly south-eastern of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_south_west =
R"doc(Returns the coordinate that is located in south-western direction of a
given coordinate `c`, i.e., the face whose x-dimension is lower by 1
and whose y-dimension is higher by 1. If `c`'s x-dimension is already
at minimum or `c`'s y-dimension is already at maximum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose south-western counterpart is desired.

Returns:
    Coordinate directly south-western of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_southern_border_of =
R"doc(Returns the coordinate with the same x and z values as a given
coordinate but that is located at the layout's southern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The southern border equivalent of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_strg = R"doc()doc";

static const char *__doc_fiction_cartesian_layout_west =
R"doc(Returns the coordinate that is directly adjacent in western direction
of a given coordinate `c`, i.e., the face whose x-dimension is lower
by 1. If `c`'s x-dimension is already at minimum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose western counterpart is desired.

Returns:
    Coordinate adjacent and west of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_western_border_of =
R"doc(Returns the coordinate with the same y and z values as a given
coordinate but that is located at the layout's western border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The western border equivalent of `c`.)doc";

static const char *__doc_fiction_cartesian_layout_x =
R"doc(Returns the layout's x-dimension, i.e., returns the biggest x-value
that still belongs to the layout.

Returns:
    x-dimension.)doc";

static const char *__doc_fiction_cartesian_layout_y =
R"doc(Returns the layout's y-dimension, i.e., returns the biggest y-value
that still belongs to the layout.

Returns:
    y-dimension.)doc";

static const char *__doc_fiction_cartesian_layout_z =
R"doc(Returns the layout's z-dimension, i.e., returns the biggest z-value
that still belongs to the layout.

Returns:
    z-dimension.)doc";

static const char *__doc_fiction_cds_configuration = R"doc(An enumeration of modes for the charge distribution surface.)doc";

static const char *__doc_fiction_cds_configuration_CHARGE_LOCATION_AND_ELECTROSTATIC =
R"doc(In addition to the location of the charge states, the electrostatic
computation is conducted and stored.)doc";

static const char *__doc_fiction_cds_configuration_CHARGE_LOCATION_ONLY =
R"doc(The charge distribution is exclusively used to store the charge
states.)doc";

static const char *__doc_fiction_cell_level_layout =
R"doc(A layout type to layer on top of a clocked layout that allows the
assignment of individual cells to clock zones in accordance with an
FCN technology, e.g., QCA, iNML, or SiDB. This type, thereby,
represents layouts on a cell-accurate abstraction without a notion of
logic functions. Gate libraries can be used to transform gate-level
layouts into cell-level ones. Furthermore, cell-level layouts can be
written to files for various physical simulators like QCADesigner,
ToPoliNano & MagCAD, SiQAD, etc.

In this layout, each coordinate, i.e., clock zone has the dimensions
of a single cell. Clock numbers can, however, be assigned in a way,
that they form larger zones, e.g., of :math:`5 \times 5` cells. These
dimensions can be specified in the constructor. They affect the way,
clock numbers are fetched from the underlying clocked layout.

The de-facto standard of cell-level FCN design is to group multiple
cells into tiles large enough to be addressable by individual clocking
electrodes buried in the layout substrate. Cell-based clocking, i.e.,
clock zones of size :math:`1 \times 1` cells are not recommended as
they are most likely not fabricable in reality.

On the implementation side, this layout distinguishes between `cell`,
`cell_type`, and `cell_mode`. A `cell` is a coordinate, i.e., a
position on the layout where a `cell_type` can be assigned. A
`cell_type` is a concrete variation of a fabricated cell and depends
on the given technology. QCA offers regular and constant cell types
while SiDB only provides regular ones. Cell types can also include
primary input and output cells if they are being treated differently
in a simulator for instance. A `cell_mode`, on the other hand, is a
variation of a cell (thus far only known from QCADesigner) that
provides further attributes like its functionality as a crossing or
via cell.

Template parameter ``Technology``:
    An FCN technology that provides notions of cell types.

Template parameter ``ClockedLayout``:
    The clocked layout that is to be extended by cell positions.)doc";

static const char *__doc_fiction_cell_level_layout_assign_cell_mode =
R"doc(Assigns a cell mode `m` to a cell position `c` in the layout. If `m`
is the normal cell mode, a potentially stored cell mode is being
erased.

Parameter ``c``:
    Cell position to assign cell mode `m` to.

Parameter ``m``:
    Cell mode to assign to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_assign_cell_name =
R"doc(Assigns a cell name `n` to a cell position `c` in the layout. If `n`
is the empty string, a potentially stored cell name is being erased.

Parameter ``c``:
    Cell position to assign cell name `n` to.

Parameter ``n``:
    Cell name to assign to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_assign_cell_type =
R"doc(Assigns a cell type `ct` to a cell position `c` in the layout. If `ct`
is the empty cell, a potentially stored cell type is being erased. If
`ct` is a primary input or output type, the number of primary inputs
or outputs in the layout is increased respectively.

Parameter ``c``:
    Cell position.

Parameter ``ct``:
    Cell type to assign to `c`.)doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout =
R"doc(Standard constructor. Creates a named cell-level layout of the given
aspect ratio. To this end, it calls `ClockedLayout`'s standard
constructor.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``name``:
    Layout name.

Parameter ``tile_size_x``:
    Clock zone size in x-dimension in cells.

Parameter ``tile_size_y``:
    Clock zone size in y-dimension in cells.)doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_2 =
R"doc(Standard constructor. Creates a named cell-level layout of the given
aspect ratio and clocks it via the given clocking scheme. To this end,
it calls `ClockedLayout`'s standard constructor.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``scheme``:
    Clocking scheme to apply to this layout.

Parameter ``name``:
    Layout name.

Parameter ``tile_size_x``:
    Clock zone size in x-dimension in cells.

Parameter ``tile_size_y``:
    Clock zone size in y-dimension in cells.)doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_3 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another cell_level_layout.)doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_4 =
R"doc(Copy constructor from another `ClockedLayout`.

Parameter ``lyt``:
    Clocked layout.)doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_cell_level_layout_storage = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_cell_mode_map = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_cell_name_map = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_cell_type_map = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_inputs = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_layout_name = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_outputs = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_tile_size_x = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_cell_level_layout_storage_tile_size_y = R"doc()doc";

static const char *__doc_fiction_cell_level_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_cell_level_layout_foreach_cell =
R"doc(Applies a function to all cell positions in the layout that have non-
empty cell types assigned.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Parameter ``fn``:
    Functor to apply to each non-empty cell position.)doc";

static const char *__doc_fiction_cell_level_layout_foreach_cell_position =
R"doc(Applies a function to all cell positions in the layout, even empty
ones. This function, thereby, renames
`ClockedLayout::foreach_coordinate`.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    the functor type in `ClockedLayout::foreach_coordinate`.

Parameter ``fn``:
    Functor to apply to each cell position.)doc";

static const char *__doc_fiction_cell_level_layout_foreach_pi =
R"doc(Applies a function to all primary input cell positions in the layout.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Parameter ``fn``:
    Functor to apply to each primary input cell.)doc";

static const char *__doc_fiction_cell_level_layout_foreach_po =
R"doc(Applies a function to all primary output cells in the layout.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Parameter ``fn``:
    Functor to apply to each primary output cell.)doc";

static const char *__doc_fiction_cell_level_layout_get_cell_mode =
R"doc(Returns the cell mode assigned to cell position `c`. If no cell mode
is assigned, the default mode is returned.

Parameter ``c``:
    Cell position whose assigned cell mode is desired.

Returns:
    Cell mode assigned to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_get_cell_name =
R"doc(Returns the cell name assigned to cell position `c`. If no cell name
is assigned, the empty string is returned.

Parameter ``c``:
    Cell position whose assigned cell name is desired.

Returns:
    Cell name assigned to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_get_cell_type =
R"doc(Returns the cell type assigned to cell position `c`.

Parameter ``c``:
    Cell position whose assigned cell type is desired.

Returns:
    Cell type assigned to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_get_cells_by_type =
R"doc(Returns all cells of the given type.

Parameter ``type``:
    Type of cells to return.

Returns:
    All cells of the layout that have the given type.)doc";

static const char *__doc_fiction_cell_level_layout_get_clock_number =
R"doc(Returns the clock number of cell position `c` by accessing
`ClockedLayout`'s underlying clocking scheme and respecting this
layout's clock zone size.

Parameter ``c``:
    Cell position whose clock number is desired.

Returns:
    Clock number of cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_get_layout_name =
R"doc(Returns the assigned layout name.

Returns:
    The layout name.)doc";

static const char *__doc_fiction_cell_level_layout_get_tile_size_x =
R"doc(Returns the underlying clock zone x-dimension size. That is, if this
cell-level layout was obtained from the application of a gate library,
this function returns the cell size in x-dimension of each gate in the
library.

Returns:
    The clock zone size in cells in the x-dimension.)doc";

static const char *__doc_fiction_cell_level_layout_get_tile_size_y =
R"doc(Returns the underlying clock zone y-dimension size. That is, if this
cell-level layout was obtained from the application of a gate library,
this function returns the cell size in y-dimension of each gate in the
library.

Returns:
    The clock zone size in cells in the y-dimension.)doc";

static const char *__doc_fiction_cell_level_layout_is_empty =
R"doc(Checks whether there are no cells assigned to the layout's
coordinates.

Returns:
    `true` iff the layout is empty.)doc";

static const char *__doc_fiction_cell_level_layout_is_empty_cell =
R"doc(Returns `true` if no cell type is assigned to cell position `c` or if
the empty type was assigned.

Parameter ``c``:
    Cell position to check for emptiness.

Returns:
    `true` iff no cell type was assigned to cell position `c`.)doc";

static const char *__doc_fiction_cell_level_layout_is_incoming_clocked = R"doc(Function is deleted for cell-level layouts.)doc";

static const char *__doc_fiction_cell_level_layout_is_outgoing_clocked = R"doc(Function is deleted for cell-level layouts.)doc";

static const char *__doc_fiction_cell_level_layout_is_pi =
R"doc(Checks whether a given cell position is marked as primary input. This
function does not check against the assigned cell type but whether the
cell position is stored in the list of PIs. Assigning a cell position
the primary input cell type automatically enlists it there.

Parameter ``c``:
    Cell position to check.

Returns:
    `true` iff cell position `c` is marked as primary input.)doc";

static const char *__doc_fiction_cell_level_layout_is_po =
R"doc(Checks whether a given cell position is marked as primary output. This
function does not check against the assigned cell type but whether the
cell position is stored in the list of POs. Assigning a cell position
the primary output cell type automatically enlists it there.

Parameter ``c``:
    Cell position to check.

Returns:
    `true` iff cell position `c` is marked as primary output.)doc";

static const char *__doc_fiction_cell_level_layout_num_cells =
R"doc(Returns the number of non-empty cell types that were assigned to the
layout.

Returns:
    Number of non-empty cell types in the layout.)doc";

static const char *__doc_fiction_cell_level_layout_num_cells_of_given_type =
R"doc(Returns the numbers of cells of the given type.

Parameter ``type``:
    Type of cells which are counted.

Returns:
    Number of the cells with the given type.)doc";

static const char *__doc_fiction_cell_level_layout_num_pis =
R"doc(Returns the number of primary input cells in the layout.

Returns:
    Number of primary input cells.)doc";

static const char *__doc_fiction_cell_level_layout_num_pos =
R"doc(Returns the number of primary output cells in the layout.

Returns:
    Number of primary output cells.)doc";

static const char *__doc_fiction_cell_level_layout_set_layout_name =
R"doc(Assigns or overrides the layout name.

Parameter ``name``:
    Layout name to assign.)doc";

static const char *__doc_fiction_cell_level_layout_set_tile_size_x =
R"doc(Sets the underlying clock zone x-dimension size.

Parameter ``tile_size_x``:
    Tile size in the x-dimension in number of cells.)doc";

static const char *__doc_fiction_cell_level_layout_set_tile_size_y =
R"doc(Sets the underlying clock zone y-dimension size.

Parameter ``tile_size_y``:
    Tile size in the y-dimension in number of cells.)doc";

static const char *__doc_fiction_cell_level_layout_strg = R"doc()doc";

static const char *__doc_fiction_cfe_clocking =
R"doc(Returns the CFE clocking as defined in \"CFE: a convenient, flexible,
and efficient clocking scheme for quantum-dot cellular automata\" by
Feifei Deng, Guang-Jun Xie, Xin Cheng, Zhang Zhang, and Yongqiang
Zhang in IET Circuits, Devices & Systems 2020.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    CFE clocking scheme.)doc";

static const char *__doc_fiction_charge_configuration_to_string =
R"doc(Converts a vector of charge states to a string representation
(`"-101..."`).

Parameter ``charge_distribution``:
    A vector of SiDBs charge states.

Returns:
    A string representation of the charge states.)doc";

static const char *__doc_fiction_charge_distribution_history =
R"doc(An enumeration of modes to decide if the previous charge distribution
is used to simply the computation of the properties of a new charge
distribution.)doc";

static const char *__doc_fiction_charge_distribution_history_CONSIDER = R"doc(The previous charge distribution is used.)doc";

static const char *__doc_fiction_charge_distribution_history_NEGLECT =
R"doc(The previous charge distribution is not used. Hence, the local
electrostatic potential of the given charge distribution is calculated
from scratch.)doc";

static const char *__doc_fiction_charge_distribution_mode =
R"doc(An enumeration of modes for handling the charge distribution when
assigning a charge index to the charge distribution surface.)doc";

static const char *__doc_fiction_charge_distribution_mode_KEEP_CHARGE_DISTRIBUTION =
R"doc(The charge distribution is kept and is not updated after a charge
index is assigned to the charge distribution surface.)doc";

static const char *__doc_fiction_charge_distribution_mode_UPDATE_CHARGE_DISTRIBUTION =
R"doc(The charge distribution is updated after the charge index is assigned
to the charge distribution surface.)doc";

static const char *__doc_fiction_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_2 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_3 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_4 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_5 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_charge_index_mode =
R"doc(An enumeration of modes for handling the charge index during charge
state assignment.)doc";

static const char *__doc_fiction_charge_index_mode_KEEP_CHARGE_INDEX =
R"doc(The charge state is assigned to the cell but the old charge index is
kept.)doc";

static const char *__doc_fiction_charge_index_mode_UPDATE_CHARGE_INDEX =
R"doc(The charge state is assigned to the cell and the charge index is
updated.)doc";

static const char *__doc_fiction_charge_index_recomputation =
R"doc(An enumeration of modes to specifying if the charge index should be
recomputed fully.)doc";

static const char *__doc_fiction_charge_index_recomputation_FROM_SCRATCH = R"doc(The charge index is recomputed from scratch.)doc";

static const char *__doc_fiction_charge_index_recomputation_IGNORE_LEADING_ZEROES =
R"doc(The charge index is recomputed with the leading zeroes ignored. This
optimization can be applied if we know that the charge index was
incremented after the last charge index computation.)doc";

static const char *__doc_fiction_charge_state_to_sign =
R"doc(Converts the charge state into an integer (`-1`, `0`, `1`).

Parameter ``cs``:
    SiDB charge state.

Returns:
    Integer representing the SiDB's charge state.)doc";

static const char *__doc_fiction_chebyshev_distance =
R"doc(The Chebyshev distance :math:`D` between two layout coordinates
:math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

:math:`D = \max(|x_2 - x_1|, |y_2 - y_1|)`

In contrast to the Manhattan distance, this function assumes the same
cost for diagonal moves as it does for horizontal and vertical ones.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral type for the distance.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Chebyshev distance between `source` and `target`.)doc";

static const char *__doc_fiction_chebyshev_distance_functor =
R"doc(A pre-defined distance functor that uses the Chebyshev distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral distance type.)doc";

static const char *__doc_fiction_chebyshev_distance_functor_chebyshev_distance_functor = R"doc()doc";

static const char *__doc_fiction_check_simulation_results_for_equivalence =
R"doc(This function compares two SiDB simulation results for equivalence.
Two results are considered equivalent if they have the same number of
charge distributions and if each corresponding charge distribution has
the same electrostatic potential energy and charge states for all
cells.

Template parameter ``Lyt``:
    The SiDB cell-level layout type used in the simulation results.

Parameter ``result1``:
    The first SiDB simulation result to compare.

Parameter ``result2``:
    The second SiDB simulation result to compare.

Returns:
    `true` if the two simulation results are equivalent, `false`
    otherwise.)doc";

static const char *__doc_fiction_clear_routing =
R"doc(Removes the entire wire routing from the passed layout. This involves
deleting all wire segments that have been placed on any tile as well
as removing stored connections (children pointers) from all gates.

Template parameter ``Lyt``:
    Gate-level Layout type.

Parameter ``lyt``:
    The layout whose routing is to be deleted.)doc";

static const char *__doc_fiction_clocked_layout =
R"doc(A layout type to layer on top of a coordinate layout, e.g.,
`cartesian_layout`, `hexagonal_layout`, or `tile_based_layout`. This
type extends the layout by providing a notion of FCN clocking. To this
end, it utilizes a clocking scheme that assigns each coordinate in the
extended coordinate layout a clock number. These clock numbers can be
manually overwritten if necessary.

In the context of this layout type, coordinates are renamed as clock
zones.

Template parameter ``CoordinateLayout``:
    The coordinate layout type whose coordinates should be clocked.)doc";

static const char *__doc_fiction_clocked_layout_assign_clock_number =
R"doc(Overrides a clock number in the stored scheme with the provided one.

Parameter ``cz``:
    Clock zone to override.

Parameter ``cn``:
    New clock number for `cz`.)doc";

static const char *__doc_fiction_clocked_layout_clocked_layout =
R"doc(Standard constructor. Creates a clocked layout of the given aspect
ratio and clocks it via the irregular 'open' clocking scheme. This
scheme is intended to be used if all clock zones are to be manually
assigned.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_2 =
R"doc(Standard constructor. Creates a clocked layout of the given aspect
ratio and clocks it via the given clocking scheme.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``scheme``:
    Clocking scheme to apply to this layout.)doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_3 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another clocked_layout.)doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_4 =
R"doc(Copy constructor from another `CoordinateLayout`.

Parameter ``lyt``:
    Coordinate layout.)doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_storage = R"doc()doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_storage_clocked_layout_storage = R"doc()doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_storage_clocking = R"doc()doc";

static const char *__doc_fiction_clocked_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_clocked_layout_degree =
R"doc(Returns the number of incoming plus outgoing clock zones of the given
one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s incoming plus outgoing clock zones.)doc";

static const char *__doc_fiction_clocked_layout_foreach_incoming_clocked_zone =
R"doc(Applies a function to all incoming clock zones of a given one.

Template parameter ``Fn``:
    Functor type.

Parameter ``cz``:
    Base clock zone.

Parameter ``fn``:
    Functor to apply to each of `cz`'s incoming clock zones.)doc";

static const char *__doc_fiction_clocked_layout_foreach_outgoing_clocked_zone =
R"doc(Applies a function to all outgoing clock zones of a given one.

Template parameter ``Fn``:
    Functor type.

Parameter ``cz``:
    Base clock zone.

Parameter ``fn``:
    Functor to apply to each of `cz`'s outgoing clock zones.)doc";

static const char *__doc_fiction_clocked_layout_get_clock_number =
R"doc(Returns the clock number for the given clock zone.

Parameter ``cz``:
    Clock zone.

Returns:
    Clock number of `cz`.)doc";

static const char *__doc_fiction_clocked_layout_get_clocking_scheme =
R"doc(Returns a copy of the stored clocking scheme object.

Returns:
    A copy of the stored clocking scheme object.)doc";

static const char *__doc_fiction_clocked_layout_in_degree =
R"doc(Returns the number of incoming clock zones to the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s incoming clock zones.)doc";

static const char *__doc_fiction_clocked_layout_incoming_clocked_zones =
R"doc(Returns a container with all clock zones that are incoming to the
given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    A container with all clock zones that are incoming to `cz`.)doc";

static const char *__doc_fiction_clocked_layout_is_clocking_scheme =
R"doc(Compares the stored clocking scheme against the provided name. Names
of pre-defined clocking schemes are given in the `clock_name`
namespace.

Parameter ``name``:
    Clocking scheme name.

Returns:
    `true` iff the layout is clocked by a clocking scheme of name
    `name`.)doc";

static const char *__doc_fiction_clocked_layout_is_incoming_clocked =
R"doc(Evaluates whether clock zone `cz2` feeds information to clock zone
`cz1`, i.e., whether `cz2` is clocked with a clock number that is
lower by 1 modulo `num_clocks()`.

Parameter ``cz1``:
    Base clock zone.

Parameter ``cz2``:
    Clock zone to check whether its clock number is lower by 1.

Returns:
    `true` iff `cz2` can feed information to `cz1`.)doc";

static const char *__doc_fiction_clocked_layout_is_outgoing_clocked =
R"doc(Evaluates whether clock zone `cz2` accepts information from clock zone
`cz1`, i.e., whether `cz2` is clocked with a clock number that is
higher by 1 modulo `num_clocks()`.

Parameter ``cz1``:
    Base clock zone.

Parameter ``cz2``:
    Clock zone to check whether its clock number is higher by 1.

Returns:
    `true` iff `cz2` can accept information from `cz1`.)doc";

static const char *__doc_fiction_clocked_layout_is_regularly_clocked =
R"doc(Returns whether the layout is clocked by a regular clocking scheme
with no overwritten zones.

Returns:
    `true` iff the layout is clocked by a regular scheme and no zones
    have been overwritten.)doc";

static const char *__doc_fiction_clocked_layout_num_clocks =
R"doc(Returns the number of clock phases in the layout. Each clock cycle is
divided into n phases. In QCA, the number of phases is usually 4. In
iNML it is 3. However, theoretically, any number >= 3 can be utilized.

Returns:
    The number of different clock signals in the layout.)doc";

static const char *__doc_fiction_clocked_layout_out_degree =
R"doc(Returns the number of outgoing clock zones from the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s outgoing clock zones.)doc";

static const char *__doc_fiction_clocked_layout_outgoing_clocked_zones =
R"doc(Returns a container with all clock zones that are outgoing from the
given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    A container with all clock zones that are outgoing from `cz`.)doc";

static const char *__doc_fiction_clocked_layout_replace_clocking_scheme =
R"doc(Replaces the stored clocking scheme with the provided one.

Parameter ``scheme``:
    New clocking scheme.)doc";

static const char *__doc_fiction_clocked_layout_strg = R"doc()doc";

static const char *__doc_fiction_clocking_scheme =
R"doc(Clocking scheme type that assigns a clock number to each element of
the provided type `ClockZone`. Clocking scheme objects are utilized,
e.g., in clocked_layout.

Usually, a clocking scheme is defined by the means of a cutout that
can be seamlessly extended in all directions to provide repeating
clock numbers.

Many regular clocking schemes have been proposed in the literature.
Some are pre-defined below.

Clocking schemes are uniquely identified via their name.

Template parameter ``ClockZone``:
    Clock zone type. Usually, a coordinate type in a layout.)doc";

static const char *__doc_fiction_clocking_scheme_clocking_scheme =
R"doc(Standard constructor.

Parameter ``n``:
    The clocking scheme's name. The name is utilized as the key to
    uniquely identify a scheme.

Parameter ``f``:
    A function that assigns a clock number to each clock zone.

Parameter ``in_deg``:
    Maximum possible in-degree in the provided scheme.

Parameter ``out_deg``:
    Maximum possible out-degree in the provided scheme.

Parameter ``cn``:
    Number of clock phases that make up one clock cycle, i.e., the
    number of different clock numbers.

Parameter ``r``:
    Flag to identify the scheme as regular.)doc";

static const char *__doc_fiction_clocking_scheme_fn = R"doc(A function that determines clock numbers for given zones.)doc";

static const char *__doc_fiction_clocking_scheme_is_regular =
R"doc(Checks for the clocking scheme's regularity.

Returns:
    `true` iff the clocking scheme is regular.)doc";

static const char *__doc_fiction_clocking_scheme_max_in_degree = R"doc(Maximum number of inputs the clocking scheme supports per clock zone.)doc";

static const char *__doc_fiction_clocking_scheme_max_out_degree = R"doc(Maximum number of outputs the clocking scheme supports per clock zone.)doc";

static const char *__doc_fiction_clocking_scheme_name = R"doc(Name of the clocking scheme.)doc";

static const char *__doc_fiction_clocking_scheme_num_clocks = R"doc(Number of different clocks in this scheme.)doc";

static const char *__doc_fiction_clocking_scheme_operator_call =
R"doc(Accesses the clock function to determine the clock number of the given
clock zone if the scheme is regular. Otherwise, the stored clock map
is accessed to look for a manually specified/overwritten clock number.
If none is found, the default one, usually 0, is returned.

Parameter ``cz``:
    Clock zone whose clock number is desired.

Returns:
    Clock number of cz.)doc";

static const char *__doc_fiction_clocking_scheme_operator_eq =
R"doc(Compares the stored name against a given one.

Parameter ``n``:
    Name to compare.

Returns:
    `true` iff the stored name is equal to n.)doc";

static const char *__doc_fiction_clocking_scheme_override = R"doc(Stores mappings clock_zone -> clock_number to override clock zones.)doc";

static const char *__doc_fiction_clocking_scheme_override_clock_number =
R"doc(Overrides a clock zone's clock number. The usage of this function
immediately labels the clocking scheme as irregular.

Parameter ``cz``:
    Clock zone to override.

Parameter ``cn``:
    Clock number to assign to cz.)doc";

static const char *__doc_fiction_clocking_scheme_regular = R"doc(Defines the clocking as regular and well-defined by the scheme.)doc";

static const char *__doc_fiction_clustercomplete =
R"doc(*ClusterComplete* is an instantiation of a general solution to
exhaustive state assignment searching for which all local predicates
hold, given respective local evaluations that may be aggregated from
individual inter-variable interactions. Applied to the problem of
exact physical simulation of SiDBs, it is able to efficiently consider
positive charges that are rare to occur, but drastically blow up exact
simulation runtimes when hierarchical pruning methods are not applied.
In fact, the exponential growth in problem complexity for added SiDBs
is tamed by *ClusterComplete*, as SiDB layouts to simulate in practise
amount to a high pruning efficacy, resulting in a layout-dependent
reduction of the simulation base. This amounts to an effective
simulation base in the real number range :math:`[1,b]`, where
:math:`b\in\{2,3\}` is the given simulation base.

The part of the *ClusterComplete* algorithm that is implemented in
this file is the destructive phase of the procedure that employs the
duality of construction and destruction, folding and unfolding. The
phase preceding it is the key ingredient to the achieved efficiency:
the *Ground State Space* algorithm, which constructs a minimized
hierarchical search space of charge configurations that adhere to the
critical population stability criterion. In particular, it generalizes
physically informed space pruning that contributes to the capabilities
of the *QuickExact* simulator, now applying to all charge states
equally, and, most importantly, it lifts the associated potential
equations to higher order, allowing us to reason over potential bounds
in a cluster hierarchy.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to simulate.

Parameter ``params``:
    Parameter required for both the invocation of *Ground State
    Space*, and the simulation following.

Returns:
    Simulation results.)doc";

static const char *__doc_fiction_clustercomplete_params =
R"doc(The struct containing the parameters both passed on to pre-simulator
Ground State Space, and used during simulation.)doc";

static const char *__doc_fiction_clustercomplete_params_available_threads =
R"doc(Number of threads to make available to *ClusterComplete* for the
unfolding stage.)doc";

static const char *__doc_fiction_clustercomplete_params_global_potential =
R"doc(Global external electrostatic potential. Value is applied on each cell
in the layout.)doc";

static const char *__doc_fiction_clustercomplete_params_ground_state_space_reporting =
R"doc(This enum class provides meaningful options for configuring the
reporting of the *Ground State Space* statistics. These statistic may
be used especially to configure the validity witness partitioning
options for *Ground State Space*, that may impair runtimes when set
too high, but could provide a large benefit to the complexity of the
unfolding process of large simulation problems by performing more
involved pruning procedures in the construction stage.)doc";

static const char *__doc_fiction_clustercomplete_params_ground_state_space_reporting_OFF =
R"doc(Disabling this option will suppress the output of *Ground State Space*
statistics.)doc";

static const char *__doc_fiction_clustercomplete_params_ground_state_space_reporting_ON =
R"doc(Enabling this option will output *Ground State Space* statistics to
the standard output.)doc";

static const char *__doc_fiction_clustercomplete_params_local_external_potential =
R"doc(Local external electrostatic potentials (e.g., locally applied
electrodes).)doc";

static const char *__doc_fiction_clustercomplete_params_num_overlapping_witnesses_limit_gss =
R"doc(The complexity is of validity witness partitioning bounded by a
factorial in the number of overlapping witnesses. This parameter thus
allows the validity witness partitioning procedure to perform the
reduction to overlapping witnesses for larger cluster sizes that could
be runtime-impairing, then limiting specifically the length of the
input to the factorial call.)doc";

static const char *__doc_fiction_clustercomplete_params_report_gss_stats =
R"doc(Option to decide if the *Ground State Space* statistics are reported
to the standard output. By default, this option is disabled.)doc";

static const char *__doc_fiction_clustercomplete_params_simulation_parameters = R"doc(Physical simulation parameters.)doc";

static const char *__doc_fiction_clustercomplete_params_validity_witness_partitioning_max_cluster_size_gss =
R"doc(This specifies the maximum cluster size for which *Ground State Space*
will solve an NP-complete sub-problem exhaustively. The sets of SiDBs
that witness local population stability for each respective charge
state may be partitioned into disjoint sets such that the number of
required witnesses for each respective charge state is satisfied. If
no such partition exists, the multiset charge configuration associated
with the requirements may be rejected.)doc";

static const char *__doc_fiction_color_routing =
R"doc(A multi-path signal routing approach based on coloring of edge
intersection graphs as originally proposed in \"Efficient Multi-Path
Signal Routing for Field-coupled Nanotechnologies\" by M. Walter and
R. Wille in NANOARCH 2022.

Given a gate-level layout and a set of routing objectives, this
algorithm tries to fulfill all objectives by routing several conflict-
free wire paths. To this end, a plethora of possible paths are
enumerated in the given layout and an edge-intersection graph of paths
on a grid (EPG) created from them. In an EPG, each vertex represents a
path and each edge conflicts between them. When two vertices are
connected by an edge, they cannot be routed together conflict-free in
the layout. To determine a (maximum) set of routable paths, a vertex
coloring is computed on the EPG. Finally, all vertices that are
colored identically can be routed together. The biggest such set is
applied to the layout.

Multiple parameters can be set to specify the behavior of the
algorithm. For instance, whether crossings should be enabled and
whether a partial routing should be conducted if not all objectives
could be fulfilled. Furthermore, the path enumeration and the coloring
can be parameterized in the first place. By default, all paths are
enumerated for each objective. While this guarantees completeness on
small layouts, it quickly becomes intractable. Therefore, a path limit
can be set that restricts the number of paths to the :math:`k`
shortest. Additionally, for the coloring process, SAT solving is used
by default, which, again, guarantees completeness, but becomes
infeasible rather quickly. However, powerful symmetry breaking is
applied that assists the solving process, e.g., it is known that all
vertices that are belonging to the same objective are forming a
clique, which can be pre-colored. Additionally, lexicographical
orderings are enforced. For a more scalable (yet incomplete) approach,
several coloring heuristics are available, from which can be chosen
(see determine_vertex_coloring).

This function will return `true` if all objectives could be satisfied
or if the partial routing parameter was set. In the case of `true`
being returned, all determined paths have been routed in the given
layout.

Template parameter ``Lyt``:
    The gate-level layout type to route.

Parameter ``lyt``:
    A gate-level layout to route.

Parameter ``objectives``:
    The routing objectives as source-target pairs to fulfill.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    `true` iff routing was successful, i.e., iff all objectives could
    be satisfied.)doc";

static const char *__doc_fiction_color_routing_params = R"doc(Parameters for the color routing algorithm.)doc";

static const char *__doc_fiction_color_routing_params_conduct_partial_routing =
R"doc(Do not abort if some objectives cannot be fulfilled, but partially
route the layout as much as possible.)doc";

static const char *__doc_fiction_color_routing_params_crossings = R"doc(Enable crossings.)doc";

static const char *__doc_fiction_color_routing_params_engine = R"doc(The engine to use.)doc";

static const char *__doc_fiction_color_routing_params_partial_sat = R"doc(Allow partial solutions when the SAT engine is used.)doc";

static const char *__doc_fiction_color_routing_params_path_limit =
R"doc(If a value is given, for each objective, only up to the `path_limit`
shortest paths will be enumerated (using Yen's algorithm) instead of
all paths.)doc";

static const char *__doc_fiction_color_routing_stats = R"doc()doc";

static const char *__doc_fiction_color_routing_stats_color_stats = R"doc(Statistics of the vertex coloring.)doc";

static const char *__doc_fiction_color_routing_stats_duration = R"doc(Runtime measurement.)doc";

static const char *__doc_fiction_color_routing_stats_epg_stats = R"doc(Statistics of the edge intersection graph generation.)doc";

static const char *__doc_fiction_color_routing_stats_number_of_unsatisfied_objectives =
R"doc(For each routing objective that was not fulfilled by the coloring
engine, this counter is incremented.)doc";

static const char *__doc_fiction_color_view_drawer =
R"doc(A DOT drawer for networks with colored nodes. Node colors represent
their painted color instead of their gate type.

Template parameter ``Ntk``:
    Logic network type.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_color_view_drawer_node_fillcolor = R"doc()doc";

static const char *__doc_fiction_color_view_drawer_node_label = R"doc()doc";

static const char *__doc_fiction_columnar_clocking =
R"doc(Returns a linear 1D clocking as originally introduced in \"A device
architecture for computing with quantum dots\" by C. S. Lent and P. D.
Tougaw in the Proceedings of the IEEE 1997.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``n``:
    Number of clocks.

Returns:
    Columnar clocking scheme.)doc";

static const char *__doc_fiction_convert_array =
R"doc(Converts an array of size `N` and type `T` to an array of size `N` and
type `ElementType` by applying `static_cast` at compile time.

Template parameter ``ElementType``:
    New type of each element in the returned array.

Template parameter ``T``:
    Element type of the input array.

Template parameter ``N``:
    Size of both the input and the output array.

Parameter ``a``:
    The array to be converted.

Returns:
    An object of type `std::array<ElementType, N>` that was created by
    casting each element in `a` to `ElementType`.)doc";

static const char *__doc_fiction_convert_array_of_arrays =
R"doc(Same as `convert_array` but for 2D arrays.

Template parameter ``ElementType``:
    New type of each element in the returned array.

Template parameter ``T``:
    Element type of the input array.

Template parameter ``N``:
    Outer size of both the input and the output array.

Template parameter ``M``:
    Inner size of both the input and the output array.

Parameter ``a``:
    The array to be converted.

Returns:
    An object of type `std::array<std::array<ElementType, M>, N>` that
    was created by casting each element in `a` to `ElementType` using
    `static_cast`.)doc";

static const char *__doc_fiction_convert_layout_to_fiction_coordinates =
R"doc(Converts the coordinates of a given SiDB cell-level layout (cds and
defect surface can be layered on top) to alternative coordinates, such
as `offset::ucoord_t` or `cube::coord_t`. Returns a new layout
equivalent to the original layout but based on the specified
coordinate system.

Template parameter ``LytDest``:
    Source SiDB cell-level layout type.

Template parameter ``LytSrc``:
    Target SiDB cell-level layout type.

Parameter ``lyt``:
    The layout that is to be converted to a new layout based on
    fiction coordinates.

Returns:
    A new equivalent layout based on fiction coordinates.)doc";

static const char *__doc_fiction_convert_layout_to_siqad_coordinates =
R"doc(Converts the coordinates of a given cell-level layout (cds and defect
surface can be layered on top) to SiQAD coordinates. A new equivalent
layout based on SiQAD coordinates is returned.

Template parameter ``Lyt``:
    SiDB cell-level layout type based on fiction coordinates, e.g.,
    `offset::ucoord_t` or `cube::coord_t`.

Parameter ``lyt``:
    The layout that is to be converted to a new layout based on SiQAD
    coordinates.

Returns:
    A new equivalent layout based on SiQAD coordinates.)doc";

static const char *__doc_fiction_convert_network =
R"doc(Converts a logic network into an equivalent one of another type.
Thereby, this function is very similar to
`mockturtle::cleanup_dangling`. However, it supports real buffer nodes
used for fanouts and path balancing in fiction.

@note If `NtkDest` and `NtkSrc` are of the same type, this function
returns `ntk` cleaned using `mockturtle::cleanup_dangling`.

Template parameter ``NtkDest``:
    Type of the returned logic network.

Template parameter ``NtkSrc``:
    Type of the input logic network.

Parameter ``ntk``:
    The input logic network.

Returns:
    A logic network of type `NtkDest` that is logically equivalent to
    `ntk`.)doc";

static const char *__doc_fiction_coord_iterator =
R"doc(An iterator type that allows to enumerate coordinates in order within
a boundary.

Template parameter ``CoordinateType``:
    Type of coordinate to enumerate.)doc";

static const char *__doc_fiction_coord_iterator_aspect_ratio = R"doc()doc";

static const char *__doc_fiction_coord_iterator_coord = R"doc()doc";

static const char *__doc_fiction_coord_iterator_coord_iterator =
R"doc(Standard constructor. Initializes the iterator with a starting
position and the boundary within to enumerate.

With `dimension = (1, 2, 1)` and `start = (0, 0, 0)`, the following
order would be enumerated for offset or cubic coordinates:

- (0, 0, 0) - (1, 0, 0) - (0, 1, 0) - (1, 1, 0) - (0, 2, 0) - (1, 2,
0) - (0, 0, 1) - (1, 0, 1) - (0, 1, 1) - (1, 1, 1) - (0, 2, 1) - (1,
2, 1)

For SiQAD coordinates with the same parameters, we have the following
order of enumeration:

- (0, 0, 0) - (1, 0, 0) - (0, 0, 1) - (1, 0, 1) - (0, 1, 0) - (1, 2,
0) - (0, 1, 1) - (1, 1, 1) - (1, 1, 0) - (0, 2, 0) - (0, 2, 1) - (1,
2, 1)

coord_iterator is compatible with the STL forward_iterator category.
Does not iterate over negative coordinates.

Parameter ``dimension``:
    Boundary within to enumerate. Iteration wraps at its limits.

Parameter ``start``:
    Starting coordinate to enumerate first.)doc";

static const char *__doc_fiction_coord_iterator_operator_eq = R"doc()doc";

static const char *__doc_fiction_coord_iterator_operator_inc =
R"doc(Increments the iterator, while keeping it within the boundary. Also
defined on iterators that are out of bounds.

Returns:
    Reference to the incremented iterator.)doc";

static const char *__doc_fiction_coord_iterator_operator_inc_2 = R"doc()doc";

static const char *__doc_fiction_coord_iterator_operator_le = R"doc()doc";

static const char *__doc_fiction_coord_iterator_operator_lt = R"doc()doc";

static const char *__doc_fiction_coord_iterator_operator_mul = R"doc()doc";

static const char *__doc_fiction_coord_iterator_operator_ne = R"doc()doc";

static const char *__doc_fiction_cost_function_chi =
R"doc(Calculates the cost function :math:` \chi = \sum_{i=1} w_{i} \cdot
\chi_{i} ` by summing the product of normalized chi values :math:`
\chi_{i} ` and weights :math:` w_{i} `.

Parameter ``chis``:
    The vector containing the chi values.

Parameter ``weights``:
    The vector containing the weights.

Returns:
    The calculated cost function :math:` \chi(L) `.

Throws:
    std::invalid_argument If the sizes of chis and weights vectors are
    different.)doc";

static const char *__doc_fiction_cost_functor =
R"doc(A functor that computes costs between coordinates and can be passed as
an object to, e.g., path finding algorithms with a standardized
signature. This class is intended to be instantiated with concrete
cost functions passed to the constructor.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Cost``:
    Cost value type.)doc";

static const char *__doc_fiction_cost_functor_cost_function = R"doc(Cost function.)doc";

static const char *__doc_fiction_cost_functor_cost_functor =
R"doc(Standard constructor that instantiates the cost function.

Parameter ``cost_fn``:
    A function that maps from layout coordinates to a cost value.)doc";

static const char *__doc_fiction_cost_functor_operator_call =
R"doc(Operator to call the cost function.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Cost between source and target.)doc";

static const char *__doc_fiction_count_gate_types =
R"doc(Gives a detailed listing of all gate types present in the provided
network (or layout). This function can distinguish most gate types
available as atomic building blocks and can easily be extended to
support more gate types. The given network (or layout) has to
implement a function to test whether a node is of the respective gate
type.

Template parameter ``Ntk``:
    Logic network (or layout) type.

Parameter ``ntk``:
    The network (or layout).

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_count_gate_types_stats = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_and2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_and3 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_and_xor = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_buf = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_dot = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_fanout = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_gamble = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_ge2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_gt2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_inv = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_le2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_lt2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_maj3 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_mux = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_nand2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_nor2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_onehot = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_or2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_or_and = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_other = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_xnor2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_xor2 = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_num_xor_and = R"doc()doc";

static const char *__doc_fiction_count_gate_types_stats_report = R"doc()doc";

static const char *__doc_fiction_cp_and_tp = R"doc(Critical path length and throughput storage struct.)doc";

static const char *__doc_fiction_cp_and_tp_critical_path_length = R"doc(Length of the critical path in tiles.)doc";

static const char *__doc_fiction_cp_and_tp_throughput =
R"doc(Throughput of the layout in clock cycles as :math:`\frac{1}{x}` where
only :math:`x` is stored.)doc";

static const char *__doc_fiction_create_and3_tt =
R"doc(Creates and returns a truth table that implements the conjunction in
three variables.

Returns:
    Conjunction in three variables.)doc";

static const char *__doc_fiction_create_and_tt =
R"doc(Creates and returns a truth table that implements the conjunction in
two variables.

Returns:
    Conjunction in two variables.)doc";

static const char *__doc_fiction_create_and_xor_tt =
R"doc(Creates and returns a truth table that implements the AND-XOR function
(a xor b and c) in three variables.

Returns:
    AND-XOR in three variables.)doc";

static const char *__doc_fiction_create_array =
R"doc(Creates an array of size `N` and initializes its fields with value of
type `T` at compile time.

Template parameter ``N``:
    Array size.

Template parameter ``T``:
    Type of array.

Parameter ``value``:
    Initial value to each field.

Returns:
    An object of type `std::array<T, N>` that is initialized with `N`
    copies of value.)doc";

static const char *__doc_fiction_create_crossing_wire_tt =
R"doc(Creates and returns a vector of truth tables for a crossing wire
multi-output function.

This function generates a vector of truth tables, each representing
one of the outputs of a crossing wire multi-output function in two
variables. The function returns a vector containing two truth tables.

Returns:
    Vector of truth tables, each representing an output of the
    crossing wire function.)doc";

static const char *__doc_fiction_create_dot_tt =
R"doc(Creates and returns a truth table that implements the Dot function (a
xor (c or a and b)) in three variables.

Returns:
    Dot function in three variables.)doc";

static const char *__doc_fiction_create_double_wire_tt =
R"doc(Creates and returns a vector of truth tables for a double wire multi-
output function.

This function generates a vector of truth tables, each representing
one of the outputs of a double wire multi-output function in two
variables. The function returns a vector containing two truth tables.

Returns:
    Vector of truth tables, each representing an output of the double
    wire function.)doc";

static const char *__doc_fiction_create_fan_out_tt =
R"doc(Creates and returns a vector of truth tables for a multi-output
function with two variables.

This function generates a vector of truth tables, each representing
one of the outputs of a multi-output function in two variables.

Returns:
    Vector of truth tables, each representing an output of the
    identity function.)doc";

static const char *__doc_fiction_create_gamble_tt =
R"doc(Creates and returns a truth table that implements the Gamble function
(all or none of a,b,c) in three variables.

Returns:
    Gamble in three variables.)doc";

static const char *__doc_fiction_create_ge_tt =
R"doc(Creates and returns a truth table that implements the greater-than-or-
equal function in two variables.

Returns:
    Greater-than-or-equal function in two variables.)doc";

static const char *__doc_fiction_create_gt_tt =
R"doc(Creates and returns a truth table that implements the greater-than
function in two variables.

Returns:
    Greater-than function in two variables.)doc";

static const char *__doc_fiction_create_half_adder_tt =
R"doc(Creates and returns a vector of truth tables for a half adder multi-
output function.

This function generates a vector of truth tables, each representing
one of the outputs of a half adder multi-output function in two
variables. The function returns a vector containing two truth tables.

Returns:
    Vector of truth tables, each representing an output of the half
    adder function.)doc";

static const char *__doc_fiction_create_id_tt =
R"doc(Creates and returns a truth table that implements the identity
function in one variable.

Returns:
    Identity function in one variable.)doc";

static const char *__doc_fiction_create_ite_tt =
R"doc(Creates and returns a truth table that implements the ITE (MUX)
function (if a then b else c) in three variables.

Returns:
    ITE (MUX) in three variables.)doc";

static const char *__doc_fiction_create_le_tt =
R"doc(Creates and returns a truth table that implements the less-than-or-
equal function in two variables.

Returns:
    Less-than-or-equal function in two variables.)doc";

static const char *__doc_fiction_create_lt_tt =
R"doc(Creates and returns a truth table that implements the less-than
function in two variables.

Returns:
    Less-than function in two variables.)doc";

static const char *__doc_fiction_create_maj_tt =
R"doc(Creates and returns a truth table that implements the majority
function in three variables.

Returns:
    Majority function in three variables.)doc";

static const char *__doc_fiction_create_nand_tt =
R"doc(Creates and returns a truth table that implements the negated
conjunction in two variables.

Returns:
    Negated conjunction in two variables.)doc";

static const char *__doc_fiction_create_nor_tt =
R"doc(Creates and returns a truth table that implements the negated
disjunction in two variables.

Returns:
    Negated disjunction in two variables.)doc";

static const char *__doc_fiction_create_not_tt =
R"doc(Creates and returns a truth table that implements the negation in one
variable.

Returns:
    Negation in one variable.)doc";

static const char *__doc_fiction_create_onehot_tt =
R"doc(Creates and returns a truth table that implements the Onehot function
(exactly one of a,b,c) in three variables.

Returns:
    Onehot in three variables.)doc";

static const char *__doc_fiction_create_or_and_tt =
R"doc(Creates and returns a truth table that implements the OR-AND function
(a and (b or c)) in three variables.

Returns:
    OR-AND in three variables.)doc";

static const char *__doc_fiction_create_or_tt =
R"doc(Creates and returns a truth table that implements the disjunction in
two variables.

Returns:
    Disjunction in two variables.)doc";

static const char *__doc_fiction_create_xnor_tt =
R"doc(Creates and returns a truth table that implements the negated
exclusive disjunction in two variables.

Returns:
    Negated exclusive disjunction in two variables.)doc";

static const char *__doc_fiction_create_xor3_tt =
R"doc(Creates and returns a truth table that implements the exclusive
disjunction in three variables.

Returns:
    Exclusive disjunction in three variables.)doc";

static const char *__doc_fiction_create_xor_and_tt =
R"doc(Creates and returns a truth table that implements the XOR-AND function
(a and (b xor c)) in three variables.

Returns:
    XOR-AND in three variables.)doc";

static const char *__doc_fiction_create_xor_tt =
R"doc(Creates and returns a truth table that implements the exclusive
disjunction in two variables.

Returns:
    Exclusive disjunction in two variables.)doc";

static const char *__doc_fiction_critical_path_length_and_throughput =
R"doc(Computes the critical path length (CP) length and the throughput (TP)
of a gate-level layout.

The critical path length is defined as the longest path from any PI to
any PO in tiles.

The throughput is defined as :math:`\frac{1}{x}` where :math:`x` is
the highest path length difference between any sets of paths that lead
to the same gate. This function provides only the denominator
:math:`x`, as the numerator is always :math:`1`. Furthermore,
:math:`x` is given in clock cycles rather than clock phases because it
is assumed that a path length difference smaller than
`lyt.num_clocks()` does not lead to any delay. Contrary, for any
throughput value :math:`\frac{1}{x}` with :math:`x > 1`, the layout
computes its represented Boolean function only every :math:`x` full
clock cycles after the first inputs have been propagated through the
design. Thereby, all PIs need to be held constant for :math:`x` clock
phases to ensure proper computation.

For more information on the concept of throughput and delay see
\"Synchronization of Clocked Field-Coupled Circuits\" by F. Sill
Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO
2018; or \"Design Automation for Field-coupled Nanotechnologies\" by
M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
Springer Nature in 2022.

The complexity of this function is :math:`\mathcal{O}(|T|)` where
:math:`T` is the set of all occupied tiles in `lyt`.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The gate-level layout whose CP and TP are desired.

Returns:
    A struct containing the CP and TP.)doc";

static const char *__doc_fiction_critical_temperature_domain =
R"doc(The `critical_temperature_domain` class collects the critical
temperature and the operational status for a range of different
physical parameters of a given SiDB layout. It allows for the
evaluation of how the critical temperature depends on variations in
the underlying parameter points. This enables simulations to explore
the critical temperature's behavior across different conditions and
configurations.)doc";

static const char *__doc_fiction_critical_temperature_domain_add_dimension =
R"doc(Adds a dimension to sweep over. The first dimension is the x
dimension, the second dimension is the y dimension, etc.

Parameter ``param``:
    The dimension to add.)doc";

static const char *__doc_fiction_critical_temperature_domain_contour_tracing =
R"doc(Computes the critical temperature domain of the given SiDB cell-level
layout. The critical temperature domain consists of all parameter
combinations for which the layout is logically operational, along with
the critical temperature for each specific parameter point.nt.

This algorithm first uses random sampling to find a set of operational
point within the parameter range. From there, it traverses outwards to
find the edge of the operational area and performs Moore neighborhood
contour tracing to explore the contour of the operational domain. This
is repeated for all initially sampled points that do not lie within a
contour. The algorithm is guaranteed to determine the contours of all
operational "islands" if the initial random sampling found at least
one operational point within them. Thereby, this algorithm works for
disconnected operational domains. The critical temperature is computed
for each operational point.

It performs `samples` uniformly-distributed random samples within the
parameter range. For each thusly discovered operational island, it
performs another number of samples equal to the distance to an edge of
each operational area. Finally, it performs up to 8 samples for each
contour point (however, the actual number is usually lower). For each
sample, the algorithm performs one operational check on the layout,
where each operational check consists of up to :math:`2^n` exact
ground state simulations, where :math:`n` is the number of inputs of
the layout. Each exact ground state simulation has exponential
complexity in of itself. Therefore, the algorithm is only feasible for
small layouts with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The critical temperature domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_critical_temperature_domain_critical_temperature_domain = R"doc(Default constructor.)doc";

static const char *__doc_fiction_critical_temperature_domain_critical_temperature_domain_2 =
R"doc(Standard constructor.

Parameter ``dims``:
    Dimensions.)doc";

static const char *__doc_fiction_critical_temperature_domain_dimensions =
R"doc(The dimensions to sweep over, ordered by priority. The first dimension
is the x dimension, the second dimension is the y dimension, etc.)doc";

static const char *__doc_fiction_critical_temperature_domain_flood_fill =
R"doc(Computes the critical temperature domain of the given SiDB cell-level
layout. The critical temperature domain consists of all parameter
combinations for which the layout is logically operational, along with
the critical temperature for each specific parameter point.

This algorithm first uses random sampling to find several operational
points within the parameter range. From there, it employs the "flood
fill" algorithm to explore the operational domain. The algorithm is
guaranteed to determine all operational "islands" in their entirety if
the initial random sampling found at least one operational point
within them. Thereby, this algorithm works for disconnected
operational domains.

It performs `samples` uniformly-distributed random samples within the
parameter range. From there, it performs another number of samples
equal to the number of points within the operational domain plus the
first non-operational point in each direction. For each sample, the
algorithm performs one operational check on the layout, where each
operational check consists of up to :math:`2^n` exact ground state
simulations, where :math:`n` is the number of inputs of the layout.
Each exact ground state simulation has exponential complexity in of
itself. Therefore, the algorithm is only feasible for small layouts
with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The critical temperature domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_critical_temperature_domain_get_dimension =
R"doc(Returns a specific dimension by index.

Parameter ``index``:
    The index of the dimension to return.

Returns:
    The dimension at the specified index.

Throws:
    std::out_of_range if the index is out of range.)doc";

static const char *__doc_fiction_critical_temperature_domain_get_number_of_dimensions =
R"doc(Returns the number of dimensions to sweep over.

Returns:
    The number of dimensions to sweep over.)doc";

static const char *__doc_fiction_critical_temperature_domain_grid_search =
R"doc(Computes the critical temperature domain of the given SiDB cell-level
layout. The critical temperature domain consists of all parameter
combinations for which the layout is logically operational, along with
the critical temperature for each specific parameter point.

This algorithm uses a grid search to find the operational domain. The
grid search is performed by exhaustively sweeping the parameter space
in the x and y dimensions. Since grid search is exhaustive, the
algorithm is guaranteed to find the operational domain, if it exists
within the parameter range. However, the algorithm performs a
quadratic number of operational checks on the layout, where each
operational check consists of up to :math:`2^n` exact ground state
simulations, where :math:`n` is the number of inputs of the layout.
Each exact ground state simulation has exponential complexity in of
itself. Therefore, the algorithm is only feasible for small layouts
with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected vector of truth tables of the layout. Each truth table
    represents an output of the Boolean function.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The critical temperature domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_critical_temperature_domain_maximum_ct =
R"doc(Finds the maximum critical temperature in the domain.

Returns:
    The maximum critical temperature.)doc";

static const char *__doc_fiction_critical_temperature_domain_minimum_ct =
R"doc(Finds the minimum critical temperature in the domain.

Returns:
    The minimum critical temperature.)doc";

static const char *__doc_fiction_critical_temperature_domain_random_sampling =
R"doc(Computes the critical temperature domain of the given SiDB cell-level
layout. The critical temperature domain consists of all parameter
combinations for which the layout is logically operational, along with
the critical temperature for each specific parameter point.

This algorithm uses random sampling to find a part of the operational
domain that might not be complete. It performs a total of `samples`
uniformly-distributed random samples within the parameter range. For
each sample, the algorithm performs one operational check on the
layout, where each operational check consists of up to :math:`2^n`
exact ground state simulations, where :math:`n` is the number of
inputs of the layout. Each exact ground state simulation has
exponential complexity in of itself. Therefore, the algorithm is only
feasible for small layouts with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The critical temperature domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_critical_temperature_gate_based =
R"doc(This algorithm performs temperature-aware SiDB simulation as proposed
in \"Temperature Behavior of Silicon Dangling Bond Logic\" by J.
Drewniok, M. Walter, and R. Wille in IEEE NANO 2023
(https://ieeexplore.ieee.org/document/10231259). It comes in two
flavors: gate-based and non-gate based.

For *Gate-based Critical Temperature* Simulation, the Critical
Temperature is defined as follows: The temperature at which the
erroneous charge distributions are populated by more than :math:`1 -
\eta`, where :math:`\eta \in [0,1]`.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Simulation and physical parameters.

Parameter ``pst``:
    Statistics.

Returns:
    The critical temperature (unit: K).)doc";

static const char *__doc_fiction_critical_temperature_non_gate_based =
R"doc(For *Non-gate-based Critical Temperature* simulation, the Critical
Temperature is defined as follows: The temperature at which the
excited charge distributions are populated by more than :math:`1 -
\eta`, where :math:`\eta \in [0,1]` is the confidence level for the
presence of a working gate.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``params``:
    Simulation and physical parameters.

Parameter ``pst``:
    Statistics.

Returns:
    The critical temperature (unit: K))doc";

static const char *__doc_fiction_critical_temperature_params =
R"doc(This struct stores the parameters for the *Critical Temperature*`
algorithm.)doc";

static const char *__doc_fiction_critical_temperature_params_alpha =
R"doc(Alpha parameter for the *QuickSim* algorithm (only applicable if
engine == QUICKSIM).)doc";

static const char *__doc_fiction_critical_temperature_params_confidence_level =
R"doc(Probability threshold for ground state population. The temperature at
which the simulation finds the ground state to be populated with a
probability of less than the given percentage, is determined to be the
critical temperature. For gate-based simulation, this is the
probability of erroneous calculations of the gate.)doc";

static const char *__doc_fiction_critical_temperature_params_iteration_steps =
R"doc(Number of iteration steps for the *QuickSim* algorithm (only
applicable if engine == QUICKSIM).)doc";

static const char *__doc_fiction_critical_temperature_params_max_temperature =
R"doc(Maximum simulation temperature beyond which no simulation will be
conducted (~ 126 °C by default) (unit: K).)doc";

static const char *__doc_fiction_critical_temperature_params_operational_params =
R"doc(The parameters used to determine if a layout is `operational` or `non-
operational`.)doc";

static const char *__doc_fiction_critical_temperature_stats = R"doc(This struct stores the result of the temperature simulation.)doc";

static const char *__doc_fiction_critical_temperature_stats_algorithm_name =
R"doc(Name of the algorithm used to compute the physically valid charge
distributions.)doc";

static const char *__doc_fiction_critical_temperature_stats_duration = R"doc(The total runtime of the critical temperature computation.)doc";

static const char *__doc_fiction_critical_temperature_stats_energy_between_ground_state_and_first_erroneous =
R"doc(Energy difference between the ground state and the first (erroneous)
excited state (unit: meV).)doc";

static const char *__doc_fiction_critical_temperature_stats_num_valid_lyt = R"doc(Number of physically valid charge configurations.)doc";

static const char *__doc_fiction_critical_temperature_stats_report =
R"doc(Prints the simulation results to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_critical_temperature_stats_simulation_parameters = R"doc(All parameters for physical SiDB simulations.)doc";

static const char *__doc_fiction_csv_writer =
R"doc(Utility class for writing delimited (e.g. CSV) data into an output
stream. It provides a variadic member function, `write_line`, that can
take an arbitrary number of arguments and write them to the provided
output stream in a line separated by a specified delimiter.

The csv_writer follows some behavior principles: - Any standard data
type can be written to the output stream. - Data arguments written
will be separated by the specified delimiter. - A newline is written
at the end of each line. - If `write_line` receives no arguments, it
only writes a newline. - The last value written in a line is not
followed by a delimiter. - No checks for escape characters are
performed.

Example usage:

```
{.cpp}
  std::ofstream file("output.csv");
  csv_writer writer(file);
  writer.write_line("Name", "Age", "City");
  writer.write_line("Alice", 20, "New York");
```)doc";

static const char *__doc_fiction_csv_writer_csv_writer =
R"doc(Standard constructor.

Parameter ``os``:
    Output stream to write CSV data into.)doc";

static const char *__doc_fiction_csv_writer_stream = R"doc(Output stream to write to.)doc";

static const char *__doc_fiction_csv_writer_write_line =
R"doc(Writes a single line of values to the output stream separated by a
DELIMITER. No delimiter placed after the last value. Note that no
escape checks are performed. Upon receiving no arguments, only a
newline is written. This function uses template recursion to process
the variadic parameters.

Template parameter ``T``:
    The type of the first argument.

Template parameter ``Ts``:
    Types of the rest of the variadic parameter pack.

Parameter ``arg``:
    First argument to write to the stream.

Parameter ``args``:
    Rest of the arguments to write to the stream if any exist.)doc";

static const char *__doc_fiction_csv_writer_write_line_2 =
R"doc(Writes a newline to the output stream. This is the base case of the
variadic template function, and is invoked when there are no
additional arguments to process.

Template parameter ``Ts``:
    An empty variadic template argument pack.)doc";

static const char *__doc_fiction_cube_coord_t =
R"doc(Signed cube coordinates.

This implementation allows for negative coordinate values and offers a
balance between memory consumption and range of values. Coordinates
span from :math:`(-2^{31}, -2^{31}, -2^{31})` to :math:`(2^{31} - 1,
2^{31} - 1, 2^{31} - 1)`. Each coordinate has a dead indicator `d`
that can be used to represent that it is not in use.)doc";

static const char *__doc_fiction_cube_coord_t_coord_t = R"doc(Default constructor. Creates a dead coordinate at (0, 0, 0).)doc";

static const char *__doc_fiction_cube_coord_t_coord_t_2 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Template parameter ``Z``:
    Type of z.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.

Parameter ``z_``:
    z position.)doc";

static const char *__doc_fiction_cube_coord_t_coord_t_3 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.)doc";

static const char *__doc_fiction_cube_coord_t_d = R"doc(Dead indicator.)doc";

static const char *__doc_fiction_cube_coord_t_get_dead =
R"doc(Returns a dead copy of the coordinate, i.e., (1, x, y, z).

Returns:
    A dead copy of the coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_is_dead =
R"doc(Returns whether the coordinate is dead.

Returns:
    `true` iff coordinate is dead.)doc";

static const char *__doc_fiction_cube_coord_t_operator_add =
R"doc(Adds another coordinate to this one and returns the result. Does not
modify this coordinate.

Parameter ``other``:
    Coordinate to add.

Returns:
    Sum of both coordinates.)doc";

static const char *__doc_fiction_cube_coord_t_operator_eq =
R"doc(Compares against another coordinate for equality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff both coordinates are identical.)doc";

static const char *__doc_fiction_cube_coord_t_operator_ge =
R"doc(Determine whether this coordinate is "greater than or equal to"
another one. This is the case if this one is not "less than" the
other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "greater than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_operator_gt =
R"doc(Determine whether this coordinate is "greater than" another one. This
is the case if the other one is "less than".

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "greater than" the other coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_operator_le =
R"doc(Determine whether this coordinate is "less than or equal to" another
one. This is the case if this one is not "greater than" the other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "less than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_operator_lt =
R"doc(Determine whether this coordinate is "less than" another one. This is
the case if z is smaller, or if z is equal but y is smaller, or if z
and y are equal but x is smaller.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "less than" the other coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_operator_ne =
R"doc(Compares against another coordinate for inequality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff both coordinates are not identical.)doc";

static const char *__doc_fiction_cube_coord_t_operator_sub =
R"doc(Subtracts another coordinate from this one and returns the result.
Does not modify this coordinate.

Parameter ``other``:
    Coordinate to subtract.

Returns:
    Difference of both coordinates.)doc";

static const char *__doc_fiction_cube_coord_t_str =
R"doc(Returns a string representation of the coordinate of the form `"(x, y,
z)"` that does not respect the dead indicator.

Returns:
    String representation of the form `"(x, y, z)"`.)doc";

static const char *__doc_fiction_cube_coord_t_wrap =
R"doc(Wraps the coordinate with respect to the given aspect ratio by
iterating over the dimensions in the order defined by the coordinate
type. For any dimension of the coordinate that is strictly larger than
the associated dimension of the aspect ratio, this dimension will be
wrapped to zero, and the next dimension is increased. The resulting
coordinate becomes a dead copy of the aspect ratio if it is not
contained in the aspect ratio after iterating. An example use case of
this function is the coordinate iterator, which implements iterator
advancing by first incrementing the x dimension, then wrapping the
coordinate to the boundary within to enumerate.

Parameter ``aspect_ratio``:
    Aspect ratio to wrap the coordinate to.)doc";

static const char *__doc_fiction_cube_coord_t_x = R"doc(x coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_y = R"doc(y coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_z = R"doc(z coordinate.)doc";

static const char *__doc_fiction_debug_print_node_to_tile_assignments = R"doc()doc";

static const char *__doc_fiction_debug_print_tile_to_node_assignments = R"doc()doc";

static const char *__doc_fiction_debug_write_dot_layout = R"doc()doc";

static const char *__doc_fiction_debug_write_dot_network = R"doc()doc";

static const char *__doc_fiction_defect_clearance =
R"doc(Results of the defect clearance calculation.

Template parameter ``CellType``:
    Cell type of the layout.)doc";

static const char *__doc_fiction_defect_clearance_defect_clearance_distance =
R"doc(The maximum of the minimum distances between any SiDB of the layout
and the defect responsible for gate failure (unit: nm).)doc";

static const char *__doc_fiction_defect_clearance_defect_position =
R"doc(Position with maximum distance to the SiDB layout at which the
placement of an SiDB defect still causes the gate to fail.)doc";

static const char *__doc_fiction_defect_extent =
R"doc(Returns the extent of a defect as a pair of SiDB distances in the
horizontal and vertical directions. If the defect type is `NONE`, `{0,
0}` is returned.

Parameter ``defect``:
    Defect type to evaluate.

Parameter ``charged_defect_spacing_overwrite``:
    Override the default influence distance of charged atomic defects
    on SiDBs with an optional pair of horizontal and vertical
    distances.

Parameter ``neutral_defect_spacing_overwrite``:
    Override the default influence distance of neutral atomic defects
    on SiDBs with an optional pair of horizontal and vertical
    distances.

Returns:
    A pair of uint16_t values representing the number of horizontal
    and vertical SiDBs affected by the given defect type.)doc";

static const char *__doc_fiction_defect_influence_domain =
R"doc(A `defect_influence_domain` defines for each defect position the
influence of the defect on the layout. Depending on the chosen
definition of influence, this can either mean that the operational
status or the ground state of the layout is changed due to the
presence of the defect.)doc";

static const char *__doc_fiction_defect_influence_grid_search =
R"doc(This algorithm uses a grid search to determine the defect influence
domain. The grid search is performed by exhaustively sweeping all
possible atomic defect positions in x and y dimensions.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``step_size``:
    The parameter specifying the interval between consecutive defect
    positions to be evaluated.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_grid_search_2 =
R"doc(This algorithm uses a grid search to determine the defect influence
domain. The grid search is performed by exhaustively sweeping all
possible atomic defect positions in x and y dimensions.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``step_size``:
    The parameter specifying the interval between consecutive defect
    positions to be evaluated.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_params =
R"doc(Parameters to determine the defect influence.

Template parameter ``CellType``:
    Type of the cell.)doc";

static const char *__doc_fiction_defect_influence_params_additional_scanning_area =
R"doc(Area around the layout for additional defect scanning. This describes
the additional space around the bounding box of the layout.)doc";

static const char *__doc_fiction_defect_influence_params_defect = R"doc(The defect to calculate the defect influence for.)doc";

static const char *__doc_fiction_defect_influence_params_influence_def = R"doc(Definition of defect influence.)doc";

static const char *__doc_fiction_defect_influence_params_influence_definition = R"doc(Definition of defect influence.)doc";

static const char *__doc_fiction_defect_influence_params_influence_definition_GROUND_STATE_CHANGE =
R"doc(Influence is considered as the ability to change the ground state of
the layout.)doc";

static const char *__doc_fiction_defect_influence_params_influence_definition_OPERATIONALITY_CHANGE =
R"doc(Influence is considered as the ability to change the operational
status of the layout.)doc";

static const char *__doc_fiction_defect_influence_params_operational_params = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_defect_influence_quicktrace =
R"doc(Applies contour tracing to identify the boundary (contour) between
influencing and non-influencing defect positions for a given SiDB
layout.

The algorithm leverages the concept of a screened Coulomb potential,
where the electrostatic interaction weakens as distance increases. If
a defect at position `p` causes the SiDB layout to be non-influential,
then defects further away from the layout are also likely to have no
influence on the layout's functionality or performance. Conversely,
defects closer to the layout may cause it to fail. This behavior
allows for efficient contour tracing of the transition between
influential and non-influential states.

The process is as follows: 1. **Initialization**: Randomly select
`samples` initial defect positions several nanometers away from the
layout where they are unlikely to influence the layout. 2. **Contour
Tracing**: For each position, perform a defect-aware physical
simulation to identify adjacent positions along the x-axis that
influence the layout. 3. **Contour Following**: Trace the contour of
non-influential positions until the starting point is reached again,
thereby closing the contour. 4. **Repetition**: Repeat steps 1-3 for
multiple initial heights to identify additional contours, since
multiple influential-to-non-influential contours may exist. This
process helps to detect all relevant transitions in the layout. This
algorithm uses contour tracing to identify the transition between
influencing and non-influencing defect positions of the SiDB layout.
It starts by searching for defect locations on the left side
(bounding_box + additional scanning area). The y-coordinate for these
positions is chosen randomly. The number of samples is determined by
the `samples` parameter. Then, the algorithm moves each defect
position to the right, searching for the first last non-influencing
defect position.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Defect influence computation statistics.

Returns:
    The (partial) defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_quicktrace_2 =
R"doc(Applies contour tracing to identify the boundary (contour) between
influencing and non-influencing defect positions for a given SiDB
layout.

The algorithm leverages the concept of a screened Coulomb potential,
where the electrostatic interaction weakens as distance increases. If
a defect at position `p` causes the SiDB layout to be non-influential,
then defects further away from the layout are also likely to have no
influence on the layout's functionality or performance. Conversely,
defects closer to the layout may cause it to fail. This behavior
allows for efficient contour tracing of the transition between
influential and non-influential states.

The process is as follows: 1. **Initialization**: Randomly select
`samples` initial defect positions several nanometers away from the
layout where they are unlikely to influence the layout. 2. **Contour
Tracing**: For each position, perform a defect-aware physical
simulation to identify adjacent positions along the x-axis that
influence the layout. 3. **Contour Following**: Trace the contour of
non-influential positions until the starting point is reached again,
thereby closing the contour. 4. **Repetition**: Repeat steps 1-3 for
multiple initial heights to identify additional contours, since
multiple influential-to-non-influential contours may exist. This
process helps to detect all relevant transitions in the layout.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Defect influence computation statistics.

Returns:
    The (partial) defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_random_sampling =
R"doc(This algorithm uses random sampling to find a part of the defect
influence domain that might not be complete. It performs a total of
`samples uniformly-distributed random samples within the specified
area.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of random samples to perform.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The (partial) defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_random_sampling_2 =
R"doc(This algorithm uses random sampling to find a part of the defect
influence domain that might not be complete. It performs a total of
`samples uniformly-distributed random samples within the specified
area.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to compute the defect influence domain for.

Parameter ``samples``:
    Number of random samples to perform.

Parameter ``params``:
    Defect influence domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The (partial) defect influence domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_stats = R"doc(Statistics.)doc";

static const char *__doc_fiction_defect_influence_stats_duration = R"doc(The total runtime of the defect influence computation.)doc";

static const char *__doc_fiction_defect_influence_stats_num_evaluated_defect_positions = R"doc(Number of evaluated parameter combinations.)doc";

static const char *__doc_fiction_defect_influence_stats_num_influencing_defect_positions =
R"doc(Number of parameter combinations, for which the layout gets
influenced.)doc";

static const char *__doc_fiction_defect_influence_stats_num_non_influencing_defect_positions =
R"doc(Number of parameter combinations, for which the layout is not
influenced.)doc";

static const char *__doc_fiction_defect_influence_stats_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_defect_influence_status =
R"doc(Defines whether the influence of a defect is present at a particular
position in the layout. It can be used to classify positions as having
an influence or not.)doc";

static const char *__doc_fiction_defect_influence_status_INFLUENTIAL =
R"doc(This indicates that the defect is actively influencing the layout at
this position. It implies that some form of impact, such as a change
in operational status or ground state, is being caused by the defect
at this position.)doc";

static const char *__doc_fiction_defect_influence_status_NON_INFLUENTIAL =
R"doc(This indicates that the defect does not influence the layout at this
position. It implies that the layout remains unaffected by the defect
at this location, meaning there is no change in the operational status
or the ground state.)doc";

static const char *__doc_fiction_delete_virtual_pis =
R"doc(Deletes virtual primary inputs (PIs) from a network and remaps all
signals connected to the virtual PIs back to their corresponding real
PIs. This ensures compatibility for equivalence checking between
networks with and without virtual inputs, as the miter requires
networks to have an identical number of primary inputs. If the network
does not contain any virtual PIs, it is returned unchanged.

Template parameter ``Ntk``:
    The type of network.

Parameter ``ntk``:
    The input network.

Returns:
    The resulting network after virtual primary inputs are deleted.)doc";

static const char *__doc_fiction_dependent_cell_mode = R"doc(An enumeration of modes for the dependent cell.)doc";

static const char *__doc_fiction_dependent_cell_mode_FIXED =
R"doc(The charge state of the dependent cell is not changed based on the
local electrostatic potential at its position.)doc";

static const char *__doc_fiction_dependent_cell_mode_VARIABLE =
R"doc(The charge state of the dependent cell is changed based on the local
electrostatic potential at its position.)doc";

static const char *__doc_fiction_depth_view_params = R"doc(Parameters for depth view.)doc";

static const char *__doc_fiction_depth_view_params_count_complements = R"doc(Take complemented edges into account for depth computation.)doc";

static const char *__doc_fiction_depth_view_params_pi_cost = R"doc(Whether PIs have costs.)doc";

static const char *__doc_fiction_design_sidb_gates =
R"doc(The *SiDB Gate Designer* designs SiDB gate implementations based on a
specified Boolean function, a skeleton layout (can hold defects),
canvas size, and a predetermined number of canvas SiDBs. Two different
design modes are implemented: `exhaustive` and `random design`.

The `exhaustive design` is composed of three steps: 1. In the initial
step, all possible distributions of `number_of_canvas_sidbs` SiDBs
within a given canvas are exhaustively determined. This ensures
exhaustive coverage of every potential arrangement of
`number_of_canvas_sidbs` SiDBs across the canvas. 2. The calculated
SiDB distributions are then incorporated into the skeleton, resulting
in the generation of distinct SiDB layouts. 3. The generated SiDB
layouts then undergo an extensive simulation process. All input
combinations possible for the given Boolean function are used to
verify if the logic is fulfilled.

The `random design` is composed of four steps: 1. A specified number
of canvas SiDBs (`number_of_canvas_sidbs`) are randomly added to the
skeleton layout. 2. The operation status of the layout is simulated
based on a given Boolean function. 3. If the layout is `operational`,
it is returned as the result, and the process terminates successfully.
4. If the layout is `non-operational`, the process is repeated from
the first step until an operational layout is found.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``skeleton``:
    The skeleton layout used for gate design.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the *SiDB Gate Designer*.

Parameter ``stats``:
    Statistics.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_design_sidb_gates_params =
R"doc(This struct contains parameters and settings to design SiDB gates.

Template parameter ``CellType``:
    Cell type.)doc";

static const char *__doc_fiction_design_sidb_gates_params_canvas = R"doc(Canvas spanned by the northwest and southeast cell.)doc";

static const char *__doc_fiction_design_sidb_gates_params_design_mode = R"doc(Gate design mode.)doc";

static const char *__doc_fiction_design_sidb_gates_params_design_sidb_gates_mode = R"doc(Selector for the available design approaches.)doc";

static const char *__doc_fiction_design_sidb_gates_params_design_sidb_gates_mode_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER = R"doc(Gates are designed by using the *Automatic Exhaustive Gate Designer*.)doc";

static const char *__doc_fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL = R"doc(Gates are designed by using *QuickCell*.)doc";

static const char *__doc_fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM = R"doc(Gate layouts are designed randomly.)doc";

static const char *__doc_fiction_design_sidb_gates_params_number_of_canvas_sidbs = R"doc(Number of SiDBs placed in the canvas to create a working gate.)doc";

static const char *__doc_fiction_design_sidb_gates_params_operational_params = R"doc(Parameters for the `is_operational` function.)doc";

static const char *__doc_fiction_design_sidb_gates_params_termination_cond =
R"doc(The design process is terminated after a valid SiDB gate design is
found.

@note This parameter has no effect unless the gate design is
exhaustive.)doc";

static const char *__doc_fiction_design_sidb_gates_params_termination_condition =
R"doc(Selector for the different termination conditions for the SiDB gate
design process.)doc";

static const char *__doc_fiction_design_sidb_gates_params_termination_condition_AFTER_FIRST_SOLUTION =
R"doc(The design process is terminated as soon as the first valid SiDB gate
design is found.)doc";

static const char *__doc_fiction_design_sidb_gates_params_termination_condition_ALL_COMBINATIONS_ENUMERATED =
R"doc(The design process ends after all possible combinations of SiDBs
within the canvas are enumerated.)doc";

static const char *__doc_fiction_design_sidb_gates_stats = R"doc(Statistics for the design of SiDB gates.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_duration = R"doc(The total runtime of SiDB gate design process.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_number_of_layouts = R"doc(The number of all possible layouts.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_number_of_layouts_after_first_pruning =
R"doc(The number of layouts that remain after first pruning (discarding
layouts with potential positive SiDBs).)doc";

static const char *__doc_fiction_design_sidb_gates_stats_number_of_layouts_after_second_pruning =
R"doc(The number of layouts that remain after second pruning (discarding
layouts that fail to satisfy the physical model).)doc";

static const char *__doc_fiction_design_sidb_gates_stats_number_of_layouts_after_third_pruning =
R"doc(The number of layouts that remain after third pruning (discarding
layouts with unstable I/O signals).)doc";

static const char *__doc_fiction_design_sidb_gates_stats_report =
R"doc(This function outputs the total time taken for the SiDB gate design
process to the provided output stream. If no output stream is
provided, it defaults to standard output (`std::cout`).

Parameter ``out``:
    The output stream to which the report will be written.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_sim_engine =
R"doc(The simulation engine to be used for the operational domain
computation.)doc";

static const char *__doc_fiction_detail_a_star_impl = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_a_star_impl = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_closed_list = R"doc(Closed list that acts as a set of already visited coordinates.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f = R"doc(Stores a coordinate and its f-value.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_coord = R"doc(Coordinate.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_f = R"doc(f-value of the coordinate.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_operator_eq =
R"doc(Equality operator that only compares the stored coordinates. This
allows for a convenient lookup in the priority queue.

Parameter ``other``:
    Other coordinate_f to compare with.

Returns:
    `true` iff this coord value is equal to the other's.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_operator_gt =
R"doc(Comparator for the priority queue. Compares only the f-values.

Parameter ``other``:
    Other coordinate_f to compare with.

Returns:
    `true` iff this f-value is greater than the other's.)doc";

static const char *__doc_fiction_detail_a_star_impl_cost = R"doc(The cost functor that implements the cost function.)doc";

static const char *__doc_fiction_detail_a_star_impl_distance =
R"doc(The distance functor that implements the heuristic estimation
function.)doc";

static const char *__doc_fiction_detail_a_star_impl_expand =
R"doc(Expands the frontier of coordinates to visit next in the direction of
the heuristic cost function.

Parameter ``current``:
    Coordinate that is currently examined.)doc";

static const char *__doc_fiction_detail_a_star_impl_g =
R"doc(Returns the g-value of a coordinate by accessing the g_values map.
Returns 0 if no entry exists.

Parameter ``c``:
    Coordinate whose g-value is desired.

Returns:
    g-value of coordinate c or 0 if no value has been stored.)doc";

static const char *__doc_fiction_detail_a_star_impl_g_values =
R"doc(Stores the g-cost for each coordinate c, i.e., the length of the
shortest path from source to c found so far.)doc";

static const char *__doc_fiction_detail_a_star_impl_get_lowest_f_coord =
R"doc(Fetches and pops the coordinate with the lowest f-value from the open
list priority queue.

Returns:
    Coordinate with the lowest f-value from the open list.)doc";

static const char *__doc_fiction_detail_a_star_impl_is_visited =
R"doc(Checks if a coordinate has been visited already.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff c has already been visited.)doc";

static const char *__doc_fiction_detail_a_star_impl_layout =
R"doc(The layout in which the shortest path between `source` and `target` is
to be found.)doc";

static const char *__doc_fiction_detail_a_star_impl_no_improvement =
R"doc(Checks if the given g-value is greater or equal than the stored
g-value of the given coordinate. If that is the case, the new g-value
does not improve the path.

Parameter ``c``:
    Coordinate to whose g-value is to be checked.

Parameter ``g_val``:
    g-value to compare to c's.

Returns:
    `true` iff the given g-value does not mean an improvement for the
    given coordinate.)doc";

static const char *__doc_fiction_detail_a_star_impl_objective = R"doc(The source-target coordinate pair.)doc";

static const char *__doc_fiction_detail_a_star_impl_open_list =
R"doc(Open list that contains all coordinates to process next sorted by
their f-value.)doc";

static const char *__doc_fiction_detail_a_star_impl_params = R"doc(Routing parameters.)doc";

static const char *__doc_fiction_detail_a_star_impl_reconstruct_path =
R"doc(Reconstruct the final path from the origin map that was created during
the path finding algorithm.

Returns:
    The shortest path connecting source and target.)doc";

static const char *__doc_fiction_detail_a_star_impl_run =
R"doc(Determine the shortest path in a layout that starts at
`objective.source` and leads to `objective.target`.

Returns:
    The shortest path in `layout` from `objective.source` to
    `objective.target`.)doc";

static const char *__doc_fiction_detail_a_star_impl_set_g =
R"doc(Updates the g-value of the given coordinate to the given value.

Parameter ``c``:
    Coordinate whose g-value is to be updated to g_val.

Parameter ``g_val``:
    New g-value for c.)doc";

static const char *__doc_fiction_detail_add_obstructions =
R"doc(Add obstructions to the layout.

This function adds obstructions to the provided
wiring_reduction_layout. It obstructs coordinates along the top and
bottom edges (for left to right) or along the left and right edges
(for top to bottom) of the layout in both layers (0 and 1).

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``lyt``:
    The wiring_reduction_layout to which obstructions will be added.)doc";

static const char *__doc_fiction_detail_adjust_tile =
R"doc(This function adjusts the tile and gates in the layout after deleting
wires. It shifts gates to fill the empty coordinates and adjusts the
layout according to the provided offset matrix.

Template parameter ``Lyt``:
    Type of the Cartesian gate-level layout.

Template parameter ``LytCpy``:
    Type of the copy of the original layout for reference.

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``lyt``:
    The 2DDWave-clocked layout whose wiring is to be reduced.

Parameter ``layout_copy``:
    Copy of the original layout for reference.

Parameter ``wiring_reduction_lyt``:
    The `wiring_reduction_layout`.

Parameter ``x``:
    X-index of the tile to adjust.

Parameter ``y``:
    Y-index of the tile to adjust.

Parameter ``z``:
    Z-index of the tile to adjust.

Parameter ``offset_mtrx``:
    The offset matrix used for adjusting the layout.)doc";

static const char *__doc_fiction_detail_adjust_tile_horizontal_search_dir =
R"doc(This function adjusts the tile and gates in the layout after deleting
wires, specifically when traversing in the horizontal search
direction. It updates the signals and coordinates accordingly based on
the offset matrix.

Template parameter ``Lyt``:
    Type of the Cartesian gate-level layout.

Template parameter ``LytCpy``:
    Type of the copy of the original layout for reference.

Parameter ``lyt``:
    The 2DDWave-clocked layout whose wiring is to be reduced.

Parameter ``layout_copy``:
    Copy of the original layout for reference.

Parameter ``fanin``:
    Reference to the tile representing the fanin signal.

Parameter ``offset_mtrx``:
    The offset matrix used for adjusting the layout.

Parameter ``old_coord``:
    The old coordinates before adjustment.

Parameter ``offset``:
    The offset value used for adjusting the layout.

Parameter ``signals``:
    Vector to store signals for the adjusted coordinates.)doc";

static const char *__doc_fiction_detail_adjust_tile_vertical_search_dir =
R"doc(This function adjusts the tile and gates in the layout after deleting
wires, specifically when traversing in the vertical search direction.
It updates the signals and coordinates accordingly based on the offset
matrix.

Template parameter ``Lyt``:
    Type of the Cartesian gate-level layout.

Template parameter ``LytCpy``:
    Type of the copy of the original layout for reference.

Parameter ``lyt``:
    The 2DDWave-clocked layout whose wiring is to be reduced.

Parameter ``layout_copy``:
    Copy of the original layout for reference.

Parameter ``fanin``:
    Reference to the tile representing the fanin signal.

Parameter ``offset_mtrx``:
    The offset matrix used for adjusting the layout.

Parameter ``old_coord``:
    The old coordinates before adjustment.

Parameter ``offset``:
    The offset value used for adjusting the layout.

Parameter ``signals``:
    Vector to store signals for the adjusted coordinates.)doc";

static const char *__doc_fiction_detail_any_to_string =
R"doc(Converts an `std::any` to a string if it contains an alpha-numerical
standard data type.

Parameter ``value``:
    The value to convert.

Returns:
    The string representation of the value.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_apply_gate_library_impl = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_assign_gate =
R"doc(This function assigns a given FCN gate implementation to the total
cell layout.

Parameter ``c``:
    Top-left cell of the tile where the gate is placed.

Parameter ``g``:
    Gate implementation.

Parameter ``n``:
    Corresponding node in the gate-level layout.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_cell_lyt = R"doc(Cell-level layout.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_determine_aspect_ratio_for_cell_level_layout =
R"doc(Computes the (inclusively) bounding coordinate for a cell-level layout
that is derived from the dimensions of the given gate-level layout,
while respecting tiling geometry in which even and odd rows/columns do
not line up.

Parameter ``gate_lyt``:
    Gate-level layout of which the dimensions are read.

Returns:
    Aspect ratio for a cell-level layout that corresponds to the
    dimensions of the given gate-level layout.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_gate_lyt = R"doc(Gate-level layout.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_run_parameterized_gate_library =
R"doc(Run the cell layout generation process.

This function performs the cell layout generation process based on the
SiDB on-the-fly gate library and the gate-level layout information
provided by `GateLibrary` and `gate_lyt`. It iterates through the
nodes in the gate-level layout and maps gates to cell implementations
based on their corresponding positions and types. Optionally, it
performs post-layout optimization and sets the layout name if certain
conditions are met.

Template parameter ``Params``:
    Type of the Parameters used for the SiDB on-the-fly gate library.

Parameter ``params``:
    Parameters used for the SiDB on-the-fly gate library.

Parameter ``defect_surface``:
    Optional defect surface.

Returns:
    A `CellLyt` object representing the generated cell layout.)doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_run_static_gate_library =
R"doc(Run the cell layout generation process.

This function performs the cell layout generation process based on the
gate library and the gate-level layout information provided by
`GateLibrary` and `gate_lyt`. It iterates through the nodes in the
gate-level layout and maps gates to cell implementations based on
their corresponding positions and types. Optionally, it performs post-
layout optimization and sets the layout name if certain conditions are
met.

Parameter ``defect_lyt``:
    Optional defect surface.

Returns:
    A `CellLyt` object representing the generated cell layout.)doc";

static const char *__doc_fiction_detail_calculate_offset_matrix =
R"doc(Calculate an offset matrix based on a to-delete list in a
`wiring_reduction_layout`.

The offset matrix represents the number of deletable coordinates in
the same column but above of each specific coordinate when searching
from left to right. When searching from top to bottom, the offset
matrix represents the number of deletable coordinates in the same row
but to the left of each specific coordinate. The matrix is initialized
with zeros and updated by incrementing the values for each deletable
coordinate.

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``lyt``:
    The `wiring_reduction_layout` for which the offset matrix is
    calculated.

Parameter ``to_delete``:
    The to-delete list representing coordinates to be considered for
    the offset matrix.

Returns:
    A 2D vector representing the calculated offset matrix.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl = R"doc()doc";

static const char *__doc_fiction_detail_clustercomplete_impl_add_composition =
R"doc(A composition is added to the given clustering state, i.e., the
projector states in the composition are added to the clustering state
and the potential bounds store is updated accordingly.

Parameter ``clustering_state``:
    Clustering state to which the given composition should be added.

Parameter ``composition``:
    The composition that needs to be added to the given clustering
    state.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_add_if_configuration_stability_is_met =
R"doc(This function handles performs the last analysis step before
collecting a simulation result. In order to judge whether a population
stable charge distribution is physically valid, the *configuration
stability* needs to be tested. If this criterion passes, the charge
distribution is added to the simulation results.

Parameter ``clustering_state``:
    A clustering state consisting of only singleton clusters along
    with associated charge states that make up a charge distribution
    that conforms to the *population stability* criterion.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_add_parent =
R"doc(After all specializations have been tried, the clustering state needs
to un-specialize in order for other specializations to take place
later. This action undoes the action performed by the function above,
adding the given parent projector state and putting it back at the
given index.

Parameter ``clustering_state``:
    Clustering state to which the parent projector state should be
    added.

Parameter ``parent_pst_ix``:
    The index in the vector of projector states in the given
    clustering state at which the added parent projector state should
    be placed.

Parameter ``parent_pst``:
    The parent projector state that needs to be added back to the
    given clustering state at the given index.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_add_physically_valid_charge_configurations =
R"doc(This recursive function is the heart of the *ClusterComplete*
destruction. The given clustering state is dissected at the largest
cluster to each possible specialization of it, which then enters the
recursive call with the clustering state modified to have a set of
sibling children replacing their direct parent. For each
specialization, appropriate updates are made to the potential bounds
store that is part of the clustering state. After a specialization has
been handled completely, i.e., when the recursive call for this
specialization returns, the specialization to the potential bounds
store is undone so that a new specialization may be applied.

The two base cases to the recursion are as follows: (1) the charge
distributions implied by the given clustering state do not meet the
population stability, meaning that this branch of the search space may
be pruned through terminating the recursion at this level, and, (2)
the clustering state hold only singleton clusters and passes the
population stability check. In the latter case, the configuration
stability check is performed before the associated charge distribution
is added to the simulation results.

Parameter ``w``:
    The worker running on the current thread. It has a clustering
    state that holds a specific combination of multiset charge
    configurations as projector states of which the respectively
    associated clusters form a clustering in the cluster hierarchy.

Parameter ``composition``:
    To enable dynamic updates of the clustering states that thieves
    can assume, the composition that the current worker's clustering
    state is specialized to is to be stored in the worker's queue if
    further compositions are to be unfolded.

Returns:
    `false` if and only if queue of this worker is found to be
    completely empty and thus backtracking is not required.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_available_threads = R"doc(Number of available threads.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_charge_layout =
R"doc(The base layout, along with the map of placed defects, that are used
to create charge distribution surface copies.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_clustercomplete_impl =
R"doc(Constructor.

Parameter ``lyt``:
    Layout to simulate.

Parameter ``params``:
    Parameter required for both the invocation of *Ground State
    Space*, and the simulation following.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_extract_work_from_top_cluster =
R"doc(Work in the form of compositions of charge space elements of the top
cluster are extracted into a vector and shuffled at random before
being returned. The shuffling may balance the initial workload
division.

Parameter ``top_cluster``:
    The top cluster that is returned by running the *Ground State
    Space* construction.

Returns:
    A vector containing all work contained by the top cluster in
    random order.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_fail_onto_negative_charge =
R"doc(Returns `true` if and only if the given potential bound closes out
SiDB-.

Parameter ``pot_bound``:
    Potential lower bound.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_fail_onto_positive_charge =
R"doc(Performs V < -e - mu+.

Parameter ``pot_bound``:
    Potential upper bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB+.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_find_cluster_of_maximum_size =
R"doc(Finds the cluster of the maximum size in the clustering associated
with the input.

Parameter ``proj_states``:
    A vector of projector states that forms a clustering when only the
    respectively contained clusters are considered.

Returns:
    The index in this vector of the projector state that contains the
    cluster of maximum size.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_get_projector_state_bound =
R"doc(Helper function for obtaining the stored lower or upper bound on the
electrostatic potential that SiDBs in the given projector state--i.e.,
a cluster together with an associated multiset charge configuration--
collectively project onto the given SiDB.

Template parameter ``bound``:
    Bound to obtain (lower/upper).

Parameter ``pst``:
    Projector state.

Parameter ``sidb_ix``:
    Receiving SiDB.

Returns:
    The potential projection value associated with this bound; i.e.,
    an electrostatic potential (in V),)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_initialize_charge_layout =
R"doc(Function to initialize the charge layout.

Parameter ``lyt``:
    Layout to simulate.

Parameter ``params``:
    Parameters for ClusterComplete.

Returns:
    The charge layout initializes with defects specified in the given
    parameters.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_initialize_worker_queues =
R"doc(Initializes the worker queues with work from the top cluster, dividing
it evenly over the available threads.

Parameter ``work_from_top_cluster``:
    A vector containing all compositions of all charge space elements
    of the top cluster.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_lb_fail_onto_neutral_charge =
R"doc(Performs V > e - mu+.

Parameter ``pot_bound``:
    Potential lower bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB0.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_meets_population_stability_criterion =
R"doc(This function performs an analysis that is crucial to the
*ClusterComplete*'s efficiency: as the *Ground State Space* construct
is broken down, combinations of multiset charge configurations are
tried together in more detail than in the construction preceding this
second phase of the simulation.

Parameter ``clustering_state``:
    The clustering state that bundles a slice (i.e., a clustering) in
    the cluster hierarchy along with respective projection states
    (i.e., multiset charge configurations) together with a store of
    accumulated potential projection bounds onto all SiDBs in the
    layout, where this information is specific to the projection
    states in the clustering state.

Returns:
    `false` if and only if a physically valid charge distribution
    cannot be extracted from the clustering state.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_mu_bounds_with_error =
R"doc(Globally available array of bounds that section the band gap, used for
pruning.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_mutex_to_protect_the_simulation_results = R"doc(Mutex to protect the simulation results.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_real_placed_defects = R"doc(Atomic defects that are placed in the layout.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_remove_composition =
R"doc(A composition is removed from the given clustering state, i.e., the
projector states in the compositions are removed from the clustering
state and the potential bounds store is updated accordingly.

Parameter ``clustering_state``:
    Clustering state from which the given composition should be
    removed.

Parameter ``composition``:
    The composition that needs to be removed from the given clustering
    state.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_result = R"doc(Simulation results.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_run =
R"doc(This function performs the *ClusterComplete* simulation; first
executing the *Ground State Space* construction, then destructing the
result into the set of all physically valid charge distributions that
are returned.

Parameter ``params``:
    Parameter required for both the invocation of *Ground State
    Space*, and the simulation following.

Returns:
    Results of the exact simulation.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_take_parent_out =
R"doc(Before the parent projector state may be specialized to a specific
composition of its children, first the projections of the parent must
be subtracted. The parent projector state is moved out and returned.

Parameter ``clustering_state``:
    The clustering state from which the parent projector state should
    be taken out.

Parameter ``parent_pst_ix``:
    The index of the parent projector state in the given clustering
    state that should be taken out.

Returns:
    The parent projector state that was taken out of the given
    clustering state.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_ub_fail_onto_neutral_charge =
R"doc(Performs V < -e - mu-.

Parameter ``pot_bound``:
    Potential upper bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB0.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_unfold_all_compositions =
R"doc(After a cluster in a clustering state was chosen to be unfolded next,
the unfolding is performed through ensuring that all compositions of
the multiset associated with the cluster in the clustering state are
each unfolded. The current worker will always unfold the first
composition, while putting the other compositions in its queue such
that threads without work may steal those if the current worker is
still working on this first composition.

Parameter ``w``:
    The worker running on the current thread.

Parameter ``compositions``:
    A vector containing all compositions to unfold.

Parameter ``informant``:
    For other workers to be able to unfold one of those compositions
    that are not being unfolded yet, they need to obtain the right
    clustering state. The informant adds to the required information
    to dynamically update the clustering state for other workers
    looking to steal work.

Returns:
    `false` if and only if the queue of this worker is found to be
    completely empty and thus backtracking is not required.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_unfold_composition =
R"doc(The clustering state of the current worker is specialized according to
the given composition preceding the recursion. If there is still work
left to do by this worker, backtracking is performed, for which also
the aforementioned specialization needs to be undone.

Parameter ``w``:
    The worker running on the current thread.

Parameter ``composition``:
    The composition to unfold.

Returns:
    `false` if and only if there is no need for backtracking after
    this return.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker = R"doc(Forward declaration of the worker struct.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_2 = R"doc(Forward declaration of the worker struct.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_all_workers = R"doc(The vector of all workers where this worker is at `ix`.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_clustering_state =
R"doc(This worker's current state, consisting of a clustering where each
cluster has an assigned multiset charge configuration, and a store
containing lower and upper bounds on the local potential for each SiDB
under this multiset charge configuration assignment.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_index = R"doc(Worker index in the vector of all workers.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_obtain_work =
R"doc(Obtains work for this worker, either from their own queue, or else
from another worker's queue (work stealing).

Returns:
    Either nothing, if no work was found (and this thread can thus
    terminate), or the work that was obtained.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue =
R"doc(A worker queue contains a double-layer queue of work items, a
clustering state for thieves that want to steal from the lowest layer
of the queue, along with a queue of moles that tell how to transition
this clustering state for thieves to facilitate stealing from one
layer to the next.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_add_to_queue =
R"doc(Adds a vector of work items to the queue, along with adding an
informant that allows for a dynamic update of the clustering state for
thieves to assume one of the work items that are added to the queue.

Parameter ``compositions``:
    Vector of work items.

Parameter ``informant``:
    A mole providing the required information to update the clustering
    state for thieves to enable forward-tracking. The mole says which
    composition to add to the clustering state, and which cluster is
    selected for the subsequent unfolding.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_apply_informant =
R"doc(A mole is popped from the queue which says which composition to add to
the clustering state for thieves, and which cluster is selected for
the subsequent unfolding, which should then be taken out.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_clustering_state_for_thieves =
R"doc(The clustering state for thieves, which enables thieves to join in and
steal work from the bottom of the queue, while the owner of this queue
will take items from the top of the queue.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_get_from_this_queue =
R"doc(Own work is obtained in a blocking fashion. If there is no more work
in the queue, `false` is returned to indicate no backtracking is
necessary, since there is no follow-up work item to backtrack towards.

Returns:
    Either work if there is work left to do on the current level---
    i.e., for the current clustering state of the worker that calls
    this function---or `true` if this not the case and backtracking is
    required in order to do more work that is in this queue, or
    `false` when there is no more such work and thus backtracking can
    be skipped.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_initialize_queue_after_stealing =
R"doc(Initializes this queue with stolen work. The work itself is kept on
the stack.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_mole =
R"doc(A mole contains information on how to transition from one clustering
state to a subsequent one.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_mole_composition =
R"doc(The composition of the previously selected parent that fills the gap
made by previously taking out this selected parent. In a clustering
state transition, first the composition is added (filling the gap made
by the previously selected parent), then the currently selected parent
is taken out according to the `parent_to_move_out_ix` above. This way,
a work item may be unfolded as it fills the gap made by taking out the
currently selected parent. Thus, this work item becomes the
`composition` value of the next mole in line.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_mole_parent_to_move_out_ix =
R"doc(The index of the cluster in the clustering state that is the selected
parent cluster to unfold next. It needs to be taken out in a
clustering state transition.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_mutex_to_protect_this_queue = R"doc(Mutex used to protect shared resources in this queue.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_pop_last_layer =
R"doc(Called during backtracking to descend to the previous layer of the
queue, along with popping the unnecessary mole.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_queue =
R"doc(Double layer queue of work items. Each layer corresponds with a
clustering state that needs to be used to unfold the items in that
layer. The clustering states of subsequent layers are each one
informant application apart.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_thief_informants =
R"doc(The queue of moles. For each transition between layers of the double
layer work queue below, there is an associated mole which informs how
the transition takes place. This way, the clustering state for thieves
can be dynamically updated through forward-tracking (opposite of
backtracking).)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_try_steal_from_this_queue =
R"doc(Attempt to steal work from this queue in a non-blocking fashion. When
a lock is acquired, forward-tracking is applied to dynamically update
the clustering state for thieves to where it can be copied for a thief
that steals the last work item in this queue.

Returns:
    Either `true` when the queue is locked, `false` when there is no
    work in this queue, or a pair of a copy of the updated (forward-
    tracked) clustering state for thieves along with the corresponding
    work item.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_work_in_queue_count =
R"doc(Counter to keep track of the total amount of work in the double-layer
work queue.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_queue_worker_queue =
R"doc(Standard constructor.

Parameter ``num_sidbs_in_layout``:
    The number of SiDBs in the layout to simulate. Required for
    initializing clustering states.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_work_stealing_queue =
R"doc(This worker's queue where work can be obtained from either by this
worker or by others (work stealing).)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_worker_worker =
R"doc(Standard constructor.

Parameter ``ix``:
    Worker index in the vector of all workers.

Parameter ``num_sidbs``:
    The number of SiDBs in the layout to simulate.

Parameter ``workers``:
    The vector of all workers where this worker is at `ix`.)doc";

static const char *__doc_fiction_detail_clustercomplete_impl_workers = R"doc(Vector containing all workers.)doc";

static const char *__doc_fiction_detail_color_routing_impl = R"doc()doc";

static const char *__doc_fiction_detail_color_routing_impl_color_routing_impl = R"doc()doc";

static const char *__doc_fiction_detail_color_routing_impl_conduct_routing =
R"doc(Extracts the routing paths of the given color from the vertex coloring
of the generated edge intersection graph and applies them to the
stored layout.

This function logs the number of unsatisfied objectives in the
statistics.

Template parameter ``Graph``:
    The type of the edge intersection graph.

Template parameter ``Color``:
    The type of the vertex colors.

Parameter ``graph``:
    The edge intersection graph.

Parameter ``coloring``:
    The vertex coloring.

Parameter ``color``:
    The color to extract and route.)doc";

static const char *__doc_fiction_detail_color_routing_impl_layout = R"doc(The layout to route.)doc";

static const char *__doc_fiction_detail_color_routing_impl_objectives = R"doc(The routing objectives.)doc";

static const char *__doc_fiction_detail_color_routing_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_color_routing_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_color_routing_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_color_east = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_color_ntk = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_color_null = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_color_south = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_coloring_container = R"doc()doc";

static const char *__doc_fiction_detail_coloring_container_opposite_color = R"doc()doc";

static const char *__doc_fiction_detail_compare_gate_tiles =
R"doc(Custom comparison function for sorting tiles based on the sum of their
coordinates that breaks ties based on the x-coordinate.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``a``:
    First tile to compare.

Parameter ``b``:
    Second tile to compare.

Returns:
    `true` iff `a < b` based on the aforementioned rule.)doc";

static const char *__doc_fiction_detail_connect_and_place = R"doc()doc";

static const char *__doc_fiction_detail_connect_and_place_2 = R"doc()doc";

static const char *__doc_fiction_detail_convert_array =
R"doc(Based on https://stackoverflow.com/questions/57756557/initializing-a-
stdarray-with-a-constant-value)doc";

static const char *__doc_fiction_detail_convert_array_of_arrays =
R"doc(Based on https://stackoverflow.com/questions/57756557/initializing-a-
stdarray-with-a-constant-value)doc";

static const char *__doc_fiction_detail_convert_network_impl = R"doc()doc";

static const char *__doc_fiction_detail_convert_network_impl_convert_network_impl = R"doc()doc";

static const char *__doc_fiction_detail_convert_network_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_convert_network_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_count_gate_types_impl = R"doc()doc";

static const char *__doc_fiction_detail_count_gate_types_impl_count_gate_types_impl = R"doc()doc";

static const char *__doc_fiction_detail_count_gate_types_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_count_gate_types_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_count_gate_types_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_create_array =
R"doc(From https://stackoverflow.com/questions/57756557/initializing-a-
stdarray-with-a-constant-value)doc";

static const char *__doc_fiction_detail_create_wiring_reduction_layout =
R"doc(Create a wiring_reduction_layout suitable for finding excess wiring
based on a Cartesian layout.

This function generates a new layout suitable for finding excess
wiring by shifting the input layout based on specified offsets. The
generated layout is wrapped in an obstruction_layout. The shifted
layout is constructed by iterating through the input Cartesian layout
diagonally and obstructing connections and coordinates accordingly.

Template parameter ``Lyt``:
    Type of the input Cartesian gate-level layout.

Parameter ``lyt``:
    The input Cartesian gate-level layout to be shifted.

Parameter ``x_offset``:
    The offset for shifting in the x-direction. Defaults to 0 if not
    specified.

Parameter ``y_offset``:
    The offset for shifting in the y-direction. Defaults to 0 if not
    specified.

Parameter ``search_direction``:
    If set to horizontally, paths are searched from left to right,
    otherwise from top to bottom.

Returns:
    wiring_reduction_layout suitable for finding excess wiring via A*.)doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_critical_path_length_and_throughput_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_delay_cache = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_lyt = R"doc(Gate-level layout.)doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_delay = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_diff = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_length = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_path_info = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_path_info_2 = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_result = R"doc(Result storage.)doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_signal_delay = R"doc()doc";

static const char *__doc_fiction_detail_critical_temperature_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_temperature_impl_bii = R"doc(Iterator that iterates over all possible input states.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_critical_temperature = R"doc(Critical temperature [K].)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_critical_temperature_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_temperature_impl_determine_critical_temperature =
R"doc(The *Critical Temperature* is determined.

Parameter ``energy_state_type``:
    All energies of all physically valid charge distributions with the
    corresponding state type (i.e. transparent, erroneous).)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_gate_based_simulation =
R"doc(*Gate-based Critical Temperature* Simulation of a SiDB layout for a
given Boolean function.

tparam TT Type of the truth table.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_get_critical_temperature =
R"doc(Returns the critical temperature.

Returns:
    The critical temperature (unit: K).)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_is_ground_state_transparent =
R"doc(The energy difference between the ground state and the first erroneous
state is determined. Additionally, the state type of the ground state
is determined and returned.

Parameter ``energy_and_state_type``:
    All energies of all physically valid charge distributions with the
    corresponding state type (i.e. transparent, erroneous).

Parameter ``min_energy``:
    Minimal energy of all physically valid charge distributions of a
    given layout (unit: eV).

Returns:
    State type (i.e. transparent, erroneous) of the ground state is
    returned.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_layout = R"doc(SiDB cell-level layout.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_non_gate_based_simulation =
R"doc(*Gate-based Critical Temperature* Simulation of a SiDB layout for a
given Boolean function.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_params = R"doc(Parameters for the critical_temperature algorithm.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_physical_simulation_of_bdl_iterator =
R"doc(This function conducts physical simulation of the given layout (gate
layout with certain input combination). The simulation results are
stored in the `sim_result_100` variable.

Parameter ``bdl_iterator``:
    A reference to a BDL input iterator representing the gate layout
    at a given input combination. The simulation is performed based on
    the configuration represented by the iterator.

Returns:
    Simulation results.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_stats = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_defect_influence_impl = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_impl_current_defect_position = R"doc(The current defect position.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_defect_influence_impl = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_impl_determine_nw_se_cells =
R"doc(This function determines the northwest and southeast cells based on
the layout and the additional scan area specified.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_dist = R"doc(Uniform distribution for the y-coordinate of the defect.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_does_defect_influence_groundstate =
R"doc(This function checks if the defect at position `defect_pos` influences
the ground state of the layout.

Parameter ``lyt_without_defect``:
    Layout without the defect.

Parameter ``defect_pos``:
    Position of the defect.

Returns:
    The influence status of the defect.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_find_last_non_influential_defect_position_moving_right =
R"doc(This function identifies the most recent non-influential defect
position while traversing from left to right towards the SiDB layout.

Parameter ``spec``:
    The optional truth table to be used for the simulation.

Parameter ``starting_defect_position``:
    The starting position of the defect, from which the traversal
    towards the right is conducted until an influential defect is
    found.

Returns:
    The last non-influential defect position. If no non-influential
    defect position is found, `std::nullopt` is returned.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_find_non_influential_defect_position_at_left_side =
R"doc(This function aims to identify an influential defect position within
the layout. It does so by selecting a defect position with the
leftmost x-coordinate and a randomly selected y-coordinate limited the
layout's bounding box.

Parameter ``spec``:
    The optional truth table to be used for the simulation.

Returns:
    Defect position which does not influence the SiDB layout. If no
    non-influential defect position is found, `std::nullopt` is
    returned.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_grid_search =
R"doc(Simulates for each position in the area (spanned by `nw_cell` and
`se_cell`) if the existence of a defect influences the layout.

Parameter ``step_size``:
    The step size used to sample defect positions in the grid. Only
    positions with x and y coordinates divisible by `step_size` will
    be checked.

Returns:
    The defect influence domain.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_influence_domain = R"doc(The defect influence domain of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_is_defect_influential =
R"doc(This function evaluates if the defect at position `c` influences the
layout.

Parameter ``spec``:
    The optional truth table to be used for the simulation.

Parameter ``defect_cell``:
    Defect position to be investigated.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_layout = R"doc(The SiDB cell-level layout to investigate.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_log_stats =
R"doc(Helper function that writes the the statistics of the defect influence
domain computation to the statistics object.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_moore_neighborhood =
R"doc(Computes the Moore neighborhood of a given cell within the SiDB
layout. The Moore neighborhood consists of the eight cells surrounding
the central cell in horizontal, vertical, and diagonal directions.

Parameter ``c``:
    The cell for which the Moore neighborhood is computed.

Returns:
    A vector containing the cells in the Moore neighborhood that are
    empty. If a cell is outside the layout boundaries or occupied, it
    is not included in the result.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_num_evaluated_defect_positions = R"doc(Number of evaluated defect positions.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_num_threads = R"doc(Number of available hardware threads.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_nw_bb_layout = R"doc(The north-west cell of the bounding box of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_nw_cell = R"doc(North-west cell.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_params = R"doc(The parameters for the defect influence domain computation.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_previous_defect_position = R"doc(The previous defect position.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_quicktrace =
R"doc(Applies contour tracing to identify the boundary (contour) between
influencing and non-influencing defect positions for a given SiDB
layout.

The algorithm leverages the concept of a screened Coulomb potential,
where the electrostatic interaction weakens as distance increases. If
a defect at position `p` causes the SiDB layout to be non-influential,
then defects further away from the layout are also likely to have no
influence on the layout's functionality or performance. Conversely,
defects closer to the layout may cause it to fail. This behavior
allows for efficient contour tracing of the transition between
influential and non-influential states.

The process is as follows: 1. **Initialization**: Randomly select
`samples` initial defect positions several nanometers away from the
layout where they are unlikely to influence the layout. 2. **Contour
Tracing**: For each position, perform a defect-aware physical
simulation to identify adjacent positions along the x-axis that
influence the layout. 3. **Contour Following**: Trace the contour of
non-influential positions until the starting point is reached again,
thereby closing the contour. 4. **Repetition**: Repeat steps 1-3 for
multiple initial heights to identify additional contours, since
multiple influential-to-non-influential contours may exist. This
process helps to detect all relevant transitions in the layout.

Parameter ``samples``:
    The number of random initial positions used to identify and trace
    contours. Higher values increase the chance of capturing all
    relevant contours but increase computation time.

Parameter ``spec``:
    The optional truth table to be used for the simulation.

Returns:
    The defect influence domain.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_random_sampling =
R"doc(Checks for a certain number of random positions (given by `samples`)
in the area (spanned by `nw_cell` and `se_cell`) if the existence of a
defect leads to an influence of the layout.

Parameter ``samples``:
    The number of positions to sample. The actual number of iterations
    may be less than the total number of positions or the `samples`
    value.

Parameter ``spec``:
    The optional truth table to be used for the simulation.

Returns:
    The defect influence domain.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_se_bb_layout = R"doc(The south-east cell of the bounding box of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_se_cell = R"doc(South-east cell.)doc";

static const char *__doc_fiction_detail_defect_influence_impl_stats = R"doc(The statistics of the defect influence domain computation.)doc";

static const char *__doc_fiction_detail_delete_virtual_pis_impl = R"doc()doc";

static const char *__doc_fiction_detail_delete_virtual_pis_impl_delete_virtual_pis_impl = R"doc()doc";

static const char *__doc_fiction_detail_delete_virtual_pis_impl_ntk = R"doc(Network without virtual PIs.)doc";

static const char *__doc_fiction_detail_delete_virtual_pis_impl_ntk_topo = R"doc(Topological view of the input network.)doc";

static const char *__doc_fiction_detail_delete_virtual_pis_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_delete_wires =
R"doc(This function deletes wires from the provided
`wiring_reduction_layout` based on the specified coordinates and
offset matrix. It clears the tiles in the to-delete list, shifts all
gates to fill the empty coordinates, and resizes the layout to an
optimized size by calculating the bounding box.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    The 2DDWave-clocked layout whose wiring is to be reduced.

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``wiring_reduction_layout``:
    The `wiring_reduction_layout`.

Parameter ``to_delete``:
    The to-delete list representing coordinates of wires to be
    deleted. each specific coordinate.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl = R"doc()doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_all_canvas_layouts = R"doc(All Canvas SiDB layout (without I/O pins).)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_all_sidbs_in_canvas = R"doc(All cells within the canvas.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_convert_canvas_cell_indices_to_layout =
R"doc(This function generates canvas SiDb layouts.

Parameter ``cell_indices``:
    A vector of indices of cells to be added to the skeleton layout.

Returns:
    An SiDB cell-level layout consisting of canvas SidBs.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_design_sidb_gates_impl =
R"doc(This constructor initializes an instance of the *SiDB Gate Designer*
implementation with the provided skeleton layout and configuration
parameters.

Parameter ``skeleton``:
    The skeleton layout used as a basis for gate design.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``ps``:
    Parameters and settings for the gate designer.

Parameter ``st``:
    Statistics for the gate design process.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_determine_all_possible_canvas_layouts =
R"doc(This function calculates all combinations of distributing a given
number of SiDBs across a specified number of positions in the canvas.
Each combination is then used to create a gate layout candidate.

Returns:
    A vector containing all possible gate layouts generated from the
    combinations.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_input_bdl_wires = R"doc(Input BDL wires.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_discarded_layouts_at_first_pruning = R"doc(Number of discarded layouts at first pruning.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_discarded_layouts_at_second_pruning = R"doc(Number of discarded layouts at second pruning.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_discarded_layouts_at_third_pruning = R"doc(Number of discarded layouts at third pruning.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_input_wires = R"doc(Number of input BDL wires.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_output_wires = R"doc(Number of output BDL wires.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_number_of_threads = R"doc(Number of threads to be used for the design process.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_output_bdl_wires = R"doc(Output BDL wires.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_params = R"doc(Parameters for the *SiDB Gate Designer*.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_automatic_exhaustive_gate_designer =
R"doc(Design gates by using the *Automatic Exhaustive Gate Desginer*. This
algorithm was proposed in \"Minimal Design of SiDB Gates: An Optimal
Basis for Circuits Based on Silicon Dangling Bonds\" by J. Drewniok,
M. Walter, and R. Wille in NANOARCH 2023
(https://dl.acm.org/doi/10.1145/3611315.3633241).

This function adds each cell combination to the given skeleton, and
determines whether the layout is operational based on the specified
parameters. The design process is parallelized to improve performance.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_pruning =
R"doc(This function processes each layout to determine if it represents a
valid gate implementation or if it can be pruned by using three
distinct physically-informed pruning steps. It leverages multi-
threading to accelerate the evaluation and ensures thread-safe access
to shared resources.

Returns:
    A vector containing the valid gate candidates that were not
    pruned.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_quickcell =
R"doc(Design Standard Cells/gates by using the *QuickCell* algorithm.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_random_design =
R"doc(Design gates randomly and in parallel.

This function adds cells randomly to the given skeleton, and
determines whether the layout is operational based on the specified
parameters. The design process is parallelized to improve performance.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_skeleton_layout =
R"doc(The skeleton layout serves as a starting layout to which SiDBs are
added to create unique SiDB layouts and, if possible, working gates.
It defines input and output wires.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_skeleton_layout_with_canvas_sidbs =
R"doc(This function adds SiDBs (given by indices) to the skeleton layout
that is returned afterwards.

Parameter ``cell_indices``:
    A vector of indices of cells to be added to the skeleton layout.

Returns:
    A copy of the original layout (`skeleton_layout`) with SiDB cells
    added at specified indices.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_stats = R"doc(The statistics of the gate design.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_truth_table = R"doc(Truth table of the given gate.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl = R"doc()doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_aggregate_bdl_pairs =
R"doc(Aggregates BDL pairs of specified types into a set.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Returns:
    A set of BDL pairs containing all pairs of the specified types.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_bdl_wires = R"doc(All detected BDL wires.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_detect_bdl_wires =
R"doc(This function operates in two main phases:

1. **Aggregation**: It first collects all BDL pairs of the specified
types.

2. **Wire Detection**: The function then iterates through the
collected BDL pairs: - For each initial BDL pair, it searches for
neighboring BDL pairs located above or below. - If a neighboring BDL
pair is found, it is added to the current wire. - This process repeats
until no additional neighboring BDL pairs can be found.

Once a wire is fully constructed, all BDL pairs belonging to that wire
are removed from the initial collection. The function then proceeds to
detect the next wire using any remaining BDL pairs.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_detect_bdl_wires_impl = R"doc()doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_filter_wires =
R"doc(This function filters the wires from the `bdl_wires` collection based
on the current `selection`. If `selection` is set to
`bdl_wire_selection::INPUT`, it returns all wires containing an input
cell. If `selection` is set to `bdl_wire_selection::OUTPUT`, it
returns all wires containing an output cell. If `selection` is set to
any other value, it returns all the wires.

The function also ensures that all selected wires of the same type
have the same length. If wires of different lengths are found, an
assertion is triggered.

Parameter ``selection``:
    The type of wires to filter by, specified by the
    `bdl_wire_selection` enum.

Returns:
    A vector of filtered `bdl_wire` objects based on the current
    selection. If no wires match the selection criteria, an empty
    vector is returned.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_filter_wires_by_type =
R"doc(This function scans through the `bdl_wires` and selects those
containing BDL pair cells of the specified type. It also checks that
all selected wires have the same length and triggers an assertion if
wires of different lengths are found.

Parameter ``type``:
    The type of the BDL pair cells to filter by.

Returns:
    A vector of `bdl_wire` objects containing cells of the specified
    type. If no such wires are found, an empty vector is returned.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_find_bdl_neighbor_above =
R"doc(This function searches for the first Binary-dot Logic (BDL) pair in a
given set of BDL pairs that is above a specified BDL pair. The
function returns the first BDL pair that meets the following criteria:
- The distance between the upper SiDB of the given BDL pair and the
lower SiDB of the candidate BDL pair is less than the specified inter-
BDL distance. - The candidate BDL pair is not equal to the given BDL
pair (ignoring the type of SiDBs). - The y-coordinate of the upper
SiDB of the given BDL pair is less than the y-coordinate of the lower
SiDB of the candidate BDL pair.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``given_bdl``:
    The BDL pair to find a neighbor for.

Parameter ``bdl_pairs``:
    A set of BDL pairs to search within.

Parameter ``inter_bdl_distance``:
    The maximum allowable distance between the lower SiDB of the given
    BDL pair and the upper SiDB of the potential neighbor BDL pair.

Returns:
    A std::optional containing the first BDL pair that meets the
    criteria, or std::nullopt if no such pair is found.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_find_bdl_neighbor_below =
R"doc(This function searches for the first Binary-dot Logic (BDL) pair in a
given set of BDL pairs that is below a specified BDL pair. The
function returns the first BDL pair that meets the following criteria:
- The distance between the lower SiDB of the given BDL pair and the
upper SiDB of the candidate BDL pair is less than the specified inter-
BDL distance. - The candidate BDL pair is not equal to the given BDL
pair (ignoring the type of SiDBs). - The y-coordinate of the lower
SiDB of the given BDL pair is less than the y-coordinate of the upper
SiDB of the candidate BDL pair.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``given_bdl``:
    The BDL pair to find a neighbor for.

Parameter ``bdl_pairs``:
    A set of BDL pairs to search within.

Parameter ``inter_bdl_distance``:
    The maximum allowable distance between the lower SiDB of the given
    BDL pair and the upper SiDB of the potential neighbor BDL pair.

Returns:
    A `std::optional` containing the first BDL pair that meets the
    criteria, or `std::nullopt` if no such pair is found.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_layout = R"doc(SiDB cell-level layout.)doc";

static const char *__doc_fiction_detail_detect_bdl_wires_impl_params = R"doc(Parameters for detecting BDL wires.)doc";

static const char *__doc_fiction_detail_determine_clocking_impl = R"doc()doc";

static const char *__doc_fiction_detail_determine_clocking_impl_determine_clocking_impl = R"doc()doc";

static const char *__doc_fiction_detail_determine_clocking_impl_layout = R"doc(The layout to assign clock numbers to.)doc";

static const char *__doc_fiction_detail_determine_clocking_impl_params = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_determine_clocking_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_determine_clocking_impl_stats = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_determine_layout_size = R"doc()doc";

static const char *__doc_fiction_detail_determine_new_coord =
R"doc(This function calculates the new coordinates of a tile after adjusting
for wire deletion based on the specified offset and search direction.

Template parameter ``Lyt``:
    Type of the Cartesian gate-level layout.

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``wiring_reduction_lyt``:
    The `wiring_reduction_layout` used to determine the search
    direction.

Parameter ``x``:
    X-coordinate of the tile.

Parameter ``y``:
    Y-coordinate of the tile.

Parameter ``z``:
    Z-coordinate of the tile.

Parameter ``offset``:
    The offset value used for adjusting the layout.

Returns:
    The new coordinates of the tile after adjustment.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl = R"doc()doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_all_possible_sidb_displacements =
R"doc(This stores all possible displacements for all SiDBs in the SiDB
layout. This means e.g. the first vector describes all possible
positions of the first SiDB due to the allowed/possible displacements.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_calculate_all_possible_displacements_for_each_sidb =
R"doc(This function calculates all permitted displacements for each SiDB
based on the specified allowed displacements.

Returns:
    A vector containing all possible displacements for each SiDB.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_determine_probability_of_fabricating_operational_gate =
R"doc(The manufacturing error rate is highly dependent on the speed of the
manufacturing process. Therefore, fast fabrication requires SiDB
layouts with high displacement tolerance to ensure functionality in
the presence of displacements. This function determines the
probability of fabricating an operational SiDB layout for a given
fabrication error rate. If the fabrication error rate is 0.0 or
negative, it means that the SiDB layout is designed without
displacement.

Parameter ``fabrication_error_rate``:
    The fabrication error rate. For example, 0.1 describes that 10% of
    all manufactured SiDBs have a slight displacement.

Returns:
    Probability of fabricating a working SiDB gate implementation.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_determine_robustness_domain =
R"doc(This function calculates the robustness domain of the SiDB layout
based on the provided truth table specification and displacement
robustness computation parameters.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_displacement_robustness_domain_impl =
R"doc(Standard constructor. Initializes the layout, the truth table, the
parameters, and the statistics.

Parameter ``lyt``:
    SiDB cell-level layout to be evaluated regarding displacement
    robustness.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``ps``:
    Parameters for the displacement robustness computation.

Parameter ``st``:
    Statistics related to the displacement robustness computation.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_generate_valid_displaced_sidb_layouts =
R"doc(This function generates all SiDB layouts with displacements based on
the original layout. It filters out layouts where two or more SiDBs
would be on the same spot due to displacement.

Returns:
    A vector containing all valid SiDB layouts with displacements.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_generator =
R"doc(Mersenne Twister random number generator. Generates high-quality
pseudo-random numbers using a random seed from 'rd'.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_layout =
R"doc(The SiDB layout for which the displacement robustness calculation is
performed.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_params = R"doc(The parameters for the displacement robustness computation.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_rd =
R"doc(Random device for obtaining seed for the random number generator.
Provides a source of quasi-non-deterministic pseudo-random numbers.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_sidbs_of_the_original_layout = R"doc(SiDB positions of the originally given SiDB layout.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_stats = R"doc(The statistics of the displacement robustness computation.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_truth_table = R"doc(The logical specification of the layout.)doc";

static const char *__doc_fiction_detail_displacement_robustness_domain_impl_update_displacement_robustness_domain =
R"doc(This function adds the provided layout and its corresponding
operational status to the list of operational values in the
displacement robustness domain. Depending on the operational status,
it also updates the count of operational or non-operational SiDB
displacements in the statistics.

Template parameter ``Lyt``:
    SiDB cell-layout type.

Parameter ``domain``:
    The displacement robustness domain to be updated.

Parameter ``lyt``:
    The SiDB layout to be added.

Parameter ``status``:
    The operational status of the provided layout.)doc";

static const char *__doc_fiction_detail_east_south_edge_coloring = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_collection = R"doc(Collection of all enumerated paths.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_enumerate_all_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_is_visited =
R"doc(Check if a coordinate has been visited.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` if the coordinate has been visited, `false` otherwise.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_layout = R"doc(The layout whose paths are to be enumerated.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_mark_unvisited =
R"doc(Mark a coordinate as unvisited.

Parameter ``c``:
    Coordinate to mark as unvisited.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_mark_visited =
R"doc(Mark a coordinate as visited.

Parameter ``c``:
    Coordinate to mark as visited.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_objective = R"doc(The source-target coordinate pair.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_params = R"doc(Routing parameters.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_recursively_enumerate_all_paths =
R"doc(Recursively enumerate all paths from `src` to `tgt` in the given
layout. This function is called recursively until the target
coordinate is reached. Along each path, each coordinate can occur at
maximum once. This function does not generate duplicate or looping
paths. If the given layout implements the obstruction interface (see
`obstruction_layout`), paths will not be routed via obstructed
coordinates or connections. If the given layout is a gate-level layout
and implements the obstruction interface (see `obstruction_layout`),
paths may contain wire crossings if specified in the parameters. Wire
crossings are only allowed over other wires and only if the crossing
layer is not obstructed. Furthermore, it is ensured that crossings do
not run along another wire but cross only in a single point
(orthogonal crossings + knock-knees/double wires).

Parameter ``src``:
    Source coordinate.

Parameter ``tgt``:
    Target coordinate.

Parameter ``p``:
    Current path to extend.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_run =
R"doc(Enumerate all possible paths in a layout that start at
`objective.source` and lead to `objective.target`.

Returns:
    A collection of all unique paths in `layout` from
    `objective.source` to `objective.target`.)doc";

static const char *__doc_fiction_detail_enumerate_all_paths_impl_visited = R"doc(Set of visited coordinates.)doc";

static const char *__doc_fiction_detail_equivalence_checking_impl = R"doc()doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_equivalence_checking_impl =
R"doc(Standard constructor.

Parameter ``specification``:
    Logical specification of intended functionality.

Parameter ``implementation``:
    Implementation of specified functionality.

Parameter ``p``:
    Parameters.

Parameter ``st``:
    Statistics.)doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_has_drvs = R"doc()doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_impl = R"doc(Implementation.)doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_equivalence_checking_impl_spec = R"doc(Specification.)doc";

static const char *__doc_fiction_detail_exact_impl = R"doc()doc";

static const char *__doc_fiction_detail_exact_impl_ari = R"doc(Iterator for the factorization of possible aspect ratios.)doc";

static const char *__doc_fiction_detail_exact_impl_ari_mutex =
R"doc(Restricts access to the aspect_ratio_iterator and the
result_aspect_ratio.)doc";

static const char *__doc_fiction_detail_exact_impl_black_list =
R"doc(Maps tiles to blacklisted gate types via their truth tables and port
information.)doc";

static const char *__doc_fiction_detail_exact_impl_exact_impl = R"doc()doc";

static const char *__doc_fiction_detail_exact_impl_explore_asynchronously =
R"doc(Thread function for the asynchronous solving strategy. It registers
its own context in the given list of thread_info objects and starts
exploring the search space. It fetches the next aspect ratio to work
on from the global aspect ratio iterator which is protected by a
mutex. When a result is found, other threads that are currently
working on larger layout aspect ratios are interrupted while smaller
ones may finish running.

Parameter ``t_num``:
    Thread's identifier.

Parameter ``ti_list``:
    Pointer to a list of shared thread info that the threads use for
    communication.

Returns:
    A found layout or nullptr if being interrupted.)doc";

static const char *__doc_fiction_detail_exact_impl_lower_bound = R"doc(Lower bound for the number of layout tiles.)doc";

static const char *__doc_fiction_detail_exact_impl_ntk = R"doc(Specification network.)doc";

static const char *__doc_fiction_detail_exact_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_exact_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_exact_impl_rar_mutex = R"doc()doc";

static const char *__doc_fiction_detail_exact_impl_result_aspect_ratio = R"doc(Aspect ratio of found result. Only needed for the asynchronous case.)doc";

static const char *__doc_fiction_detail_exact_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_exact_impl_run_asynchronously =
R"doc(Launches params.num_threads threads and evaluates their return
statements.

Returns:
    A placed and routed gate-level layout or std::nullopt in case a
    timeout or an upper bound was reached.)doc";

static const char *__doc_fiction_detail_exact_impl_run_synchronously =
R"doc(Does the same as explore_asynchronously but without thread
synchronization overhead.

Returns:
    A placed and routed gate-level layout or std::nullopt in case a
    timeout or an upper bound was reached.)doc";

static const char *__doc_fiction_detail_exact_impl_scheme = R"doc(The utilized clocking scheme.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler =
R"doc(Sub-class to exact to handle construction of SMT instances as well as
house-keeping like storing solver states across incremental calls etc.
Multiple handlers can be created in order to explore possible aspect
ratios in parallel.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_apply_to_added_and_updated_tiles =
R"doc(Applies a given function to all added and updated tiles in the current
solver check point.

Template parameter ``Fn``:
    Functor type.

Parameter ``fn``:
    Unary function to apply to all added and updated tiles. Must
    receive a tile as parameter.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_apply_to_added_tiles =
R"doc(Applies a given function to all added tiles in the current solver
check point.

Template parameter ``Fn``:
    Functor type.

Parameter ``fn``:
    Unary function to apply to all added tiles. Must receive a tile as
    parameter.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_apply_to_updated_tiles =
R"doc(Applies a given function to all updated tiles in the current solver
check point.

Template parameter ``Fn``:
    Functor type.

Parameter ``fn``:
    Unary function to apply to all updated tiles. Must receive a tile
    as parameter.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assign_layout =
R"doc(Assigns vertices, edges and directions to the stored layout sketch
with respect to the given model.

Parameter ``model``:
    A satisfying assignment to the created variables under all created
    constraints that can be used to extract a layout description.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assign_layout_clocking =
R"doc(If an open clocking scheme was provided, this function extracts the
clocking information from the given model and assigns the respective
clock zones to the stored layout.

Parameter ``model``:
    Satisfying model to the generated instance.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assign_pi_clockings =
R"doc(Adds constraints to the solver to ensure that the cl variable of
primary input pi is set to the clock zone value of the tile pi is
assigned to. Necessary to be taken into account for path lengths.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assumption_literals =
R"doc(During incremental SMT calls, several created assertions need to be
reformulated. To still be able to reuse information like learned
clauses and lemmas, the push/pop strategy is not employed. Instead,
assumption literals are used that track, i.e., imply, certain
assertions and can be negated in order to deactivate them. Over
incremental calls, the layout size grows either in eastern or southern
direction. Assertions to deactivate affect tiles sitting at the
growing borders as they certainly get new neighbors. To not deactivate
both at once if not needed, there is one assertion literal tracking
eastern growth and one tracking southern growth. The corner tile is
implied by their conjunction.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assumption_literals_e =
R"doc(Watched literals for eastern and southern constraints which are used
to reformulate them.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_assumption_literals_s =
R"doc(Watched literals for eastern and southern constraints which are used
to reformulate them.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_black_list =
R"doc(Maps tiles to blacklisted gate types via their truth tables and port
information.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_black_list_gates =
R"doc(Adds constraints to the solver to enforce blacklisting of certain
gates.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_check_point = R"doc(Current solver checkpoint extracted from the solver tree.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_ctx = R"doc(The context used for all solvers.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_define_gate_fanin_tiles =
R"doc(Adds constraints to the solver to enforce that a tile which was
assigned with some node n has a predecessor that is assigned to the
inversely adjacent node of n or an incoming edge of n.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_define_gate_fanout_tiles =
R"doc(Adds constraints to the solver to enforce that a tile which was
assigned with some node n has a successor that is assigned to the
adjacent node of n or an outgoing edge of n.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_define_number_of_connections =
R"doc(Adds constraints to the solver to define the number of connection
variables to be set for each tile, i.e. empty tiles are not allowed to
have connections at all, edges need to have one ingoing and one
outgoing connection and so on. Symmetry breaking constraints.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_define_wire_fanin_tiles =
R"doc(Adds constraints to the solver to enforce that a tile that was
assigned with some edge has a predecessor which is assigned to the
inversely adjacent node or another edge.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_define_wire_fanout_tiles =
R"doc(Adds constraints to the solver to enforce that a tile that was
assigned with some edge has a successor which is assigned to the
adjacent node or another edge.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_depth_ntk = R"doc(Mapping of levels to nodes used for symmetry breaking.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_eliminate_cycles =
R"doc(Adds constraints to the solver to prohibit cycles that loop back
information. To this end, the formerly established paths are used.
Without this constraint, useless wire loops appear.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_enforce_border_io =
R"doc(Adds constraints to the solver to position the primary inputs and
primary outputs at the layout's borders.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_enforce_straight_inverters =
R"doc(Adds constraints to the solver to enforce that no bent inverters are
used.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_establish_sub_paths =
R"doc(Adds constraints to the solver to map established connections between
single tiles to sub-paths. They are spanned transitively by the next
set of constraints.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_establish_transitive_paths =
R"doc(Adds constraints to the solver to expand the formerly created sub-
paths transitively.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_fetch_solver =
R"doc(Accesses the solver tree and looks for a solver state that is
associated with an aspect ratio smaller by 1 row or column than given
aspect ratio. The found one is returned together with the tiles that
are new to this solver.

If no such solver could be found, a new solver is created from the
context given.

Parameter ``ar``:
    aspect ratio of size x * y.

Returns:
    Solver state associated with an aspect ratio of size x - 1 * y or
    x * y - 1 and, additionally, the tiles new to the solver. If no
    such solver is available, a new one is created.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_generate_smt_instance =
R"doc(Generates the SMT instance by calling the constraint generating
functions.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_lit_e =
R"doc(Returns the lc-th eastern assumption literal from the stored context.

Returns:
    Eastern literal.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_lit_s =
R"doc(Returns the lc-th southern assumption literal from the stored context.

Returns:
    Southern literal.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_ncl =
R"doc(Returns an ncl variable from the stored context representing node n's
(pi) clock number.

Parameter ``n``:
    Node to be considered.

Returns:
    ncl variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_solver_statistics =
R"doc(Returns a statistics object from the current solver state.

Returns:
    Statistics.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_tc =
R"doc(Returns a tc variable from the stored context representing that
information flows from tile t1 to tile t2.

Parameter ``t1``:
    Tile 1 to be considered.

Parameter ``t2``:
    Tile 2 to be considered that is adjacent to t1.

Returns:
    tc variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_tcl =
R"doc(Returns a tcl variable from the stored context representing tile t's
clock number.

Parameter ``t``:
    Tile to be considered.

Returns:
    tcl variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_te =
R"doc(Returns a te variable from the stored context representing that tile t
has edge e assigned.

Parameter ``t``:
    Tile to be considered.

Parameter ``e``:
    Edge to be considered.

Returns:
    te variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_tn =
R"doc(Returns a tn variable from the stored context representing that tile t
has node n assigned.

Parameter ``t``:
    Tile to be considered.

Parameter ``n``:
    Node to be considered.

Returns:
    tn variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_tp =
R"doc(Returns a tp variable from the stored context representing that a path
from tile t1 to tile t2 exists.

Parameter ``t1``:
    Tile 1 to be considered.

Parameter ``t2``:
    Tile 2 to be considered.

Returns:
    tp variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_get_tse =
R"doc(Returns a tse variable from the stored context representing tile t's
synchronization element delay in cycles.

Parameter ``t``:
    Tile to be considered.

Returns:
    tse variable from ctx.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_global_synchronization =
R"doc(Adds constraints to the solver to ensure that fan-in paths to the same
tile need to have the same length in the layout modulo timing, i.e.,
plus the clock zone assigned to their PIs.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_inv_levels = R"doc(Mapping of inverse levels to nodes used for symmetry breaking.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_is_added_tile =
R"doc(Checks whether a given tile belongs to the added tiles of the current
solver check point.

Parameter ``t``:
    Tile to check.

Returns:
    `true` iff t is contained in check_point->added_tiles.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_is_satisfiable =
R"doc(Generates the SMT instance for the current solver check point and runs
the solver check. In case the instance was satisfiable, all
constraints are moved to an z3::optimize if optimization criteria were
specified. This way, no unnecessary optimization constraints need to
be generated over and over for UNSAT instances.

If the instance was found SAT on both levels, a layout is extract from
the model and stored. The function returns then true.

Returns:
    `true` iff the instance generated for the current configuration is
    SAT.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_is_updated_tile =
R"doc(Checks whether a given tile belongs to the updated tiles of the
current solver check point.

Parameter ``t``:
    Tile to check.

Returns:
    `true` iff t is contained in check_point->updated_tiles.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_layout = R"doc(The sketch that later contains the layout generated from a model.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_lc = R"doc(Assumption literal counter.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_lit =
R"doc(Shortcut to the assumption literals.

Returns:
    Reference to check_point->state->lit.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_minimize_crossings =
R"doc(Adds constraints to the given optimize to minimize the number of
crossing tiles to use.

Parameter ``optimize``:
    Pointer to an z3::optimize to add constraints to.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_minimize_synchronization_elements =
R"doc(Adds constraints to the given optimize to enforce that the overall sum
of synchronization element values should be minimized.

Parameter ``optimize``:
    Pointer to an z3::optimize to add constraints to.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_minimize_wires =
R"doc(Adds constraints to the given optimize to minimize the number of
crossing tiles to use.

Parameter ``optimize``:
    Pointer to an z3::optimize to add constraints to.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_mk_as =
R"doc(Helper function for generating an implication lit -> constraint where
lit is the given assumption literal.

Parameter ``constraint``:
    Constraint to be implied.

Parameter ``lit``:
    Assumption literal.

Returns:
    lit -> constraint.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_mk_as_if_se =
R"doc(Helper function for generating an implication lit -> constraint where
lit is the assumption literal responsible for t, i.e., e if t is at
eastern border, s if t is at southern border, and (e and s) if t is
the corner tile.

Parameter ``constraint``:
    Constraint to be implied.

Parameter ``t``:
    Tile to consider for literal picking.

Returns:
    lit -> constraint.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_mk_clk_mod =
R"doc(Helper function to create an expression that assigns a matching
clocking to a tile given its outgoing tile. This function is only
needed for irregular clocking schemes.

Parameter ``t1``:
    Tile.

Parameter ``t2``:
    Tile outgoing to t1 given its dataflow.

Returns:
    An expression of the form tcl(t1, 0) --> tcl(t2, 1) and tcl(t1, 1)
    --> tcl(t2, 2) and ... up to the number of clock phases in the
    layout.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_mk_eq =
R"doc(Helper function for generating an equality of an arbitrary number of
expressions.

Parameter ``v``:
    Vector of expressions to equalize.

Returns:
    Expression that represents the equality of all elements in v.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_ncl_to_real =
R"doc(Helper function to create an expression that maps from an ncl variable
to a real value during a solver call.

Parameter ``n``:
    Network node.

Returns:
    An expression of the form ite(ncl == 0, 0, ite(ncl == 1, 1, ...))
    up to the number of clock phases in the layout.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_network = R"doc(Logical specification for the layout.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_network_in_degree =
R"doc(Determines the number of child nodes to some given node n in the
stored logic network, not counting constants and not counting primary
inputs if params.io_pins is not set.

Parameter ``n``:
    Node in the stored network.

Returns:
    Number of incoming nodes to n.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_network_out_degree =
R"doc(Determines the number of parent nodes to some given node n in the
stored logic network, not counting constants and not counting primary
outputs if params.io_pins is not set.

Parameter ``n``:
    Node in the stored network.

Returns:
    Number of outgoing nodes of n.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_node2pos =
R"doc(Maps nodes to tile positions when creating the layout from the SMT
model.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_optimize =
R"doc(Creates and returns a z3::optimize if optimization criteria were set
by the configuration. The optimize gets passed all constraints from
the current solver and the respective optimization constraints are
added to it, too.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_params =
R"doc(Configurations specifying layout restrictions. Used in instance
generation among other places.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_place_output =
R"doc(Places a primary output pin represented by node n of the stored
network onto tile t in the stored layout.

Parameter ``t``:
    Tile to place the PO pin.

Parameter ``n``:
    Node in the stored network representing a PO.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_prevent_insufficiencies =
R"doc(Adds constraints to the solver to prevent edges or vertices to be
assigned to tiles with an insufficient number of
predecessors/successors. Symmetry breaking constraints.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_restrict_clocks =
R"doc(Adds constraints to the solver to enforce that each clock zone
variable has valid bounds of 0 <= cl <= C, where C is the maximum
clock number. Uses a one-hot encoding.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_restrict_synchronization_elements =
R"doc(Adds constraints to the solver to prevent negative valued
synchronization elements and that gate tiles cannot be assigned
synchronization elements.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_restrict_tile_elements =
R"doc(Adds constraints to the solver to limit the number of elements that
are going to be assigned to a tile to one (node or edge) if no
crossings are allowed. Otherwise, one node per tile or two edges per
tile can be placed.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_restrict_vertices =
R"doc(Adds constraints to the solver to enforce that each node is placed
exactly once on exactly one tile.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_route =
R"doc(Starting from t, all outgoing clocked tiles are recursively considered
and checked against the given model. Consequently, e is routed through
all tiles with a match in model.

Parameter ``t``:
    Initial tile to start recursion from (not included in model
    evaluations).

Parameter ``e``:
    Edge to check for.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_set_timeout =
R"doc(Sets the given timeout for the current solver.

Parameter ``t``:
    Timeout in ms.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_skip_const_or_io_edge =
R"doc(Returns true, iff skip_const_or_io_node returns true for either source
or target of the given edge..

Parameter ``e``:
    Edge in network.

Returns:
    `true` iff e is to be skipped in a loop due to it having constant
    or I/O nodes while params.io_ports == false.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_skip_const_or_io_node =
R"doc(Returns true, iff params.io_ports is set to false and n is either a
constant or PI or PO node in network.

Parameter ``n``:
    Node in network.

Returns:
    `true` iff n is to be skipped in a loop due to it being a constant
    or an I/O and params.io_ports == false.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_skippable =
R"doc(Evaluates a given aspect ratio regarding the stored configurations
whether it can be skipped, i.e., does not need to be explored by the
SMT solver. The better this function is at predicting unsatisfying
inputs, the more UNSAT instances can be skipped without losing the
optimality guarantee. This function should never be overly
restrictive!

Parameter ``ar``:
    Aspect ratio to evaluate.

Returns:
    `true` if ar can safely be skipped because it is UNSAT anyway.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_smt_handler =
R"doc(Standard constructor.

Parameter ``ctxp``:
    The context that is used in all solvers.

Parameter ``lyt``:
    The empty gate-level layout that is going to contain the created
    layout.

Parameter ``ps``:
    The parameters to respect in the SMT instance generation process.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver = R"doc(Shortcut to the solver stored in check_point.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_check_point =
R"doc(To reuse solver states, more information is necessary in the SMT
instance generation process. Namely, which tiles have been added in
contrast to the last generation and which tiles got new neighbors,
i.e., have been updated. Additionally, a container for assumptions,
i.e., assertions that are only valid in this and only this run, is
needed. All of that is packaged in a solver check point.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_check_point_added_tiles =
R"doc(added_tiles contains only the newly added tiles in eastern or southern
direction. updated_tiles instead contains the column (eastern) or row
(southern) of tiles that used to be at the border but is not anymore
now. In these tiles, certain assertions change so that their previous
assertions need to be reformulated.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_check_point_assumptions =
R"doc(Assumptions that are true for only one solver run. Always includes the
assumption literal that did not change.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_check_point_state = R"doc(Solver and watched literals.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_check_point_updated_tiles =
R"doc(added_tiles contains only the newly added tiles in eastern or southern
direction. updated_tiles instead contains the column (eastern) or row
(southern) of tiles that used to be at the border but is not anymore
now. In these tiles, certain assertions change so that their previous
assertions need to be reformulated.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_state =
R"doc(Incremental solver calls require some kind of continuous solver state
that can be extended. This state involves a pointer to the solver
alongside its assumption literals.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_state_lit =
R"doc(Watched literals for eastern and southern constraints which are used
to reformulate them.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_state_solver = R"doc(The actual stored solver.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_solver_tree =
R"doc(Tree-like structure mapping already examined aspect ratios to
respective solver states for later reuse. Due to the rather suboptimal
exploration strategy of factorizing tile counts, multiple solver
states need to be kept. It would always be easiest to simply add an
entire row or column to the layout but that way, several tile counts
are skipped. E.g. by exploring 4 x 5 after 4 x 4, one would go
directly from 16 tiles to 20 tiles. If the optimal layout can be found
at 18 tiles, it would be missed. Therefore, the exploration strategy
using factorizations is kept and several solvers are employed that can
be reused at a later point. In the example, the 4 x 4 solver would be
stored and revisited when 4 x 5 is to be explored.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_store_solver_state =
R"doc(Stores the current solver state in the solver tree with aspect ratio
ar as key.

Parameter ``ar``:
    Key to storing the current solver state.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_technology_specific_constraints =
R"doc(Adds constraints to the solver to enforce technology-specific
restrictions.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_tile_ite_counters =
R"doc(Constructs a series of expressions to evaluate how many tiles were
occupied by a given edge. Therefore, all te variables are translated
to expressions of the form ite(te, 1, 0) which allows for applying
z3::sum to them. This is a work around because no such api function
for pseudo boolean exists.

Parameter ``e``:
    Edge to consider.

Parameter ``ve``:
    Vector of expressions to extend.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_update =
R"doc(Resizes the layout and creates a new solver checkpoint from where on
the next incremental instance can be generated.

Parameter ``ar``:
    Current aspect ratio to work on.)doc";

static const char *__doc_fiction_detail_exact_impl_smt_handler_utilize_hierarchical_information =
R"doc(Adds constraints to the solver to prohibit certain node placements
based on the network hierarchy if the clocking scheme is feed-back-
free. Symmetry breaking constraints.)doc";

static const char *__doc_fiction_detail_exact_impl_thread_info =
R"doc(Contains a context pointer and a currently worked on aspect ratio and
can be shared between multiple worker threads so that they can notify
each other via context interrupts based on their individual results,
i.e., a thread that found a result at aspect ratio x * y can interrupt
all other threads that are working on larger layout sizes.)doc";

static const char *__doc_fiction_detail_exact_impl_thread_info_ctx = R"doc(Pointer to a context.)doc";

static const char *__doc_fiction_detail_exact_impl_thread_info_worker_aspect_ratio = R"doc(Currently examined layout aspect ratio.)doc";

static const char *__doc_fiction_detail_exact_impl_update_timeout =
R"doc(Calculates the time left for solving by subtracting the time passed
from the configured timeout and updates Z3's timeout accordingly.

Parameter ``handler``:
    Handler whose timeout is to be updated.

Parameter ``time``:
    Time passed since beginning of the solving process.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data =
R"doc(This struct stores information about the fan-in and fan-out
connections of a gate in a layout. These fan-in and fan-outs are the
preceding and succeeding gates in the logic network. It contains
vectors for fan-ins, fan-outs, and temporary coordinates to clear
before routing. Additionally, it includes layout coordinate paths for
routing signals between the gate and its fan-in/fan-out connections.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_fanins =
R"doc(This vector holds the layout coordinates of all fan-in connections to
the gate.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_fanouts =
R"doc(This vector holds the layout coordinates of all fan-out connections
from the gate.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_route_fanin_1_to_gate =
R"doc(This layout_coordinate_path object represents the path for routing
signals from the first fan-in to the gate within the layout.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_route_fanin_2_to_gate =
R"doc(This layout_coordinate_path object represents the path for routing
signals from the second fan-in to the gate within the layout.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_route_gate_to_fanout_1 =
R"doc(This layout_coordinate_path object represents the path for routing
signals from the gate to the first fan-out within the layout.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_route_gate_to_fanout_2 =
R"doc(This layout_coordinate_path object represents the path for routing
signals from the gate to the second fan-out within the layout.)doc";

static const char *__doc_fiction_detail_fanin_fanout_data_to_clear =
R"doc(During the gate relocation process, this vector holds temporary layout
coordinates that need to be cleared or reset.)doc";

static const char *__doc_fiction_detail_fanout_substitution_impl = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_available_fanouts = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_fanout_substitution_impl = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_generate_fanout_tree = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_get_fanout = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_ntk_topo = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_fanout_substitution_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl = R"doc()doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_border_io_check =
R"doc(Checks if all PI/POs are located at the layout's borders.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_clocked_data_flow_check =
R"doc(Checks for proper clocking of connected tiles based on their assigned
nodes.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_crossing_gates_check =
R"doc(Check for wires crossing gates.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_empty_io_check =
R"doc(Checks if no PI/PO is assigned to an empty tile.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_gate_level_drvs_impl =
R"doc(Standard constructor.

Parameter ``lyt``:
    Gate layout to check for design rule flaws.

Parameter ``p``:
    Parameters.

Parameter ``st``:
    Statistics.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_has_io_check =
R"doc(Checks if PI/PO assignments are present.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_io_pin_check =
R"doc(Checks if all PI/POs are designated pins.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_log_node = R"doc()doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_log_tile =
R"doc(Logs information about the given tile in the given report. Nodes are
logged in this process under the tile position.

Parameter ``t``:
    Tile whose attributes are to be logged.

Parameter ``report``:
    Report to log into.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_lyt = R"doc(Layout to perform design rule checks on.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_missing_connections_check =
R"doc(Checks for non-PO tiles with successors and non-PI tiles without
predecessors.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_non_adjacent_connections_check =
R"doc(Checks for proper clocking of connected tiles based on their assigned
nodes.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_placed_dead_nodes_check =
R"doc(Checks for nodes that are placed but dead.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_run =
R"doc(Performs design rule checks on the stored gate layout. The following
properties are checked.

Design breaking: - Non-adjacent connections - Missing connections -
Wires crossing operations - Non-consecutive clocking of connected
tiles - I/O assigned to empty tiles

Warning: - Unplaced (alive) nodes - Non-wire I/O - Non-border I/O)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_summary =
R"doc(Generates a summarizing one liner for a design rule check.

Parameter ``msg``:
    Message to output in success case. For failure, a "not" will be
    added as a prefix.

Parameter ``chk``:
    Result of the check.

Parameter ``brk``:
    Flag to indicate that a failure is design breaking. If it's not,
    msg is printed as a warning.

Returns:
    Formatted summary message.)doc";

static const char *__doc_fiction_detail_gate_level_drvs_impl_unplaced_nodes_check =
R"doc(Checks for nodes that are not placed but still alive.

Returns:
    Check summary as a one liner.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl = R"doc()doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_all_paths =
R"doc(Stores a collection of all annotated paths
(labeled_layout_coordinate_lookup_path objects) computed thus far to
find intersections with new ones. The edge intersection graph stores
plain paths without the extra set and label. Therefore, after the
generate_edge_intersection_graph function terminates, the extra memory
overhead is being released again.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_connect_clique =
R"doc(Given a collection of paths belonging to the same objective, this
function creates edges in the edge intersection graph between each
pair of corresponding nodes, thus, forming a clique (complete sub-
graph).

Parameter ``objective_paths``:
    Collection of paths belonging to the same objective.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_create_intersection_edges =
R"doc(Given a collection of paths belonging to the same objective, this
function creates edges in the edge intersection graph between each
corresponding node and all of the already existing nodes that
represent paths that intersect with it, i.e., that share at least one
coordinate.

Parameter ``objective_paths``:
    Collection of paths belonging to the same objective.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_edge_id = R"doc()doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_generate_edge_intersection_graph_impl = R"doc()doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_graph = R"doc(The edge intersection graph to be created.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_initiate_objective_nodes =
R"doc(Given a collection of paths belonging to the same objective, this
function assigns them unique labels and generates corresponding nodes
in the edge intersection graph.

Since each path of the same objective objective belongs to a clique in
the final graph, this function additionally stores their node IDs in
the statistics.

Parameter ``objective_paths``:
    Collection of paths belonging to the same objective.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path =
R"doc(Extends the layout_coordinate_path to additionally to the vector
representation of the path also hold a set that allows fast lookup
needed to find intersections (O(log n)). Additionally, a label is
assigned to each path to identify it in the edge intersection graph.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path_append =
R"doc(Overwrites the append function to additionally store the given
coordinate in a set.

Parameter ``c``:
    Coordinate to append to the path.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path_has_intersection_with =
R"doc(Given another path, this function checks if they are not disjoint,
i.e., it looks for at least one coordinate that both paths share.

If, at some point, the set approach is not to be used anymore,
std::find_first_of offers the same functionality on any kind of range.

Template parameter ``Path``:
    Type of other path.

Parameter ``other``:
    The other path.

Returns:
    `true` iff this path and the given one are not disjoint, i.e.,
    share at least one coordinate.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path_has_overlap_with =
R"doc(Like has_intersection_with but allows paths to share crossings, i.e.,
single-tile intersections.

Similar to has_intersection_with, this function also returns `true` if
source and target are matching in both paths.

Template parameter ``Path``:
    Type of other path.

Parameter ``other``:
    The other path.

Returns:
    `true` iff this path and the given one are overlapping, i.e.,
    share at least one coordinate segment of size 2.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path_label = R"doc(Label to identify the path in the edge intersection graph.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_labeled_layout_coordinate_lookup_path_path_elements =
R"doc(Uniquely identify path elements in a set to make them searchable in
O(1).)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_layout = R"doc(Reference to the layout.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_node_id = R"doc(IDs for nodes and edges.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_objectives = R"doc(The routing objectives.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_generate_edge_intersection_graph_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_get_offset =
R"doc(Utility function to calculate the offset that has to be subtracted
from any x-coordinate on the hexagonal layout.

This function iterates through diagonals starting from the bottom left
corner until it finds a non-empty tile or until it has traversed all
diagonals. In each iteration, it checks tiles along the diagonal line.
If it finds a non-empty tile, it calculates an offset value, which is
the number of columns on the left side of the hexagonal layout that
will be empty. This offset is based on the leftmost tile that will
appear in the hexagonal layout.

Template parameter ``HexLyt``:
    Even-row hexagonal gate-level layout return type.

Template parameter ``CartLyt``:
    Input Cartesian gate-level layout type.

Parameter ``lyt``:
    2DDWave-clocked Cartesian gate-level layout to hexagonalize.

Parameter ``cartesian_layout_width``:
    Width of the Cartesian layout.

Parameter ``cartesian_layout_height``:
    Height of the Cartesian layout.

Returns:
    offset.)doc";

static const char *__doc_fiction_detail_get_path =
R"doc(This helper function computes a path between two coordinates using the
A* algorithm.

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``lyt``:
    Reference to the layout.

Parameter ``start``:
    The starting coordinate of the path.

Parameter ``end``:
    The ending coordinate of the path.

Returns:
    The computed path as a sequence of coordinates in the layout.)doc";

static const char *__doc_fiction_detail_get_po_levels = R"doc()doc";

static const char *__doc_fiction_detail_graph_coloring_impl = R"doc()doc";

static const char *__doc_fiction_detail_graph_coloring_impl_convert_node_index =
R"doc(Converts the given node ID of a Brian Crites graph to the
corresponding one used in Graph.

Parameter ``node``:
    Node ID to convert between graph structures.

Returns:
    Corresponding node ID in Graph.)doc";

static const char *__doc_fiction_detail_graph_coloring_impl_graph = R"doc(The graph to be colored.)doc";

static const char *__doc_fiction_detail_graph_coloring_impl_graph_coloring_impl = R"doc()doc";

static const char *__doc_fiction_detail_graph_coloring_impl_is_brian_crites_engine =
R"doc(Checks whether the given engine points towards the usage of an
algorithm from the graph-coloring library by Brian Crites.

Parameter ``engine``:
    Graph coloring engine to check.

Returns:
    `true` iff the given engine is DSATUR, MCS, LMXRLF, or TABUCOL.)doc";

static const char *__doc_fiction_detail_graph_coloring_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_graph_coloring_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_graph_coloring_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl =
R"doc(Implementation of the graph-oriented layout design algorithm. This
class handles the initialization and execution of the algorithm.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Template parameter ``Ntk``:
    Network type.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_adjust_layout_size =
R"doc(Adjusts the layout size based on the last position.

Parameter ``position``:
    The last position in the layout.

Parameter ``layout``:
    The layout to be adjusted.

Parameter ``ssg``:
    The search space graph.

Parameter ``place_info``:
    The placement information.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_acp_solution =
R"doc(The current best solution with respect to the area-crossing product
(ACP), initialized to the maximum possible value. This value will be
updated as better solutions are found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_area_solution =
R"doc(The current best solution with respect to area, initialized to the
maximum possible value. This value will be updated as better solutions
are found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_crossing_solution =
R"doc(The current best solution with respect to the number of crossings,
initialized to the maximum possible value. This value will be updated
as better solutions are found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_custom_solution =
R"doc(The current best solution with respect to a custom cost objective,
initialized to the maximum possible value. This value will be updated
as better solutions are found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_optimized_solution = R"doc(Current best solution w.r.t. area after relocating POs.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_best_wire_solution =
R"doc(The current best solution with respect to the number of wire segments,
initialized to the maximum possible value. This value will be updated
as better solutions are found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_calculate_cost = R"doc()doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_calculate_num_search_space_graphs =
R"doc(Determines the number of search space graphs to generate based on the
selected effort mode and cost objective.

Parameter ``mode``:
    The effort mode chosen for the layout design, determining the
    level of computational effort.

Parameter ``cost``:
    The cost that specifies the optimization objective for the layout
    design.

Returns:
    The number of search space graphs to be generated.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_check_path =
R"doc(Checks if there is a path between the source and destination tiles in
the given layout.

Parameter ``layout``:
    The layout to be checked.

Parameter ``src``:
    The source tile.

Parameter ``dest``:
    The destination tile.

Parameter ``new_gate_loc``:
    Enum indicating if the src or dest have to host a new gate and
    therefore have to be empty. Defaults to `new_gate_location::NONE`.

Parameter ``planar``:
    Only consider crossing-free paths.

Returns:
    A path from `src` to `dest` if one exists.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_custom_cost_objective = R"doc(Custom cost objective.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_get_possible_positions =
R"doc(Retrieves the possible positions for a given node in the layout based
on its type and preceding nodes. It determines the type of the node
(PI, PO, single fan-in, double fan-in) and returns the corresponding
possible positions.

Parameter ``layout``:
    The layout in which to find the possible positions.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``ssg``:
    The search space graph.

Returns:
    A vector of tiles representing the possible positions for the
    current node.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_get_possible_positions_double_fanin =
R"doc(Retrieves the possible positions for a double fan-in node in the given
layout, based on the positions of preceding nodes and a specified
number of expansions.

Parameter ``layout``:
    The layout in which to find the possible positions for a double
    fan-in node.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``num_expansions``:
    The maximum number of positions to be returned.

Parameter ``fc``:
    A vector of nodes that precede the double fanin node.

Parameter ``planar``:
    Only consider crossing-free paths.

Returns:
    A vector of tiles representing the possible positions for a double
    fan-in node.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_get_possible_positions_pis =
R"doc(Retrieves the possible positions for Primary Inputs (PIs) in the given
layout based on the specified criteria of positioning at the top or
left side, with a limit on the number of possible positions.

Parameter ``layout``:
    The layout in which to find the possible positions for PIs.

Parameter ``pi_locs``:
    Struct indicating if PIs are allowed at the top or left side of
    the layout.

Parameter ``num_expansions``:
    The maximum number of positions to be returned (is doubled for
    PIs).

Parameter ``planar``:
    Only consider crossing-free paths.

Returns:
    A vector of tiles representing the possible positions for PIs.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_get_possible_positions_pos =
R"doc(Retrieves the possible positions for Primary Outputs (POs) in the
given layout based on the positions of the preceding nodes.

Parameter ``layout``:
    The layout in which to find the possible positions for POs.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``fc``:
    A vector of nodes that precede the PO nodes.

Parameter ``planar``:
    Only consider crossing-free paths.

Returns:
    A vector of tiles representing the possible positions for POs.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_get_possible_positions_single_fanin =
R"doc(Retrieves the possible positions for a single fan-in node in the given
layout, based on the positions of preceding nodes and a specified
number of expansions.

Parameter ``layout``:
    The layout in which to find the possible positions for a single
    fan-in node.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``num_expansions``:
    The maximum number of positions to be returned.

Parameter ``fc``:
    A vector of nodes that precede the single fanin node.

Parameter ``planar``:
    Only consider crossing-free paths.

Returns:
    A vector of tiles representing the possible positions for a single
    fan-in node.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_graph_oriented_layout_design_impl =
R"doc(Constructor for the graph-oriented layout design algorithm.

Parameter ``src``:
    The source network to be placed.

Parameter ``p``:
    The parameters for the graph-enhanced layout search algorithm.

Parameter ``st``:
    The statistics object to record execution details.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_improve_acp_solution =
R"doc(Flag indicating that an initial solution has been found with the area-
crossings product as cost objective. When set to `true`, subsequent
search space graphs with the area-crossing product as cost objective
can be pruned.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_improve_area_solution =
R"doc(Flag indicating that an initial solution has been found with the
layout area as cost objective. When set to `true`, subsequent search
space graphs with the layout area as cost objective can be pruned.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_improve_crossing_solution =
R"doc(Flag indicating that an initial solution has been found with the
number of crossings as cost objective. When set to `true`, subsequent
search space graphs with the number of crossings as cost objective can
be pruned.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_improve_custom_solution =
R"doc(Flag indicating that an initial solution has been found with a custom
cost objective. When set to `true`, subsequent search space graphs
with a custom cost objective can be pruned.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_improve_wire_solution =
R"doc(Flag indicating that an initial solution has been found with the
number of wire segments as cost objective. When set to `true`,
subsequent search space graphs with the number of wire segments as
cost objective can be pruned.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_initialize_layout =
R"doc(Initializes the layout with minimum width

Parameter ``min_layout_width``:
    The minimum width of the layout.

Parameter ``planar``:
    Create planar layouts with a depth of 0.

Returns:
    The initialized layout.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_max_placed_nodes = R"doc(Keep track of the maximum number of placed nodes.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_ntk = R"doc(The network to be placed and routed.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_num_search_space_graphs = R"doc(Number of search space graphs.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_num_search_space_graphs_high_efficiency = R"doc(In high-efficiency mode, only 2 search space graphs are used)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_num_search_space_graphs_high_effort =
R"doc(In high-effort mode, 12 search space graphs are used: 3 (possible PI
locations) * 2 (fanout substitution strategies) * 2 (topological
orderings))doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_num_search_space_graphs_highest_effort =
R"doc(In highest-effort mode, 48 search space graphs are used. This includes
12 search space graphs for each of the four base cost objectives
layout area, number of wire segments, number of wire crossings, and
area-crossing product.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_num_search_space_graphs_highest_effort_custom =
R"doc(In highest-effort mode with a custom cost function, 60 search space
graphs are used (48 with the standard cost objectives and 12 for the
custom one).)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_place_and_route =
R"doc(Executes a single placement step in the layout for the given network
node. It determines the type of the node, places it accordingly, and
checks if a solution was found.

Parameter ``position``:
    The tile representing the position for placement.

Parameter ``layout``:
    The layout in which to place the node.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``ssg``:
    The search space graph.

Returns:
    A boolean indicating if a solution was found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_print_placement_info =
R"doc(Outputs placement information, including the current runtime, the
number of evaluated paths in the search space graphs and the layout
dimensions.

Parameter ``lyt``:
    Current layout.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_route_double_input_node =
R"doc(Places a node with two inputs in the layout and routes it.

Parameter ``position``:
    The tile representing the position for placement.

Parameter ``layout``:
    The layout in which to place the node.

Parameter ``node2pos``:
    A dictionary mapping nodes from the network to signals in the
    layout.

Parameter ``fc``:
    A vector of nodes that precede the double fanin node.

Parameter ``planar``:
    Only consider crossing-free paths.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_route_single_input_node =
R"doc(Places a node with a single input in the layout and routes it.

Parameter ``position``:
    The tile representing the position for placement.

Parameter ``layout``:
    The layout in which to place the node.

Parameter ``node2pos``:
    A dictionary mapping nodes from the network to signals in the
    layout.

Parameter ``fc``:
    A vector of nodes that precede the single fanin node.

Parameter ``planar``:
    Only consider crossing-free paths.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_run =
R"doc(Executes the graph-oriented layout design algorithm and returns the
best found layout.

Returns:
    The best layout found by the algorithm.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_ssg_vec = R"doc(Vector of search space graphs.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_start = R"doc(Start time.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_timeout = R"doc(Timeout limit (in ms).)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_timeout_limit_reached = R"doc(Timeout limit reached.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_update_stats =
R"doc(This function updates statistical metrics.

Parameter ``best_lyt``:
    The new best layout found.)doc";

static const char *__doc_fiction_detail_graph_oriented_layout_design_impl_valid_layout =
R"doc(Validates the given layout based on the nodes in the network and their
mappings in the node dictionary. It checks if the placement of nodes
in the layout is possible and ensures there are valid paths from each
tile to the drain.

Parameter ``layout``:
    The layout to be validated.

Parameter ``place_info``:
    The placement context containing current node, primary output
    index, node to position mapping, and PI to node mapping.

Parameter ``ssg``:
    The search space graph.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl = R"doc()doc";

static const char *__doc_fiction_detail_ground_state_space_impl_check_charge_space =
R"doc(The charge space of the given cluster is checked by performing the
potential bound analysis on each multiset charge configuration in it
(without composition information). For each invalid one found,
corresponding updates are made to maintain consistency in the cluster
hierarchy data structure. Updates make the stored bound information
more strict, and as a result, more invalid states may be found.

Parameter ``c``:
    The cluster to check the charge space of.

Returns:
    `true` if and only if charge space of `c` has no invalid elements.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_clustering =
R"doc(The clustering starts at all singletons, then moves up through merges
until only the top cluster remains)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_compute_external_pot_bounds_for_saved_compositions =
R"doc(This function performs the first step to merging a set of clusters to
their direct parent. When clusters are merged, their respective charge
spaces have reached a fixed point in the construction; thereby, the
projections specific to each stored composition in the respective
charge spaces, for which previously only the receiving SiDBs in the
respective child cluster were considered, are now composed to
potential bounds onto each SiDB outside the respective cluster, thus
making a complete potential bounds store.

Parameter ``parent``:
    The cluster that parents the clusters to merge.

Returns:
    The number of projector states that is the accumulation of the
    number of projector states in the composition of each charge space
    element of each child.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_compute_meets_for_internal_pot_bounds =
R"doc(This function performs the flatten operation; the partial sum of the
electrostatic potential local to all contained SiDBs as received from
within the cluster is, for each multiset charge configuration in the
new charge space, flattened, with respect to all compositions of the
multiset, to the self-projection of the parent cluster.

The above, and the fact that the self-projection for singletons is
their respective local external electrostatic potential, thus defines
"self-projections" inductively.

Parameter ``parent``:
    The newly-forming parent cluster.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_construct_merged_charge_state_space =
R"doc(This function calls the recursive function above that goes through all
combination of charge space elements of children to form the charge
space of their direct parent.

Parameter ``parent``:
    The parent cluster to which charge spaces are combined.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_construct_merged_potential_projections =
R"doc(After the charge space of the parent has been created, this function
combines associated potential projections by going through the charge
space elements of the children, and, for each recipient SiDB,
aggregates the potential projections from each child onto that SiDB.

Parameter ``parent``:
    The newly-forming parent cluster.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_derive_children_received_bounds_without_siblings =
R"doc(This function dynamically updates the externally received partial sums
of local electrostatic potential of the children, and sets that of the
parent

Parameter ``parent``:
    The newly forming parent cluster.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_fail_onto_negative_charge =
R"doc(Returns `true` if and only if the given potential bound closes out
SiDB-.

Parameter ``pot_bound``:
    Potential lower bound.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_fail_onto_positive_charge =
R"doc(Performs V < -e - mu+.

Parameter ``pot_bound``:
    Potential upper bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB+.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_fill_merged_charge_state_space =
R"doc(This recursive function goes through all combinations of charge space
elements of the children (exactly one element per child for each
combination), and verifies each combination before adding it to the
merged charge space of the parent. Combinations with equivalent
multiset signatures are preserved under different compositions of the
multiset charge configuration in the charge space of the parent.

Parameter ``parent``:
    The newly-forming parent cluster.

Parameter ``cur_child_ix``:
    The index of the child of which precisely one element of its
    charge space should be added to the combination of charge space
    element. When it reaches out of bounds, a combination is complete
    and may be verified before adding it to the parent charge space.

Parameter ``m``:
    The dynamically combined cluster charge state (i.e., multiset
    charge configuration). A copy of `m` when it is a complete
    composition of charge space elements of the children is added to
    the parent charge space if verification passes. After, `m` is
    dynamically transformed to the next combination until no new
    combination exists.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_find_valid_witness_partitioning =
R"doc(A simple brute-force algorithm that solves the validity witness
partitioning problem by looking for a partitioning, i.e., an
assignment that maps each SiDB to a charge state that they are a
witness of in such a way that required witness counts are met for each
SiDB. Starting with the negative charge state, the requirement is
fulfilled in a greedy manner by taking SiDBs witnessing this charge
state. This process continues for the other charge states in the same
way, though when an SiDB has already been assigned, it cannot be used
to reduce the required witness count again. When all SiDBs have been
assigned successfully, a partitioning is found and `true` is returned.
Otherwise, when an in-progress assignment cannot be completed, the
next assignment is tried through back-tracking. When no assignment can
be completed, `false` is returned, which then proves that the multiset
charge configuration that was given to create the witness partitioning
state is invalid.

Template parameter ``current_fill_cs``:
    The charge state for which the function currently looks for
    available witnesses until no more witnesses are required.

Parameter ``st``:
    The witness partitioning state, which holds the required witnesses
    counters for each charge state.

Parameter ``num_witnesses_for_current_cs``:
    The number of available witnesses that are left for
    `current_fill_cs`.

Returns:
    `true` if a validity witness partitioning was found, and `false`
    if no consistent partitioning was found.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_get_received_potential_bounds =
R"doc(This function obtains the pair of potential bounds relevant to a
potential projection associated with a multiset charge configuration
as received by the given SiDB. The entire layout of SiDBs is taken
into account in the returned bounds, as this function accumulates the
received potential that is external to the projecting cluster with
that which is internal to it. Depending on the given potential bound
analysis mode, the latter considers different levels of detail.

Template parameter ``mode``:
    This selects the potential bound analysis mode. Intuitively, when
    analysing a composition, the information that constitutes bounds
    has one level more detail; this information from the deeper level
    in the cluster hierarchy is later flattened to enable analysis of
    the composed multiset charge configuration without the additional
    information.

Parameter ``pst``:
    The projector state that, together with a receiving SiDB, yields a
    pair of bounds on the potential projection from the cluster
    associated with the projector state onto the receiving SiDB.

Parameter ``sidb_ix``:
    The receiving SiDB.

Parameter ``composition_pot_bounds``:
    This optional parameter supplies the additional composition
    information when available.

Returns:
    A pair of doubles that represent the lower and upper bound of the
    potential projection onto the given SiDB.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_ground_state_space_impl =
R"doc(Constructor. Invokes the algorithm with the given parameters on the
given layout.

Parameter ``lyt``:
    Layout to construct the *Ground State Space* of.

Parameter ``parameters``:
    The parameters that *Ground State Space* will use throughout the
    construction.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_lb_fail_onto_neutral_charge =
R"doc(Performs V > e - mu+.

Parameter ``pot_bound``:
    Potential lower bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB0.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_maximum_top_level_multisets =
R"doc(This function computes the maximum size of the charge space of the top
cluster. Comparing this number with the size of the charge space of
the top cluster that is returned after running *Ground State Space*
gives an indication of how the search space was reduced.

Parameter ``number_of_sidbs``:
    The number of SiDBs (:math:`N`).

Returns:
    :math:`\binom{N + 2}{2}` if base = 3, or otherwise :math:`\binom{N
    + 1}{1}` when the base is 2.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_merge_pot_projection_bounds =
R"doc(This function aggregates the bounds on the electrostatic potential
from the respective children onto the given receiving SiDB, giving
rise to the bound from their direct parent onto this SiDB.

Template parameter ``bound``:
    Bound to handle (lower/upper)

Parameter ``parent``:
    The newly-forming parent cluster.

Parameter ``rst``:
    The receptor state with the receiving SiDB that is currently
    handled.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_move_up_hierarchy =
R"doc(This function performs the complete merging operation from a set of
sibling clusters to their direct parent. In this process, combinations
of charge space elements of the children are considered in special
detail, before passing them to the charge space of the parent and
flattening these details such that the process of checking multiset
charge configurations in respective charge spaces of the clusters in
the clustering may continue as before. The termination of the *Ground
State Space* algorithm is signalled when the clustering only contains
the top cluster.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_mu_bounds_with_error =
R"doc(Globally available array of bounds that section the band gap, used for
pruning.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_params = R"doc(Parameters used during the construction.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_perform_potential_bound_analysis =
R"doc(This function receives a multiset charge configuration with an
associated cluster and finds whether this multiset charge
configuration can be declared invalid for this cluster.

Template parameter ``mode``:
    The potential bound analysis mode that switches the function
    between analysing a multiset charge configuration either with or
    without composition information.

Parameter ``pst``:
    The projector state for which the potential bound analysis is to
    be performed.

Parameter ``composition_potential_bounds``:
    This optional parameter supplies the additional composition
    information when available.

Returns:
    `false` if and only if `pst` can be excluded from the *Ground
    State Space*.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_potential_bound_analysis_mode =
R"doc(A switch to enable the potential bound analysis function to analyse
both multiset charge configurations without composition information,
and those with.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_potential_bound_analysis_mode_ANALYZE_COMPOSITION =
R"doc(Switch used to analyse multiset charge configurations with information
of specific composition thereof.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_potential_bound_analysis_mode_ANALYZE_MULTISET =
R"doc(Switch used to analyse multiset charge configurations without
composition information.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_projector_state_count =
R"doc(Count the total number of projector states that are stored in the
constructed hierarchy.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_run =
R"doc(The main loop in the *Ground State Space* construction. Charge spaces
are updated until a fixed point is reached, after which a merging of
children to their direct parent is performed. This process repeats
until the parent to which the children are merged is the top cluster,
the cluster containing all SiDBs in the layout. The Ground State Space
is the resulting hierarchical charge space structure.

Returns:
    The results of the construction, which include the top cluster
    which parents all other clusters, and thereby contains the charge
    spaces of each cluster.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_subtract_sibling_pot_from_received_ext_pot_bound =
R"doc(This function derives the new externally received partial sums of
electrostatic potential local to an SiDB contained by the child
(together forming `child_rst`) through subtracting the projections of
its siblings.

Template parameter ``bound``:
    The Bound to handle (lower/upper)

Parameter ``parent``:
    The newly forming parent cluster.

Parameter ``child_rst``:
    Receptor state from one of the children of the parent.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_terminate = R"doc(`true` if and only if the construction is to be terminated.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_top_cluster =
R"doc(The top cluster, the cluster that contains all SiDBs, is returned as
the result of the construction.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_ub_fail_onto_neutral_charge =
R"doc(Performs V < -e - mu-.

Parameter ``pot_bound``:
    Potential upper bound.

Returns:
    `true` if and only if the given potential bound closes out SiDB0.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_update_charge_spaces =
R"doc(The charge spaces of each cluster in the current clustering are
checked and updated accordingly when needed.

Parameter ``skip_cluster``:
    This optional parameter specifies a cluster to skip in the pass
    over all clusters in the current clustering.

Returns:
    `true` if and only if a fixed point has been reached; i.e., none
    of the charge space contain an element that may be removed.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_verify_composition =
R"doc(This function determines whether a newly composed candidate for the
charge space of the newly forming parent cluster can be rejected in
the creation of the combined charge space. Here, multiset charge
configurations for the parents are checked with composition
information: a combination of respective charge space elements of the
children is considered, thus, intuitively, the information considered
in the analysis goes one level deeper than when later considering the
composed multiset with flattened information with respect to its
compositions.

Parameter ``composition``:
    A composition of charge space elements of the children, making a
    candidate for a charge space element of their direct parent.

Returns:
    `false` if and only if the given composition can be rejected.)doc";

static const char *__doc_fiction_detail_ground_state_space_impl_write_children_pot_bounds_to_complete_store =
R"doc(To facilitate efficient unfolding for the second stage of the
simulation by *ClusterComplete*, potential bound data that is stored
in the hierarchy which will not be subject to change anymore is
converted to a handy format: a complete potential bounds store. It
holds potential bounds for all SiDBs in the considered layout and thus
addition and subtraction of other complete potential bound stores are
easy to perform.

Parameter ``parent``:
    The newly-forming parent cluster whose children's charge spaces
    become fixed upon their merging.)doc";

static const char *__doc_fiction_detail_handle_virtual_pis =
R"doc(Removes virtual primary inputs from a network if supported. Otherwise
the input network is returned unmodified.

Template parameter ``Ntk``:
    The network type.

Parameter ``network``:
    The input network to process.

Returns:
    The network with virtual primary inputs removed, or the original
    network if unsupported.)doc";

static const char *__doc_fiction_detail_is_balanced_impl = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_balanced = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_is_balanced_impl = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_ntk_depth = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_is_balanced_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_is_east_south_colored = R"doc()doc";

static const char *__doc_fiction_detail_is_eastern_po_orientation_available = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl_is_fanout_substituted_impl = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_is_fanout_substituted_impl_substituted = R"doc()doc";

static const char *__doc_fiction_detail_is_operational_impl =
R"doc(Implementation of the `is_operational` algorithm for a given SiDB
layout.

This class provides an implementation of the `is_operational`
algorithm for a specified SiDB layout and parameters. It checks
whether the SiDB layout is operational by simulating its behavior for
different input combinations and comparing the results to expected
outputs from a truth table.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.)doc";

static const char *__doc_fiction_detail_is_operational_impl_bii = R"doc(Iterator that iterates over all possible input states.)doc";

static const char *__doc_fiction_detail_is_operational_impl_canvas_lyt = R"doc(Layout consisting of all canvas SiDBs.)doc";

static const char *__doc_fiction_detail_is_operational_impl_check_existence_of_kinks_in_input_wires =
R"doc(This function iterates through the input wires and evaluates their
charge states against the expected states derived from the input
pattern. A kink is considered to exist if an input wire's charge state
does not match the expected value (i.e., bit one or bit zero) for the
given input index.

Parameter ``ground_state``:
    The ground state charge distribution surface.

Parameter ``current_input_index``:
    The current input index used to retrieve the expected output from
    the truth table.

Returns:
    `true` if any input wire contains a kink (i.e., an unexpected
    charge state), `false` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_check_existence_of_kinks_in_output_wires =
R"doc(This function iterates through the output wires and evaluates their
charge states against the expected states derived from the truth
table. A kink is considered to exist if an output wire's charge state
does not match the expected value (i.e., bit one or bit zero) for the
given input index.

Parameter ``ground_state``:
    The ground state charge distribution surface.

Parameter ``current_input_index``:
    The current input index used to retrieve the expected output from
    the truth table.

Returns:
    `true` if any output wire contains a kink (i.e., an unexpected
    charge state), `false` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_dependent_cell = R"doc(Dependent cell of the canvas SiDBs.)doc";

static const char *__doc_fiction_detail_is_operational_impl_determine_non_operational_input_patterns_and_non_operationality_reason =
R"doc(Determines the input combinations for which the layout is non-
operational and the reason why the layout is non-operational.

Returns:
    Vector of pairs where the first element of the pair is the input
    pattern (e.g. 2-input Boolean function: 00 ^= 0; 10 ^= 2) for
    which the layout is non-operational. The second entry indicates
    the reason why the layout is non-operational
    (`non_operationality_reason`) for the given input pattern.)doc";

static const char *__doc_fiction_detail_is_operational_impl_encodes_bit_one =
R"doc(This function returns `true` if `1` is encoded in the charge state of
the given BDL pair. `false` otherwise.

Parameter ``ground_state``:
    The ground state charge distribution surface.

Parameter ``bdl``:
    BDL pair to be evaluated.

Returns:
    `true` if `1` is encoded, `false` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_encodes_bit_zero =
R"doc(This function returns `true` if `0` is encoded in the charge state of
the given BDL pair. `false` otherwise.

Parameter ``ground_state``:
    The ground state charge distribution surface.

Parameter ``bdl``:
    BDL pair to be evaluated.

Returns:
    `true` if `0` is encoded, `false` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_get_number_of_simulator_invocations =
R"doc(Returns the total number of simulator invocations.

Returns:
    The number of simulator invocations.)doc";

static const char *__doc_fiction_detail_is_operational_impl_input_bdl_wires = R"doc(Input BDL wires.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_io_signal_unstable =
R"doc(This function iterates through various input patterns and output wire
indices to determine if any configuration results in a physically
valid layout with energy below the given energy value, indicating I/O
signal instability.

Parameter ``cds_layout``:
    The charge distribution surface layout to be modified and checked.

Parameter ``max_input_pattern_index``:
    The maximum index for input pattern

Parameter ``input_pattern``:
    The specific input pattern for which the stability check is
    conducted.

Parameter ``logical_correct_output_pattern``:
    The expected correct output pattern for the given input.

Parameter ``minimal_energy_of_physically_valid_layout``:
    The minimum energy threshold below which the layout is considered
    unstable.

Returns:
    `true` if the I/O signal is unstable, `false` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_layout_invalid =
R"doc(This function evaluates whether the given layout is invalid, i.e., it
cannot implement the given Boolean function. This is done in three
separate filtering steps: (1) discarding SiDB layouts with potentially
positively charged SiDBs, (2) utilizing an efficient method to
identify and discard SiDB layouts that do not satisfy physical model
constraints under the I/O pin conditions required for the desired
Boolean function, and (3) detecting I/O signal instability.

Template parameter ``ChargeLyt``:
    The charge distribution surface layout type.

Parameter ``input_pattern``:
    The current input pattern.

Parameter ``cds_canvas``:
    The charge distribution of the canvas layout.

Returns:
    A `layout_invalidity_reason` object indicating why the layout is
    non-operational; or `std::nullopt` if it could not certainly be
    determined to be in fact non-operational.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_operational_impl =
R"doc(Constructor to initialize the algorithm with a layout and parameters.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``tt``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_operational_impl_2 =
R"doc(Constructor to initialize the algorithm with a layout, parameters,
input and output wires.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``tt``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Parameter ``input_wires``:
    BDL input wires of lyt.

Parameter ``output_wires``:
    BDL output wires of lyt.

Parameter ``initialize_bii``:
    If `true`, the BDL input iterator is initialized, `false`
    otherwise. This parameter is only needed in special cases
    (verify_logic_match.hpp).)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_operational_impl_3 =
R"doc(Constructor to initialize the algorithm with a layout, parameters,
input and output wires, and a canvas layout.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Parameter ``input_wires``:
    BDL input wires of lyt.

Parameter ``output_wires``:
    BDL output wires of lyt.

Parameter ``c_lyt``:
    Canvas layout.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_operational_impl_4 =
R"doc(Constructor to initialize the algorithm with a layout and parameters.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_physical_validity_feasible =
R"doc(This function determines if there is a charge distribution of the
canvas SiDBs for which the charge distribution of the whole layout is
physically valid.

Parameter ``cds_layout``:
    The charge distribution surface layout to be evaluated.

Returns:
    The minimum energy value if a physically valid configuration is
    found, `std::nullopt` otherwise.)doc";

static const char *__doc_fiction_detail_is_operational_impl_layout = R"doc(SiDB cell-level layout.)doc";

static const char *__doc_fiction_detail_is_operational_impl_number_of_input_wires = R"doc(Number of input BDL wires.)doc";

static const char *__doc_fiction_detail_is_operational_impl_number_of_output_wires = R"doc(Number of output BDL wires.)doc";

static const char *__doc_fiction_detail_is_operational_impl_output_bdl_pairs = R"doc(Output BDL pairs.)doc";

static const char *__doc_fiction_detail_is_operational_impl_output_bdl_wires = R"doc(Output BDL wires.)doc";

static const char *__doc_fiction_detail_is_operational_impl_parameters = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_detail_is_operational_impl_physical_simulation_of_layout =
R"doc(This function conducts physical simulation of the given SiDB layout.
The simulation results are stored in the `sim_result` variable.

Parameter ``bdl_iterator``:
    BDL input iterator representing the SiDB layout with a given input
    combination.

Returns:
    Simulation results.)doc";

static const char *__doc_fiction_detail_is_operational_impl_run =
R"doc(Run the `is_operational` algorithm.

This function executes the operational status checking algorithm for
the given SiDB layout and parameters provided during initialization.

Returns:
    Pair with the first element indicating the operational status
    (either `OPERATIONAL` or `NON_OPERATIONAL`) and the second element
    indicating the reason if it is non-operational.)doc";

static const char *__doc_fiction_detail_is_operational_impl_set_charge_distribution_of_input_pins =
R"doc(This function assigns the charge states of the input pins in the
layout according to the input index provided. This means that when a
zero is applied, each BDL pair in the wire is set to zero.

Parameter ``cds``:
    The charge distribution surface layout to be modified.

Parameter ``current_input_index``:
    The index representing the current input pattern.)doc";

static const char *__doc_fiction_detail_is_operational_impl_set_charge_distribution_of_output_pins =
R"doc(This function assigns the charge states of the output pins in the
layout according to the input index provided. This means that when a
zero is applied, each BDL pair in the wire is set to zero.

Parameter ``cds``:
    The charge distribution surface layout to be modified.

Parameter ``output_wire_index``:
    The index representing the current input pattern of the output
    wire.)doc";

static const char *__doc_fiction_detail_is_operational_impl_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_is_operational_impl_truth_table = R"doc(The specification of the layout.)doc";

static const char *__doc_fiction_detail_is_operational_impl_verify_logic_match_of_cds =
R"doc(Checks if the given charge distribution correctly encodes the expected
logic for the given input pattern, based on a provided truth table.

Example: In the ground state charge distribution of an AND gate, kinks
are rejected for the gate to be considered operational. Given an input
pattern of `01`, this function will: - Verify that the left input wire
encodes `0`. - Verify that the right input wire encodes `1`. - Verify
that the output wire encodes `0`. Determines if the given charge
distribution fulfills the correct logic based on the provided charge
index and truth table.

Parameter ``given_cds``:
    The charge distribution surface to be checked for operation.

Parameter ``input_pattern``:
    Input pattern represented by the position of perturbers.

Returns:
    Pair with the first element indicating the operational status
    (either `OPERATIONAL` or `NON_OPERATIONAL`) and the second element
    indicating the reason if it is non-operational.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_closed_list = R"doc(Closed list that acts as a set of already visited coordinates.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f = R"doc(Stores a coordinate and its f-value.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_coord = R"doc(Coordinate.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_f = R"doc(f-value of the coordinate.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_operator_eq =
R"doc(Equality operator that only compares the stored coordinates. This
allows for a convenient lookup in the priority queue.

Parameter ``other``:
    Other coordinate_f to compare with.

Returns:
    `true` iff this coord value is equal to the other's.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_operator_gt =
R"doc(Comparator for the priority queue. Compares only the f-values.

Parameter ``other``:
    Other coordinate_f to compare with.

Returns:
    `true` iff this f-value is greater than the other's.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_distance =
R"doc(The distance functor that implements the heuristic estimation
function.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_expand =
R"doc(Expands the frontier of coordinates to visit next in the direction of
the heuristic cost function.

Parameter ``current``:
    Coordinate that is currently examined.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_fill_in_jumps =
R"doc(Completes a reconstructed path by filling in the jumps that were made
during JPS.

Parameter ``p``:
    Incomplete path with jump gaps.

Returns:
    Completed interpolated path.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_g =
R"doc(Returns the g-value of a coordinate by accessing the g_values map.
Returns 0 if no entry exists.

Parameter ``c``:
    Coordinate whose g-value is desired.

Returns:
    g-value of coordinate c or 0 if no value has been stored.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_g_values =
R"doc(Stores the g-cost for each coordinate c, i.e., the length of the
shortest path from source to c found so far.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_get_lowest_f_coord =
R"doc(Fetches and pops the coordinate with the lowest f-value from the open
list priority queue.

Returns:
    Coordinate with the lowest f-value from the open list.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_interpolate =
R"doc(Interpolates a straight line between two given coordinates using
Bresenham's algorithm:
http://en.wikipedia.org/wiki/Bresenham's_line_algorithm

This function is repeatedly used in the fill_in_jumps function to
interpolate the jump gaps.

Parameter ``c1``:
    Start coordinate.

Parameter ``c2``:
    Goal coordinate.

Returns:
    A straight path between c1 and c2.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_is_traversable =
R"doc(Checks if coordinate c2 is directly reachable from coordinate c1. That
is if the coordinates are not identical, if they are adjacent or if
the clocking checks out, and if there is no obstacle in the way.

Parameter ``c1``:
    Start coordinate.

Parameter ``c2``:
    Goal coordinate.

Returns:
    `true` iff c2 is directly reachable from c1.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_is_visited =
R"doc(Checks if a coordinate has been visited already.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff c has already been visited.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_jump =
R"doc(Compute the next jump point starting from successor with current as
its parent. The vector (current, successor) thus defines the direction
in which to look for the jump point. A jump point has to be stepwise
traversable from successor. That is, there must be a valid path
through non-obstructed tiles.

Parameter ``current``:
    Parent coordinate.

Parameter ``successor``:
    Coordinate from which to look for the next jump point.

Returns:
    The next jump point or `std::nullopt` if no such coordinate is
    available.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_jump_point_search_impl = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_layout =
R"doc(The layout in which the shortest path between `source` and `target` is
to be found.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_no_improvement =
R"doc(Checks if the given g-value is greater or equal than the stored
g-value of the given coordinate. If that is the case, the new g-value
does not improve the path.

Parameter ``c``:
    Coordinate to whose g-value is to be checked.

Parameter ``g_val``:
    g-value to compare to c's.

Returns:
    `true` iff the given g-value does not mean an improvement for the
    given coordinate.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_objective = R"doc(The source-target coordinate pair.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_open_list =
R"doc(Open list that contains all coordinates to process next sorted by
their f-value.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_reconstruct_path =
R"doc(Reconstruct the final path from the origin map that was created during
the path finding algorithm.

Returns:
    The shortest path connecting source and target.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_set_g =
R"doc(Updates the g-value of the given coordinate to the given value.

Parameter ``c``:
    Coordinate whose g-value is to be updated to g_val.

Parameter ``g_val``:
    New g-value for c.)doc";

static const char *__doc_fiction_detail_layout_invalidity_reason =
R"doc(Reason why the layout is not a valid gate implementation for the given
Boolean function.)doc";

static const char *__doc_fiction_detail_layout_invalidity_reason_IO_INSTABILITY =
R"doc(I/O signals are unstable, indicating that an information flip results
in a lower energy state.)doc";

static const char *__doc_fiction_detail_layout_invalidity_reason_PHYSICAL_INFEASIBILITY =
R"doc(The layout is physically infeasible, meaning no charge distribution of
the canvas SiDBs satisfies the criteria for physical validity.)doc";

static const char *__doc_fiction_detail_layout_invalidity_reason_POTENTIAL_POSITIVE_CHARGES = R"doc(Positive SiDBs can potentially occur.)doc";

static const char *__doc_fiction_detail_nested_vector_hash =
R"doc(This struct defines a hash function for a nested vector of layout
tiles. It calculates a combined hash value for a vector of tiles based
on the coordinates of each tile.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.)doc";

static const char *__doc_fiction_detail_nested_vector_hash_operator_call =
R"doc(Computes a hash value for a vector of `tile` objects.

Parameter ``vec``:
    The vector of tiles to be hashed.

Returns:
    A combined hash value for the vector of tiles.)doc";

static const char *__doc_fiction_detail_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_depth = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_topo = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_new_gate_location =
R"doc(When checking for possible paths on a layout between two tiles SRC and
DEST, one of them could also be the new tile for the next gate to be
placed and it therefore has to be checked if said tile is still empty)doc";

static const char *__doc_fiction_detail_new_gate_location_DEST = R"doc(Check if the destination tile is empty.)doc";

static const char *__doc_fiction_detail_new_gate_location_NONE = R"doc(Do not check any tiles.)doc";

static const char *__doc_fiction_detail_new_gate_location_SRC = R"doc(Check if the source tile is empty.)doc";

static const char *__doc_fiction_detail_non_operationality_reason = R"doc(Reason why a layout is non-operational.)doc";

static const char *__doc_fiction_detail_non_operationality_reason_KINKS = R"doc(Kinks induced the layout to become non-operational.)doc";

static const char *__doc_fiction_detail_non_operationality_reason_LOGIC_MISMATCH = R"doc(The layout is non-operational because of logic mismatch.)doc";

static const char *__doc_fiction_detail_non_operationality_reason_NONE = R"doc(No reason for non-operationality could be determined.)doc";

static const char *__doc_fiction_detail_non_operationality_reason_POTENTIAL_POSITIVE_CHARGES = R"doc(Positive charges may occur but the simulation base is set to `2`.)doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl = R"doc()doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_design_circuit_on_defective_surface = R"doc()doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_lattice_tiling = R"doc(Gate-level layout.)doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_network = R"doc(Network.)doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_on_the_fly_circuit_design_impl = R"doc()doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_params = R"doc(Parameters for the on-the-fly circuit design.)doc";

static const char *__doc_fiction_detail_on_the_fly_circuit_design_impl_stats = R"doc(Statistics for the on-the-fly circuit design.)doc";

static const char *__doc_fiction_detail_operational_domain_impl = R"doc()doc";

static const char *__doc_fiction_detail_operational_domain_impl_canvas_lyt = R"doc(This layout consists of the canvas cells of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_contour_tracing =
R"doc(Performs contour tracing to determine the operational domain. The
algorithm first performs a random sampling of up to the specified
number of samples. It stops random sampling once it finds a single
operational point, from which it moves straight outwards until it
encounters the counter of the operational domain. From this point, it
traces the contour until it reaches the initial contour point again.
The operational domain will finally only contain up to `samples`
random non-operational points as well as the contour of the found
operational domain plus a one pixel wide border around it.

Parameter ``samples``:
    Maximum number of random samples to be taken before contour
    tracing.

Returns:
    The (partial) operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_find_operational_contour_step_point =
R"doc(Finds a boundary starting point for the contour tracing algorithm.
This function starts at the given starting point and moves towards the
left edge of the parameter range. It returns the last operational
point it encounters before it reaches the edge. If no non-operational
point is found, the operational area extends outside the parameter
range and the function returns the last operational point that was
investigated, i.e., a point at the border of the parameter range.

Parameter ``starting_point``:
    Starting step point for the boundary search.

Returns:
    An operational step point at the edge of the operational domain
    `starting_point` is located in.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_find_operational_step_point_via_random_sampling =
R"doc(Performs random sampling to find any operational parameter
combination. This function is useful if a single starting point is
required within the domain to expand from. This function returns the
step in all dimensions of the first operational point found. If no
operational parameter combination can be found within the given number
of samples, the function returns `std::nullopt`.

This function adds any sampled points to the `op_domain` member
variables.

Parameter ``samples``:
    Maximum number of samples to take. Works as a timeout.

Returns:
    The first operational step point, if any could be found,
    `std::nullopt` otherwise.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_flood_fill =
R"doc(Performs flood fill to determine the operational domain. The algorithm
first performs a random sampling of the specified number of samples.
From each operational point found in this way, it starts the flood
fill. The operational domain will finally only contain up to `samples`
random non-operational points as well as all operational points that
are reachable via flood fill from the found operational points plus a
one pixel wide border around the domain.

Parameter ``samples``:
    Maximum number of random samples to be taken before flood fill.

Parameter ``given_parameter_point``:
    Optional parameter point in the parameter space. If it lies within
    the operational region, it is used as a starting point for flood
    fill.

Returns:
    The (partial) operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_generate_random_step_points =
R"doc(Generates unique random `step_points` in the stored parameter range.
The number of generated points is at most equal to `samples`.

Parameter ``samples``:
    Maximum number of random `step_point`s to generate.

Returns:
    A vector of unique random `step_point`s in the stored parameter
    range of size at most equal to `samples`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_grid_search =
R"doc(Performs a grid search over the specified parameter ranges with the
specified step sizes. The grid search always has quadratic complexity.
The operational status is computed for each parameter combination.

Returns:
    The operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_grid_search_for_physically_valid_parameters =
R"doc(Performs a grid search over the specified parameter ranges. For each
physical parameter combination found for which the given CDS is
physically valid, it is determined whether the CDS is the ground state
or the n-th excited state.

Parameter ``lyt``:
    SiDB cell-level layout that is simulated and compared to the given
    CDS.

Returns:
    All physically valid physical parameters and the excited state
    number.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_indices = R"doc(Dimension steps.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_infer_operational_status_in_enclosing_contour =
R"doc(Given a starting point, this function marks all points that are
enclosed by the operational domain contour as 'inferred operational'.
This assumes that the operational domain does not have holes. To the
best of the author's knowledge, at the time of writing this code,
there exists no proof that operational domains are always continuous,
i.e., hole-free. Marking points as 'inferred operational' can be
useful to avoid recomputation in, e.g., contour tracing if an
operational domain with multiple islands is investigated.

The function starts at the given starting point and performs flood
fill to mark all points that are reachable from the starting point
until it encounters the non-operational edges.

Note that no physical simulation is conducted by this function!

Parameter ``starting_point``:
    Step point at which to start the inference. If `starting_point` is
    non-operational, this function might invoke undefined behavior.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_inferred_op_domain =
R"doc(All the points inferred (assumed) to be operational but not actually
simulated.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_input_bdl_wires = R"doc(Input BDL wires.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_is_step_point_inferred_operational =
R"doc(Checks whether the given step point is part of the inferred
operational domain. If it is, the point is marked as enclosed in the
operational domain. No simulation is performed on `sp`. If `sp` is not
contained in the inferred operational domain, it does not mean that
`sp` is definitely non-operational. It could still appear in the
regular operational domain with either status.

This function is used by the contour tracing algorithm.

Parameter ``sp``:
    Step point to check for inferred operational status.

Returns:
    `true` iff `sp` is contained in `inferred_op_domain`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_is_step_point_operational =
R"doc(Logs and returns the operational status at the given point `sp = (d1,
..., dn)`. If the point has already been sampled, it returns the
cached value. Otherwise, a ground state simulation is performed for
all input combinations of the stored layout using the given simulation
parameters. It terminates as soon as a non-operational state is found.
In the worst case, the function performs :math:`2^i` simulations,
where :math:`i` is the number of inputs of the layout. This function
is used by all operational domain computation techniques.

Any investigated point is added to the stored `op_domain`, regardless
of its operational status.

Parameter ``sp``:
    Step point to be investigated.

Returns:
    The operational status of the layout under the given simulation
    parameters.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_is_step_point_suitable =
R"doc(This function checks if the given charge distribution surface (CDS) is
physically valid for the parameter point represented by the step point
`sp`.

Parameter ``lyt``:
    CDS to check.

Parameter ``sp``:
    Step point to be investigated.

Returns:
    The operational status of the layout under the given simulation
    parameters.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_layout = R"doc(The SiDB cell-level layout to investigate.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_log_stats =
R"doc(Helper function that writes the the statistics of the operational
domain computation to the statistics object. Due to data races that
can occur during the computation, each value is temporarily held in an
atomic variable and written to the statistics object only after the
computation has finished.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_moore_neighborhood_2d =
R"doc(Returns the 2D Moore neighborhood of the step point at `sp = (x, y)`.
The 2D Moore neighborhood is the set of all points that are adjacent
to `(x, y)` in the plane including the diagonals. Thereby, the 2D
Moore neighborhood contains up to 8 points as points outside of the
parameter range are not gathered. The points are returned in clockwise
order starting from the right neighbor.

Parameter ``sp``:
    Step point to get the 2D Moore neighborhood of.

Returns:
    The 2D Moore neighborhood of the step point at `sp = (x, y)`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_moore_neighborhood_3d =
R"doc(Returns the 3D Moore neighborhood of the step point at `sp = (x, y,
z)`. The 3D Moore neighborhood is the set of all points that are
adjacent to `(x, y, z)` in the 3D space including the diagonals.
Thereby, the 3D Moore neighborhood contains up to 26 points as points
outside of the parameter range are not gathered. The points are
returned in no particular order.

Parameter ``sp``:
    Step point to get the 3D Moore neighborhood of.

Returns:
    The 3D Moore neighborhood of the step point at `sp = (x, y, z)`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_dimensions = R"doc(The number of dimensions.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_evaluated_parameter_combinations = R"doc(Number of evaluated parameter combinations.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_steps =
R"doc(Calculates the number of steps in the given dimension based on the
provided parameters.

Returns:
    The number of steps in the given dimension.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_number_of_threads = R"doc(Number of available hardware threads.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_op_domain = R"doc(The operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_operational_domain_impl =
R"doc(Standard constructor. Initializes the lyt, the truth table, the
parameters and the statistics. Also detects the output BDL pair, which
is necessary for the operational domain computation. The lyt must have
exactly one output BDL pair.

Parameter ``lyt``:
    SiDB cell-level lyt to be evaluated.

Parameter ``tt``:
    Expected Boolean function of the lyt given as a multi-output truth
    table.

Parameter ``ps``:
    Parameters for the operational domain computation.

Parameter ``st``:
    Statistics of the process.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_operational_domain_impl_2 =
R"doc(Additional Constructor. Initializes the layout, the parameters and the
statistics.

Parameter ``lyt``:
    SiDB cell-level layout to be evaluated.

Parameter ``ps``:
    Parameters for the operational domain computation.

Parameter ``st``:
    Statistics of the process.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_output_bdl_pairs = R"doc(The output BDL pairs of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_output_bdl_wires = R"doc(Output BDL wires.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_params = R"doc(The parameters for the operational domain computation.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_random_sampling =
R"doc(Performs a random sampling of the specified number of samples within
the specified parameter range. The operational status is computed for
each sample point.

Parameter ``samples``:
    Number of random samples to be taken.

Returns:
    The (partial) operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_set_dimension_value =
R"doc(Helper function that sets the value of a sweep dimension in the
simulation parameters.

Parameter ``sim_parameters``:
    Simulation parameter object to set the sweep dimension `dim` to
    value `val`.

Parameter ``val``:
    Value to set the dimension `dim` to.

Parameter ``dim``:
    Sweep dimension to set the value `val` to.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_simulate_operational_status_in_parallel =
R"doc(Simulates the operational status of the given points in parallel. It
divides the work among multiple threads to speed up the computation.

@note The distribution of the work among threads is a simple slice-
based approach. If your step points are ordered, consider shuffling
the vector first for better load balancing. Otherwise, some threads
might finish early if they got assigned a slice with mainly non-
operational samples, which are faster to compute due to the early
termination condition.

Parameter ``step_points``:
    A vector of step points for which the operational status is to be
    simulated.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_stats = R"doc(The statistics of the operational domain computation.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point = R"doc(Forward-declare step_point.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_2 = R"doc(Forward-declare step_point.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_operator_eq =
R"doc(Equality operator.

Parameter ``other``:
    Other step point to compare with.

Returns:
    `true` iff the step points are equal.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_operator_lt =
R"doc(Less than operator.

Parameter ``other``:
    Other step point to compare with.

Returns:
    `true` if this step point is less than to the other.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_operator_ne =
R"doc(Inequality operator.

Parameter ``other``:
    Other step point to compare with.

Returns:
    `true` iff the step points are not equal.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_step_point = R"doc(Standard default constructor.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_step_point_2 =
R"doc(Standard constructor.

Parameter ``steps``:
    All dimension step values.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_step_values = R"doc(All dimension step values.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_to_parameter_point =
R"doc(Converts a step point to a parameter point.

Parameter ``sp``:
    Step point to convert.

Returns:
    The parameter point corresponding to the step point `sp`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_to_step_point =
R"doc(Converts a parameter point to a step point.

Parameter ``pp``:
    Parameter point to convert.

Returns:
    The step point corresponding to the parameter point `pp`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_truth_table = R"doc(The logical specification of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_values = R"doc(All dimension values.)doc";

static const char *__doc_fiction_detail_optimize_output_positions =
R"doc(Utility function that moves outputs from the last row to the previous
row, and from the last column to the previous column, if possible.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    Gate-level layout.)doc";

static const char *__doc_fiction_detail_orthogonal_impl = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_orthogonal_impl = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_po_counter = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_physical_population_stability_impl =
R"doc(This class implements the simulation of the population stability for a
given SiDB layout. It determines the minimum electrostatic potential
required for charge state transitions within the layout and identifies
the corresponding critical SiDB along with the type of charge state
transition.

Template parameter ``Lyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_collect_energy_and_charge_index =
R"doc(Collects the system energy with the corresponding charge index
information of all physically valid charge distributions of a given
SiDB layout.

Parameter ``sim_results``:
    The simulation results, including all physically valid charge
    distributions.

Returns:
    A vector of energy_and_charge_index pairs, where each pair
    consists of a double value representing the system energy and a
    uint64_t representing the unique charge index. The vector is
    sorted in ascending order of the energy values.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_energy_and_charge_index =
R"doc(This struct represents the electrostatic energy and charge index of a
charge distribution.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_energy_and_charge_index_charge_index = R"doc(Charge index of the charge distribution.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_energy_and_charge_index_energy = R"doc(Electrostatic energy of the charge distribution (unit: eV).)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_handle_negative_charges =
R"doc(This function checks if the absolute difference between the given
local potential and µ- is smaller than the current minimum potential
difference to transition for a negatively charged SiDB. If `true`, it
updates the population stability information with the new minimum
difference and critical cell.

Parameter ``local_potential``:
    The local potential associated with the cell.

Parameter ``c``:
    The cell for which the charge state is being considered (SiDB is
    negatively charged).

Parameter ``pop_stability_information``:
    The current population stability information.

Returns:
    An updated population stability information with potential
    transition details.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_handle_neutral_charges =
R"doc(This function checks if the absolute difference between the given
local potential and µ- or µ+ is smaller than the current minimum
potential difference. If `true`, it updates the population stability
information with the new minimum difference and critical cell.

Parameter ``local_potential``:
    The local potential associated with the cell.

Parameter ``c``:
    The cell for which the charge state is being considered (SiDB is
    neutrally charged).

Parameter ``pop_stability_information``:
    The current population stability information.

Returns:
    An updated population stability information with potential
    transition details.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_handle_positive_charges =
R"doc(This function checks if the absolute difference between the given
local potential and µ+ is smaller than the current minimum potential
difference. If true`, it updates the population stability information
with the new minimum difference and critical cell.

Parameter ``local_potential``:
    The local potential associated with the cell.

Parameter ``c``:
    The cell for which the charge state is being considered (SiDB is
    positively charged).

Parameter ``pop_stability_information``:
    The current population stability information.

Returns:
    An updated population stability information with potential
    transition details.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_layout = R"doc(Layout to analyze.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_params = R"doc(Parameters required to simulate the population stability.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_physical_population_stability_impl =
R"doc(Constructor for physical_population_stability_impl.

Parameter ``lyt``:
    SiDB layout.

Parameter ``parameters``:
    The simulation parameters used.)doc";

static const char *__doc_fiction_detail_physical_population_stability_impl_run =
R"doc(Runs a population stability simulation for a given SiDB layout using
the provided simulation parameters. This function determines the
minimum electrostatic potential required for charge state transitions
within the layout and identifies the corresponding critical SiDB along
with the type of charge state transition.

Returns:
    A vector of population stability information structures, where
    each element represents a charge distribution in ascending energy
    order. Each structure contains details about the critical SiDB,
    the type of charge state transition, and the minimum electrostatic
    potential required for the charge transition.)doc";

static const char *__doc_fiction_detail_pi_locations = R"doc(This enum class indicates the allowed positions for PIs.)doc";

static const char *__doc_fiction_detail_pi_locations_LEFT = R"doc(Flag indicating if primary inputs (PIs) can be placed at the left.)doc";

static const char *__doc_fiction_detail_pi_locations_TOP = R"doc(Flag indicating if primary inputs (PIs) can be placed at the top.)doc";

static const char *__doc_fiction_detail_pi_locations_TOP_AND_LEFT =
R"doc(Flag indicating if primary inputs (PIs) can be placed at the top and
at the left.)doc";

static const char *__doc_fiction_detail_place_outputs =
R"doc(Places the primary outputs (POs) in the layout.

This function positions the POs within the provided layout. If a PO is
a multi-output node (a fanout with two POs connected to it), the
second PO is automatically placed and connected below the first
output.

The positioning of multi-output nodes will follow this structure:

F→=→...→O ↓ =→=→...→O

Template parameter ``Lyt``:
    Desired gate-level layout type.

Template parameter ``Ntk``:
    Network type that acts as specification.

Parameter ``layout``:
    Current gate-level layout.

Parameter ``ctn``:
    Contains the colored network.

Parameter ``po_counter``:
    Counter for POs.

Parameter ``node2pos``:
    Mapping from network nodes to layout signals, i.e., a pointer to
    their position in the layout. The map is used to fetch location of
    the fanins. The `mockturtle::node_map` is not updated by this
    function.)doc";

static const char *__doc_fiction_detail_placement_info =
R"doc(Struct to hold information necessary for gate placement during layout
generation for one vertex.

Template parameter ``ObstrLyt``:
    The type of the layout.)doc";

static const char *__doc_fiction_detail_placement_info_current_node = R"doc(The index of the current node being placed.)doc";

static const char *__doc_fiction_detail_placement_info_current_po = R"doc(The index of the current primary output.)doc";

static const char *__doc_fiction_detail_placement_info_node2pos = R"doc(Mapping of nodes to their positions in the layout.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl = R"doc()doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_add_fanin_to_route =
R"doc(This helper function is used to add a fanin coordinate to the
appropriate route based on whether it belongs to the the route from
the first or second fanin to the gate.

Parameter ``fanin``:
    The fanin coordinate to be added to the route.

Parameter ``is_first_fanin``:
    A boolean indicating whether this is part of the route from the
    first fanin to the gate.

Parameter ``ffd``:
    Reference to the fanin_fanout_data structure containing the
    routes.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_add_fanout_to_route =
R"doc(This helper function is used to add a fanout coordinate to the
appropriate route based on whether it belongs to the the route from
the gate to the first or second fanout.

Parameter ``fanout``:
    The fanout coordinate to be added to the route.

Parameter ``is_first_fanout``:
    A boolean indicating whether it belongs to the route from the gate
    to the first fanout.

Parameter ``ffd``:
    Reference to the fanin_fanout_data structure containing the
    routes.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_check_new_position =
R"doc(Attempts to relocate a gate to a new position within the layout and
updates routing connections accordingly.

Parameter ``lyt``:
    Obstructed gate-level layout being optimized.

Parameter ``new_pos``:
    The target tile position to which the gate is to be relocated.

Parameter ``num_gate_relocations``:
    Reference to a counter tracking the number of gate relocations
    performed.

Parameter ``current_pos``:
    Reference to the current position of the gate being relocated.
    This will be updated upon successful relocation.

Parameter ``fanins``:
    Vector containing the tile positions of all fan-in connections to
    the gate.

Parameter ``fanouts``:
    Vector containing the tile positions of all fan-out connections
    from the gate.

Parameter ``moved_gate``:
    Reference to a boolean flag that will be set to `true` if the gate
    is successfully moved.

Parameter ``old_pos``:
    The original tile position of the gate before the relocation
    attempt.

Returns:
    `true` if the gate was successfully relocated to `new_pos` and all
    routing paths were established. `false` if the relocation resulted
    in no movement (i.e., `new_pos` is the same as `old_pos`).)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_fix_wires =
R"doc(Utility function to move wires that cross over empty tiles down one
layer. This can happen if the wiring of a gate is deleted.

Parameter ``lyt``:
    Obstructed gate-level layout.

Parameter ``deleted_coords``:
    Tiles that got deleted.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_get_fanin_and_fanouts =
R"doc(Utility function to trace back fanins and fanouts of a gate. Based on
the gate to be moved, this function returns the location of the fanins
and fanouts, as well as the wiring in between them. Additionally, all
wire tiles between fanins and the gate, as well as between the gate
and fanouts are collected for deletion.

Parameter ``lyt``:
    Obstructed gate-level layout.

Parameter ``op``:
    coordinate of the gate to be moved.

Returns:
    fanin and fanout gates, wires to be deleted and old routing paths.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_get_path_and_obstruct =
R"doc(This helper function computes a path between two coordinates using the
A* algorithm. It then obstructs the tiles along the path in the given
layout.

Parameter ``lyt``:
    Obstructed gate-level layout.

Parameter ``start_tile``:
    The starting coordinate of the path.

Parameter ``end_tile``:
    The ending coordinate of the path.

Returns:
    The computed path as a sequence of coordinates in the layout.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_improve_gate_location =
R"doc(Utility function that moves gates to new coordinates and checks if
routing is possible. This includes:

- removing the old wiring between fanins, the gate and fanouts -
updating the incoming signals - determining coordinates that would
improve the layout - testing all those coordinates by moving the gate
to each one and checking if a new wiring can be found - if a new
coordinate is found and wiring is possible, it is applied and incoming
signals are updated - if no better coordinate is found, the old wiring
is restored

Parameter ``lyt``:
    Obstructed gate-level layout.

Parameter ``old_pos``:
    Old position of the gate to be moved.

Returns:
    `true` if the gate was moved successfully, `false` otherwise.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_max_gate_relocations = R"doc(Maximum number of relocations to try for each gate.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_max_non_po = R"doc(Maximum coordinate of all gates that are not POs.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_plyt = R"doc(2DDWave-clocked Cartesian gate-level layout to optimize.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_post_layout_optimization_impl = R"doc()doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_ps = R"doc(Post-layout optimization parameters.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_pst = R"doc(Statistics about the post-layout optimization process.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_restore_original_wiring =
R"doc(Restores the original wiring if relocation of a gate fails.

This function moves the gate back to its original position and
reinstates the previous wiring paths between the gate and its fan-
in/fan-out connections. It also updates the obstructions in the layout
accordingly.

Parameter ``lyt``:
    Obstructed gate-level layout.

Parameter ``old_path_from_fanin_1_to_gate``:
    The original routing path from the first fan-in to the gate (if
    exists).

Parameter ``old_path_from_fanin_2_to_gate``:
    The original routing path from the second fan-in to the gate (if
    exists).

Parameter ``old_path_from_gate_to_fanout_1``:
    The original routing path from the gate to the first fan-out (if
    exists).

Parameter ``old_path_from_gate_to_fanout_2``:
    The original routing path from the gate to the second fan-out (if
    exists).

Parameter ``current_pos``:
    Current position of the gate after relocation attempt.

Parameter ``old_pos``:
    Original position of the gate before relocation attempt.

Parameter ``fanouts``:
    Vector of fanout tiles connected to the gate.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_start = R"doc(Start time.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_timeout_limit_reached = R"doc(Timeout limit reached.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_update_timeout =
R"doc(Calculates the elapsed milliseconds since the `start` time, sets the
`timeout_limit_reached` flag if the timeout is exceeded, and returns
the remaining time.

Returns:
    Remaining time in milliseconds before timeout, or `0` if timeout
    has been reached.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_wiring_reduction_params = R"doc(Wiring reduction parameters.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_wiring_reduction_stats = R"doc(Wiring reduction stats.)doc";

static const char *__doc_fiction_detail_priority_queue =
R"doc(A priority queue class for managing elements with associated
priorities. The elements are stored in a priority queue, with the
highest priority elements being retrieved first.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.)doc";

static const char *__doc_fiction_detail_priority_queue_counter = R"doc(Counter to keep track of the insertion order of elements.)doc";

static const char *__doc_fiction_detail_priority_queue_empty =
R"doc(Checks if the priority queue is empty.

Returns:
    True if the priority queue is empty, false otherwise.)doc";

static const char *__doc_fiction_detail_priority_queue_get =
R"doc(Retrieves and removes the element with the highest priority from the
queue.

Returns:
    The element with the highest priority.)doc";

static const char *__doc_fiction_detail_priority_queue_put =
R"doc(Adds an element to the priority queue with a given priority.

Parameter ``item``:
    The element to be added.

Parameter ``priority``:
    The priority of the element.)doc";

static const char *__doc_fiction_detail_qca_energy_dissipation_impl = R"doc()doc";

static const char *__doc_fiction_detail_qca_energy_dissipation_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_qca_energy_dissipation_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_qca_energy_dissipation_impl_qca_energy_dissipation_impl = R"doc()doc";

static const char *__doc_fiction_detail_qca_energy_dissipation_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_qcad_cell_pos = R"doc()doc";

static const char *__doc_fiction_detail_qcad_cell_pos_x = R"doc()doc";

static const char *__doc_fiction_detail_qcad_cell_pos_y = R"doc()doc";

static const char *__doc_fiction_detail_qcad_color = R"doc()doc";

static const char *__doc_fiction_detail_qcad_color_blue = R"doc()doc";

static const char *__doc_fiction_detail_qcad_color_green = R"doc()doc";

static const char *__doc_fiction_detail_qcad_color_red = R"doc()doc";

static const char *__doc_fiction_detail_quickexact_impl = R"doc()doc";

static const char *__doc_fiction_detail_quickexact_impl_all_sidbs_in_lyt_without_negative_preassigned_ones = R"doc(All SiDBs of the layout but without the negatively-charged SiDBs.)doc";

static const char *__doc_fiction_detail_quickexact_impl_charge_lyt = R"doc(Charge distribution surface.)doc";

static const char *__doc_fiction_detail_quickexact_impl_conduct_simulation =
R"doc(This function initializes the charge layout with necessary parameters,
and conducts the physical simulation based on whether a three-state
simulation is required.

Template parameter ``ChargeLyt``:
    Type of the charge distribution surface.

Parameter ``base_number``:
    `THREE` if a three-state simulation is required, `TWO` otherwise.)doc";

static const char *__doc_fiction_detail_quickexact_impl_generate_layout_without_negative_sidbs =
R"doc(This function is used to generate a layout without the SiDBs that are
pre-assigned to be negatively charged in a physically-valid layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_initialize_charge_layout =
R"doc(This function is responsible for preparing the charge layout and
relevant data structures for the simulation.

This function initializes the charge layout within the context of the
current simulation. It performs the following tasks:

- If the provided layout type `Lyt` supports a `foreach_sidb_defect`
method, it iterates through each defect in the layout. - If a defect
is found, it adds the SiDB defect to the potential landscape. - It
assigns the local external potential from the
`params.local_external_potential` configuration to the charge layout.
- It assigns the global external potential from
`params.global_potential` to the charge layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_layout = R"doc(Layout to simulate.)doc";

static const char *__doc_fiction_detail_quickexact_impl_number_of_sidbs = R"doc(Number of SiDBs of the input layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_params = R"doc(Parameters used for the simulation.)doc";

static const char *__doc_fiction_detail_quickexact_impl_preassigned_negative_sidb_indices =
R"doc(Indices of all SiDBs that are pre-assigned to be negatively charged in
a physically valid layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_preassigned_negative_sidbs =
R"doc(All SiDBs that are pre-assigned to be negatively charged in a
physically valid layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_quickexact_impl = R"doc()doc";

static const char *__doc_fiction_detail_quickexact_impl_required_simulation_base_number = R"doc(Base number required for the correct physical simulation.)doc";

static const char *__doc_fiction_detail_quickexact_impl_required_simulation_base_number_THREE =
R"doc(Three state simulation (i.e., negative, neutral, and positive) is
required.)doc";

static const char *__doc_fiction_detail_quickexact_impl_required_simulation_base_number_TWO = R"doc(Two state simulation (i.e., negative and neutral) is sufficient.)doc";

static const char *__doc_fiction_detail_quickexact_impl_result = R"doc(Simulation results.)doc";

static const char *__doc_fiction_detail_quickexact_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_quickexact_impl_three_state_simulation =
R"doc(This function conducts 3-state physical simulation (negative, neutral,
positive).

Template parameter ``ChargeLyt``:
    Type of the charge distribution surface.

Parameter ``charge_layout``:
    Initialized charge layout.)doc";

static const char *__doc_fiction_detail_quickexact_impl_two_state_simulation =
R"doc(This function conducts 2-state physical simulation (negative,
neutral).

Template parameter ``ChargeLyt``:
    Type of the charge distribution surface.

Parameter ``charge_layout``:
    Initialized charge layout.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage =
R"doc(@struct gate_storage

Represents a gate in a fcn layout, storing its unique ID, type, name,
location, and incoming connections.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_compare_by_id =
R"doc(Static member function to compare gate_storage objects by their IDs.

Parameter ``gate1``:
    First gate to be compared.

Parameter ``gate2``:
    Second gate to be compared.

Returns:
    True if gate1's ID is less than gate2's ID, false otherwise.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_id = R"doc(Unique identifier for the gate.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_incoming = R"doc(List of incoming connections to the gate.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_loc = R"doc(Location of the gate represented its x-, y- and z-coordinate.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_name = R"doc(Name of the gate (for inputs and outputs).)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_gate_storage_type =
R"doc(Type of the gate, can be an alias (AND, OR, PI, ..) or the implemented
function in a binary or hexadecimal form.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_is = R"doc(The input stream from which the gate-level layout is read.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_lyt = R"doc(The layout which will be altered based on the parsed information.)doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_read_fgl_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_read_fgl_layout_impl_2 = R"doc()doc";

static const char *__doc_fiction_detail_read_fgl_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_cell_label_map = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_current_cell_row = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_current_labeled_cell = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_fqca_section = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_fqca_section_CELL_DEFINITION = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_fqca_section_LAYOUT_DEFINITION = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_is = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_max_cell_pos = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_next_cell_layer = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_parse_cell = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_parsing_status = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_read_fqca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_read_fqca_layout_impl_to_clock_number = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl_defect_matrix = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl_max_cell_pos = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl_read_sidb_surface_defects_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_sidb_surface_defects_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_dimer_to_cell =
R"doc(Converts a dimer position to a cell position. Additionally updates the
maximum cell position parsed so far.

Parameter ``n``:
    The x-coordinate of the dimer.

Parameter ``m``:
    The y-coordinate of the dimer.

Parameter ``l``:
    0 for the upper dot, 1 for the lower dot.

Returns:
    The cell position converted from the dimer position.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_is = R"doc(The input stream from which the SQD file is read.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_lyt = R"doc(The layout to which the parsed cells are added.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_max_cell_pos = R"doc(The maximum position of a cell in the layout.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_db_dot =
R"doc(Parses a <dbdot> element from the SQD file and adds the respective dot
to the layout.

Parameter ``db_dot``:
    The <dbdot> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_defect =
R"doc(Parses a <defect> element from the SQD file and adds the respective
defect to the layout if it implements the has_assign_sidb_defect
function..

Parameter ``label``:
    The <defect> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_defect_label =
R"doc(Parses a <val> attribute of a <type_label> element of a <property_map>
element from the SQD file and converts it to the respective SiDB
defect type.

Parameter ``label``:
    The <type_label> element's <val> attribute.

Returns:
    The SiDB defect type corresponding to the given label.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_dot_type =
R"doc(Parses the <type> attribute of a <dbdot> element from the SQD file and
returns the corresponding cell type.

Parameter ``db_dot``:
    The <dbdot> element.

Returns:
    The cell type specified by the <dbdot> element. If non is
    specified, the cell type is assumed to be normal.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_lat_type =
R"doc(Parses a <latcoord> element from the SQD file and returns its
specified cell position.

Parameter ``latcoord``:
    The <latcoord> element.

Returns:
    The cell position specified by the <latcoord> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_latcoord =
R"doc(Parses a <latcoord> element from the SQD file and returns its
specified cell position.

Parameter ``latcoord``:
    The <latcoord> element.

Returns:
    The cell position specified by the <latcoord> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_read_sqd_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_read_sqd_layout_impl_2 = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_update_bounding_box =
R"doc(Updates the bounding box given by the maximum position of a cell in
the layout

Parameter ``cell``:
    The cell to challenge the stored maximum position of a cell in the
    layout against.)doc";

static const char *__doc_fiction_detail_recursively_paint_edges = R"doc()doc";

static const char *__doc_fiction_detail_sat_clocking_handler = R"doc()doc";

static const char *__doc_fiction_detail_sat_clocking_handler_assign_clock_numbers =
R"doc(Assigns clock numbers to the layout based on the provided model.

Parameter ``model``:
    The model to extract the clocking scheme from.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_at_least_one_clock_number_per_tile =
R"doc(Adds constraints to the solver that enforce the assignment of at least
one clock number per tile.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_at_most_one_clock_number_per_tile =
R"doc(Adds constraints to the solver that enforce the assignment of at most
one clock number per tile.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_determine_clocks =
R"doc(Determines clock numbers for the layout.

Constructs a SAT instance and passes it to a solver to find a valid
clocking scheme.

Returns:
    `true` iff a valid clocking scheme could be found.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_ensure_same_clock_number_on_crossing_tiles =
R"doc(Adds constraints to the solver that ensure the assignment of the same
clock number to crossing tiles.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_exclude_clock_assignments_that_violate_information_flow =
R"doc(Adds constraints to the solver that exclude the assignment of non-
adjacently clocked tiles.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_layout = R"doc(The layout to clock.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_number_of_clocks = R"doc(Number of clocks in layout's clocking scheme.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_sat_clocking_handler = R"doc(Default constructor.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_solver = R"doc(The solver used to find a solution to the clocking problem.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_symmetry_breaking =
R"doc(Adds constraints to the solver that help to speed up the solving
process by breaking symmetries in the solution space.)doc";

static const char *__doc_fiction_detail_sat_clocking_handler_variables = R"doc(Stores all variables.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_at_least_one_color_per_vertex = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_at_most_one_color_per_vertex = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_check_k_coloring = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_check_sat = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_color = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_color_frequency_equal_to_largest_clique_size = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_determine_min_coloring_with_binary_search = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_determine_min_coloring_with_linearly_ascending_search = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_determine_min_coloring_with_linearly_descending_search = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_exclude_identical_adjacent_colors = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_extract_vertex_coloring = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_get_model = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_graph = R"doc(A reference to the graph to be colored.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_largest_clique = R"doc(Iterator to the largest given clique.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_pre_assign_largest_clique =
R"doc(Reduce the search space by symmetry breaking. To this end, each vertex
in the provided clique gets a different color assigned from the
beginning.

Parameter ``instance``:
    Pointer to the solver instance.

Returns:
    A clique-first ordering of the vertices that assigns the clique
    vertices the lowest indices.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_pst = R"doc(Statistics.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_q = R"doc(Largest clique size.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_sat_coloring_handler = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_2 = R"doc()doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_k = R"doc(Number of colors.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_solver = R"doc(SAT solver.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_solver_instance =
R"doc(Standard constructor. Initialize one variable for each vertex-color
pair.

Parameter ``graph``:
    The graph to color.

Parameter ``num_colors``:
    Number of colors to attempt the coloring with.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_solver_instance_2 = R"doc(Default constructor is not available.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_solver_instance_variables = R"doc(Stores all variables.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_symmetry_breaking =
R"doc(Reduce the search space by symmetry breaking. Two tactics are
implemented:

- pre-assigning different colors to the largest given clique, and -
transforming the coloring solution to be lexicographically minimal

Parameter ``instance``:
    Pointer to the solver instance.)doc";

static const char *__doc_fiction_detail_sat_coloring_handler_transform_solution_to_lexicographical_minimum =
R"doc(Reduce the search space by symmetry breaking. To this end, the
solution is transformed to be lexicographical minimal, i.e., no vertex
is assigned color c unless color c - 1 has been assigned to any vertex
of lower index.

Parameter ``instance``:
    Pointer to the solver instance.)doc";

static const char *__doc_fiction_detail_search_direction =
R"doc(The two search directions: horizontal (from left to right) and
vertical (from top to bottom).)doc";

static const char *__doc_fiction_detail_search_direction_HORIZONTAL = R"doc(Search from left to right.)doc";

static const char *__doc_fiction_detail_search_direction_VERTICAL = R"doc(Search from top to bottom.)doc";

static const char *__doc_fiction_detail_search_space_graph =
R"doc(A structure representing a search space graph.

This struct encapsulates all the necessary data for managing a search
space graph during the graph-oriented layout design process. It holds
the current vertex, network, nodes to be placed, and other relevant
information.

Template parameter ``Lyt``:
    The layout type.

Template parameter ``Ntk``:
    The network type.)doc";

static const char *__doc_fiction_detail_search_space_graph_cost = R"doc(The cost objective used to expand a vertex in the search space graph.)doc";

static const char *__doc_fiction_detail_search_space_graph_current_vertex = R"doc(The current vertex in the search space graph.)doc";

static const char *__doc_fiction_detail_search_space_graph_frontier = R"doc(Priority queue containing vertices of the search space graph.)doc";

static const char *__doc_fiction_detail_search_space_graph_frontier_flag = R"doc(Flag indicating if this graph's frontier is active.)doc";

static const char *__doc_fiction_detail_search_space_graph_network = R"doc(The network associated with this search space graph.)doc";

static const char *__doc_fiction_detail_search_space_graph_num_expansions = R"doc(The maximum number of positions to be considered for expansions.)doc";

static const char *__doc_fiction_detail_search_space_graph_pi_locs =
R"doc(Enum indicating if primary inputs (PIs) can be placed at the top or
left.)doc";

static const char *__doc_fiction_detail_search_space_graph_planar = R"doc(Create planar layouts.)doc";

static const char *__doc_fiction_detail_sweep_parameter_to_string =
R"doc(Converts a sweep parameter to a string representation. This is used to
write the parameter name to the CSV file.

Parameter ``param``:
    The sweep parameter to be converted.

Returns:
    The string representation of the sweep parameter.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl = R"doc()doc";

static const char *__doc_fiction_detail_technology_mapping_impl_ntk = R"doc(Input network to be mapped.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl_params = R"doc(Technology mapping parameters.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl_perform_mapping =
R"doc(Perform technology mapping with the given number of inputs.

Template parameter ``NumInp``:
    Maximum input number of the gates in the technology library.

Parameter ``gates``:
    Technology library.

Returns:
    Mapped network.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_technology_mapping_impl_set_up_gates =
R"doc(Create a mockturtle gate library from the given parameters.

Returns:
    A vector of mockturtle gates.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl_stats = R"doc(Technology mapping statistics.)doc";

static const char *__doc_fiction_detail_technology_mapping_impl_technology_mapping_impl = R"doc()doc";

static const char *__doc_fiction_detail_to_hex =
R"doc(Utility function to transform a Cartesian tile into a hexagonal one.

Parameter ``cartesian_tile``:
    Tile on the Cartesian grid.

Parameter ``cartesian_layout_height``:
    Height of the Cartesian layout.

Returns:
    corresponding tile on the hexagonal grid.)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co =
R"doc(Custom view class derived from mockturtle::topo_view.

This class inherits from mockturtle::topo_view and overrides certain
functions to provide custom behavior. The topological order is
generated from CIs to COs.)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_create_topo_rec = R"doc()doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_foreach_gate = R"doc(! Reimplementation of `foreach_gate`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_foreach_gate_reverse = R"doc(! Implementation of `foreach_gate` in reverse topological order. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_foreach_node = R"doc(! Reimplementation of `foreach_node`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_index_to_node = R"doc(! Reimplementation of `index_to_node`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_node_to_index = R"doc(! Reimplementation of `node_to_index`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_num_gates = R"doc(! Reimplementation of `num_gates`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_size = R"doc(! Reimplementation of `size`. */)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_topo_order = R"doc()doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_topo_view_ci_to_co =
R"doc(! Default constructor.

Constructs topological view on another network.)doc";

static const char *__doc_fiction_detail_topo_view_ci_to_co_update_topo = R"doc()doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci =
R"doc(Custom view class derived from mockturtle::topo_view.

This class inherits from mockturtle::topo_view and overrides certain
functions to provide custom behavior. The topological order is
generated from COs to CIs.)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_create_topo_rec = R"doc()doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_foreach_gate = R"doc(! Reimplementation of `foreach_gate`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_foreach_gate_reverse = R"doc(! Implementation of `foreach_gate` in reverse topological order. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_foreach_node = R"doc(! Reimplementation of `foreach_node`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_index_to_node = R"doc(! Reimplementation of `index_to_node`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_node_to_index = R"doc(! Reimplementation of `node_to_index`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_num_gates = R"doc(! Reimplementation of `num_gates`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_size = R"doc(! Reimplementation of `size`. */)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_topo_order = R"doc()doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_topo_view_co_to_ci =
R"doc(! Default constructor.

Constructs topological view on another network.)doc";

static const char *__doc_fiction_detail_topo_view_co_to_ci_update_topo = R"doc()doc";

static const char *__doc_fiction_detail_update_to_delete_list =
R"doc(Update the to-delete list based on a possible path in a
wiring_reduction_layout.

This function updates the to-delete list by appending coordinates from
the given possible path in a wiring_reduction_layout. It considers
coordinates that are not at the leftmost (`x == 0`) or rightmost (`x
== lyt.x()`) positions for left to right, or at the top (`y == 0`) or
bottom (`y == lyt.y()`) positions for top to bottom and shifts them to
get the corresponding coordinates on the original layout. The
coordinates are then obstructed in both layers (0 and 1).

Template parameter ``WiringReductionLyt``:
    Type of the `wiring_reduction_layout`.

Parameter ``lyt``:
    The `wiring_reduction_layout` to be updated.

Parameter ``possible_path``:
    The path of coordinates to be considered for updating the to-
    delete list.

Parameter ``to_delete``:
    Reference to the to-delete list to be updated with new
    coordinates.)doc";

static const char *__doc_fiction_detail_validate_sweep_parameters =
R"doc(This function validates the given sweep parameters for the operational
domain computation. It checks if the minimum value of any sweep
dimension is larger than the corresponding maximum value.
Additionally, it checks if the step size of any sweep dimension is
negative or zero.

Parameter ``params``:
    The operational domain parameters to validate.

Throws:
    std::invalid_argument if the sweep parameters are invalid.)doc";

static const char *__doc_fiction_detail_wire_east = R"doc()doc";

static const char *__doc_fiction_detail_wire_south = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_impl = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_plyt = R"doc(The 2DDWave-clocked layout whose wiring is to be reduced.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_ps = R"doc(Wiring reduction parameters.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_pst = R"doc(Statistics about the wiring_reduction process.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_start = R"doc(Start time.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_timeout_limit_reached = R"doc(Timeout limit reached.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_wiring_reduction_impl = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_layout =
R"doc(Represents a layout used for wiring reduction derived from the
`cartesian_layout` class.

This class provides functionality for a wiring reduction layout based
on a Cartesian coordinate system. It inherits from the
`cartesian_layout` class and extends it with specific behavior for
finding excess wiring.

Template parameter ``OffsetCoordinateType``:
    The type of coordinates used in the layout. Defaults to
    `offset::ucoord_t` if not explicitly provided.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate =
R"doc(Iterates over adjacent coordinates of a given coordinate and applies a
given functor.

This function iterates over adjacent coordinates of the given
coordinate `c` and applies the provided functor `fn` to each valid
adjacent coordinate. The behavior depends on the position of `c`
within the layout.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_first_column =
R"doc(Iterates over adjacent coordinates of a given coordinate in the first
column.

This function iterates over adjacent coordinates of the given
coordinate `c` in the first column and applies the provided functor
`fn` to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_first_row =
R"doc(Iterates over adjacent coordinates of a given coordinate in the first
row.

This function iterates over adjacent coordinates of the given
coordinate `c` in the first row and applies the provided functor `fn`
to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_last_column =
R"doc(Iterates over adjacent coordinates of a given coordinate in the last
column.

This function iterates over adjacent coordinates of the given
coordinate `c` in the last column and applies the provided functor
`fn` to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_last_row =
R"doc(Iterates over adjacent coordinates of a given coordinate in the last
row.

This function iterates over adjacent coordinates of the given
coordinate `c` in the last row and applies the provided functor `fn`
to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_middle_columns =
R"doc(Iterates over adjacent coordinates of a given coordinate in the middle
columns.

This function iterates over adjacent coordinates of the given
coordinate `c` in the middle columns and applies the provided functor
`fn` to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_foreach_adjacent_coordinate_middle_rows =
R"doc(Iterates over adjacent coordinates of a given coordinate in the middle
rows.

This function iterates over adjacent coordinates of the given
coordinate `c` in the middle rows and applies the provided functor
`fn` to each valid adjacent coordinate.

Template parameter ``Fn``:
    Type of the functor to apply to each adjacent coordinate.

Parameter ``c``:
    The reference coordinate for which adjacent coordinates are
    determined.

Parameter ``fn``:
    The functor to apply to each adjacent coordinate.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_get_search_direction =
R"doc(Getter for the search direction.

Returns:
    The current search direction.)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_search_dir =
R"doc(The current search direction: horizontal (from left to right) and
vertical (from top to bottom).)doc";

static const char *__doc_fiction_detail_wiring_reduction_layout_wiring_reduction_layout =
R"doc(This constructor initializes the `wiring_reduction_layout` with an
optional aspect ratio.

Parameter ``ar``:
    The aspect ratio for the layout. Defaults to an empty aspect ratio
    if not provided.

Parameter ``direction``:
    The search direction to be used. Defaults to HORIZONTAL if not
    provided.)doc";

static const char *__doc_fiction_detail_write_fgl_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_fgl_layout_impl_lyt = R"doc(The layout to be written.)doc";

static const char *__doc_fiction_detail_write_fgl_layout_impl_os = R"doc(The output stream to which the gate-level layout is written.)doc";

static const char *__doc_fiction_detail_write_fgl_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_fgl_layout_impl_write_fgl_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_alphabet_iterator = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_labeled_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_labeled_cell_c = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_labeled_cell_designator = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_labeled_cell_labeled_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_labeled_cells = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_next_cell_designator =
R"doc(Might throw an 'out_of_cell_names_exception'.

Returns:
    The next cell designator in the alphabet.)doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_cell_definition = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_fqca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_header = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_labeled_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_layer_separator = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_layout_definition = R"doc()doc";

static const char *__doc_fiction_detail_write_fqca_layout_impl_write_via_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_location_and_ground_state_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_location_and_ground_state_impl_os = R"doc(Output stream used for writing the simulation result.)doc";

static const char *__doc_fiction_detail_write_location_and_ground_state_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_location_and_ground_state_impl_sim_result = R"doc(Simulation results.)doc";

static const char *__doc_fiction_detail_write_location_and_ground_state_impl_write_location_and_ground_state_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_via_counter = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_via_layer_cells = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell_colors = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell_function = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell_layers = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell_mode = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_cell_name = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_header = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_qca_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_quantum_dots = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_impl_write_via_cells = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_generate_cell_based_svg =
R"doc(Generates an SVG string representing the cell-based clocked cell
layout and appends it to the output stream.)doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_generate_description_color =
R"doc(Generates and returns a pair of strings representing the description
and color of the given cell.

Parameter ``c``:
    The cell for which to generate the description and color.

Returns:
    A pair of strings representing the description and color of the
    given cell `c`.)doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_generate_tile_based_svg =
R"doc(Generates an SVG string representing the tile-based clocked cell
layout and appends it to the output stream.)doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_write_qca_layout_svg_impl = R"doc(Default constructor.)doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_bb = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_bb_x = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_bb_y = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_generate_layout_id_hash = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_get_pin_data = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_has_border_io_pins = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_num_magnets = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_sorted_pi_list = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_sorted_pis = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_sorted_po_list = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_sorted_pos = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_write_components = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_write_entity = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_write_header = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_write_layout = R"doc()doc";

static const char *__doc_fiction_detail_write_qcc_layout_impl_write_qcc_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_bb = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_bb_x = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_bb_y = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_cell_id = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_has_border_io_pins = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_sorted_pi_list = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_sorted_pis = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_sorted_po_list = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_sorted_pos = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_tech_name = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_components = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_header = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_layout = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_pins = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_qll_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_qll_layout_impl_write_technology_settings = R"doc()doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_background_color = R"doc(The color mode for the SVG output.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_generate_lattice_point =
R"doc(Generates an SVG string representing an H-Si lattice point.

Parameter ``x``:
    The x-coordinate of the lattice point.

Parameter ``y``:
    The y-coordinate of the lattice point.

Parameter ``fill_color``:
    The fill color of the lattice point.

Returns:
    The SVG string representing the lattice point.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_generate_sidb =
R"doc(Generates an SVG string representing an SiDB.

Parameter ``x``:
    The x-coordinate of the SiDB.

Parameter ``y``:
    The y-coordinate of the SiDB.

Parameter ``charge_state``:
    The charge state of the SiDB.

Returns:
    The SVG string representing the SiDB.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_generate_svg = R"doc(Generates the SVG layout with both H-Si lattice points and SiDBs.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_lyt = R"doc(The SiDB layout to be written.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_os = R"doc(The output stream to write into.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_ps = R"doc(Parameters for the SVG generation.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_run = R"doc(Runs the SVG generation process.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_set_colors = R"doc(Sets the colors based on the color mode.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_sidb_color = R"doc(The color of the SiDB without charge information.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_sidb_edge_color = R"doc(The edge color of the SiDB without charge information.)doc";

static const char *__doc_fiction_detail_write_sidb_layout_svg_impl_write_sidb_layout_svg_impl = R"doc(Constructor.)doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_generate_db_blocks = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_generate_defect_blocks = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_get_defect_type_name = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_layout_impl_write_sqd_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_obtain_ordered_cells =
R"doc(Obtains all cells in the surfaces and orders them by their position to
achieve a reproducible output.

Returns:
    An ordering of cells.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_ordered_cells = R"doc(A reproducible ordering of cells in the surfaces.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_os = R"doc(The output stream to write to.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_sim_result = R"doc(The simulation result to write.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_write_electron_distributions = R"doc(Writes all charge distributions to the output stream in XML format.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_write_engine_info = R"doc(Writes the engine information to the output stream in XML format.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_write_physical_locations =
R"doc(Writes the physical locations of all SiDBs to the output stream in XML
format.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_write_simulation_parameters =
R"doc(Writes both the `simulation_parameters` as well as the
`additional_simulation_parameters` to the output stream in XML format.)doc";

static const char *__doc_fiction_detail_write_sqd_sim_result_impl_write_sqd_sim_result_impl = R"doc()doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_k_shortest_paths = R"doc(The list of k shortest paths that is created during the algorithm.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_layout =
R"doc(The layout in which k shortest paths are to be found extended by an
obstruction functionality layer.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_num_shortest_paths = R"doc(The number of paths to determine, i.e., k.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_objective = R"doc(Source and target coordinates.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_params = R"doc(Routing parameters.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_path_cost =
R"doc(Computes the cost of a path. This function can be adjusted to fetch
paths of differing costs.

Currently, the cost is equal to its length.

Parameter ``p``:
    Path whose costs are to be calculated.

Returns:
    Costs of path p.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_reset_temporary_obstructions = R"doc(Resets all temporary obstructions.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_run =
R"doc(Enumerate up to k shortest paths in a layout that start at
`objective.source` and lead to `objective.target`.

Returns:
    A collection of up to k shortest paths in `layout` from
    `objective.source` to `objective.target`.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_shortest_path_candidates = R"doc(A set of potential shortest paths.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_temporarily_obstructed_coordinates =
R"doc(A temporary storage for coordinates that are obstructed during the
algorithm.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_yen_k_shortest_paths_impl = R"doc()doc";

static const char *__doc_fiction_detect_bdl_pairs =
R"doc(This algorithm detects BDL pairs in an SiDB layout. It does so by
first collecting all dots of the given type and then uniquely pairing
them up based on their distance. Lower and upper distance thresholds
can be defined (defaults = 0.75 nm and 1.5 nm, respectively) to narrow
down the range in which SiDBs could be considered a BDL pair. The
distance between two dots is computed using the `sidb_nm_distance`
function. The algorithm returns a vector of BDL pairs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to detect BDL pairs in.

Parameter ``type``:
    Optional parameter to specify the SiDB type for which BDL pairs
    should be detected. If omitted, the function will detect BDL pairs
    for all types. Valid types include `INPUT`, `OUTPUT`, `NORMAL`,
    among others.

Parameter ``params``:
    Parameters for the BDL pair detection algorithm.

Returns:
    A vector of BDL pairs.)doc";

static const char *__doc_fiction_detect_bdl_pairs_params = R"doc(Parameters for the BDL pair detection algorithms.)doc";

static const char *__doc_fiction_detect_bdl_pairs_params_maximum_distance =
R"doc(The maximum distance between two dots to be considered a BDL pair.
This is useful to prevent unlikely pairings of SiDBs that are far
apart and to improve performance of the matching algorithm. (unit:
nm).)doc";

static const char *__doc_fiction_detect_bdl_pairs_params_minimum_distance =
R"doc(The minimum distance between two SiDBs to be considered a BDL pair.
This is useful to prevent, e.g., SiDBs of atomic wires to be
considered BDL pairs. (unit: nm).)doc";

static const char *__doc_fiction_detect_bdl_wires =
R"doc(This function identifies BDL wires in a given SiDB cell-level layout
by detecting BDL pairs and linking them based on their spatial
relationships. The function supports selection of different types of
wires, such as input wires, output wires, or all wires.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The SiDB layout to detect BDL wires in.

Parameter ``params``:
    Parameters used for detecting BDL wires.

Parameter ``wire_selection``:
    The type of wires to detect, specified by the `bdl_wire_selection`
    enum. Default is `bdl_wire_selection::ALL`.

Returns:
    A vector of BDL wires, where each wire is represented as a vector
    of BDL pairs.)doc";

static const char *__doc_fiction_detect_bdl_wires_params = R"doc(This struct encapsulates parameters used for detecting BDL wires.)doc";

static const char *__doc_fiction_detect_bdl_wires_params_bdl_pairs_params = R"doc(Parameters for the `detect_bdl_pairs` algorithm.)doc";

static const char *__doc_fiction_detect_bdl_wires_params_threshold_bdl_interdistance =
R"doc(A distance threshold, which is used to determine if two pairs of BDLs
are part of the same wire. (unit: nm).)doc";

static const char *__doc_fiction_determine_all_combinations_of_distributing_k_entities_on_n_positions =
R"doc(This function generates all possible combinations of distributing k
entities onto n positions. Each combination is represented as a vector
of indices, where each index indicates the position of an entity.

Parameter ``k``:
    The number of entities to distribute.

Parameter ``n``:
    The number of positions available for distribution.

Returns:
    A vector of vectors representing all possible combinations of
    distributing k entities on n positions.)doc";

static const char *__doc_fiction_determine_clocking =
R"doc(Determines clock numbers for the given (unclocked) gate-level layout.
This algorithm parses the layout's gate and wire connections,
disregarding any existing clocking information, and constructs a SAT
instance to find a valid clock number assignment under which the
information flow is respected. It then assigns these clock numbers as
an irregular clock map to the given layout via the
`assign_clock_number` function, overriding any existing clocking
scheme.

If no valid clock number assignment exists for `lyt`, this function
returns `false` and does not modify `lyt`.

This algorithm was proposed in \"Ending the Tyranny of the Clock: SAT-
based Clock Number Assignment for Field-coupled Nanotechnologies\" by
M. Walter, J. Drewniok, and R. Wille in IEEE NANO 2024
(https://ieeexplore.ieee.org/abstract/document/10628908).

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The gate-level layout to assign clock numbers to.

Parameter ``params``:
    Parameters.

Parameter ``stats``:
    Statistics.

Returns:
    `true` iff `lyt` could be successfully clocked via a valid clock
    number assignment.)doc";

static const char *__doc_fiction_determine_clocking_params = R"doc(Parameters for the `determine_clocking` algorithm.)doc";

static const char *__doc_fiction_determine_clocking_params_sat_engine = R"doc(The SAT solver to use.)doc";

static const char *__doc_fiction_determine_clocking_stats = R"doc(Statistics for the `determine_clocking` algorithm.)doc";

static const char *__doc_fiction_determine_clocking_stats_duration = R"doc(Total runtime.)doc";

static const char *__doc_fiction_determine_clocking_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    The output stream to report to.)doc";

static const char *__doc_fiction_determine_displacement_robustness_domain =
R"doc(During fabrication, SiDBs may not align precisely with their intended
atomic positions, resulting in displacement. This means that an SiDB
is fabricated close to the desired one, typically one or a few H-Si
positions away. Consequently, depending on the fabrication speed, a
certain number of SiDBs may experience displacement.

This function determines the operational status of all possible
displacements of the SiDBs of the given SiDB layout, based on the
provided truth table specification and displacement robustness
computation parameters. The number of displacements grows
exponentially with the number of SiDBs. For small layouts, all
displacements can be analyzed. For larger layouts, random sampling can
be applied, controllable by the `analysis_mode` and
`percentage_of_analyzed_displaced_layouts` in `params.

Template parameter ``Lyt``:
    The SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters for the displacement robustness computation.

Parameter ``stats``:
    Statistics related to the displacement robustness computation.

Returns:
    The displacement robustness domain of the SiDB layout.)doc";

static const char *__doc_fiction_determine_probability_of_fabricating_operational_gate =
R"doc(During fabrication, SiDBs may not align precisely with their intended
atomic positions, resulting in displacement. This means that an SiDB
is fabricated close to the desired one, typically one or a few H-Si
positions away. The percentage of displaced SiDBs depends on the
fabrication speed. Therefore, SiDB layouts with high displacement
tolerance are preferred to speed up the fabrication process.

This function calculates the probability of fabricating an operational
SiDB layout for an originally given SiDB layout and a given
fabrication error rate. A fabrication error rate of 0.0 or negative
indicates that the SiDB layout is designed without displacement.

Template parameter ``Lyt``:
    The SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table.

Parameter ``layout``:
    The SiDB cell-level layout which is analyzed.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters for the displacement robustness computation.

Parameter ``fabrication_error_rate``:
    The fabrication error rate. For example, 0.1 describes that 10% of
    all manufactured SiDBs have a slight displacement.

Returns:
    The probability of fabricating an operational SiDB layout.)doc";

static const char *__doc_fiction_determine_vertex_coloring =
R"doc(This function provides an interface to call various vertex coloring
algorithms on the given graph. A vertex coloring is the assignment of
colors to graph vertices such that no two vertices that share an edge
receive the same color. If a graph is colorable with :math:`k` colors,
the graph is said to be :math:`k`-colorable. The minimum value of
:math:`k` for a graph is called its chromatic number. To determine the
chromatic number of a graph is :math:`NP`-complete in general. The
provided algorithms attempt to get as close to the optimum coloring as
possible. However, no heuristic can give an optimality guarantee. If
the exact chromatic number is required, the SAT-based engine must be
used. This may require exponential runtime in the worst case but is,
on average, a lot faster due to the smart traversal of search spaces
that SAT solvers provide.

See graph_coloring_engine for a list of all supported engines.

@note If the `clique_size_color_frequency` parameter is set together
with the SAT engine, there is no guarantee that the SAT solver is able
to find a valid coloring. In that case, this algorithm falls back to
MCS.

Template parameter ``Graph``:
    Graph type to color.

Template parameter ``Color``:
    Color type to use.

Parameter ``graph``:
    The graph whose vertices are to be colored.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    An assignment of graph vertices to colors such that no two
    adjacent vertices share the same color.)doc";

static const char *__doc_fiction_determine_vertex_coloring_heuristic_params = R"doc(Parameters for heuristic graph coloring.)doc";

static const char *__doc_fiction_determine_vertex_coloring_heuristic_params_k_color_value =
R"doc(:math:`k`-color value for :math:`k`-coloring algorithms, e.g.,
TABUCOL.)doc";

static const char *__doc_fiction_determine_vertex_coloring_params =
R"doc(Common parameters for the graph coloring algorithm.

Template parameter ``Graph``:
    Type of the graph to color.)doc";

static const char *__doc_fiction_determine_vertex_coloring_params_engine = R"doc(The engine to use.)doc";

static const char *__doc_fiction_determine_vertex_coloring_params_heuristic_params = R"doc(Parameters for `engine != SAT`.)doc";

static const char *__doc_fiction_determine_vertex_coloring_params_sat_params = R"doc(Parameters for `engine == SAT`.)doc";

static const char *__doc_fiction_determine_vertex_coloring_params_verify_coloring_after_computation = R"doc(Verify that the found coloring is valid.)doc";

static const char *__doc_fiction_determine_vertex_coloring_sat_params =
R"doc(Parameters for SAT-based graph coloring.

Template parameter ``Graph``:
    Type of the graph to color.)doc";

static const char *__doc_fiction_determine_vertex_coloring_sat_params_clique_size_color_frequency =
R"doc(Tries to establish the color frequency of color 0 such that it equals
the largest clique size.)doc";

static const char *__doc_fiction_determine_vertex_coloring_sat_params_cliques =
R"doc(If cliques in the passed graph are known, they can be used for
symmetry breaking in the SAT engine which significantly speeds up
runtime. The bigger the cliques, the better.)doc";

static const char *__doc_fiction_determine_vertex_coloring_sat_params_sat_engine = R"doc(The SAT solver to use.)doc";

static const char *__doc_fiction_determine_vertex_coloring_sat_params_sat_search_tactic = R"doc(The search tactic to apply.)doc";

static const char *__doc_fiction_determine_vertex_coloring_stats = R"doc()doc";

static const char *__doc_fiction_determine_vertex_coloring_stats_chromatic_number =
R"doc(The determined chromatic number (could be non-optimal depending on the
applied engine).)doc";

static const char *__doc_fiction_determine_vertex_coloring_stats_color_frequency = R"doc(The frequency of the most used color.)doc";

static const char *__doc_fiction_determine_vertex_coloring_stats_coloring_verified =
R"doc(Validation result of the coloring (std::nullopt = none attempted, true
= valid, false = invalid).)doc";

static const char *__doc_fiction_determine_vertex_coloring_stats_duration = R"doc(Runtime measurement.)doc";

static const char *__doc_fiction_determine_vertex_coloring_stats_most_frequent_color = R"doc(The color that appeared the most.)doc";

static const char *__doc_fiction_displacement_robustness_domain =
R"doc(During fabrication, SiDBs may not align precisely with their intended
atomic positions, resulting in displacement. This means that an SiDB
is fabricated close to the desired one, typically one or a few H-Si
positions away. Consequently, depending on the fabrication speed, a
certain number of SiDBs may experience displacement. To address and
analyze this occurrence, we introduce the *Displacement Robustness
Domain*. This domain consists of SiDB layouts derived from an original
layout, each showing displaced SiDBs, together with the `operational`
or `non-operational` status, based on the specified logic.)doc";

static const char *__doc_fiction_displacement_robustness_domain_operational_values =
R"doc(Represents a domain of displacement robustness for layouts resulting
from applying a displacement to a given SiDB layout.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params =
R"doc(Parameters for the `determine_displacement_robustness_domain` and
`determine_probability_of_fabricating_operational_gate` algorithms.

Parameter ``CellType``:
    SiDB layout cell type.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_analysis_mode =
R"doc(This parameter defines the mode of the displacement. If `EXHAUSTIVE`,
all possible displacements are analyzed. Otherwise, a certain amount
of all possible displacements is analyzed randomly.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_dimer_displacement_policy =
R"doc(Specifies the allowed displacement range options for SiDB fabrication
simulation.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_dimer_displacement_policy_ALLOW_OTHER_DIMER =
R"doc(In this mode, SiDBs are allowed to be displaced from the original
dimer to any other dimer within the layout.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_dimer_displacement_policy_STAY_ON_ORIGINAL_DIMER =
R"doc(In this mode, any displacement of SiDBs must remain within the
boundaries of the initial dimer they are placed on.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_dimer_policy =
R"doc(This flag controls whether the displacement in the y-direction can
lead to changes in the Si dimer.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_displacement_analysis_mode = R"doc(Possible modes to determine the displacement robustness domain.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_displacement_analysis_mode_EXHAUSTIVE = R"doc(All possible displacements are analyzed.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_displacement_analysis_mode_RANDOM =
R"doc(A certain amount of all possible displacements is analyzed randomly.
Defined by `percentage_of_analyzed_displaced_layouts`.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_displacement_variations =
R"doc(Possible displacement range of H-Si positions in the x- and
y-directions. The default value is (1, 0), which means that
displacements of ±1 position in the x-direction are analyzed, with no
displacement in the y-direction.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_fixed_sidbs = R"doc(SiDBs in the given layout which shall not be affected by variations.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_operational_params = R"doc(Parameters to check the operational status of the SiDB layout.)doc";

static const char *__doc_fiction_displacement_robustness_domain_params_percentage_of_analyzed_displaced_layouts =
R"doc(This parameter defines the percentage of all possible displaced SiDB
layouts that are analyzed. The default value is 1.0 (100 %), which
means that all possible displacements are covered.)doc";

static const char *__doc_fiction_displacement_robustness_domain_stats = R"doc(Statistics for the displacement robustness domain computation.)doc";

static const char *__doc_fiction_displacement_robustness_domain_stats_duration =
R"doc(Total runtime in seconds to determine the robustness of the passed
SiDB layout.)doc";

static const char *__doc_fiction_displacement_robustness_domain_stats_num_non_operational_sidb_displacements =
R"doc(The number of non-operational SiDB layouts resulting from the given
layout by displacements.)doc";

static const char *__doc_fiction_displacement_robustness_domain_stats_num_operational_sidb_displacements =
R"doc(The number of operational SiDB layouts resulting from the given layout
by displacements.)doc";

static const char *__doc_fiction_distance_functor =
R"doc(A functor that computes distances between coordinates and can be
passed as an object to, e.g., path finding algorithms with a
standardized signature. This class is intended to be instantiated with
concrete distance functions passed to the constructor.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance value type.)doc";

static const char *__doc_fiction_distance_functor_distance_function = R"doc(Distance function.)doc";

static const char *__doc_fiction_distance_functor_distance_functor =
R"doc(Standard constructor that instantiates the distance function.

Parameter ``dist_fn``:
    A function that maps from layout coordinates to a distance value.)doc";

static const char *__doc_fiction_distance_functor_operator_call =
R"doc(Operator to call the distance function.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Distance between source and target.)doc";

static const char *__doc_fiction_distance_map_functor =
R"doc(A distance functor that uses a fully precomputed `distance_map` to
determine distances between coordinates. It can be used as a drop-in
replacement for any other distance functor in path-finding algorithms.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.)doc";

static const char *__doc_fiction_distance_map_functor_coordinate_index =
R"doc(This function calculates the coordinate index of a given coordinate in
a given layout.

Parameter ``lyt``:
    Layout.

Parameter ``c``:
    Coordinate.

Returns:
    Coordinate index.)doc";

static const char *__doc_fiction_distance_map_functor_dist_map = R"doc(Distance map.)doc";

static const char *__doc_fiction_distance_map_functor_distance_map_functor =
R"doc(Construct the distance functor from a `distance_map`.

Parameter ``dm``:
    Distance map.)doc";

static const char *__doc_fiction_distance_map_functor_operator_call =
R"doc(Override the call operator to query the distance map instead of the
distance function.

@note This function will throw an exception if the queried distance is
not stored in the distance map.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Distance between source and target according to the stored
    distance map.)doc";

static const char *__doc_fiction_edge_color_view_drawer =
R"doc(A DOT drawer for networks with colored edges. Node colors represent
their painted color instead of their gate type.

Template parameter ``Ntk``:
    Logic network type.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_edge_color_view_drawer_last_accessed = R"doc()doc";

static const char *__doc_fiction_edge_color_view_drawer_node_fillcolor =
R"doc(Override function to store the previously accessed node such that
edges can be colored when signal_style is called without specific
information on the target node. This is a little bit hacky and depends
on the way mockturtle's dot drawer works.)doc";

static const char *__doc_fiction_edge_color_view_drawer_signal_style = R"doc()doc";

static const char *__doc_fiction_energy_calculation =
R"doc(An enumeration of modes for calculation of the electrostatic potential
energy of a given charge distribution.)doc";

static const char *__doc_fiction_energy_calculation_KEEP_OLD_ENERGY_VALUE =
R"doc(The electrostatic potential energy of a given charge distribution is
not updated after it is changed.)doc";

static const char *__doc_fiction_energy_calculation_UPDATE_ENERGY =
R"doc(The electrostatic potential energy of a given charge distribution is
updated after it is changed.)doc";

static const char *__doc_fiction_energy_dissipation_stats = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_fast = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_report = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_slow =
R"doc(Energy dissipation information in meV for slow (25 GHz) and fast (100
GHz) clocking.)doc";

static const char *__doc_fiction_energy_dissipation_stats_unknown = R"doc()doc";

static const char *__doc_fiction_energy_distribution =
R"doc(This class is used to store the energy distribution of an SiDB layout.
The energy distribution is a map that contains the electrostatic
potential as a key and its degeneracy as a value. To be more precise,
if two different charge distributions occur with the same energy, the
degeneracy value of the energy state is 2.)doc";

static const char *__doc_fiction_energy_distribution_add_state =
R"doc(Adds a state to the energy distribution.

Parameter ``energy``:
    The energy of the state to be added.

Parameter ``degeneracy``:
    The degeneracy of the state to be added.)doc";

static const char *__doc_fiction_energy_distribution_degeneracy_of_given_energy =
R"doc(Returns the degeneracy value (number of states) with the given energy
value.

Parameter ``energy``:
    The energy value for which the excited state number is to be
    determined.

Returns:
    The excited state number of the given energy value. If the energy
    value is not found, `std::nullopt` is returned instead.)doc";

static const char *__doc_fiction_energy_distribution_distribution =
R"doc(The energy distribution map. The key is the energy value and the value
is the degeneracy of the energy value.)doc";

static const char *__doc_fiction_energy_distribution_empty =
R"doc(Checks if the energy distribution is empty.

Returns:
    `true` if the energy distribution is empty, `false` otherwise.)doc";

static const char *__doc_fiction_energy_distribution_energy_distribution = R"doc(Default constructor.)doc";

static const char *__doc_fiction_energy_distribution_for_each =
R"doc(Applies a function to all states in the energy distribution.

Template parameter ``Fn``:
    Functor type.

Parameter ``fn``:
    Functor to apply to each key-value pair.)doc";

static const char *__doc_fiction_energy_distribution_get_nth_state =
R"doc(Returns the nth state (energy + degeneracy) in the energy
distribution.

Parameter ``state_index``:
    The index of the state to be retrieved.

Returns:
    The energy and degeneracy of the state at the specified index. If
    the index is out of range, `std::nullopt` is returned instead.)doc";

static const char *__doc_fiction_energy_distribution_max_energy =
R"doc(Returns the maximum energy value in the energy distribution.

Returns:
    The maximum energy value in the energy distribution.)doc";

static const char *__doc_fiction_energy_distribution_min_energy =
R"doc(Returns the minimum energy value in the energy distribution.

Returns:
    The minimum energy value in the energy distribution.)doc";

static const char *__doc_fiction_energy_distribution_size =
R"doc(Returns the number of states in the energy distribution.

Returns:
    The number of states in the energy distribution.)doc";

static const char *__doc_fiction_energy_state =
R"doc(This struct stores the energy state of an SiDB layout. The energy
state consists of the electrostatic potential energy and the
degeneracy of the state.)doc";

static const char *__doc_fiction_energy_state_degeneracy = R"doc(The degeneracy of the state.)doc";

static const char *__doc_fiction_energy_state_electrostatic_potential_energy = R"doc(The electrostatic potential energy of the charge distribution (eV).)doc";

static const char *__doc_fiction_energy_state_energy_state =
R"doc(Default constructor.

Parameter ``electrostatic_potential_energy``:
    The electrostatic potential energy of the charge distribution
    (eV).

Parameter ``degeneracy``:
    The degeneracy of the state.)doc";

static const char *__doc_fiction_enumerate_all_paths =
R"doc(Enumerates all possible paths in a layout that start at a given source
coordinate and lead to given target coordinate. This function
automatically detects whether the given layout implements a clocking
interface (see `clocked_layout`) and respects the underlying
information flow imposed by `layout`'s clocking scheme. This algorithm
does neither generate duplicate nor looping paths, even in a cyclic
clocking scheme. That is, along each path, each coordinate can occur
at maximum once.

If the given layout implements the obstruction interface (see
`obstruction_layout`), paths will not be routed via obstructed
coordinates or connections.

If the given layout is a gate-level layout and implements the
obstruction interface (see `obstruction_layout`), paths may contain
wire crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

In certain cases it might be desirable to enumerate regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:

```
{.cpp}
using clk_lyt = clocked_layout<cartesian_layout<>>;
using path = layout_coordinate_path<cartesian_layout<>>;
clk_lyt layout = ...;
auto all_paths = enumerate_all_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target});
```

Template parameter ``Path``:
    Type of the returned individual paths.

Template parameter ``Lyt``:
    Type of the layout to perform path finding on.

Parameter ``layout``:
    The layout whose paths are to be enumerated.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``params``:
    Parameters.

Returns:
    A collection of all unique paths in `layout` from
    `objective.source` to `objective.target`.)doc";

static const char *__doc_fiction_enumerate_all_paths_params = R"doc(Parameters for the algorithm that enumerates all paths in a layout.)doc";

static const char *__doc_fiction_enumerate_all_paths_params_crossings =
R"doc(Allow paths to cross over obstructed tiles if they are occupied by
wire segments.)doc";

static const char *__doc_fiction_eq_type = R"doc(The different equivalence types possible.)doc";

static const char *__doc_fiction_eq_type_NO = R"doc(`Spec` and `Impl` are logically not equivalent OR `Impl` has DRVs.)doc";

static const char *__doc_fiction_eq_type_STRONG =
R"doc(`Spec` and `Impl` are logically equivalent AND `Impl` has a throughput
of :math:`\frac{1}{1}`.)doc";

static const char *__doc_fiction_eq_type_WEAK =
R"doc(`Spec` and `Impl` are logically equivalent BUT `Impl` has a throughput
of :math:`\frac{1}{x}` with :math:`x > 1`.)doc";

static const char *__doc_fiction_equivalence_checking =
R"doc(Performs SAT-based equivalence checking between a specification of
type `Spec` and an implementation of type `Impl`. Both `Spec` and
`Impl` need to be network types (that is, gate-level layouts can be
utilized as well).

This implementation enables the comparison of two logic networks, a
logic network and a gate-level layout or two gate-level layouts. Since
gate-level layouts have a notion of timing that logic networks do not,
this function does not simply prove logical equivalence but,
additionally, takes timing aspects into account as well.

Thereby, three different types of equivalences arise:

- `NO` equivalence: Spec and Impl are not logically equivalent or one
of them is a gate-level layout that contains DRVs and, thus, cannot be
checked for equivalence. - `WEAK` equivalence: Spec and Impl are
logically equivalent but either one of them is a gate-level layout
with TP of :math:`\frac{1}{x}` with :math:`x > 1` or both of them are
gate-level layouts with TP of :math:`\frac{1}{x}` and
:math:`\frac{1}{y}`, respectively, where :math:`x \neq y`. - `STRONG`
equivalence: Spec and Impl are logically equivalent and all involved
gate-level layouts have TP of :math:`\frac{1}{1}`.

This approach was first proposed in \"Verification for Field-coupled
Nanocomputing Circuits\" by M. Walter, R. Wille, F. Sill Torres, D.
Große, and R. Drechsler in DAC 2020.

Template parameter ``Spec``:
    Specification type.

Template parameter ``Impl``:
    Implementation type.

Parameter ``spec``:
    The specification.

Parameter ``impl``:
    The implementation.

Parameter ``pst``:
    Statistics.

Returns:
    The equivalence type of `spec` and `impl`.)doc";

static const char *__doc_fiction_equivalence_checking_stats = R"doc()doc";

static const char *__doc_fiction_equivalence_checking_stats_counter_example = R"doc(Stores a possible counter example.)doc";

static const char *__doc_fiction_equivalence_checking_stats_duration = R"doc(Stores the runtime.)doc";

static const char *__doc_fiction_equivalence_checking_stats_eq = R"doc(Stores the equivalence type.)doc";

static const char *__doc_fiction_equivalence_checking_stats_impl_drv_stats = R"doc()doc";

static const char *__doc_fiction_equivalence_checking_stats_spec_drv_stats = R"doc(Stores DRVs.)doc";

static const char *__doc_fiction_equivalence_checking_stats_tp_diff = R"doc(Throughput values at which weak equivalence manifests.)doc";

static const char *__doc_fiction_equivalence_checking_stats_tp_impl = R"doc(Throughput values at which weak equivalence manifests.)doc";

static const char *__doc_fiction_equivalence_checking_stats_tp_spec = R"doc(Throughput values at which weak equivalence manifests.)doc";

static const char *__doc_fiction_esr_clocking =
R"doc(Returns the ESR clocking as defined in \"An efficient, scalable,
regular clocking scheme based on quantum dot cellular automata\" by
Jayanta Pal, Amit Kumar Pramanik, Jyotirmoy Sil Sharma, Apu Kumar
Saha, and Bibhash Sen in Analog Integrated Circuits and Signal
Processing 2021.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    ESR clocking scheme.)doc";

static const char *__doc_fiction_euclidean_distance =
R"doc(The Euclidean distance :math:`D` between two layout coordinates
:math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

:math:`D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}`

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Floating-point type for the distance.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Euclidean distance between `source` and `target`.)doc";

static const char *__doc_fiction_euclidean_distance_functor =
R"doc(A pre-defined distance functor that uses the Euclidean distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Floating-point distance type.)doc";

static const char *__doc_fiction_euclidean_distance_functor_euclidean_distance_functor = R"doc()doc";

static const char *__doc_fiction_evaluate_output =
R"doc(This function evaluates the given multi-output truth table at the
given input index.

Parameter ``truth_tables``:
    The truth tables to evaluate.

Parameter ``current_input_index``:
    The index representing the current input pattern.

Returns:
    Output of the truth tables.)doc";

static const char *__doc_fiction_even_column_cartesian =
R"doc(\verbatim +-------+ +-------+ | | | | +-------+ (1,0) +-------+ (3,0)
| | | | | | | (0,0) +-------+ (2,0) +-------+ | | | | | +-------+
(1,1) +-------+ (3,1) | | | | | | | (0,1) +-------+ (2,1) +-------+ |
| | | | +-------+ (1,2) +-------+ (3,2) | | | | | +-------+ +-------+
\endverbatim)doc";

static const char *__doc_fiction_even_column_hex =
R"doc(\verbatim _____ _____ / \ / \ _____/ (1,0) \_____/ (3,0) \ / \ / \ / /
(0,0) \_____/ (2,0) \_____/ \ / \ / \ \_____/ (1,1) \_____/ (3,1) \ /
\ / \ / / (0,1) \_____/ (2,1) \_____/ \ / \ / \ \_____/ (1,2) \_____/
(3,2) \ \ / \ / \_____/ \_____/ \endverbatim)doc";

static const char *__doc_fiction_even_row_cartesian =
R"doc(\verbatim +-------+-------+-------+ | | | | | (0,0) | (1,0) | (2,0) |
| | | | +---+---+---+---+---+---+---+ | | | | | (0,1) | (1,1) | (2,1)
| | | | | +---+---+---+---+---+---+---+ | | | | | (0,2) | (1,2) |
(2,2) | | | | | +-------+-------+-------+ \endverbatim)doc";

static const char *__doc_fiction_even_row_hex =
R"doc(\verbatim / \ / \ / \ / \ / \ / \ | (0,0) | (1,0) | (2,0) | | | | | /
\ / \ / \ / / \ / \ / \ / | (0,1) | (1,1) | (2,1) | | | | | \ / \ / \
/ \ \ / \ / \ / \ | (0,2) | (1,2) | (2,2) | | | | | \ / \ / \ / \ / \
/ \ / \endverbatim)doc";

static const char *__doc_fiction_exact =
R"doc(An exact placement & routing approach using SMT solving as originally
proposed in \"An Exact Method for Design Exploration of Quantum-dot
Cellular Automata\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
and R. Drechsler in DATE 2018. A more extensive description can be
found in \"Design Automation for Field-coupled Nanotechnologies\" by
M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
Springer Nature in 2022.

Via incremental SMT calls, an optimal gate-level layout for a given
logic network will be found under constraints. Starting with :math:`n`
tiles, where :math:`n` is the number of logic network nodes, each
possible layout aspect ratio will be examined by factorization and
tested for routability with the SMT solver Z3. When no upper bound is
given, this approach will run until it finds a solution to the
placement & routing problem instance.

Note that there a combinations of constraints for which no valid
solution under the given parameters exist for the given logic network.
Such combinations cannot be detected automatically. It is, thus,
recommended to always set a timeout. Recommended settings include the
use of I/O pins located at the layout borders for better integration.
Most networks are not realizable without crossings enabled. Specifying
a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
allows for the strictest constraints and, thereby, finds a solution
the quickest. However, for high input degree networks, no valid
solution exists when border I/Os are to be used unless global
synchronization is disabled. Generally, solutions are found the
fastest with the following settings: Crossings enabled, de-
synchronization enabled, and 2DDWave clocking given. Multi-threading
can sometimes speed up the process, especially for large networks.
Note that the more threads are being used, the less information can be
shared across the individual solver runs which destroys the benefits
of incremental solving and thereby, comparatively, slows down each
run.

The SMT instance works with a single layer of variables even though it
is possible to allow crossings in the solution. The reduced number of
variables saves a considerable amount of runtime. That's why
`layout.foreach_ground_tile()` is used even though the model will be
mapped to a 3-dimensional layout afterwards. Generally, the algorithm
incorporates quite a few encoding optimizations to be as performant as
possible on various layout topologies and clocking schemes.

The approach applies to any data structures that implement the
necessary functions to comply with `is_network_type` and
`is_gate_level_layout`, respectively. It is, thereby, mostly
technology-independent but can make certain assumptions if needed, for
instance for ToPoliNano-compliant circuits.

This approach requires the Z3 SMT solver to be installed on the
system. Due to this circumstance, it is excluded from (CLI)
compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
cmake call.

May throw a high_degree_fanin_exception if `ntk` contains any node
with a fan-in too large to be handled by the specified clocking
scheme.

Template parameter ``Lyt``:
    Desired gate-level layout type.

Template parameter ``Ntk``:
    Network type that acts as specification.

Parameter ``ntk``:
    The network that is to place and route.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    A gate-level layout of type `Lyt` that implements `ntk` as an FCN
    circuit if one is found under the given parameters;
    `std::nullopt`, otherwise.)doc";

static const char *__doc_fiction_exact_physical_design_params = R"doc(Parameters for the exact physical design algorithm.)doc";

static const char *__doc_fiction_exact_physical_design_params_border_io = R"doc(Flag to indicate that I/Os should be placed at the layout's border.)doc";

static const char *__doc_fiction_exact_physical_design_params_crossings = R"doc(Flag to indicate that crossings may be used.)doc";

static const char *__doc_fiction_exact_physical_design_params_desynchronize =
R"doc(Flag to indicate that a discrepancy in fan-in path lengths is allowed
(reduces runtime!).)doc";

static const char *__doc_fiction_exact_physical_design_params_fixed_size =
R"doc(Exclusively investigate aspect ratios that conform with the
restrictions imposed by the upper bound options. E.g., if `fixed_size
== true` *and* `upper_bound_area == 20`, only aspect ratios with
exactly 20 tiles will be examined. Restricted imposed by the
`upper_bound_x` and `upper_bound_y` flags additionally apply.)doc";

static const char *__doc_fiction_exact_physical_design_params_io_pins =
R"doc(Flag to indicate that I/Os should be realized by designated wire
segments (preferred).)doc";

static const char *__doc_fiction_exact_physical_design_params_minimize_crossings =
R"doc(Flag to indicate that the number of used crossing tiles should be
minimized.)doc";

static const char *__doc_fiction_exact_physical_design_params_minimize_wires =
R"doc(Flag to indicate that the number of used crossing tiles should be
minimized.)doc";

static const char *__doc_fiction_exact_physical_design_params_num_threads =
R"doc(Number of threads to use for exploring the possible aspect ratios.

@note This is an unstable beta feature.)doc";

static const char *__doc_fiction_exact_physical_design_params_scheme = R"doc(Clocking scheme to be used.)doc";

static const char *__doc_fiction_exact_physical_design_params_straight_inverters =
R"doc(Flag to indicate that straight inverters should be used over bend
ones.)doc";

static const char *__doc_fiction_exact_physical_design_params_synchronization_elements =
R"doc(Flag to indicate that artificial clock latch delays should be used to
balance paths (runtime expensive!).)doc";

static const char *__doc_fiction_exact_physical_design_params_technology_specifics =
R"doc(Technology-specific constraints that are only to be added for a
certain target technology.)doc";

static const char *__doc_fiction_exact_physical_design_params_timeout =
R"doc(Sets a timeout in ms for the solving process. Standard is 4294967
seconds as defined by Z3.)doc";

static const char *__doc_fiction_exact_physical_design_params_upper_bound_area =
R"doc(Number of total tiles to use as an upper bound.

@note If `upper_bound_area` and (either) `upper_bound_x` or
`upper_bound_y` are set, the imposed search space restrictions are
cumulative. E.g., if `upper_bound_area == 20` and `upper_bound_x ==
4`, all aspect ratios with an x-dimension of more than 4 *and* a total
area of more than 20 will be skipped.)doc";

static const char *__doc_fiction_exact_physical_design_params_upper_bound_x = R"doc(Number of tiles to use as an upper bound in x direction.)doc";

static const char *__doc_fiction_exact_physical_design_params_upper_bound_y = R"doc(Number of tiles to use as an upper bound in y direction.)doc";

static const char *__doc_fiction_exact_physical_design_stats = R"doc(Statistics.)doc";

static const char *__doc_fiction_exact_physical_design_stats_duration = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_num_aspect_ratios = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_num_crossings = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_num_gates = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_num_wires = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_report = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_x_size = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_y_size = R"doc()doc";

static const char *__doc_fiction_exact_sidb_simulation_engine = R"doc(Selector exclusively for exact SiDB simulation engines.)doc";

static const char *__doc_fiction_exact_sidb_simulation_engine_CLUSTERCOMPLETE =
R"doc(*ClusterComplete* is a novel exact simulation engine that requires
exponential runtime, though, depending on the simulation problem, it
effectively reduces the base number by a real number, thus allowing
problem sizes that were previously considered astronomical. Inherent
to the simulation methodology that does not depend on the simulation
base, it simulates very effectively for either base number (2 or 3).)doc";

static const char *__doc_fiction_exact_sidb_simulation_engine_EXGS =
R"doc(*Exhaustive Ground State Search* (EXGS) is an exact simulation engine
that always has exponential runtime.)doc";

static const char *__doc_fiction_exact_sidb_simulation_engine_QUICKEXACT =
R"doc(*QuickExact* is also an exact simulation engine that requires
exponential runtime, but it scales a lot better than ExGS due to its
effective search-space pruning.)doc";

static const char *__doc_fiction_exact_with_blacklist =
R"doc(The same as `exact` but with a black list of tiles that are not
allowed to be used to a specified set of Boolean functions and their
orientations. For example, a black list could be used to exclude the
use of a tile only for an AND gate of a certain rotation, but not for
other gates. This is useful if a tile is known to be faulty or if it
is known to be used for a different purpose.

Template parameter ``Lyt``:
    Desired gate-level layout type.

Template parameter ``Ntk``:
    Network type that acts as specification.

Parameter ``ntk``:
    The network that is to place and route.

Parameter ``black_list``:
    The black list of tiles and their gate orientations.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    A gate-level layout of type `Lyt` that implements `ntk` as an FCN
    circuit if one is found under the given parameters;
    `std::nullopt`, otherwise.)doc";

static const char *__doc_fiction_exhaustive_ground_state_simulation =
R"doc(*Exhaustive Ground State Simulation* (ExGS) which was proposed in
\"Computer-Aided Design of Atomic Silicon Quantum Dots and
Computational Applications\" by S. S. H. Ng
(https://dx.doi.org/10.14288/1.0392909) computes all physically valid
charge configurations of a given SiDB layout. All possible charge
configurations are passed and checked for physical validity. As a
consequence, its runtime grows exponentially with the number of SiDBs
per layout. Therefore, only layouts with up to 30 SiDBs can be
simulated in a reasonable time. However, since all charge
configurations are checked for validity, 100 % simulation accuracy is
guaranteed.

@note This was the first exact simulation approach. However, it is
replaced by *QuickExact* and *ClusterComplete* due to the much better
runtimes and more functionality.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``params``:
    Simulation parameters.

Parameter ``ps``:
    Simulation statistics.

Returns:
    sidb_simulation_result is returned with all results.)doc";

static const char *__doc_fiction_extract_routing_objectives =
R"doc(Extracts all routing objectives from the given layout. To this end,
all routing paths in the layout are traversed, starting at each PI.
Whenever the next regular node (non-IO, non-constant, non-wire) is
encountered, this connection is added to the list of all objectives.

Example: Let a layout have connections from `(0,0)` to `(2,3)` via a
cascade of wires and a direct connection from `(2,2)` to `(2,3)`. The
list of routing objectives extracted from that layout would contain
`{(0,0), (2,3)}` and `{(2,2), (2,3)}`.

In other words, if all wires were removed from the layout and all
connections ripped-up, an equivalent layout could be recreated from
the list of routing objectives.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    Layout whose routing objectives are to be extracted.

Returns:
    List of all routing objectives in the given layout.)doc";

static const char *__doc_fiction_fanin_container =
R"doc(Container that stores fanins of a node in a network, including whether
one of them is a constant.

Note that this container assumes that each node has a maximum of one
constant fanin.

Template parameter ``Ntk``:
    `mockturtle` network type.)doc";

static const char *__doc_fiction_fanin_container_constant_fanin =
R"doc(Has a value if a fanin node is constant. In that case, it represents
the constant value.)doc";

static const char *__doc_fiction_fanin_edge_container =
R"doc(Container that stores fanin edges of a node in a network, including
whether one of them is a constant.

Template parameter ``Ntk``:
    `mockturtle` network type.)doc";

static const char *__doc_fiction_fanin_edge_container_fanin_edges = R"doc(A vector of all fanin edges excluding for constants.)doc";

static const char *__doc_fiction_fanin_edges =
R"doc(Returns a fanin edge container filled with all fanin edges `(fanin,
node)` of some given network node.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    Network in which the fanin edges are to be gathered.

Parameter ``n``:
    Node whose fanins are desired.

Returns:
    A container of all incoming edges `(fanin, n)` in `ntk`.)doc";

static const char *__doc_fiction_fanins =
R"doc(Returns a fanin container filled with all fanin nodes of some given
network node.

Note that this function assumes that each node has a maximum of one
constant fanin.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    Network in which the fanins are to be gathered.

Parameter ``n``:
    Node whose fanins are desired.

Returns:
    A container of all incoming nodes directly adjacent to `n` in
    `ntk`.)doc";

static const char *__doc_fiction_fanout_substitution =
R"doc(Substitutes high-output degrees in a logic network with fanout nodes
that compute the identity function. For this purpose, `create_buf` is
utilized. Therefore, `NtkDest` should support identity nodes. If it
does not, no new nodes will in fact be created. In either case, the
returned network will be logically equivalent to the input one.

The process is rather naive with two possible strategies to pick from:
breath-first and depth-first. The former creates partially balanced
fanout trees while the latter leads to fanout chains. Further
parameterization includes thresholds for the maximum number of output
each node and fanout is allowed to have.

The returned network is newly created from scratch because its type
`NtkDest` may differ from `NtkSrc`.

@note The physical design algorithms natively provided in fiction do
not require their input networks to be fanout-substituted. If that is
necessary, they will do it themselves. Providing already substituted
networks does however allow for the control over maximum output
degrees.

Template parameter ``NtkDest``:
    Type of the returned logic network.

Template parameter ``NtkSrc``:
    Type of the input logic network.

Parameter ``ntk_src``:
    The input logic network.

Parameter ``ps``:
    Parameters.

Returns:
    A fanout-substituted logic network of type `NtkDest` that is
    logically equivalent to `ntk_src`.)doc";

static const char *__doc_fiction_fanout_substitution_params = R"doc(Parameters for the fanout substitution algorithm.)doc";

static const char *__doc_fiction_fanout_substitution_params_degree = R"doc(Maximum output degree of each fan-out node.)doc";

static const char *__doc_fiction_fanout_substitution_params_strategy =
R"doc(Substitution strategy of high-degree fanout networks (depth-first vs.
breadth-first).)doc";

static const char *__doc_fiction_fanout_substitution_params_substitution_strategy = R"doc(Breadth-first vs. depth-first fanout-tree substitution strategies.)doc";

static const char *__doc_fiction_fanout_substitution_params_substitution_strategy_BREADTH = R"doc(Breadth-first substitution. Creates balanced fanout trees.)doc";

static const char *__doc_fiction_fanout_substitution_params_substitution_strategy_DEPTH = R"doc(Depth-first substitution. Creates fanout trees with one deep branch.)doc";

static const char *__doc_fiction_fanout_substitution_params_threshold =
R"doc(Maximum number of outputs any gate is allowed to have before
substitution applies.)doc";

static const char *__doc_fiction_fcn_gate_library =
R"doc(Base class for various FCN libraries used to map gate-level layouts to
cell-level ones. Any new gate library can extend `fcn_gate_library` if
it benefits from its features but does not have to. The only
requirement is that it must be a static class that provides a

\verbatim embed:rst .. code-block:: c++

template <typename GateLyt> static fcn_gate set_up_gate(const GateLyt&
lyt, const tile<GateLyt>& t)

\endverbatim

public member function. Additionally, a

\verbatim embed:rst .. code-block:: c++

template <typename CellLyt> static void
post_layout_optimization(CellLyt& lyt) \endverbatim

can optionally be provided if some cleanup or optimization is
necessary on the cell-level layout after each gate has been mapped.

Additionally, a

\verbatim embed:rst .. code-block:: c++

static gate_functions get_functional_implementations() \endverbatim

can optionally be provided to allow reverse access to the gate
structures by functional implementation. This interface is for example
used to determine which gate types to blacklist on tiles in P&R
algorithms by considering gate implementation.

Finally, a

\verbatim embed:rst .. code-block:: c++

static gate_ports<PortType> get_gate_ports() \endverbatim

can optionally be provided to allow reverse access to the gate ports
given a gate implementation. This interface is for example used in
`sidb_surface_analysis` to determine which ports to blacklist.

Template parameter ``Technology``:
    FCN technology type of the implementing gate library.

Template parameter ``GateSizeX``:
    Tile size in x-dimension.

Template parameter ``GateSizeY``:
    Tile size in y-dimension.)doc";

static const char *__doc_fiction_fcn_gate_library_cell_list_to_gate =
R"doc(Converts a `cell_list` of type `T` to an `fcn_gate` at compile time.
This function allows to conveniently specify `fcn_gate` instances in a
semi-readable way in code. For examples usages see
`qca_one_library.hpp`.

Template parameter ``T``:
    Element type of given `cell_list`.

Parameter ``c``:
    Cell list to convert.

Returns:
    An `fcn_gate` created from the representation provided in `c`.)doc";

static const char *__doc_fiction_fcn_gate_library_fcn_gate_library = R"doc(Gate libraries should not be instantiated but used as static objects.)doc";

static const char *__doc_fiction_fcn_gate_library_gate_x_size =
R"doc(Returns horizontal size of gate blocks.

Returns:
    `GateSizeX`.)doc";

static const char *__doc_fiction_fcn_gate_library_gate_y_size =
R"doc(Returns vertical size of gate blocks.

Returns:
    `GateSizeY`.)doc";

static const char *__doc_fiction_fcn_gate_library_mark_cell =
R"doc(Applies given mark to given `fcn_gate` `g` at given port `p` at
compile time.

Parameter ``g``:
    Gate to apply mark to.

Parameter ``p``:
    Port specifying where to apply the mark.

Parameter ``mark``:
    Mark to be applied

Returns:
    Marked `fcn_gate`.)doc";

static const char *__doc_fiction_fcn_gate_library_merge =
R"doc(Merges multiple `fcn_gate`s into one at compile time. This is intended
to be used for wires. Unexpected behavior can be caused, if more than
one `fcn_gate` has a cell at the same position.

Parameter ``gates``:
    Vector of gates to be merged.

Returns:
    Merged `fcn_gate`.)doc";

static const char *__doc_fiction_fcn_gate_library_reverse_columns =
R"doc(Reverses the columns of the given `fcn_gate` at compile time.

Parameter ``g``:
    `fcn_gate` whose columns are to be reversed.

Returns:
    `fcn_gate` with reversed columns.)doc";

static const char *__doc_fiction_fcn_gate_library_reverse_rows =
R"doc(Reverses the rows of the given `fcn_gate` at compile time.

Parameter ``g``:
    `fcn_gate` whose rows are to be reversed.

Returns:
    `fcn_gate` with reversed rows.)doc";

static const char *__doc_fiction_fcn_gate_library_rotate_180 =
R"doc(Rotates the given `fcn_gate` by 180° at compile time.

Parameter ``g``:
    `fcn_gate` to rotate.

Returns:
    Rotated `fcn_gate`.)doc";

static const char *__doc_fiction_fcn_gate_library_rotate_270 =
R"doc(Rotates the given `fcn_gate` by 270° clockwise at compile time.

Parameter ``g``:
    `fcn_gate` to rotate.

Returns:
    Rotated `fcn_gate`.)doc";

static const char *__doc_fiction_fcn_gate_library_rotate_90 =
R"doc(Rotates the given `fcn_gate` by 90° clockwise at compile time.

Parameter ``g``:
    `fcn_gate` to rotate.

Returns:
    Rotated `fcn_gate`.)doc";

static const char *__doc_fiction_fcn_gate_library_transpose =
R"doc(Transposes the given `fcn_gate` at compile time.

Parameter ``g``:
    `fcn_gate` to transpose.

Returns:
    Transposed `fcn_gate`.)doc";

static const char *__doc_fiction_fgl_parsing_error =
R"doc(Exception thrown when an error occurs during parsing of a .fgl file
containing a gate-level layout.)doc";

static const char *__doc_fiction_fgl_parsing_error_fgl_parsing_error =
R"doc(Constructs a `fgl_parsing_error` object with the given error message.

Parameter ``msg``:
    The error message describing the parsing error.)doc";

static const char *__doc_fiction_find_first_two_of =
R"doc(A derivative of `std::find_first_of` that uses the example
implementation given at
https://en.cppreference.com/w/cpp/algorithm/find_first_of.

This version searches the range `[first, last)` for any two
consecutive elements of the elements in the range `[s_first, s_last)`,
i.e., for a 2-element sub-sequence shared between the ranges.

Example: Let two ranges be `[0, 1, 1, 2, 3, 3]` and `[1, 2, 3, 3]`. A
call to `find_first_two_of` would return an iterator to index 2, i.e.,
the second `1` in the first list, because the 2-element sub-sequence
`[1,2]` is shared between the two ranges.

Template parameter ``InputIt``:
    must meet the requirements of `LegacyInputIterator`.

Template parameter ``ForwardIt``:
    must meet the requirements of `LegacyForwardIterator`.

Parameter ``first``:
    Begin of the range to examine.

Parameter ``last``:
    End of the range to examine.

Parameter ``s_first``:
    Begin of the range to search for.

Parameter ``s_last``:
    End of the range to search for.

Returns:
    Iterator in the range `[first, last)` to the first position of the
    first 2-element sub-sequence shared between the two ranges, or
    `last` if no such shared sub-sequence exists.)doc";

static const char *__doc_fiction_find_key_with_tolerance =
R"doc(This function searches for a floating-point value specified by the
`key` in the provided map `map`, applying a tolerance specified by
`fiction::constants::ERROR_MARGIN`. Each key in the map is compared to
the specified key within this tolerance.

Template parameter ``MapType``:
    The type of the map containing parameter points as keys.

Parameter ``map``:
    The map containing parameter points as keys and associated values.

Parameter ``key``:
    The parameter point to search for in the map.

Returns:
    An iterator to the found parameter point in the map, or
    `map.cend()` if not found.)doc";

static const char *__doc_fiction_flat_top_hex = R"doc(\verbatim _____ / \ / \ \ / \_____/ \endverbatim)doc";

static const char *__doc_fiction_foreach_edge =
R"doc(Applies a function to all edges in a `mockturtle` network.

Template parameter ``Ntk``:
    `mockturtle` network type.

Template parameter ``Fn``:
    Unary function type that takes a `mockturtle::edge<Ntk>` object as
    parameter.

Parameter ``ntk``:
    Network to iterate over.

Parameter ``fn``:
    Function object to apply to each edge in `ntk`.)doc";

static const char *__doc_fiction_foreach_incoming_edge =
R"doc(Applies a function to all incoming edges in a `mockturtle` network.

Template parameter ``Ntk``:
    `mockturtle` network type.

Template parameter ``Fn``:
    Unary function type that takes a `mockturtle::edge<Ntk>` object as
    parameter.

Parameter ``ntk``:
    Network to iterate over.

Parameter ``n``:
    Node of `ntk` whose incoming edges are to be considered.

Parameter ``fn``:
    Function object to apply to each incoming edge of `n` in `ntk`.)doc";

static const char *__doc_fiction_foreach_outgoing_edge =
R"doc(Applies a function to all outgoing edges in a `mockturtle` network.

Template parameter ``Ntk``:
    `mockturtle` network type.

Template parameter ``Fn``:
    Unary function type that takes a `mockturtle::edge<Ntk>` object as
    parameter.

Parameter ``ntk``:
    Network to iterate over.

Parameter ``n``:
    Node of `ntk` whose outgoing edges are to be considered.

Parameter ``fn``:
    Function object to apply to each outgoing edge of `n` in `ntk`.)doc";

static const char *__doc_fiction_gate_design_exception =
R"doc(This exception is thrown when an error occurs during the design of an
SiDB gate. It provides information about the tile, truth table, and
port list associated with the error.

Template parameter ``TT``:
    The type representing the truth table.

Template parameter ``GateLyt``:
    The type representing the gate-level layout.)doc";

static const char *__doc_fiction_gate_design_exception_error_tile = R"doc(The tile associated with the error.)doc";

static const char *__doc_fiction_gate_design_exception_gate_design_exception =
R"doc(Constructor for the gate_design_exception class.

Parameter ``ti``:
    The tile associated with the error.

Parameter ``spec``:
    The truth table associated with the error.

Parameter ``portlist``:
    The port list associated with the error.)doc";

static const char *__doc_fiction_gate_design_exception_p = R"doc(The port list associated with the error.)doc";

static const char *__doc_fiction_gate_design_exception_truth_table = R"doc(The truth table associated with the error.)doc";

static const char *__doc_fiction_gate_design_exception_which_port_list = R"doc(Get the port list associated with the exception.)doc";

static const char *__doc_fiction_gate_design_exception_which_tile = R"doc(Get the tile associated with the exception.)doc";

static const char *__doc_fiction_gate_design_exception_which_truth_table = R"doc(Get the truth table associated with the exception.)doc";

static const char *__doc_fiction_gate_layout_cartesian_drawer =
R"doc(An extended gate-level layout DOT drawer for Cartesian layouts.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Template parameter ``ClockColors``:
    Flag to toggle the drawing of clock colors instead of gate type
    colors.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_gate_layout_cartesian_drawer_additional_graph_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_cartesian_drawer_additional_node_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_cartesian_drawer_enforce_topology = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer =
R"doc(An extended gate-level layout DOT drawer for hexagonal layouts.

Template parameter ``Lyt``:
    Hexagonal gate-level layout type.

Template parameter ``ClockColors``:
    Flag to toggle the drawing of clock colors instead of gate type
    colors.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_additional_graph_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_additional_node_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_enforce_same_hexagonal_column = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_enforce_same_hexagonal_row = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_enforce_topology = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_invisible_node = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_shift_column = R"doc()doc";

static const char *__doc_fiction_gate_layout_hexagonal_drawer_shift_row = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer =
R"doc(An extended gate-level layout DOT drawer for shifted Cartesian
layouts.

Template parameter ``Lyt``:
    Shifted Cartesian gate-level layout type.

Template parameter ``ClockColors``:
    Flag to toggle the drawing of clock colors instead of gate type
    colors.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_additional_graph_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_additional_node_attributes = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_enforce_same_shifted_column = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_enforce_same_shifted_row = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_enforce_topology = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_invisible_node = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_shift_column = R"doc()doc";

static const char *__doc_fiction_gate_layout_shifted_cartesian_drawer_shift_row = R"doc()doc";

static const char *__doc_fiction_gate_level_drv_params =
R"doc(Parameters for design rule violation checking that specify the checks
that are to be executed.)doc";

static const char *__doc_fiction_gate_level_drv_params_border_io = R"doc(Check if the I/Os are located at the layout's border.)doc";

static const char *__doc_fiction_gate_level_drv_params_clocked_data_flow = R"doc(Check if all node connections obey the clocking scheme data flow.)doc";

static const char *__doc_fiction_gate_level_drv_params_crossing_gates = R"doc(Check for wires that are crossing gates.)doc";

static const char *__doc_fiction_gate_level_drv_params_empty_io = R"doc(Check if the I/Os are assigned to empty tiles.)doc";

static const char *__doc_fiction_gate_level_drv_params_has_io = R"doc(Check if the layout has I/Os.)doc";

static const char *__doc_fiction_gate_level_drv_params_io_pins = R"doc(Check if the I/Os are assigned to wire segments.)doc";

static const char *__doc_fiction_gate_level_drv_params_missing_connections = R"doc(Check for nodes without connections.)doc";

static const char *__doc_fiction_gate_level_drv_params_non_adjacent_connections = R"doc()doc";

static const char *__doc_fiction_gate_level_drv_params_out = R"doc(Stream to write the report into.)doc";

static const char *__doc_fiction_gate_level_drv_params_placed_dead_nodes = R"doc(Check for placed but dead nodes.)doc";

static const char *__doc_fiction_gate_level_drv_params_unplaced_nodes = R"doc(Check for nodes without locations.)doc";

static const char *__doc_fiction_gate_level_drv_stats = R"doc()doc";

static const char *__doc_fiction_gate_level_drv_stats_drvs = R"doc(Number of design rule violations.)doc";

static const char *__doc_fiction_gate_level_drv_stats_report = R"doc(Report.)doc";

static const char *__doc_fiction_gate_level_drv_stats_warnings = R"doc(Number of warnings.)doc";

static const char *__doc_fiction_gate_level_drvs =
R"doc(Performs design rule violation (DRV) checking on the given gate-level
layout. The implementation of gate_level_layout allows for layouts
with structural defects like the connection of non-adjacent tiles or
connections that defy the clocking scheme. This function checks for
such violations and documents them in the statistics. A brief report
can be printed and more in-depth information including with error
sites can be obtained from a generated json object.

Furthermore, this function does not only find and log DRVs but can
also warn for instances that are not per se errors but defy best
practices of layout generation, e.g., I/Os not being placed at the
layout borders.

For this function to work, `detail::gate_level_drvs_impl` need to be
declared as a `friend class` to the layout type that is going to be
examined.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The gate-level layout that is to be examined for DRVs and
    warnings.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_gate_level_layout =
R"doc(A layout type to layer on top of a clocked layout that allows the
assignment of gates to clock zones (aka tiles in this context). This
class represents a gate-level FCN layout and, thus, adds a notion of
Boolean logic. The gate_level_layout class fulfills the requirements
of a `mockturtle` logic network so that it can be used in many of
`mockturtle`'s algorithms. Since a layout has to assign fixed
positions to its gates (logic nodes), most generative member functions
like `create_pi`, `create_po`, `create_and`, etc. require additional
coordinate parameters. Consequently, `mockturtle`'s algorithms cannot
be used to generate gate_level_layout networks. To make the class
compliant with the API anyways, these member functions have their
parameters defaulted but they are, in fact required to create
meaningful layouts.

The following notion is utilized in this implementation: - a node `n`
is an index representing the `n`th created gate. All properties of
said gate, e.g., its type and position, are stored independently and
can be requested from the layout. An empty layout has 2 nodes, namely
`const0` and `const1` as required by `mockturtle`. At the moment, they
are not used for anything meaningful but could be.

- a signal is an unsigned integer representation of a `tile`, i.e., a
coordinate in the layout. It can be seen as a pointer to a position.
Consequently, the utilized coordinates need to be convertible to
`uint64_t`.

- the creation of PIs and POs creates nodes (the latter in contrast to
other `mockturtle` networks) that have a position on the layout.

- the creation of buffers (`create_buf`) creates nodes as well. A
buffer with more than one output is a fanout such that `is_fanout`
will return `true` on it. However, it is also still a buffer (`is_buf`
returns `true` as well). Buffers and wires are used interchangeably.

- each node has an associated gate function. PIs, POs, and buffers
compute the identity function.

- signals (pointers to tiles) cannot be inverting. Thereby, inverter
nodes (gates) have to be created that can be checked for via is_inv.

- each `create_...` function requires a tile parameter that determines
its placement. If the provided tile is dead, the location will not be
stored and the node will not count towards number of gates or wires.

- a node can be overwritten by creating another node on its location.
This can, however, lead to unwanted effects and should be avoided.

- nodes can be moved via the `move_node` function. This function can
also be used to update their children, i.e., incoming signals.

Most implementation details regarding `mockturtle`-specific functions
are borrowed from `mockturtle/networks/klut.hpp`. Therefore,
`mockturtle` API functions are only sporadically documented where
their behavior might differ. Information on their functionality can be
found in `mockturtle`'s docs.

Template parameter ``ClockedLayout``:
    The clocked layout that is to be extended by gate functions.)doc";

static const char *__doc_fiction_gate_level_layout_assign_node = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_clear_tile =
R"doc(Removes all assigned nodes from the given tile and marks them as dead.

@note This function does not reduce the number of nodes in the layout
nor does it reduce the number of PIs that are being returned via
`num_pis()` even if the tile to clear is an input tile. However, the
number of POs is reduced if the tile to clear is an output tile. While
this seems counter-intuitive and inconsistent, it is in line with
mockturtle's understanding of nodes and primary outputs.

Parameter ``t``:
    Tile whose nodes are to be removed.)doc";

static const char *__doc_fiction_gate_level_layout_clear_values = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_clear_visited = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_gate_level_layout_compute = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_compute_2 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_connect =
R"doc(Connects the given signal `s` to the given node `n` as a child. The
new child `s` is appended at the end of `n`'s list of children. Thus,
if the order of children is important, `move_node()` should be used
instead. Otherwise, this function has a smaller overhead and is to be
preferred.

Parameter ``s``:
    New incoming signal to `n`.

Parameter ``n``:
    Node that should add `s` as its child.

Returns:
    Signal pointing to `n`.)doc";

static const char *__doc_fiction_gate_level_layout_constant_value = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_and = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_buf = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_ge = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_gt = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_le = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_lt = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_maj = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_nand = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_node = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_node_from_literal = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_nor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_not = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_or = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_pi = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_po = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_xnor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_create_xor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_decr_value = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_events = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_evnts = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_fanin_size =
R"doc(Returns the number of incoming, adjacently placed, and properly
clocked signals to the given node.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``n``:
    Node to check.

Returns:
    Number of fanins to `n`.)doc";

static const char *__doc_fiction_gate_level_layout_fanout_size =
R"doc(Returns the number of outgoing, adjacently placed, and properly
clocked signals of the given node.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``n``:
    Node to check.

Returns:
    Number of fanouts to `n`.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_ci = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_foreach_co = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_foreach_fanin =
R"doc(Applies a function to all nodes that are incoming to a given one.
Thereby, only incoming clocked zones (+/- one layer to include
crossings) are being considered whose data flow connections are
respectively established. That is, the given function is applied to
all nodes that are connected to the one assigned to `t` as fanins on
neighboring tiles.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``n``:
    Node whose fanins are desired.

Parameter ``fn``:
    Functor to apply to each of `n`'s fanins.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_fanout =
R"doc(Applies a function to all nodes that are outgoing from a given one.
Thereby, only outgoing clocked zones (+/- one layer to include
crossings) are being considered whose data flow connections are
respectively established. That is, the given function is applied to
all nodes that are connected to the one assigned to `t` as fanouts on
neighboring tiles.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``n``:
    Node whose fanouts are desired.

Parameter ``fn``:
    Functor to apply to each of `n`'s fanouts.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_gate =
R"doc(Applies a function to all gates (excluding dead ones) in the layout.
Uses `is_gate` to check whether a node is a gate.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_if`.

Parameter ``fn``:
    Functor to apply to each gate that is not dead.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_node =
R"doc(Applies a function to all nodes (excluding dead ones) in the layout.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_if`.

Parameter ``fn``:
    Functor to apply to each node that is not dead.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_pi =
R"doc(Applies a function to all primary input nodes (including dead ones) in
the layout.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Parameter ``fn``:
    Functor to apply to each primary input node.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_po =
R"doc(Applies a function to all primary output signals (including those that
point to dead nodes) in the layout. Note the difference to
`foreach_pi` in the signature of `fn`. This function applies to all
POs as signals whereas `foreach_pi` applies to all PIs as nodes. This
is with respect to `mockturtle`'s API.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_transform`.

Parameter ``fn``:
    Functor to apply to each primary output signal.)doc";

static const char *__doc_fiction_gate_level_layout_foreach_wire =
R"doc(Applies a function to all wires (excluding dead ones) in the layout.
Uses `is_wire` to check whether a node is a wire.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element_if`.

Parameter ``fn``:
    Functor to apply to each wire that is not dead.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout =
R"doc(Standard constructor. Creates a named gate-level layout of the given
aspect ratio. To this end, it calls `ClockedLayout`'s standard
constructor.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``name``:
    Layout name.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_2 =
R"doc(Standard constructor. Creates a gate-level layout of the given aspect
ratio and clocks it via the given clocking scheme. To this end, it
calls `ClockedLayout`'s standard constructor.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``scheme``:
    Clocking scheme to apply to this layout.

Parameter ``name``:
    Layout name.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_3 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another gate_level_layout.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_4 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another gate_level_layout.

Parameter ``e``:
    Event storage of another gate_level_layout.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_5 =
R"doc(Copy constructor from another `ClockedLayout`.

Parameter ``lyt``:
    Clocked layout.)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_const0 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_const1 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_fn_cache = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_layout_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_node_names = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_node_tile_map = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_num_crossings = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_num_gates = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_num_wires = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_tile_node_map = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_data_trav_id = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_node =
R"doc(! gate-level layout node

`data[0].h1`: Internal (data-flow independent) fan-out size (MSB
indicates dead nodes) `data[0].h2`: Application-specific value
`data[1].h1`: Function literal in truth table cache `data[2].h2`:
Visited flags)doc";

static const char *__doc_fiction_gate_level_layout_gate_level_layout_storage_node_operator_eq = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_constant = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_input_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_layout_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_name_2 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_node =
R"doc(Fetches the node that is placed onto a tile pointed to by a given
signal. If no node is placed there, the `const0` node is returned.

Parameter ``s``:
    Pointer to a tile.

Returns:
    Node at position `t` where `s` points at `t`; or 0 if no node is
    placed at `t`.)doc";

static const char *__doc_fiction_gate_level_layout_get_node_2 =
R"doc(Fetches the node that is placed onto the provided tile If no node is
placed there, the `const0` node is returned.

Parameter ``t``:
    Tile in the layout.

Returns:
    Node at position `t`; or 0 if no node is placed at `t`.)doc";

static const char *__doc_fiction_gate_level_layout_get_output_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_get_tile =
R"doc(The inverse function of `get_node`. Fetches the tile that the provided
node is placed on. Returns a default dead tile if the node is not
placed.

Parameter ``n``:
    Node whose location is desired.

Returns:
    Tile at which `n` is placed or a default dead tile if `n` is not
    placed.)doc";

static const char *__doc_fiction_gate_level_layout_has_eastern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_input_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_name_2 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_no_incoming_signal =
R"doc(Checks whether the given tile has no incoming tiles.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` does not have incoming tiles.)doc";

static const char *__doc_fiction_gate_level_layout_has_no_outgoing_signal =
R"doc(Checks whether the given tile has no outgoing tiles.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` does not have outgoing tiles.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_eastern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in north-eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in north-eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in north-western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in north-western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_west(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_northern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in northern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_northern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in northern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_opposite_incoming_and_outgoing_signals =
R"doc(Checks whether the given tile `t` has its incoming and outgoing
signals on opposite sides of the tile. For this purpose, the function
relies on `foreach_adjacent_opposite_coordinates` of the underlying
`ClockedLayout`.

This function is very helpful for many gate libraries to check for
(non-)straight gates, which might look different.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins and fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` has incoming and outgoing signals on opposite
    sides.)doc";

static const char *__doc_fiction_gate_level_layout_has_output_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_south_eastern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in south-eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in south-eastern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in south-western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in south-western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_west(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_southern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in southern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_southern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in southern
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in western
direction.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `west(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_incoming_data_flow =
R"doc(Returns a container that contains all tiles that feed information to
the given one. Thereby, only incoming clocked zones (+/- one layer to
include crossings) are being considered whose data flow connections
are respectively established. That is, the returned container contains
all tiles that host nodes that are connected to the one assigned to
`t` as fanins.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Tile whose incoming data flow ones are desired.

Returns:
    A container that contains all of `t`'s incoming data flow tiles.)doc";

static const char *__doc_fiction_gate_level_layout_incr_trav_id = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_incr_value = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_index_to_node = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_initialize_truth_table_cache = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_and = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_buf =
R"doc(Returns whether `n` computes the identity function.

Parameter ``n``:
    Node to check.

Returns:
    `true` iff `n` computes the identity.)doc";

static const char *__doc_fiction_gate_level_layout_is_child = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_ci = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_co = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_combinational = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_complemented =
R"doc(Necessary function in the `mockturtle` API. However, in this layout
type, signals cannot be complemented.

Parameter ``s``:
    Signal to check.

Returns:
    `false`.)doc";

static const char *__doc_fiction_gate_level_layout_is_constant = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_dead =
R"doc(Checks whether a node (not its assigned tile) is dead. Nodes can be
dead for a variety of reasons. For instance if they are dangling (see
the `mockturtle` API). In this layout type, nodes are also marked dead
when they are not assigned to a tile (which is considered equivalent
to dangling).

Parameter ``n``:
    Node to check for liveliness.

Returns:
    `true` iff `n` is dead.)doc";

static const char *__doc_fiction_gate_level_layout_is_empty =
R"doc(Checks whether there are no gates or wires assigned to the layout's
coordinates.

Returns:
    `true` iff the layout is empty.)doc";

static const char *__doc_fiction_gate_level_layout_is_empty_tile =
R"doc(Returns whether `t` does not have a node assigned to it.

Parameter ``t``:
    Tile to check.

Returns:
    `true` iff `t` is an empty tile.)doc";

static const char *__doc_fiction_gate_level_layout_is_fanout =
R"doc(Returns whether `n` is a wire and has multiple outputs, thereby,
acting as a fanout gate. Note that a fanout will return `true` for
both `is_wire` and `is_fanout`.

Parameter ``n``:
    Node to check.

Returns:
    `true` iff `n` is a fanout gate.)doc";

static const char *__doc_fiction_gate_level_layout_is_function =
R"doc(Returns whether `n`ode `n` computes a function. That is, this function
returns `true` iff `n` is not a constant.

Parameter ``n``:
    Node to check.

Returns:
    `true` iff `n` is not a constant.)doc";

static const char *__doc_fiction_gate_level_layout_is_gate =
R"doc(Returns whether a given node is a gate in accordance with
`mockturtle`'s definition, i.e., whether it not a constant and not a
PI. Thereby, any wire/buffer (including POs) is a gate if this
function is used to check for it. This poses an inconsistency but is
required to comply with certain `mockturtle` algorithms.

Parameter ``n``:
    Node to check.

Returns:
    `true` iff `n` is neither a constant nor a PI.)doc";

static const char *__doc_fiction_gate_level_layout_is_gate_tile =
R"doc(Returns whether the node assigned to `t` fulfills `is_gate` (in
accordance with `mockturtle`'s definition of gates).

Parameter ``t``:
    Tile to check.

Returns:
    `true` iff `t` hosts a node that is a neither a constant nor a PI.)doc";

static const char *__doc_fiction_gate_level_layout_is_ge = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_gt = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_incoming_signal =
R"doc(Checks whether signal `s` is incoming to tile `t`. That is, whether
tile `t` hosts a node that has a fanin assigned to the tile that
signal `s` points to.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanins.

Parameter ``t``:
    Base tile.

Parameter ``s``:
    Signal pointing to a potential incoming tile to `t`.

Returns:
    `true` iff `s` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_is_inv =
R"doc(Returns whether `n` computes the binary inversion (NOT gate).

Parameter ``n``:
    Node to check.

Returns:
    `true` iff `n` is a NOT gate.)doc";

static const char *__doc_fiction_gate_level_layout_is_le = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_lt = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_maj = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_nand = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_nor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_or = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_outgoing_signal =
R"doc(Checks whether signal `s` is outgoing from tile `t`. That is, whether
tile `t` hosts a node that has a fanout assigned to the tile that
signal `s` points to.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Base tile.

Parameter ``s``:
    Signal pointing to a potential outgoing tile of `t`.

Returns:
    `true` iff `s` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_is_pi = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_pi_tile =
R"doc(Check whether tile `t` hosts a primary input.

Parameter ``t``:
    Tile to be checked.

Returns:
    `true` iff the node located at tile `t` is a PI.)doc";

static const char *__doc_fiction_gate_level_layout_is_po = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_po_tile =
R"doc(Check whether tile `t` hosts a primary output.

Parameter ``t``:
    Tile to be checked.

Returns:
    `true` iff the node located at tile `t` is a PO.)doc";

static const char *__doc_fiction_gate_level_layout_is_wire = R"doc(Equivalent to `is_buf`.)doc";

static const char *__doc_fiction_gate_level_layout_is_wire_tile =
R"doc(Returns whether the node assigned to `t` fulfills `is_wire`.

Parameter ``t``:
    Tile to check.

Returns:
    `true` iff `t` hosts a node that computes the identity.)doc";

static const char *__doc_fiction_gate_level_layout_is_xnor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_is_xor = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_kill_node = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_make_signal =
R"doc(Invokes the same behavior as `get_tile(n)` but additionally casts the
return value to a signal. That is, this function returns the signal
representation of the tile that the node `n` is assigned to.

Parameter ``n``:
    Node whose signal is desired.

Returns:
    Signal that points to `n`.)doc";

static const char *__doc_fiction_gate_level_layout_move_node =
R"doc(Moves a given node to a new position and also updates its children,
i.e., incoming signals.

Parameter ``n``:
    Node to move.

Parameter ``t``:
    Tile to move `n` to.

Parameter ``new_children``:
    New incoming signals to `n`.

Returns:
    Signal pointing to `n`'s new tile.)doc";

static const char *__doc_fiction_gate_level_layout_node_function = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_node_to_index = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_cis = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_cos = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_crossings =
R"doc(Returns the number of placed nodes in the layout that compute the
identity function and cross other nodes.

Returns:
    Number of crossings in the layout.)doc";

static const char *__doc_fiction_gate_level_layout_num_gates =
R"doc(Returns the number of placed nodes in the layout that do not compute
the identity function.

Returns:
    Number of gates in the layout.)doc";

static const char *__doc_fiction_gate_level_layout_num_latches = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_pis = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_pos = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_registers = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_num_wires =
R"doc(Returns the number of placed nodes in the layout that compute the
identity function including PIs and POs.

Returns:
    Number of wires in the layout.)doc";

static const char *__doc_fiction_gate_level_layout_outgoing_data_flow =
R"doc(Returns a container that contains all tiles that accept information
from the given one. Thereby, only outgoing clocked zones (+/- one
layer to include crossings) are being considered whose data flow
connections are respectively established. That is, the returned
container contains all tiles that host nodes that are connected to the
one assigned to `t` as fanouts.

Template parameter ``RespectClocking``:
    Flag to indicate that the underlying clocking is to be respected
    when evaluating fanouts.

Parameter ``t``:
    Tile whose outgoing data flow ones are desired.

Returns:
    A container that contains all of `t`'s outgoing data flow tiles.)doc";

static const char *__doc_fiction_gate_level_layout_pi_at = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_po_at = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_revive_node = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_input_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_layout_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_name_2 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_output_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_value = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_set_visited = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_size =
R"doc(Does NOT return the layout dimensions but the number of nodes
(including constants and dead ones) in accordance with the
`mockturtle` API.

Returns:
    Number of all nodes.)doc";

static const char *__doc_fiction_gate_level_layout_strg = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_trav_id = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_value = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_visited = R"doc()doc";

static const char *__doc_fiction_generate_edge_intersection_graph =
R"doc(Creates an edge intersection graph of all paths that satisfy a given
list of routing objectives. That is, this function generates an
undirected graph whose nodes represent paths in the given layout and
whose edges represent intersections of these paths. An intersection is
understood as the non-disjunction of paths, i.e., they share at least
one coordinate. To generate the paths for the routing objectives, all
possible paths from source to target in the layout are enumerated
while taking obstructions into consideration. The given layout must be
clocked.

Template parameter ``Lyt``:
    Type of the clocked layout.

Parameter ``lyt``:
    The layout to generate the edge intersection graph for.

Parameter ``objectives``:
    A list of routing objectives given as source-target pairs.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    An edge intersection graph of paths satisfying the given routing
    objectives in `lyt`.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_params = R"doc(Parameters for the edge intersection graph generation algorithm.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_params_crossings =
R"doc(Allow crossings by not creating edges between paths that only share
single-coordinate sub-paths.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_params_path_limit =
R"doc(If a value is given, for each objective, only up to the `path_limit`
shortest paths will be enumerated (using Yen's algorithm) instead of
all paths.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats = R"doc()doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_cliques =
R"doc(Stores all cliques in the resulting graph that were created during
path enumeration. There might be more cliques in the overall graph but
these ones correspond to one routing objective each, which could be
useful information to have in certain algorithms.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_duration = R"doc(Runtime measurement.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_num_edges = R"doc(Stores the size of the generated edge intersection graph.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_num_vertices = R"doc(Stores the size of the generated edge intersection graph.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_number_of_unroutable_objectives =
R"doc(For each routing objective that cannot be fulfilled in the given
layout, this counter is incremented.)doc";

static const char *__doc_fiction_generate_multiple_random_sidb_layouts =
R"doc(Generates multiple random layouts featuring a random arrangement of
SiDBs. These randomly placed dots can be incorporated into an existing
layout skeleton that may be optionally provided.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Parameter ``params``:
    The parameters for generating the random SiDB layouts.

Parameter ``skeleton``:
    Optional layout to which random dots are added.

Returns:
    A vector containing the unique randomly generated SiDB layouts. If
    the design is impossible, `std::nullopt`)doc";

static const char *__doc_fiction_generate_random_sidb_layout =
R"doc(Generates a layout featuring a random arrangement of SiDBs. These
randomly placed dots can be incorporated into an existing layout
skeleton that may be optionally provided.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Parameter ``params``:
    The parameters for generating the random layout.

Parameter ``skeleton``:
    Optional layout to which random dots are added.

Returns:
    A randomly generated SiDB layout, or `std::nullopt` if the process
    failed due to conflicting parameters.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params =
R"doc(This struct stores the parameters for the
`generate_random_sidb_layout` algorithm.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_coordinate_pair =
R"doc(Two coordinates that span the region where SiDBs may be placed (order
is not important). The first coordinate is the upper left corner and
the second coordinate is the lower right corner of the area.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_maximal_attempts =
R"doc(Maximum number of steps to place the specified number of SiDBs.
Example: If the area, where SiDBs can be placed, is small and many
SiDBs are to be placed, several tries are required to generate a
layout with no positively charged SiDBs.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_maximal_attempts_for_multiple_layouts =
R"doc(The maximum number of attempts allowed to generate the given number of
unique layouts (default: :math:`10^{6}`). Example: If the area, where
SiDBs can be placed, is small and many SiDBs are to be placed, it may
be difficult or even impossible to find several unique (given by
number_of_unique_generated_layouts) layouts. Therefore, this parameter
sets a limit for the maximum number of tries.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_number_of_sidbs = R"doc(Number of SiDBs that are placed on the layout.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_number_of_unique_generated_layouts = R"doc(The desired number of unique layouts to be generated.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_charges =
R"doc(An enumeration of modes to use for the generation of random SiDB
layouts to control control the appearance of positive charges.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_charges_ALLOWED =
R"doc(Positive charges can occur (i.e. SiDBs can be placed right next to
each other).)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_charges_FORBIDDEN =
R"doc(Positive charges are not allowed to occur (i.e. SiDBs need to be
seperated by a few lattice points).)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_charges_MAY_OCCUR =
R"doc(Positive charges can occur, which means that the
`can_positive_charges_occur` function returns `true`.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_sidbs =
R"doc(If positively charged SiDBs should be prevented, SiDBs are not placed
closer than the minimal_spacing.)doc";

static const char *__doc_fiction_generate_random_sidb_layout_params_simulation_parameters = R"doc(Simulation parameters.)doc";

static const char *__doc_fiction_geometric_temperature_schedule =
R"doc(A logarithmically decreasing temperature schedule. The temperature is
altered by multiplying it with `0.99`.

Parameter ``t``:
    The current temperature.

Returns:
    The next temperature, i.e. :math:`\texttt{t} \cdot 0.99`.)doc";

static const char *__doc_fiction_get_clocking_scheme =
R"doc(Returns a clocking scheme by name.

Template parameter ``Lyt``:
    Layout type.

Parameter ``name``:
    Name of the desired clocking scheme.

Returns:
    Clocking scheme object that matches the given `name`, or
    `std::nullopt` if no clocking scheme by the given `name` exists.)doc";

static const char *__doc_fiction_get_cluster_size =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_get_cluster_size_2 =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_get_name =
R"doc(Helper function to conveniently fetch the name from a layout or
network as they use different function names for the same purpose.

Template parameter ``NtkOrLyt``:
    Network or layout type.

Parameter ``ntk_or_lyt``:
    Network or layout object.

Returns:
    Name of given network or layout.)doc";

static const char *__doc_fiction_get_projector_state_compositions =
R"doc(This function is used to obtain the cluster charge state compositions
of the multiset charge configuration in a projector state. The
corresponding charge space element of the projector is found and its
compositions are returned.

Parameter ``pst``:
    Projector state of which the corresponding compositions are
    requested.

Returns:
    The compositions associated with the multiset charge configuration
    of the projecting cluster.)doc";

static const char *__doc_fiction_get_sidb_simulation_engine =
R"doc(Returns a simulation engine by name.

Parameter ``name``:
    Name of the desired SiDB simulation engine.

Returns:
    SiDB simulation engine that matches the given `name`, or
    `std::nullopt` if no simulation engine by the given `name` exists.)doc";

static const char *__doc_fiction_get_singleton_sidb_ix =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_get_singleton_sidb_ix_2 =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_get_unique_cluster_id =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_get_unique_cluster_id_2 =
R"doc(Forward declaration. Required for compilation due to the mutually
recursive structure in this file.)doc";

static const char *__doc_fiction_graph_coloring_engine =
R"doc(An enumeration of coloring engines to use for the graph coloring. All
but SAT are using the graph-coloring library by Brian Crites.)doc";

static const char *__doc_fiction_graph_coloring_engine_DSATUR =
R"doc(Saturation degree algorithm proposed in \"New Methods to Color the
Vertices of a Graph\" by Daniel Brélaz in Communications of the ACM,
1979. This algorithm is a heuristic but is exact for bipartite graphs.)doc";

static const char *__doc_fiction_graph_coloring_engine_LMXRLF =
R"doc(A randomized heuristic algorithm that combines various paradigms like
divide-and-conquer, objective functions, reuse of intermediate
solutions etc. It was proposed in \"Efficient Coloring of a Large
Spectrum of Graphs\" by Darko Kirovski and Miodrag Potkonjak in DAC
1998. While this algorithm is really performant, it tends to find non-
optimal solutions even for small instances.)doc";

static const char *__doc_fiction_graph_coloring_engine_MCS =
R"doc(Optimal coloring for chordal graphs proposed in \"Register Allocation
via Coloring of Chordal Graphs\" by Jens Palsberg in CATS 2007.)doc";

static const char *__doc_fiction_graph_coloring_engine_SAT = R"doc(Custom iterative SAT-based encoding that finds optimal colorings.)doc";

static const char *__doc_fiction_graph_coloring_engine_TABUCOL =
R"doc(A :math:`k`-coloring algorithm using tabu search proposed in \"Using
Tabu Search Techniques for Graph Coloring\" by A. Hertz and D. de
Werra in Computing 1987. The authors claim that it significantly
outperforms simulated annealing. However, since it is a
:math:`k`-coloring algorithm, it is required to set `k_color_value` in
`determine_vertex_coloring_params` to the chromatic number that is to
be checked for.)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic =
R"doc(An enumeration of search tactics to use for the SAT-based graph
coloring to determine a min-coloring.)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_BINARY_SEARCH =
R"doc(First ascend exponentially by checking for :math:`k = 2^0, 2^1, 2^2,
\dots` until SAT, then perform binary search in the window
:math:`[2^{h-1}, 2^h]`, where :math:`2^h` was the first SAT. If at
least one clique is passed, :math:`k` starts at the largest clique
size :math:`|C|` instead with :math:`k = 2^0 \cdot |C|, 2^1 \cdot |C|,
2^2 \cdot |C|, \dots`)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_LINEARLY_ASCENDING =
R"doc(Ascend linearly by checking for :math:`k = 1, 2, 3, \dots` until SAT.
If at least one clique is passed, :math:`k` starts at the largest
clique size :math:`|C|` instead with :math:`k = |C|, |C| + 1, |C| + 2,
\dots`)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_LINEARLY_DESCENDING =
R"doc(Descend linearly by checking for :math:`k = |G|, |G| - 1, |G| - 2,
\dots` until UNSAT.)doc";

static const char *__doc_fiction_graph_oriented_layout_design =
R"doc(A scalable and efficient placement & routing approach based on
spanning a search space graph of partial layouts and finding a path to
one of its leaves, i.e., a complete layout.

The search space graph starts with an empty layout and then expands it
based on where the first node in a topological sort of the logic
network can be placed. Based on the position of this first node, a
cost is assigned to each expansion based on the position of the placed
node. The vertex with the lowest cost, which is the smallest layout
w.r.t. the cost objective (e.g. area), is then chosen for the next
expansion. This iterative process continues until a leaf node is
found, which is a layout with all nodes placed. The algorithm then
continues to backtrack through the search space graph to find other
complete layouts with lower cost.

Exclusively generates 2DDWave-clocked layouts.

This algorithm was proposed in \"A* is Born: Efficient and Scalable
Physical Design for Field-coupled Nanocomputing\" by S. Hofmann, M.
Walter, and R. Wille in IEEE NANO 2024
(https://ieeexplore.ieee.org/document/10628808).

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Template parameter ``Ntk``:
    Network type.

Parameter ``ntk``:
    The network to be placed and routed.

Parameter ``ps``:
    The parameters for the A* priority routing algorithm. Defaults to
    an empty parameter set.

Parameter ``pst``:
    A pointer to a statistics object to record execution details.
    Defaults to nullptr.

Parameter ``custom_cost_objective``:
    A custom cost objective that is evaluated at every expansion of
    the search space graph. Should be a function that can be
    calculated based on the current partial layout and returns an
    uint64_t that should be minimized.

Returns:
    The smallest layout yielded by the graph-oriented layout design
    algorithm under the given parameters.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params = R"doc(Parameters for the graph-oriented layout design algorithm.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost = R"doc(The cost objective used. Defaults to AREA)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective =
R"doc(The `cost_objective` enum defines various cost objectives that can be
used in the graph-oriented layout design process. Each cost objective
represents a different metric used to expand a vertex in the search
space graph.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective_ACP =
R"doc(ACP (Area-Crossings Product): Optimizes for a combination of layout
area and the number of crossings.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective_AREA =
R"doc(AREA: Optimizes for the total area of the layout, aiming to minimize
the space required for the design.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective_CROSSINGS = R"doc(CROSSINGS: Optimizes for the number of wire crossings in the layout.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective_CUSTOM =
R"doc(CUSTOM: Allows for a user-defined cost objective, enabling
optimization based on specific criteria outside the predefined
options.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_cost_objective_WIRES =
R"doc(WIRES: Optimizes for the number of wire segments in the layout,
reducing the delay and increasing throughput.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_effort_mode =
R"doc(The `effort_mode` enum defines different levels of computational
effort for generating and exploring search space graphs for during the
graph-oriented layout design process. Each mode varies in the number
of search space graphs generated and the strategies employed,
balancing between runtime efficiency and the likelihood of finding
optimal solutions.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_effort_mode_HIGHEST_EFFORT =
R"doc(HIGHEST_EFFORT mode builds upon HIGH_EFFORT by duplicating the 12
search space graphs for different cost objectives. If the cost
objective involves layout area, number of crossings, number of wire
segments, or a combination of area and crossings, a total of 48 search
space graphs are generated. For a custom cost objective, an additional
12 graphs are created, resulting in 60 graphs in total. This mode
provides the best guarantee of finding optimal solutions but
significantly increases runtime.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_effort_mode_HIGH_EFFICIENCY =
R"doc(HIGH_EFFICIENCY mode generates 2 search space graphs. This option
minimizes runtime but may not always yield the optimal results.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_effort_mode_HIGH_EFFORT =
R"doc(HIGH_EFFORT mode generates 12 search space graphs using various fanout
substitution strategies, PI placements, and other parameters. This
wider exploration increases the chance of finding optimal layouts but
also extends runtime. When a solution is found in any graph, its cost
is used to prune the remaining graphs.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_enable_multithreading =
R"doc(BETA feature: Flag to enable or disable multithreading during the
execution of the layout design algorithm.

When set to `true`, the algorithm will utilize multiple threads to
process different search space graphs in parallel, improving
performance by distributing the workload across available CPU cores.
If set to `false`, the algorithm will run sequentially on a single
thread.

Only recommended for `HIGH_EFFORT` and `HIGHEST_EFFORT` modes and
complex networks (> 100 nodes).

Enabling multithreading can significantly speed up the algorithm,
especially when using multiple search space graphs and dealing with
complex networks, by concurrently expanding them. However, it may
introduce additional overhead for thread synchronization and can
increase memory usage. It is therefore not recommended for small input
networks.

Default value: `false`)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_mode = R"doc(The effort mode used. Defaults to HIGH_EFFORT.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_num_vertex_expansions =
R"doc(Number of expansions for each vertex that should be explored. For each
partial layout, `num_vertex_expansions` positions will be checked for
the next node/gate to be placed. A lower value requires less runtime,
but the layout might have a larger area or it could also lead to no
solution being found. A higher value might lead to better solutions,
but also requires more runtime. Defaults to 4 expansions for each
vertex.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_planar =
R"doc(Disable the creation of crossings during layout generation. If set to
true, gates will only be placed if a crossing-free wiring is found.
Defaults to false.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_return_first =
R"doc(Return the first found layout, which might still have a high cost but
can be found fast.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_timeout = R"doc(Timeout limit (in ms).)doc";

static const char *__doc_fiction_graph_oriented_layout_design_params_verbose = R"doc(Verbosity.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats =
R"doc(This struct stores statistics about the graph-oriented layout design
process.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_duration = R"doc(Runtime of the graph-oriented layout design process.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_num_crossings = R"doc(Number of crossings.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_num_gates = R"doc(Number of gates.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_num_wires = R"doc(Number of wires.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_x_size = R"doc(Layout width.)doc";

static const char *__doc_fiction_graph_oriented_layout_design_stats_y_size = R"doc(Layout height.)doc";

static const char *__doc_fiction_gray_code_iterator =
R"doc(An iterator type that iterates over Gray code representations for
decimal numbers.

The `gray_code_iterator` class provides an iterator for generating
Gray code representations for a range of decimal numbers. It starts
from a specified number and produces Gray codes in ascending order
based on bitwise XOR operations.)doc";

static const char *__doc_fiction_gray_code_iterator_binary_to_gray =
R"doc(Converts the current decimal number into its corresponding Gray code
representation.

This function operates on the current decimal number and produces its
Gray code representation using the bitwise XOR operation. Gray code is
a binary numeral system in which two successive values differ in only
one bit.

The result is stored in the 'current_gray_code' variable.)doc";

static const char *__doc_fiction_gray_code_iterator_current_gray_code = R"doc(Current Gray Code.)doc";

static const char *__doc_fiction_gray_code_iterator_current_iteration = R"doc(Current number (i.e., current iteration number).)doc";

static const char *__doc_fiction_gray_code_iterator_gray_code_iterator =
R"doc(Constructs a Gray Code Iterator with a specified starting number.

Constructs a `gray_code_iterator` that generates Gray codes for
decimal numbers starting from the given `start` number.

Parameter ``start``:
    The starting decimal number for the iterator.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_add =
R"doc(Addition operator. Computes the Gray code of the current iterator plus
the given integer.

Parameter ``m``:
    The amount of Gray codes to skip.

Returns:
    Iterator of the current iterator plus the given integer.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_array =
R"doc(Subscript operator. Returns the Gray code at a specific position in
the iteration range.

Parameter ``index``:
    The position in the iteration range.

Returns:
    The Gray code at the specified position.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_assign =
R"doc(Assignment operator. Sets the current number to the given integer.

Parameter ``m``:
    The number to set.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_dec =
R"doc(Prefix decrement operator. Sets the previous Gray code.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_dec_2 =
R"doc(Postfix decrement operator. Sets the previous Gray Code.

Returns:
    Copy of `this` before decrementing.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_eq =
R"doc(Equality comparison operator. Compares the current iterator with
another iterator.

Parameter ``other``:
    The iterator to compare with.

Returns:
    `true` if the current iterator is equal to the other iterator,
    `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_eq_2 =
R"doc(Equality operator. Compares the current number with the given integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is equal to `m`, `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_ge =
R"doc(Greater-or-equal-than operator. Compares the current number with the
given integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is greater than or equal to `m`,
    `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_gt =
R"doc(Greater-than operator. Compares the current number with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is greater than `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_iadd =
R"doc(Addition assignment operator. Iterator is increased by given number.

Parameter ``m``:
    The amount of Gray codes to skip.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_inc =
R"doc(Prefix increment operator. Sets the number and the corresponding Gray
code.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_inc_2 =
R"doc(Postfix increment operator. Sets the next Gray Code.

Returns:
    Copy of `this` before incrementing.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_isub =
R"doc(Subtraction assignment operator. Sets a previous Gray code.

Parameter ``m``:
    The amount of Gray codes to skip.

Returns:
    Reference to `this`.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_le =
R"doc(Less-than or equal-to comparison operator. Compares the current
iterator with another iterator.

Parameter ``other``:
    The iterator to compare with.

Returns:
    `true` if the current iterator is less than or equal to the other
    iterator, `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_le_2 =
R"doc(Less-or-equal-than operator. Compares the current number with the
given integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is less than or equal to `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_lt =
R"doc(Less-than comparison operator. Compares the current iterator with
another iterator.

Parameter ``other``:
    The iterator to compare with.

Returns:
    `true` if the current iterator is less than the other iterator,
    `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_lt_2 =
R"doc(Less-than operator. Compares the current number with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is less than `m`, `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_mul =
R"doc(Dereference operator. Returns a reference to the Gray code of the
current iteration.

Returns:
    Reference to the current Gray code.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_ne =
R"doc(Inequality comparison operator. Compares the current iterator with
another iterator.

Parameter ``other``:
    The iterator to compare with.

Returns:
    `true` if the current iterator is not equal to the other iterator,
    `false` otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_ne_2 =
R"doc(Inequality operator. Compares the current number with the given
integer.

Parameter ``m``:
    Integer to compare with.

Returns:
    `true` if the current number is not equal to `m`, `false`
    otherwise.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_sub =
R"doc(Subtraction operator to calculate the difference between two
gray_code_iterators.

This operator calculates the difference between the current iterator
and another gray_code_iterator provided as input. The result is
returned as an int64_t representing the number of positions between
the iterators.

Parameter ``other``:
    The gray_code_iterator to subtract from the current iterator.

Returns:
    The difference between the current iterator and the input iterator
    as int64_t.)doc";

static const char *__doc_fiction_gray_code_iterator_operator_sub_2 =
R"doc(Subtraction operator. Computes the Gray code of the current iterator
minus the given integer.

Parameter ``m``:
    The amount of Gray codes to skip.

Returns:
    Iterator of the current iterator minus the given integer.)doc";

static const char *__doc_fiction_gray_code_iterator_start_number = R"doc(Start number of the iteration.)doc";

static const char *__doc_fiction_ground_state_space =
R"doc(The purely constructive *Ground State Space* algorithm is the key
ingredient of the *ClusterComplete* exact SiDB simulator that lifts
exact SiDB simulation to permit multiple gates in connection. It uses
iterative "loop until fixpoint" concepts to prune the simulation
search space for not only a flat layout of SiDBs, but rather
generalizes, and lifts the physically informed space pruning technique
introduced with *QuickExact* to higher order, allowing *Ground State
Space* to prune multiset charge state configurations at any level in a
cluster hierarchy.

The role of the cluster hierarchy is to rank interactions between
groups, or clusters of SiDBs that together make up the whole layout,
such that the variation in electrostatic potential under different
charge state assignments is highest between the children clusters of
clusters that low in the hierarchy. Thereby, the structure allows us
to consider the most charge state assignment-dependent interaction in
a more detailed physically informed space pruning analysis, enabling
high pruning efficacy for the few pruning tests (with respect to the
exponential search space).

Starting at a clustering of all singleton clusters, the charge spaces,
ie. a set of multiset charge configurations (initially { {{-}}, {{0}},
{{+}} } or omitting the singleton multiset {{+}} in the case of base 2
pre-simulation), are pruned iteratively through potential bound
analysis. Through merges, ie., replacing a set of children in the
clustering with their parent, we may inspect the most crucially
dependant interactions in the layout separately. The procedure
finishes when the charge spaces have been folded all the way up to the
top cluster, parent of all, which then contains all information
resulting from the construction. *ClusterComplete*, without much
trickery, now simply unfolds this result, allowing simulation of
problems that were previously seen as astronomical, due to the (base 2
or 3) exponential growth in the number of SiDBs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to construct the *Ground State Space* of.

Parameter ``params``:
    The parameters that *Ground State Space* will use throughout the
    construction. The physical parameters that *Ground State Space*
    will use to prune simulation search space are stored in there. In
    particular, the user may configure parameters that decide limits
    on the problem sizes of pruning by validity witness partitioning.
    By default, these are set to avoid runtimes from being affected,
    as these sub-problems may scale factorially. Thereby, these
    parameters are especially useful for large simulation problems
    that could benefit from extra intensive pruning before
    *ClusterComplete* unfolds the constructed hierarchical charge
    space.

Returns:
    The results of the construction, which include the top cluster
    which parents all other clusters, and thereby contains the charge
    spaces of each cluster.)doc";

static const char *__doc_fiction_ground_state_space_params = R"doc(The set of parameters used in the *Ground State Space* construction.)doc";

static const char *__doc_fiction_ground_state_space_params_num_overlapping_witnesses_limit_gss =
R"doc(The complexity is of validity witness partitioning bounded by a
factorial in the number of overlapping witnesses. This parameter thus
allows the validity witness partitioning procedure to perform the
reduction to overlapping witnesses for larger cluster sizes that could
be runtime-impairing, then limiting specifically the length of the
input to the factorial call. As above, the defaulted value ensures no
hindrance in runtimes.)doc";

static const char *__doc_fiction_ground_state_space_params_simulation_parameters =
R"doc(The physical parameters that *Ground State Space* will use to prune
the simulation search space.)doc";

static const char *__doc_fiction_ground_state_space_params_witness_partitioning_cluster_size_limit =
R"doc(This specifies the maximum cluster size for which *Ground State Space*
will solve an NP-complete sub-problem exhaustively. The sets of SiDBs
that witness local population stability for each respective charge
state may be partitioned into disjoint sets such that the number of
required witnesses for each respective charge state is satisfied. If
no such partition exists, the multiset charge configuration associated
with the requirements may be rejected. The defaulted value is chosen
such that some extra pruning may be performed, while the impact on the
runtime remains negligible. Validity witness partitioning parameters
are relevant for large simulation problems.)doc";

static const char *__doc_fiction_ground_state_space_results =
R"doc(This struct is used to store the results of the *Ground State Space*
construction.)doc";

static const char *__doc_fiction_ground_state_space_results_maximum_top_level_multisets =
R"doc(The maximum size of the charge space of the top cluster, given the
simulation base, can be inferred by the \"stars and bars\"
combinatorial idea: the solution to this analogous problem determines
the maximum amount of multisets of size :math:`N` (where :math:`N` is
the number of SiDBs in the layout, and therefore in the top cluster)
for the given base :math:`b`. In particular, the analogy is as
follows: any such multiset can be seen as :math:`N` stars and :math:`b
- 1` bars separating those stars. Then, the :math:`b` partitions
forming from these :math:`b - 1` separators each have a respective
size, adding up to :math:`N`. Therefore each partition is associated
with an amount of one of the charge states of the multiset. Now we may
compute total number of possible multisets for the top cluster as the
number of combinations of :math:`N` stars and :math:`b - 1` bars.
Hence this is computed with the following combinatorial formula:
:math:`\binom{N + b - 1}{b - 1}`.)doc";

static const char *__doc_fiction_ground_state_space_results_projector_state_count =
R"doc(The total number of distinct projector states is counted. At each
merge, the projector states in charge space compositions in the charge
spaces of the clusters to merge are locked in the final construct, and
can therefore be counted. This may be used to estimate the time it
would take *ClusterComplete* to unfold the hierarchy.)doc";

static const char *__doc_fiction_ground_state_space_results_report =
R"doc(Report *Ground State Space* statistics. A quick heuristic to assess
the quality of the pruning is captured by the size of the charge space
of the top cluster, which depends on the charge spaces of all clusters
below it.

Parameter ``os``:
    The output stream to write to (default: standard output).

Returns:
    Prints the runtime and the number of pruned top level multisets
    versus the total amount possible.)doc";

static const char *__doc_fiction_ground_state_space_results_runtime = R"doc(The runtime of the construction is stored.)doc";

static const char *__doc_fiction_ground_state_space_results_top_cluster =
R"doc(The top cluster is the root of the cluster hierarchy. It therefore
allows access to the entire cluster hierarchy, including the charge
spaces of each cluster.)doc";

static const char *__doc_fiction_has_above = R"doc()doc";

static const char *__doc_fiction_has_assign_charge_state = R"doc()doc";

static const char *__doc_fiction_has_assign_sidb_defect = R"doc()doc";

static const char *__doc_fiction_has_below = R"doc()doc";

static const char *__doc_fiction_has_cardinal_operations = R"doc()doc";

static const char *__doc_fiction_has_create_dot = R"doc()doc";

static const char *__doc_fiction_has_dimensions = R"doc()doc";

static const char *__doc_fiction_has_east = R"doc()doc";

static const char *__doc_fiction_has_elevation_operations = R"doc()doc";

static const char *__doc_fiction_has_foreach_adjacent_coordinate = R"doc()doc";

static const char *__doc_fiction_has_foreach_adjacent_opposite_coordinates = R"doc()doc";

static const char *__doc_fiction_has_foreach_adjacent_opposite_tiles = R"doc()doc";

static const char *__doc_fiction_has_foreach_adjacent_tile = R"doc()doc";

static const char *__doc_fiction_has_foreach_cell = R"doc()doc";

static const char *__doc_fiction_has_foreach_coordinate = R"doc()doc";

static const char *__doc_fiction_has_foreach_incoming_clocked_zone = R"doc()doc";

static const char *__doc_fiction_has_foreach_outgoing_clocked_zone = R"doc()doc";

static const char *__doc_fiction_has_foreach_real_pi = R"doc()doc";

static const char *__doc_fiction_has_foreach_sidb_defect = R"doc()doc";

static const char *__doc_fiction_has_foreach_tile = R"doc()doc";

static const char *__doc_fiction_has_get_charge_state = R"doc()doc";

static const char *__doc_fiction_has_get_functional_implementations = R"doc()doc";

static const char *__doc_fiction_has_get_gate_ports = R"doc()doc";

static const char *__doc_fiction_has_get_layout_name = R"doc()doc";

static const char *__doc_fiction_has_get_real_pi = R"doc()doc";

static const char *__doc_fiction_has_get_sidb_defect = R"doc()doc";

static const char *__doc_fiction_has_high_degree_fanin_nodes =
R"doc(Checks if a given network exceeds a given fanin threshold in any of
its nodes.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    Network to check.

Parameter ``threshold``:
    Maximum number of legal fanins.

Returns:
    `true` iff any node in `ntk` exceeds `threshold` fanins.)doc";

static const char *__doc_fiction_has_incoming_primary_input =
R"doc(Checks if a given node in a given network has fanins that are primary
inputs.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    Network to check in.

Parameter ``n``:
    Node to check.

Returns:
    `true` iff any of `n`'s fanins are primary inputs.)doc";

static const char *__doc_fiction_has_is_and3 = R"doc()doc";

static const char *__doc_fiction_has_is_and_xor = R"doc()doc";

static const char *__doc_fiction_has_is_buf = R"doc()doc";

static const char *__doc_fiction_has_is_dot = R"doc()doc";

static const char *__doc_fiction_has_is_empty = R"doc()doc";

static const char *__doc_fiction_has_is_empty_cell = R"doc()doc";

static const char *__doc_fiction_has_is_empty_tile = R"doc()doc";

static const char *__doc_fiction_has_is_fanout = R"doc()doc";

static const char *__doc_fiction_has_is_gamble = R"doc()doc";

static const char *__doc_fiction_has_is_gate_tile = R"doc()doc";

static const char *__doc_fiction_has_is_ge = R"doc()doc";

static const char *__doc_fiction_has_is_gt = R"doc()doc";

static const char *__doc_fiction_has_is_incoming_clocked = R"doc()doc";

static const char *__doc_fiction_has_is_inv = R"doc()doc";

static const char *__doc_fiction_has_is_le = R"doc()doc";

static const char *__doc_fiction_has_is_lt = R"doc()doc";

static const char *__doc_fiction_has_is_mux = R"doc()doc";

static const char *__doc_fiction_has_is_nand = R"doc()doc";

static const char *__doc_fiction_has_is_nor = R"doc()doc";

static const char *__doc_fiction_has_is_obstructed_connection = R"doc()doc";

static const char *__doc_fiction_has_is_obstructed_coordinate = R"doc()doc";

static const char *__doc_fiction_has_is_onehot = R"doc()doc";

static const char *__doc_fiction_has_is_or_and = R"doc()doc";

static const char *__doc_fiction_has_is_outgoing_clocked = R"doc()doc";

static const char *__doc_fiction_has_is_po = R"doc()doc";

static const char *__doc_fiction_has_is_real_pi = R"doc()doc";

static const char *__doc_fiction_has_is_virtual_pi = R"doc()doc";

static const char *__doc_fiction_has_is_wire_tile = R"doc()doc";

static const char *__doc_fiction_has_is_xnor = R"doc()doc";

static const char *__doc_fiction_has_is_xor_and = R"doc()doc";

static const char *__doc_fiction_has_north = R"doc()doc";

static const char *__doc_fiction_has_north_east = R"doc()doc";

static const char *__doc_fiction_has_north_west = R"doc()doc";

static const char *__doc_fiction_has_num_real_pis = R"doc()doc";

static const char *__doc_fiction_has_num_virtual_pis = R"doc()doc";

static const char *__doc_fiction_has_ordinal_operations = R"doc()doc";

static const char *__doc_fiction_has_post_layout_optimization = R"doc()doc";

static const char *__doc_fiction_has_set_layout_name = R"doc()doc";

static const char *__doc_fiction_has_south = R"doc()doc";

static const char *__doc_fiction_has_south_east = R"doc()doc";

static const char *__doc_fiction_has_south_west = R"doc()doc";

static const char *__doc_fiction_has_synchronization_elements = R"doc()doc";

static const char *__doc_fiction_has_update_ranks = R"doc()doc";

static const char *__doc_fiction_has_west = R"doc()doc";

static const char *__doc_fiction_hash_combine =
R"doc(A recursive `hash_combine` implementation from
https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-
values-in-c0x

Overrides the passed seed.

Template parameter ``T``:
    Type to hash.

Template parameter ``Rest``:
    Parameter pack.

Parameter ``seed``:
    Hashing seed. This value is overridden with the hash value.

Parameter ``v``:
    Value to hash next.

Parameter ``rest``:
    Remaining values to hash.)doc";

static const char *__doc_fiction_heuristic_sidb_simulation_engine = R"doc(Selector exclusively for heuristic SiDB simulation engines.)doc";

static const char *__doc_fiction_heuristic_sidb_simulation_engine_QUICKSIM =
R"doc(*QuickSim* is a heuristic simulation engine that only requires
polynomial runtime.)doc";

static const char *__doc_fiction_hexagonal_layout =
R"doc(A layout type that utilizes offset coordinates to represent a
hexagonal grid. Its faces are organized in an offset coordinate system
as provided. Hexagons can be in the pointy_top_hex or flat_top_hex
orientation. Based on that, two respectively possible coordinate
systems emerge accordingly: odd_row_hex and even_row_hex for pointy
tops and odd_column_hex and even_column_hex for flat tops. All are
sketched in ASCII above.

Other representations would be using cube or axial coordinates for
instance, but since we want the layouts to be rectangular-ish, offset
coordinates make the most sense here.

https://www.redblobgames.com/grids/hexagons/ is a wonderful resource
on the topic.

Template parameter ``OffsetCoordinateType``:
    The coordinate implementation to be used. Offset coordinates are
    required.

Template parameter ``HexagonalCoordinateSystem``:
    One of the following: odd_row_hex, even_row_hex, odd_column_hex,
    even_column_hex.

Template parameter ``CubeCoordinateType``:
    Internally, cube coordinates are needed for certain algorithms or
    calculations.)doc";

static const char *__doc_fiction_hexagonal_layout_above =
R"doc(Returns the coordinate that is directly above a given coordinate `c`,
i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
is already at maximum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose above counterpart is desired.

Returns:
    Coordinate directly above `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_adjacent_coordinates =
R"doc(Returns a container that contains all coordinates that are adjacent to
a given one. Thereby, cardinal and ordinal directions are being
considered, i.e., the container will contain all coordinates `ac` for
which `is_adjacent(c, ac)` returns `true`.

Coordinates that are outside of the layout bounds are not considered.
Thereby, the size of the returned container is at most 6.

Parameter ``c``:
    Coordinate whose adjacent ones are desired.

Returns:
    A container that contains all of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_adjacent_opposite_coordinates =
R"doc(Returns a container that contains all coordinates pairs of opposing
adjacent coordinates with respect to a given one. In this hexagonal
layout, the container content depends on the hexagonal orientation.

In case of a pointy_top_hex orientation, the container will contain
(`east(c)`, `west(c)`), (`north_east(c)`, `south_west(c)`),
(`north_west(c)`, `south_east(c)`). In case of a flat_top_hex
orientation, the container will contain (`north(c)`, `south(c)`),
(`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
instead.

This function comes in handy when straight lines on the layout are to
be examined.

Coordinates outside of the layout bounds are not being considered.

Parameter ``c``:
    Coordinate whose opposite ones are desired.

Returns:
    A container that contains pairs of `c`'s opposing coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_area =
R"doc(Returns the layout's number of faces depending on the coordinate type.

Returns:
    Area of layout.)doc";

static const char *__doc_fiction_hexagonal_layout_below =
R"doc(Returns the coordinate that is directly below a given coordinate `c`,
i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
is already at minimum, `c` is returned instead.

Parameter ``c``:
    Coordinate whose below counterpart is desired.

Returns:
    Coordinate directly below `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_hexagonal_layout_coord =
R"doc(Creates and returns a coordinate in the layout from the given x-, y-,
and z-values.

@note This function is equivalent to calling `OffsetCoordinateType(x,
y, z)`.

Template parameter ``X``:
    x-type.

Template parameter ``Y``:
    y-type.

Template parameter ``Z``:
    z-type.

Parameter ``x``:
    x-value.

Parameter ``y``:
    y-value.

Parameter ``z``:
    z-value.

Returns:
    A coordinate in the layout of type `OffsetCoordinateType`.)doc";

static const char *__doc_fiction_hexagonal_layout_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout between
`start` and `stop`. If no values are provided, all coordinates in the
layout will be included. The returned iterator range points to the
first and last coordinate, respectively. The range object can be used
within a for-each loop. Incrementing the iterator is equivalent to
nested for loops in the order z, y, x. Consequently, the iteration
will happen inside out, i.e., x will be iterated first, then y, then
z.

Parameter ``start``:
    First coordinate to include in the range of all coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all
    coordinates.

Returns:
    An iterator range from `start` to `stop`. If they are not
    provided, the first/last coordinate is used as a default.)doc";

static const char *__doc_fiction_hexagonal_layout_east =
R"doc(Returns the coordinate that is directly adjacent in eastern direction
of a given coordinate `c`, i.e., the face whose x-dimension is higher
by 1. If `c`'s x-dimension is already at maximum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose eastern counterpart is desired.

Returns:
    Coordinate adjacent and east of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_eastern_border_of =
R"doc(Returns the coordinate with the same y and z values as a given
coordinate but that is located at the layout's eastern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The eastern border equivalent of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_foreach_adjacent_coordinate =
R"doc(Applies a function to all coordinates adjacent to a given one in
accordance with `adjacent_coordinates`. Thereby, cardinal and ordinal
directions are being considered, i.e., the given function is applied
to all coordinates ac for which `is_adjacent(c, ac)` returns `true`.

Coordinates that are outside of the layout bounds are not considered.
Thereby, at most 6 coordinates are touched.

Template parameter ``Fn``:
    Functor type.

Parameter ``c``:
    Coordinate whose adjacent ones are desired.

Parameter ``fn``:
    Functor to apply to each of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_foreach_adjacent_opposite_coordinates =
R"doc(Applies a function to all opposing coordinate pairs adjacent to a
given one. In this hexagonal layout, the function application depends
on the hexagonal orientation.

In case of a pointy_top_hex orientation, the function will apply to
(`east(c)`, `west(c)`), (`north_east(c)`, `south_west(c)`),
(`north_west(c)`, `south_east(c)`). In case of a flat_top_hex
orientation, the function will apply to (`north(c)`, `south(c)`),
(`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
instead.

This function comes in handy when straight lines on the layout are to
be examined.

Coordinates outside of the layout bounds are not being considered.

Template parameter ``Fn``:
    Functor type.

Parameter ``c``:
    Coordinate whose opposite adjacent ones are desired.

Parameter ``fn``:
    Functor to apply to each of `c`'s opposite adjacent coordinate
    pairs.)doc";

static const char *__doc_fiction_hexagonal_layout_foreach_coordinate =
R"doc(Applies a function to all coordinates accessible in the layout between
`start` and `stop`. The iteration order is the same as for the
coordinates function.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``fn``:
    Functor to apply to each coordinate in the range.

Parameter ``start``:
    First coordinate to include in the range of all coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all
    coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_foreach_ground_coordinate =
R"doc(Applies a function to all coordinates accessible in the layout's
ground layer between `start` and `stop`. The iteration order is the
same as for the ground_coordinates function.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``fn``:
    Functor to apply to each coordinate in the range.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all ground
    coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_ground_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout's ground
layer between `start` and `stop`. The iteration order is the same as
for the coordinates function but without the z dimension.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate (exclusive) to include in the range of all ground
    coordinates.

Returns:
    An iterator range from `start` to `stop`. If they are not
    provided, the first/last coordinate in the ground layer is used as
    a default.)doc";

static const char *__doc_fiction_hexagonal_layout_hexagonal_layout =
R"doc(Standard constructor. The given aspect ratio points to the highest
possible coordinate in the layout. That means in the even_column_hex
ASCII layout representation above `ar = (3,2)`. Consequently, with `ar
= (0,0)`, the layout has exactly one coordinate.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

static const char *__doc_fiction_hexagonal_layout_hexagonal_layout_2 = R"doc()doc";

static const char *__doc_fiction_hexagonal_layout_hexagonal_layout_storage = R"doc()doc";

static const char *__doc_fiction_hexagonal_layout_hexagonal_layout_storage_dimension = R"doc()doc";

static const char *__doc_fiction_hexagonal_layout_hexagonal_layout_storage_hexagonal_layout_storage = R"doc()doc";

static const char *__doc_fiction_hexagonal_layout_is_above =
R"doc(Returns `true` iff coordinate `c2` is directly above coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly above `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_adjacent_elevation_of =
R"doc(Similar to is_adjacent_of but also considers `c1`'s elevation, i.e.,
if `c2` is adjacent to `above(c1)` or `below(c1)`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is either adjacent of `c1` or `c1`'s elevations.)doc";

static const char *__doc_fiction_hexagonal_layout_is_adjacent_of =
R"doc(Returns `true` iff coordinate `c2` is either north, north-east, east,
south-east, south, south-west, west, or north-west of coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly adjacent to `c1` in one of the six
    different ordinal directions possible for the layout's hexagonal
    orientation.)doc";

static const char *__doc_fiction_hexagonal_layout_is_at_any_border =
R"doc(Returns whether the given coordinate is located at any of the layout's
borders where x or y are either minimal or maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at any of the layout's borders.)doc";

static const char *__doc_fiction_hexagonal_layout_is_at_eastern_border =
R"doc(Returns whether the given coordinate is located at the layout's
eastern border where x is maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's northern border.)doc";

static const char *__doc_fiction_hexagonal_layout_is_at_northern_border =
R"doc(Returns whether the given coordinate is located at the layout's
northern border where y is minimal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's northern border.)doc";

static const char *__doc_fiction_hexagonal_layout_is_at_southern_border =
R"doc(Returns whether the given coordinate is located at the layout's
southern border where y is maximal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's southern border.)doc";

static const char *__doc_fiction_hexagonal_layout_is_at_western_border =
R"doc(Returns whether the given coordinate is located at the layout's
western border where x is minimal.

Parameter ``c``:
    Coordinate to check for border location.

Returns:
    `true` iff `c` is located at the layout's western border.)doc";

static const char *__doc_fiction_hexagonal_layout_is_below =
R"doc(Returns `true` iff coordinate `c2` is directly below coordinate `c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly below `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_crossing_layer =
R"doc(Returns whether the given coordinate is located in a crossing layer
where z is not minimal.

Parameter ``c``:
    Coordinate to check for elevation.

Returns:
    `true` iff `c` is in a crossing layer.)doc";

static const char *__doc_fiction_hexagonal_layout_is_east_of =
R"doc(Returns `true` iff coordinate `c2` is directly east of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly east of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_eastwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere east of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere east of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_ground_layer =
R"doc(Returns whether the given coordinate is located in the ground layer
where z is minimal.

Parameter ``c``:
    Coordinate to check for elevation.

Returns:
    `true` iff `c` is in ground layer.)doc";

static const char *__doc_fiction_hexagonal_layout_is_in_even_column =
R"doc(Checks if the given coordinate is located in a column with an even
index.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff `c` is located in an even column.)doc";

static const char *__doc_fiction_hexagonal_layout_is_in_even_row =
R"doc(Checks if the given coordinate is located in a row with an even index.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff `c` is located in an even row.)doc";

static const char *__doc_fiction_hexagonal_layout_is_in_odd_column =
R"doc(Checks if the given coordinate is located in a column with an odd
index.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff `c` is located in an odd column.)doc";

static const char *__doc_fiction_hexagonal_layout_is_in_odd_row =
R"doc(Checks if the given coordinate is located in a row with an odd index.

Parameter ``c``:
    Coordinate to check.

Returns:
    `true` iff `c` is located in an odd row.)doc";

static const char *__doc_fiction_hexagonal_layout_is_north_of =
R"doc(Returns `true` iff coordinate `c2` is directly north of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly north of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_northwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere north of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere north of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_south_of =
R"doc(Returns `true` iff coordinate `c2` is directly south of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly south of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_southwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere south of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere south of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_west_of =
R"doc(Returns `true` iff coordinate `c2` is directly west of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is directly west of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_westwards_of =
R"doc(Returns `true` iff coordinate `c2` is somewhere west of coordinate
`c1`.

Parameter ``c1``:
    Base coordinate.

Parameter ``c2``:
    Coordinate to test for its location in relation to `c1`.

Returns:
    `true` iff `c2` is somewhere west of `c1`.)doc";

static const char *__doc_fiction_hexagonal_layout_is_within_bounds =
R"doc(Returns whether the given coordinate is located within the layout
bounds.

Parameter ``c``:
    Coordinate to check for boundary.

Returns:
    `true` iff `c` is located within the layout bounds.)doc";

static const char *__doc_fiction_hexagonal_layout_north =
R"doc(Returns the coordinate that is directly adjacent in northern direction
of a given coordinate `c`, i.e., the face whose y-dimension is lower
by 1. If `c`'s y-dimension is already at minimum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose northern counterpart is desired.

Returns:
    Coordinate adjacent and north of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_north_east =
R"doc(Returns the coordinate that is located in north-eastern direction of a
given coordinate `c`. Depending on the hexagonal orientation of the
layout, the dimension values of the returned coordinate may differ.

Parameter ``c``:
    Coordinate whose north-eastern counterpart is desired.

Returns:
    Coordinate directly north-eastern of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_north_west =
R"doc(Returns the coordinate that is located in north-western direction of a
given coordinate `c`. Depending on the hexagonal orientation of the
layout, the dimension values of the returned coordinate may differ.

Parameter ``c``:
    Coordinate whose north-western counterpart is desired.

Returns:
    Coordinate directly north-western of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_northern_border_of =
R"doc(Returns the coordinate with the same x and z values as a given
coordinate but that is located at the layout's northern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The northern border equivalent of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_resize =
R"doc(Updates the layout's dimensions, effectively resizing it.

Parameter ``ar``:
    New aspect ratio.)doc";

static const char *__doc_fiction_hexagonal_layout_south =
R"doc(Returns the coordinate that is directly adjacent in southern direction
of a given coordinate `c`, i.e., the face whose y-dimension is higher
by 1. If `c`'s y-dimension is already at maximum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose southern counterpart is desired.

Returns:
    Coordinate adjacent and south of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_south_east =
R"doc(Returns the coordinate that is located in south-eastern direction of a
given coordinate `c`. Depending on the hexagonal orientation of the
layout, the dimension values of the returned coordinate may differ.

Parameter ``c``:
    Coordinate whose south-eastern counterpart is desired.

Returns:
    Coordinate directly south-eastern of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_south_west =
R"doc(Returns the coordinate that is located in south-western direction of a
given coordinate `c`. Depending on the hexagonal orientation of the
layout, the dimension values of the returned coordinate may differ.

Parameter ``c``:
    Coordinate whose south-western counterpart is desired.

Returns:
    Coordinate directly south-western of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_southern_border_of =
R"doc(Returns the coordinate with the same x and z values as a given
coordinate but that is located at the layout's southern border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The southern border equivalent of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_strg = R"doc()doc";

static const char *__doc_fiction_hexagonal_layout_to_cube_coordinate =
R"doc(Converts an offset coordinate to a cube coordinate.

This implementation is adapted from
https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp

Parameter ``offset_coord``:
    Offset coordinate to convert.

Returns:
    Cube coordinate representing `offset_coord` in the layout's
    hexagonal orientation.)doc";

static const char *__doc_fiction_hexagonal_layout_to_offset_coordinate =
R"doc(Converts a cube coordinate to an offset coordinate.

This implementation is adapted from
https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp

Parameter ``cube_coord``:
    Cube coordinate to convert.

Returns:
    Offset coordinate representing `cube_coord` in the layout's
    hexagonal orientation.)doc";

static const char *__doc_fiction_hexagonal_layout_west =
R"doc(Returns the coordinate that is directly adjacent in western direction
of a given coordinate `c`, i.e., the face whose x-dimension is lower
by 1. If `c`'s x-dimension is already at minimum, `c` is returned
instead.

Parameter ``c``:
    Coordinate whose western counterpart is desired.

Returns:
    Coordinate adjacent and west of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_western_border_of =
R"doc(Returns the coordinate with the same y and z values as a given
coordinate but that is located at the layout's western border.

Parameter ``c``:
    Coordinate whose border counterpart is desired.

Returns:
    The western border equivalent of `c`.)doc";

static const char *__doc_fiction_hexagonal_layout_x =
R"doc(Returns the layout's x-dimension, i.e., returns the biggest x-value
that still belongs to the layout.

Returns:
    x-dimension.)doc";

static const char *__doc_fiction_hexagonal_layout_y =
R"doc(Returns the layout's y-dimension, i.e., returns the biggest y-value
that still belongs to the layout.

Returns:
    y-dimension.)doc";

static const char *__doc_fiction_hexagonal_layout_z =
R"doc(Returns the layout's z-dimension, i.e., returns the biggest z-value
that still belongs to the layout.

Returns:
    z-dimension.)doc";

static const char *__doc_fiction_hexagonalization =
R"doc(Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even
row clocked layout suitable for SiDBs by remapping all gates and wires
as originally proposed in \"Scalable Physical Design for Silicon
Dangling Bond Logic: How a 45° Turn Prevents the Reinvention of the
Wheel\" by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2023
(https://ieeexplore.ieee.org/document/10231278).

Template parameter ``HexLyt``:
    Even-row hexagonal gate-level layout return type.

Template parameter ``CartLyt``:
    Input Cartesian gate-level layout type.

Parameter ``lyt``:
    2DDWave-clocked Cartesian gate-level layout to hexagonalize.

Returns:
    Hexagonal representation of the Cartesian layout.)doc";

static const char *__doc_fiction_hexagonalization_stats = R"doc(This struct stores statistics about the hexagonalization process.)doc";

static const char *__doc_fiction_hexagonalization_stats_duration = R"doc(Runtime of the hexagonalization process.)doc";

static const char *__doc_fiction_hexagonalization_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_high_degree_fanin_exception =
R"doc(Exception class that can be thrown if some network exceeds a legal
number of fanins.)doc";

static const char *__doc_fiction_high_degree_fanin_exception_high_degree_fanin_exception = R"doc()doc";

static const char *__doc_fiction_horizontal_shift_cartesian =
R"doc(\verbatim +-------+ | | | | | | +---+---+---+ | | | | | | +-------+
\endverbatim)doc";

static const char *__doc_fiction_initialize_distance_map =
R"doc(This function fully initializes a `distance_map` for a given layout
and distance functor. It computes the distances between all pairs of
coordinates in the layout and stores them in the distance map for
quick subsequent access.

This function performs :math:`\mathcal{O}(|L|^2)` distance
computations, where :math:`|L|` is the number of coordinates in the
layout.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.

Parameter ``lyt``:
    Layout to compute distances for.

Parameter ``dist_fn``:
    Distance functor to apply to all pairs of coordinates in `lyt`.

Returns:
    Fully initialized `distance_map` for `lyt`.)doc";

static const char *__doc_fiction_initialize_sparse_distance_map =
R"doc(This function fully initializes a `sparse_distance_map` for a given
layout and distance functor. It computes the distances between all
pairs of coordinates in the layout and stores them in the distance map
for quick subsequent access.

This function performs :math:`\mathcal{O}(|L|^2)` distance
computations, where :math:`|L|` is the number of coordinates in the
layout.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.

Parameter ``lyt``:
    Layout to compute distances for.

Parameter ``dist_fn``:
    Distance functor to apply to all pairs of coordinates in `lyt`.

Returns:
    Fully initialized `sparse_distance_map` for `lyt`.)doc";

static const char *__doc_fiction_inml_technology =
R"doc(in-plane Nanomagnet Logic (iNML) technology implementation of the FCN
concept.)doc";

static const char *__doc_fiction_inml_technology_cell_mark = R"doc(Possible marks to be applied to a cell to change its type.)doc";

static const char *__doc_fiction_inml_technology_cell_mark_EMPTY = R"doc()doc";

static const char *__doc_fiction_inml_technology_cell_mark_INPUT = R"doc()doc";

static const char *__doc_fiction_inml_technology_cell_mark_OUTPUT = R"doc()doc";

static const char *__doc_fiction_inml_technology_cell_mode = R"doc(iNML cells do not have modes.)doc";

static const char *__doc_fiction_inml_technology_cell_type = R"doc(Possible types of iNML cells.)doc";

static const char *__doc_fiction_inml_technology_cell_type_CROSSWIRE_MAGNET = R"doc(Symbol used for cross-wire magnets.)doc";

static const char *__doc_fiction_inml_technology_cell_type_EMPTY = R"doc(Symbol used for empty iNML cells.)doc";

static const char *__doc_fiction_inml_technology_cell_type_FANOUT_COUPLER_MAGNET = R"doc(Symbol used for coupler (fan-out) magnets.)doc";

static const char *__doc_fiction_inml_technology_cell_type_INPUT = R"doc(Symbol used for input iNML cells.)doc";

static const char *__doc_fiction_inml_technology_cell_type_INVERTER_MAGNET = R"doc(Symbol used for inverter magnets.)doc";

static const char *__doc_fiction_inml_technology_cell_type_NORMAL = R"doc(Symbol used for normal iNML cells.)doc";

static const char *__doc_fiction_inml_technology_cell_type_OUTPUT = R"doc(Symbol used for output iNML cells.)doc";

static const char *__doc_fiction_inml_technology_cell_type_SLANTED_EDGE_DOWN_MAGNET = R"doc(Symbol used for lower slanted edge magnets.)doc";

static const char *__doc_fiction_inml_technology_cell_type_SLANTED_EDGE_UP_MAGNET = R"doc(Symbol used for upper slanted edge magnets.)doc";

static const char *__doc_fiction_inml_technology_inml_technology = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_crosswire_magnet = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_empty_cell = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_fanout_coupler_magnet = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_input_cell = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_inverter_magnet = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_normal_cell = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_normal_cell_mode = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_output_cell = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_slanted_edge_down_magnet = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_slanted_edge_magnet = R"doc()doc";

static const char *__doc_fiction_inml_technology_is_slanted_edge_up_magnet = R"doc()doc";

static const char *__doc_fiction_inml_topolinano_library =
R"doc(A concrete FCN gate library as used in \"ToPoliNano\"
(https://topolinano.polito.it/) for the iNML technology. In fiction,
this is emulated by using vertically shifted layouts and implementing
the ToPoliNano library with :math:`4 \times 4` magnet positions with
one empty row in most tiles (except for MAJ which needs to be handled
differently as this library is not uniform otherwise). Theoretically,
it allows for multiple wires in the same tile.)doc";

static const char *__doc_fiction_inml_topolinano_library_determine_port_routing = R"doc()doc";

static const char *__doc_fiction_inml_topolinano_library_has_and_or_maj_fanin =
R"doc(Checks whether the given node has an AND, OR, or MAJ fanin node.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The layout to check.

Parameter ``n``:
    Node whose fanins are to be considered.

Returns:
    `true` iff `n` has an AND, OR, or MAJ fanin node.)doc";

static const char *__doc_fiction_inml_topolinano_library_has_fanout_fanout =
R"doc(Checks whether the given node has an fanout node as fanout.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The layout to check.

Parameter ``n``:
    Node whose fanouts are to be considered.

Returns:
    `true` iff `n` has a fanout node as fanout.)doc";

static const char *__doc_fiction_inml_topolinano_library_inml_topolinano_library = R"doc()doc";

static const char *__doc_fiction_inml_topolinano_library_opposite = R"doc()doc";

static const char *__doc_fiction_inml_topolinano_library_post_layout_optimization =
R"doc(Post-layout optimization that straightens the wire segments to save
cells.

Template parameter ``CellLyt``:
    Cell-level layout type.

Parameter ``lyt``:
    The cell-level layout that has been created via application of
    `set_up_gate`.)doc";

static const char *__doc_fiction_inml_topolinano_library_set_up_gate =
R"doc(Overrides the corresponding function in fcn_gate_library. Given a tile
`t`, this function takes all necessary information from the stored
grid into account to choose the correct fcn_gate representation for
that tile. May it be a gate or wires. Rotation and special marks like
input and output, const cells etc. are computed additionally.

Template parameter ``GateLyt``:
    Shifted Cartesian gate-level layout type.

Parameter ``lyt``:
    Layout that hosts tile `t`.

Parameter ``t``:
    Tile to be realized as a ToPoliNano gate.

Returns:
    ToPoliNano gate representation of `t` including I/Os, rotation,
    etc.)doc";

static const char *__doc_fiction_integral_abs =
R"doc(Takes the absolute value of an integral number if it is signed, and
otherwise computes the identity. This avoids a compiler warning when
taking the absolute value of an unsigned number.

Template parameter ``T``:
    The type of the number to take the absolute value of. Must be
    integral.

Parameter ``n``:
    The number to take the absolute value of.

Returns:
    |n|.)doc";

static const char *__doc_fiction_inverse_levels =
R"doc(A clumsy implementation that returns the inverse level of each node in
a given network. Its behavior is similar to `mockturtle::depth_view`
but starting from the primary outputs instead of the primary inputs.
An implementation along the lines of `inv_depth_view` would be a lot
more sophisticated and desirable, but this quick hack does the job
well so far. If anyone wants to build an `inv_depth_view`, please be
my guest.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    The network whose inverse levels are desired.

Returns:
    A vector of inverse levels for each node where
    `ntk.node_to_index(n)` is the position where `n`'s inverse level
    is stored.)doc";

static const char *__doc_fiction_is_balanced =
R"doc(Checks if a logic network is properly path-balanced with regard to the
provided parameters.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``ntk``:
    The logic network to check.

Parameter ``ps``:
    Parameters.

Returns:
    `true` iff `ntk` is properly path-balanced with regard to `ps`.)doc";

static const char *__doc_fiction_is_cartesian_layout = R"doc()doc";

static const char *__doc_fiction_is_cell_level_layout = R"doc()doc";

static const char *__doc_fiction_is_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_is_charged_defect_type =
R"doc(Checks whether the given defect type is a charged one. `DB` and
`SI_VACANCY` types are charged. Those charged defects are to be
avoided by a larger distance.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` is of a charged type.)doc";

static const char *__doc_fiction_is_clocked_layout = R"doc()doc";

static const char *__doc_fiction_is_coordinate_layout = R"doc()doc";

static const char *__doc_fiction_is_crossable_wire =
R"doc(Checks whether a given coordinate `successor` hosts a crossable wire
when coming from coordinate `src` in a given layout. A wire is said to
be crossable if a potential cross-over would not result in running
along the same information flow direction. For example, a wire segment
hosted by `successor` that is horizontal and runs from west to east is
crossable by a wire segment coming from `src` that is vertical and
runs from north to south. However, if the wire segment coming from
`src` were also horizontal and ran from west to east, the cross-over
would be prohibited.

@Note This function can be called on layout types other than gate-
level layouts, but will then always return `false`. This is helpful
for general routing in, e.g., clocked layouts.

Template parameter ``Lyt``:
    Layout type.

Parameter ``lyt``:
    The layout.

Parameter ``src``:
    Source coordinate in `lyt`.

Parameter ``successor``:
    Successor coordinate in lyt reachable from `src`.

Returns:
    `true` iff `successor` hosts a wire that is crossable from `src`.)doc";

static const char *__doc_fiction_is_fanout_substituted =
R"doc(Checks if a logic network is properly fanout-substituted with regard
to the provided parameters, i.e., if no node exceeds the specified
fanout limits.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``ntk``:
    The logic network to check.

Parameter ``ps``:
    Parameters.

Returns:
    `true` iff `ntk` is properly fanout-substituted with regard to
    `ps`.)doc";

static const char *__doc_fiction_is_gate_level_layout = R"doc()doc";

static const char *__doc_fiction_is_ground_state =
R"doc(This function checks if the elstrostatic ground state of an SiDB
layout is found by a heuristic for the physical simulation (e.g.,
*QuickSim* or *SimAnneal*).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``heuristic_results``:
    Simulation results obtained from a heuristic physical simulation.

Parameter ``exact_results``:
    Simulation results obtained from an exact physical simulation.

Returns:
    Returns `true` if the ground state is contained in the simulation
    result provided by the heuristic physical simulation. `false`
    otherwise.)doc";

static const char *__doc_fiction_is_hexagonal_layout = R"doc()doc";

static const char *__doc_fiction_is_kink_induced_non_operational =
R"doc(This function determines if the layout is only considered non-
operational because of kinks. This means that the layout would be
considered as operational, if kinks were accepted.

@note "Kink induced non-operational" refers to the non-operational
status being exclusively caused by kinks with an otherwise correct
logic match.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Returns:
    Bool that indicates whether kinks induce the layout to become non-
    operational. `true` if the layout is non-operational due to kinks,
    `false` otherwise.)doc";

static const char *__doc_fiction_is_kink_induced_non_operational_2 =
R"doc(This function determines if the layout is only considered non-
operational because of kinks. This means that the layout would be
considered as operational, if kinks were accepted.

@note "Kink induced non-operational" refers to the non-operational
status being exclusively caused by kinks with an otherwise correct
logic match.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Parameter ``input_bdl_wire``:
    Optional BDL input wires of lyt.

Parameter ``output_bdl_wire``:
    Optional BDL output wires of lyt.

Parameter ``canvas_lyt``:
    Optional canvas layout.

Returns:
    Bool that indicates whether kinks induce the layout to become non-
    operational. `true` if the layout is non-operational due to kinks,
    `false` otherwise.)doc";

static const char *__doc_fiction_is_linear_scheme =
R"doc(Checks whether a given clocking scheme is registered as a cycle-free
one. These currently are

- COLUMNAR - ROW - 2DDWAVE - 2DDWAVEHEX

Template parameter ``Lyt``:
    Layout type.

Parameter ``scheme``:
    Clocking scheme to check.

Returns:
    `true` iff `scheme` is listed as one of the linear clocking
    schemes.)doc";

static const char *__doc_fiction_is_negatively_charged_defect =
R"doc(Checks whether the given defect has a negative charge value assigned
to it. This function is irrespective of the associated defect type.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` has a negative charge value.)doc";

static const char *__doc_fiction_is_neutral_defect_type =
R"doc(Checks whether the given defect type is not a charged one. Neutral
defects are to be avoided as well, but not by such a large distance.
Even though the `NONE` defect type is technically neutral, it is not a
defect per se which is why this function returns false on the `NONE`
defect input.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` is not of a charged type.)doc";

static const char *__doc_fiction_is_neutrally_charged_defect =
R"doc(Checks whether the given defect has a neutral charge value, i.e., `0`,
assigned to it. This function is irrespective of the associated defect
type.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` has a neutral charge value.)doc";

static const char *__doc_fiction_is_operational =
R"doc(Determine the operational status of an SiDB layout.

This function checks the operational status of a given SiDB layout
using the `is_operational` algorithm. It determines whether the SiDB
layout is operational and returns the correct result for all
:math:`2^n` input combinations.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Returns:
    A pair containing the operational status of the SiDB layout
    (either `OPERATIONAL` or `NON_OPERATIONAL`) and the number of
    input combinations tested.)doc";

static const char *__doc_fiction_is_operational_2 =
R"doc(Determine the operational status of an SiDB layout.

This function checks the operational status of a given SiDB layout
using the `is_operational` algorithm. It determines whether the SiDB
layout is operational and returns the correct result for all
:math:`2^n` input combinations.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Parameter ``input_bdl_wire``:
    Optional BDL input wires of lyt.

Parameter ``output_bdl_wire``:
    Optional BDL output wires of lyt.

Parameter ``canvas_lyt``:
    Optional canvas layout.

Returns:
    A pair containing the operational status of the SiDB layout
    (either `OPERATIONAL` or `NON_OPERATIONAL`) and the number of
    input combinations tested.)doc";

static const char *__doc_fiction_is_operational_params = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_is_operational_params_input_bdl_iterator_params = R"doc(Parameters for the BDL input iterator.)doc";

static const char *__doc_fiction_is_operational_params_op_condition =
R"doc(Condition to decide whether a layout is operational or non-
operational.)doc";

static const char *__doc_fiction_is_operational_params_operational_analysis_strategy =
R"doc(Simulation method to determine if the layout is operational or non-
operational. There are three possible strategies:

- `SIMULATION_ONLY`: This setting does not apply any filtering
strategies to determine if the layout is operational. Instead, it
relies solely on physical simulation to make this determination. -
`FILTER_ONLY`: This setting does only apply filtering strategies to
determine if the layout is non-operational. If the layout passes all
filtering strategies, it is considered operational. This is only an
approximation. It may be possible that the layout is non-operational,
but the filtering strategies do not detect it. -
`FILTER_THEN_SIMULATION`: Before a physical simulation is conducted,
the algorithm checks if filtering strategies have detected whether the
layout is non-operational. This only provides any runtime benefits if
kinks are rejected.)doc";

static const char *__doc_fiction_is_operational_params_operational_analysis_strategy_FILTER_ONLY =
R"doc(Apply filtering exclusively to determine whether the layout is non-
operational. If the layout passes all filter steps, it is considered
operational.

@note This is an extremely fast approximation that may sometimes lead
to false positives.)doc";

static const char *__doc_fiction_is_operational_params_operational_analysis_strategy_FILTER_THEN_SIMULATION =
R"doc(Before a physical simulation is conducted, the algorithm checks if
filter strategies can determine that the layout is non-operational.
This only provides any runtime benefits if kinks are rejected.)doc";

static const char *__doc_fiction_is_operational_params_operational_analysis_strategy_SIMULATION_ONLY =
R"doc(Do not apply filter strategies to determine whether the layout is
operational. Instead, rely solely on physical simulation.)doc";

static const char *__doc_fiction_is_operational_params_operational_condition =
R"doc(Condition to decide whether a layout is operational or non-
operational.)doc";

static const char *__doc_fiction_is_operational_params_operational_condition_REJECT_KINKS =
R"doc(The I/O pins are not allowed to show kinks. If kinks exist, the layout
is considered non-operational.)doc";

static const char *__doc_fiction_is_operational_params_operational_condition_TOLERATE_KINKS =
R"doc(Even if the I/O pins show kinks, the layout is still considered as
operational.)doc";

static const char *__doc_fiction_is_operational_params_sim_engine =
R"doc(The simulation engine to be used for the operational domain
computation.)doc";

static const char *__doc_fiction_is_operational_params_simulation_parameters =
R"doc(The simulation parameters for the physical simulation of the ground
state.)doc";

static const char *__doc_fiction_is_operational_params_strategy_to_analyze_operational_status =
R"doc(Strategy to determine whether a layout is operational or non-
operational.)doc";

static const char *__doc_fiction_is_positively_charged_defect =
R"doc(Checks whether the given defect has a positive charge value assigned
to it. This function is irrespective of the associated defect type.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` has a positive charge value.)doc";

static const char *__doc_fiction_is_shifted_cartesian_layout = R"doc()doc";

static const char *__doc_fiction_is_sidb_defect_surface = R"doc()doc";

static const char *__doc_fiction_is_sidb_gate_design_impossible =
R"doc(This function evaluates whether it is impossible to design an SiDB
gate for a given truth table and a given skeleton with atomic defects.
It determines the possible charge states at the output BDL pairs.
Atomic defects can cause a BDL pair to be neutrally charged only.
Thus, the BDL pair would not work as intended.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The truth table type.

Parameter ``skeleton_with_defects``:
    An SiDB skeleton layout with atomic defects.

Parameter ``spec``:
    A vector of truth tables (each truth table is representing one
    output) representing the gate's intended functionality.

Parameter ``params``:
    Parameters to determine if the gate design is impossible.

Returns:
    `true` if gate design is impossible, `false` otherwise.)doc";

static const char *__doc_fiction_is_sidb_gate_design_impossible_params =
R"doc(This struct contains parameters to determine if SiDB gate design is
impossible.)doc";

static const char *__doc_fiction_is_sidb_gate_design_impossible_params_bdl_iterator_params = R"doc(Parameters used for the BDL input iterator.)doc";

static const char *__doc_fiction_is_sidb_gate_design_impossible_params_simulation_params = R"doc(All parameters for physical SiDB simulations.)doc";

static const char *__doc_fiction_is_sidb_lattice = R"doc()doc";

static const char *__doc_fiction_is_sidb_lattice_100 = R"doc()doc";

static const char *__doc_fiction_is_sidb_lattice_111 = R"doc()doc";

static const char *__doc_fiction_is_tile_based_layout = R"doc()doc";

static const char *__doc_fiction_is_virtual_network_type = R"doc()doc";

static const char *__doc_fiction_jump_point_search =
R"doc(The Jump Point Search (JPS) path finding algorithm for shortest loop-
less paths between a given source and target coordinate in a Cartesian
layout. This function automatically detects whether the given layout
implements a clocking interface (see `clocked_layout`) and respects
the underlying information flow imposed by `layout`'s clocking scheme.

JPS was proposed as an optimization of A* for shortest paths and
offers better average complexity on uniform-cost grids that allow
diagonal connections. It uses a heuristic distance function that
estimates the remaining costs towards the target in every step. Thus,
this heuristic function should neither be complex to calculate nor
overestimating the remaining costs. Common heuristics to be used are
the Manhattan and the Euclidean distance functions. See
`distance_functor` for implementations. Since JPS assumes a unit-cost
grid, the use of cost functions together with JPS is not possible.

If the given layout implements the obstruction interface (see
`obstruction_layout`), paths will not be routed via obstructed
coordinates and connections.

In certain cases it might be desirable to determine regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:

```
{.cpp}
using clk_lyt = clocked_layout<cartesian_layout<>>;
using path = layout_coordinate_path<cartesian_layout<>>;
clk_lyt layout = ...;
auto shortest_path = jump_point_search<path>(static_cast<cartesian_layout<>>(layout), {source, target});
```

JPS was introduced in \"Online Graph Pruning for Pathfinding on Grid
Maps\" by Daniel Harabor and Alban Grastien in AAAI 2011.

Parts of this implementation are based on
https://github.com/qiao/PathFinding.js.

@note The original JPS highly relies on diagonal paths in the grid
which are not possible in most Cartesian grid-based FCN technologies.
Therefore, this implementation disallows diagonal paths. Consequently,
and due to non-uniform clocking schemes, JPS might perform worse than
A* in terms of runtime. It is recommended to use A* (see `a_star`).

@note JPS does not support wire crossings.

Template parameter ``Path``:
    Type of the returned path.

Template parameter ``Lyt``:
    Type of the layout to perform path finding on.

Template parameter ``Dist``:
    Distance value type to be used in the heuristic estimation
    function.

Parameter ``layout``:
    The layout in which the shortest path between a source and target
    is to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``dist_fn``:
    A distance functor that implements the desired heuristic
    estimation function.

Returns:
    The shortest loop-less path in `layout` from `objective.source` to
    `objective.target`.)doc";

static const char *__doc_fiction_kink_induced_non_operational_input_patterns =
R"doc(This function determines all input combinations for which kinks induce
the SiDB layout to become non-operational. This means that the layout
is operational if kinks would be accepted.

@note "Kink induced non-operational" refers to the non-operational
status being exclusively caused by kinks with an otherwise correct
logic match.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Returns:
    The input combinations where kinks induce the SiDB layout to
    become non-operational.)doc";

static const char *__doc_fiction_kink_induced_non_operational_input_patterns_2 =
R"doc(This function determines all input combinations for which kinks induce
the SiDB layout to become non-operational. This means that the layout
is operational if kinks would be accepted.

@note "Kink induced non-operational" refers to the non-operational
status being exclusively caused by kinks with an otherwise correct
logic match.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Parameter ``input_bdl_wire``:
    Optional BDL input wires of lyt.

Parameter ``output_bdl_wire``:
    Optional BDL output wires of lyt.

Parameter ``canvas_lyt``:
    Optional canvas layout.

Returns:
    The input combinations where kinks induce the SiDB layout to
    become non-operational.)doc";

static const char *__doc_fiction_layout_coordinate_path =
R"doc(A path in a layout defined as an ordered sequence of coordinates.

Template parameter ``Lyt``:
    Coordinate layout type.)doc";

static const char *__doc_fiction_layout_coordinate_path_append = R"doc()doc";

static const char *__doc_fiction_layout_coordinate_path_source = R"doc()doc";

static const char *__doc_fiction_layout_coordinate_path_target = R"doc()doc";

static const char *__doc_fiction_linear_temperature_schedule =
R"doc(A linearly decreasing temperature schedule. The temperature is altered
in decrements of `10`.

Parameter ``t``:
    The current temperature.

Returns:
    The next temperature, i.e. :math:`\texttt{t} - 10`.)doc";

static const char *__doc_fiction_magcad_magnet_count =
R"doc(Calculates the number of magnets for an iNML layout the way MagCAD
(https://topolinano.polito.it/) would do it. That is, counting chains
of 4 inverters as a single entity.

Template parameter ``Lyt``:
    iNML cell-level layout type.

Parameter ``lyt``:
    The iNML cell-level layout whose area is desired.

Returns:
    Number of magnets as counted by MagCAD.)doc";

static const char *__doc_fiction_manhattan_distance =
R"doc(The Manhattan distance :math:`D` between two layout coordinates
:math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

:math:`D = |x_1 - x_2| + |y_1 - y_2|`

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral type for the distance.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Manhattan distance between `source` and `target`.)doc";

static const char *__doc_fiction_manhattan_distance_functor =
R"doc(A pre-defined distance functor that uses the Manhattan distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral distance type.)doc";

static const char *__doc_fiction_manhattan_distance_functor_manhattan_distance_functor = R"doc()doc";

static const char *__doc_fiction_minimum_energy =
R"doc(Computes the minimum energy of a range of
`charge_distribution_surface` objects. If the range is empty, infinity
is returned.

Template parameter ``InputIt``:
    Must meet the requirements of `LegacyInputIterator`.

Parameter ``first``:
    Begin of the range to examime.

Parameter ``last``:
    End of the range to examine.

Returns:
    Value of the minimum energy found in the input range (unit: eV),
    or infinity if the range is empty.)doc";

static const char *__doc_fiction_minimum_energy_distribution =
R"doc(Returns an iterator to the charge distribution of minimum energy
contained in a range of `charge_distribution_surface` objects. If the
range is empty, `last` is returned.

Template parameter ``InputIt``:
    Must meet the requirements of `LegacyInputIterator`.

Parameter ``first``:
    Begin of the range to examime.

Parameter ``last``:
    End of the range to examine.

Returns:
    Iterator to the minimum energy charge distribution found in the
    input range, or `last` if the range is empty.)doc";

static const char *__doc_fiction_missing_sidb_position_exception =
R"doc(Exception thrown when a missing SiDB position is encountered in the
parsed file.)doc";

static const char *__doc_fiction_missing_sidb_position_exception_line = R"doc()doc";

static const char *__doc_fiction_missing_sidb_position_exception_missing_sidb_position_exception = R"doc()doc";

static const char *__doc_fiction_missing_sidb_position_exception_where = R"doc()doc";

static const char *__doc_fiction_mutable_rank_view =
R"doc(@class mutable_rank_view<Ntk, true>

If already a rank_interface exists only the static_depth_view
constructor gets called.

Template parameter ``Ntk``:
    The network type.)doc";

static const char *__doc_fiction_mutable_rank_view_2 =
R"doc(Deduction guide for `mutable_rank_view'.

Template parameter ``T``:
    Network type deduced from the construction context of
    `mutable_rank_view`.)doc";

static const char *__doc_fiction_mutable_rank_view_3 =
R"doc(Deduction guide for `mutable_rank_view` with two constructor
arguments.

Template parameter ``T``:
    Network type deduced from the construction context of
    `mutable_rank_view`.)doc";

static const char *__doc_fiction_mutable_rank_view_mutable_rank_view = R"doc()doc";

static const char *__doc_fiction_network_balancing =
R"doc(Balances a logic network with buffer nodes that compute the identity
function. For this purpose, `create_buf` is utilized. Therefore,
`NtkDest` should support identity nodes. If it does not, no new nodes
will in fact be created. In either case, the returned network will be
logically equivalent to the input one.

The process is rather naive and is not combined with fanout
substitution.

The returned network is newly created from scratch because its type
`NtkDest` may differ from `NtkSrc`.

@note The physical design algorithms natively provided in fiction do
not require their input networks to be balanced. If that is necessary,
they will do it themselves. Providing already balanced networks may
lead to substantial overhead.

Template parameter ``NtkDest``:
    Type of the returned logic network.

Template parameter ``NtkSrc``:
    Type of the input logic network.

Parameter ``ntk_src``:
    The input logic network.

Parameter ``ps``:
    Parameters.

Returns:
    A path-balanced logic network of type `NtkDest` that is logically
    equivalent to `ntk_src`.)doc";

static const char *__doc_fiction_network_balancing_params = R"doc(Parameters for the network balancing algorithm.)doc";

static const char *__doc_fiction_network_balancing_params_unify_outputs = R"doc(Flag to indicate that all output nodes should be in the same rank.)doc";

static const char *__doc_fiction_network_reader =
R"doc(Helper class to read directories of mockturtle networks of certain
types.

Template parameter ``NtkPtr``:
    Pointer type to a logic network.)doc";

static const char *__doc_fiction_network_reader_get_networks =
R"doc(Getter for networks generated from given files.

Parameter ``sorted``:
    Flag to indicate that the networks are to be returned in ascending
    order of their vertex count.

Returns:
    Vector of read and constructed logic networks.)doc";

static const char *__doc_fiction_network_reader_network_reader =
R"doc(Standard constructor. Reads and constructs logic networks. May throw.

Parameter ``filename``:
    Path to the file or folder of files to read.

Parameter ``out``:
    Output stream to write status updates into.)doc";

static const char *__doc_fiction_network_reader_networks = R"doc(Vector of logic networks read from files given at construction.)doc";

static const char *__doc_fiction_network_reader_out = R"doc(Output stream to which status updates are to be written.)doc";

static const char *__doc_fiction_network_reader_read =
R"doc(Actual read function that constructs the logic network from a file.

Template parameter ``Reader``:
    The functor type which constructs the network, i.e. handles the
    callbacks.

Template parameter ``ReadFun``:
    The file parser type.

Parameter ``file``:
    The file name.

Parameter ``rfun``:
    The actual parsing function.)doc";

static const char *__doc_fiction_normalize_layout_coordinates =
R"doc(A new layout is constructed and returned that is equivalent to the
given cell-level layout. However, its coordinates are normalized,
i.e., start at `(0, 0)` and are all positive. To this end, all
existing coordinates are shifted by an x and y offset.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout which is to be normalized.

Returns:
    New normalized equivalent layout.)doc";

static const char *__doc_fiction_num_adjacent_coordinates =
R"doc(Returns the number of adjacent coordinates of a given one. This is not
a constant value because `c` could be located at a layout border.

Template parameter ``Lyt``:
    Layout type.

Parameter ``lyt``:
    Layout.

Parameter ``c``:
    Coordinate whose number of adjacencies are required.

Returns:
    Number of `c`'s adjacent coordinates.)doc";

static const char *__doc_fiction_num_clks = R"doc()doc";

static const char *__doc_fiction_num_clks_FOUR = R"doc(Four clocks.)doc";

static const char *__doc_fiction_num_clks_THREE = R"doc(Three clocks.)doc";

static const char *__doc_fiction_num_constant_fanins =
R"doc(Computes the number of constant fanin nodes of some network node `n`.

Template parameter ``Ntk``:
    `mockturtle` network type.

Parameter ``ntk``:
    Network in which the constant fanins are to be counted.

Parameter ``n``:
    Node whose constant fanins are to be counted.

Returns:
    Number of constant fanins to `n` in `ntk`.)doc";

static const char *__doc_fiction_obstruction_layout = R"doc()doc";

static const char *__doc_fiction_obstruction_layout_2 = R"doc()doc";

static const char *__doc_fiction_obstruction_layout_obstruction_layout = R"doc()doc";

static const char *__doc_fiction_occupation_probability_gate_based =
R"doc(This function computes the occupation probability of erroneous charge
distributions (output charge does not match the expected output
according the truth table) at a given temperature.

Parameter ``energy_and_state_type``:
    This contains the energies of all possible charge distributions
    together with the information if the charge distribution (state)
    is transparent or erroneous.

Parameter ``temperature``:
    System temperature to assume (unit: K).

Returns:
    The occupation probability of all erroneous states is returned.)doc";

static const char *__doc_fiction_occupation_probability_non_gate_based =
R"doc(This function computes the occupation probability of excited states
(charge distributions with energy higher than the ground state) at a
given temperature.

Parameter ``energy_distribution``:
    This contains the energies in eV of all possible charge
    distributions with the degeneracy.

Parameter ``temperature``:
    System temperature to assume (unit: K).

Returns:
    The total occupation probability of all excited states is
    returned.)doc";

static const char *__doc_fiction_odd_column_cartesian =
R"doc(\verbatim +-------+ +-------+ | | | | | (0,0) +-------+ (2,0)
+-------+ | | | | | +-------+ (1,0) +-------+ (3,0) | | | | | | |
(0,1) +-------+ (2,1) +-------+ | | | | | +-------+ (1,1) +-------+
(3,1) | | | | | | | (0,2) +-------+ (2,2) +-------+ | | | | +-------+
+-------+ \endverbatim)doc";

static const char *__doc_fiction_odd_column_hex =
R"doc(\verbatim _____ _____ / \ / \ / (0,0) \_____/ (2,0) \_____ \ / \ / \
\_____/ (1,0) \_____/ (3,0) \ / \ / \ / / (0,1) \_____/ (2,1) \_____/
\ / \ / \ \_____/ (1,1) \_____/ (3,1) \ / \ / \ / / (0,2) \_____/
(2,2) \_____/ \ / \ / \_____/ \_____/ \endverbatim)doc";

static const char *__doc_fiction_odd_row_cartesian =
R"doc(\verbatim +-------+-------+-------+ | | | | | (0,0) | (1,0) | (2,0) |
| | | | +---+---+---+---+---+---+---+ | | | | | (0,1) | (1,1) | (2,1)
| | | | | +---+---+---+---+---+---+---+ | | | | | (0,2) | (1,2) |
(2,2) | | | | | +-------+-------+-------+ \endverbatim)doc";

static const char *__doc_fiction_odd_row_hex =
R"doc(\verbatim / \ / \ / \ / \ / \ / \ | (0,0) | (1,0) | (2,0) | | | | | \
/ \ / \ / \ \ / \ / \ / \ | (0,1) | (1,1) | (2,1) | | | | | / \ / \ /
\ / / \ / \ / \ / | (0,2) | (1,2) | (2,2) | | | | | \ / \ / \ / \ / \
/ \ / \endverbatim)doc";

static const char *__doc_fiction_offset_operator_lshift = R"doc()doc";

static const char *__doc_fiction_offset_to_cube_coord =
R"doc(Converts offset coordinates to cube coordinates.

@note This function assumes that the input coordinates are within the
valid range for cube coordinates. Specifically, the x, y, and z
coordinates should be within the range of :math:`(0, 0, 0)` to
:math:`(2^{31} - 1, 2^{31} - 1, 1)`. If the input coordinates are
outside this range, the behavior of the function is undefined. If the
input coordinate is dead, a dead cube coordinate is returned.

Parameter ``coord``:
    Offset coordinate to convert to a cube coordinate.

Returns:
    Cube coordinate equivalent to `coord`.)doc";

static const char *__doc_fiction_offset_ucoord_t =
R"doc(Unsigned offset coordinates.

This implementation is optimized for memory-efficiency and fits within
64 bits. Coordinates span from :math:`(0, 0, 0)` to :math:`(2^{31} -
1, 2^{31} - 1, 1)`. Each coordinate has a dead indicator `d` that can
be used to represent that it is not in use.)doc";

static const char *__doc_fiction_offset_ucoord_t_d = R"doc(MSB acts as dead indicator.)doc";

static const char *__doc_fiction_offset_ucoord_t_get_dead =
R"doc(Returns a dead copy of the coordinate, i.e., (1, x, y, z).

Returns:
    A dead copy of the coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_is_dead =
R"doc(Returns whether the coordinate is dead.

Returns:
    `true` iff coordinate is dead.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_eq =
R"doc(Compares against another coordinate for equality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff both coordinates are identical.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_eq_2 =
R"doc(Compares against another coordinate's `uint64_t` representation for
equality. Respects the dead indicator.

Parameter ``other``:
    Right-hand side coordinate representation in `uint64_t` format.

Returns:
    `true` iff this coordinate is equal to the converted one.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_ge =
R"doc(Determine whether this coordinate is "greater than or equal to"
another one. This is the case if this one is not "less than" the
other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "greater than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_gt =
R"doc(Determine whether this coordinate is "greater than" another one. This
is the case if the other one is "less than".

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "greater than" the other coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_le =
R"doc(Determine whether this coordinate is "less than or equal to" another
one. This is the case if this one is not "greater than" the other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "less than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_lt =
R"doc(Determine whether this coordinate is "less than" another one. This is
the case if z is smaller, or if z is equal but y is smaller, or if z
and y are equal but x is smaller.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff this coordinate is "less than" the other coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_ne =
R"doc(Compares against another coordinate for inequality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    `true` iff both coordinates are not identical.)doc";

static const char *__doc_fiction_offset_ucoord_t_operator_unsigned_long =
R"doc(Allows explicit conversion to `uint64_t`. Segments an unsigned 64-bit
integer into four parts (from MSB to LSB): - 1 bit for the dead
indicator - 1 bit for the z position - 31 bit for the y position - 31
bit for the x position)doc";

static const char *__doc_fiction_offset_ucoord_t_str =
R"doc(Returns a string representation of the coordinate of the form `"(x, y,
z)"` that does not respect the dead indicator.

Returns:
    String representation of the form `"(x, y, z)"`.)doc";

static const char *__doc_fiction_offset_ucoord_t_ucoord_t = R"doc(Default constructor. Creates a dead coordinate at (0, 0, 0).)doc";

static const char *__doc_fiction_offset_ucoord_t_ucoord_t_2 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Template parameter ``Z``:
    Type of z.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.

Parameter ``z_``:
    z position.)doc";

static const char *__doc_fiction_offset_ucoord_t_ucoord_t_3 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.)doc";

static const char *__doc_fiction_offset_ucoord_t_ucoord_t_4 =
R"doc(Standard constructor. Instantiates a coordinate from an uint64_t,
where the positions are encoded in the following four parts of the
unsigned 64-bit integer (from MSB to LSB): - 1 bit for the dead
indicator - 1 bit for the z position - 31 bit for the y position - 31
bit for the x position

Parameter ``t``:
    Unsigned 64-bit integer to instantiate the coordinate from.)doc";

static const char *__doc_fiction_offset_ucoord_t_wrap =
R"doc(Wraps the coordinate with respect to the given aspect ratio by
iterating over the dimensions in the order defined by the coordinate
type. For any dimension of the coordinate that is strictly larger than
the associated dimension of the aspect ratio, this dimension will be
wrapped to zero, and the next dimension is increased. The resulting
coordinate becomes a dead copy of the aspect ratio if it is not
contained in the aspect ratio after iterating. An example use case of
this function is the coordinate iterator, which implements iterator
advancing by first incrementing the x dimension, then wrapping the
coordinate to the boundary within to enumerate.

Parameter ``aspect_ratio``:
    Aspect ratio to wrap the coordinate to.)doc";

static const char *__doc_fiction_offset_ucoord_t_x = R"doc(31 bit for the x coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_y = R"doc(31 bit for the y coordinate.)doc";

static const char *__doc_fiction_offset_ucoord_t_z = R"doc(1 bit for the z coordinate.)doc";

static const char *__doc_fiction_on_the_fly_circuit_design_on_defective_surface_stats =
R"doc(Statistics for the on-the-fly defect-aware circuit design.

Template parameter ``GateLyt``:
    Gate-level layout type.)doc";

static const char *__doc_fiction_on_the_fly_circuit_design_on_defective_surface_stats_duration = R"doc(The total runtime of the on-the-fly circuit design.)doc";

static const char *__doc_fiction_on_the_fly_circuit_design_on_defective_surface_stats_exact_stats = R"doc(The `stats` of the *exact* algorithm.)doc";

static const char *__doc_fiction_on_the_fly_circuit_design_on_defective_surface_stats_gate_layout = R"doc(The gate-level layout after P&R.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design =
R"doc(This function implements an on-the-fly SiDB circuit design algorithm.

The process begins with an already placed and routed gate-level
layout. For each gate, the corresponding SiDB implementation is
designed by using an SiDB gate design algorithm.

Template parameter ``CellLyt``:
    SiDB cell-level layout type.

Template parameter ``GateLyt``:
    Gate-level layout type.

Parameter ``gate_lyt``:
    Gate-level layout.

Parameter ``lattice_tiling``:
    The lattice tiling used for the circuit design.

Parameter ``params``:
    The parameters used for designing the circuit, encapsulated in an
    `on_the_fly_sidb_circuit_design_params` object.

Parameter ``stats``:
    Pointer to a structure for collecting statistics. If `nullptr`,
    statistics are discarded.

Returns:
    Layout representing the designed SiDB circuit.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_on_defective_surface =
R"doc(This function implements an on-the-fly circuit design algorithm for a
defective SiDB surface.

The process begins with placement and routing using a blacklist and
the `exact` method. The blacklist includes skeleton-tile pairs that
are excluded due to collisions between skeleton and neutral defects on
specific tiles. After identifying a valid placement and routing, a
defect-aware SiDB gate design algorithm is applied. This algorithm
designs gates for each tile while accounting for atomic defects. If
the gate design is unsuccessful, the blacklist is updated with the
problematic skeleton-gate pair, and the placement and routing process
is restarted. If the gate design succeeds, the algorithm finalizes the
design and returns the SiDB circuit. This approach ensures that the
circuit remains functional even in the presence of defects.

This methodology is detailed in the paper "On-the-fly Defect-Aware
Design of Circuits based on Silicon Dangling Bond Logic" by J.
Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille, IEEE NANO
2024 (https://ieeexplore.ieee.org/abstract/document/10628962).

Template parameter ``Ntk``:
    The type of the input network.

Template parameter ``CellLyt``:
    SiDB defect surface type.

Template parameter ``GateLyt``:
    Gate-level layout type.

Parameter ``ntk``:
    The input network to be mapped onto the defective surface.

Parameter ``lattice_tiling``:
    The lattice tiling used for the circuit design.

Parameter ``defective_surface``:
    The defective surface on which the SiDB circuit is designed.

Parameter ``params``:
    The parameters used for designing the circuit, encapsulated in an
    `on_the_fly_sidb_circuit_design_params` object.

Parameter ``stats``:
    Pointer to a structure for collecting statistics. If `nullptr`,
    statistics are discarded.

Returns:
    Layout representing the designed circuit on the defective surface.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_on_defective_surface_params =
R"doc(This struct stores the parameters to design an SiDB circuit on a
defective surface.

Template parameter ``CellLyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_on_defective_surface_params_exact_design_parameters = R"doc(Parameters for the *exact* placement and routing algorithm.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_on_defective_surface_params_sidb_on_the_fly_gate_library_parameters = R"doc(Parameters for the SiDB on-the-fly gate library.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_params =
R"doc(This struct stores the parameters to design an SiDB circuit.

Template parameter ``CellLyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_on_the_fly_sidb_circuit_design_params_sidb_on_the_fly_gate_library_parameters = R"doc(Parameters for the SiDB on-the-fly gate library.)doc";

static const char *__doc_fiction_open_clocking =
R"doc(Returns an irregular clocking that maps every coordinate to the
standard clock. It is intended to be overridden.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``n``:
    Number of clocks.

Returns:
    Irregular clocking scheme.)doc";

static const char *__doc_fiction_operational_domain =
R"doc(An operational domain is a set of simulation parameter values for
which a given SiDB layout is logically operational. This means that a
layout is deemed operational if the layout's ground state corresponds
with a given Boolean function at the layout's outputs for all possible
input combinations. In this implementation, :math:`n` BDL input wires
and a single BDL output wire are assumed for a given layout. Any
operational domain computation algorithm toggles through all
:math:`2^n` input combinations and evaluates the layout's output
behavior in accordance with the given Boolean function. The layout is
only considered operational for a certain parameter combination, if
the output behavior is correct for all input combinations. The
operational domain can be computed by sweeping over specified
simulation parameters and checking the operational status of the
layout for each parameter combination. The operational domain is then
defined as the set of all parameter combinations for which the layout
is operational. Different techniques for performing these sweep are
implemented.)doc";

static const char *__doc_fiction_operational_domain_add_dimension =
R"doc(Adds a dimension to sweep over. The first dimension is the x
dimension, the second dimension is the y dimension, etc.

Parameter ``dim``:
    The dimension to add.)doc";

static const char *__doc_fiction_operational_domain_contour_tracing =
R"doc(Computes the operational domain of the given SiDB cell-level layout.
The operational domain is the set of all parameter combinations for
which the layout is logically operational. Logical operation is
defined as the layout implementing the given truth table. The input
BDL pairs of the layout are assumed to be in the same order as the
inputs of the truth table.

This algorithm first uses random sampling to find a set of operational
point within the parameter range. From there, it traverses outwards to
find the edge of the operational area and performs Moore neighborhood
contour tracing to explore the contour of the operational domain. This
is repeated for all initially sampled points that do not lie within a
contour. The algorithm is guaranteed to determine the contours of all
operational "islands" if the initial random sampling found at least
one operational point within them. Thereby, this algorithm works for
disconnected operational domains.

It performs `samples` uniformly-distributed random samples within the
parameter range. For each thusly discovered operational island, it
performs another number of samples equal to the distance to an edge of
each operational area. Finally, it performs up to 8 samples for each
contour point (however, the actual number is usually lower). For each
sample, the algorithm performs one operational check on the layout,
where each operational check consists of up to :math:`2^n` exact
ground state simulations, where :math:`n` is the number of inputs of
the layout. Each exact ground state simulation has exponential
complexity in of itself. Therefore, the algorithm is only feasible for
small layouts with few inputs.

This flavor of operational domain computation was proposed in
\"Reducing the Complexity of Operational Domain Computation in Silicon
Dangling Bond Logic\" by M. Walter, J. Drewniok, S. S. H. Ng, K.
Walus, and R. Wille in NANOARCH 2023.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The operational domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_operational_domain_dimensions =
R"doc(The dimensions to sweep over. The first dimension is the x dimension,
the second dimension is the y dimension, etc.)doc";

static const char *__doc_fiction_operational_domain_flood_fill =
R"doc(Computes the operational domain of the given SiDB cell-level layout.
The operational domain is the set of all parameter combinations for
which the layout is logically operational. Logical operation is
defined as the layout implementing the given truth table. The input
BDL pairs of the layout are assumed to be in the same order as the
inputs of the truth table.

This algorithm first uses random sampling to find several operational
points within the parameter range. From there, it employs the "flood
fill" algorithm to explore the operational domain. The algorithm is
guaranteed to determine all operational "islands" in their entirety if
the initial random sampling found at least one operational point
within them. Thereby, this algorithm works for disconnected
operational domains.

It performs `samples` uniformly-distributed random samples within the
parameter range. From there, it performs another number of samples
equal to the number of points within the operational domain plus the
first non-operational point in each direction. For each sample, the
algorithm performs one operational check on the layout, where each
operational check consists of up to :math:`2^n` exact ground state
simulations, where :math:`n` is the number of inputs of the layout.
Each exact ground state simulation has exponential complexity in of
itself. Therefore, the algorithm is only feasible for small layouts
with few inputs.

This flavor of operational domain computation was proposed in
\"Reducing the Complexity of Operational Domain Computation in Silicon
Dangling Bond Logic\" by M. Walter, J. Drewniok, S. S. H. Ng, K.
Walus, and R. Wille in NANOARCH 2023.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The operational domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_operational_domain_get_dimension =
R"doc(Returns a specific dimension by index.

Parameter ``index``:
    The index of the dimension to return.

Returns:
    The dimension at the specified index.

Throws:
    std::out_of_range if the index is out of range.)doc";

static const char *__doc_fiction_operational_domain_get_number_of_dimensions =
R"doc(Returns the number of dimensions to sweep over.

Returns:
    The number of dimensions to sweep over.)doc";

static const char *__doc_fiction_operational_domain_grid_search =
R"doc(Computes the operational domain of the given SiDB cell-level layout.
The operational domain is the set of all parameter combinations for
which the layout is logically operational. Logical operation is
defined as the layout implementing the given truth table. The input
BDL pairs of the layout are assumed to be in the same order as the
inputs of the truth table.

This algorithm uses a grid search to find the operational domain. The
grid search is performed by exhaustively sweeping the parameter space
in the x and y dimensions. Since grid search is exhaustive, the
algorithm is guaranteed to find the operational domain, if it exists
within the parameter range. However, the algorithm performs a
quadratic number of operational checks on the layout, where each
operational check consists of up to :math:`2^n` exact ground state
simulations, where :math:`n` is the number of inputs of the layout.
Each exact ground state simulation has exponential complexity in of
itself. Therefore, the algorithm is only feasible for small layouts
with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected vector of truth tables of the layout. Each truth table
    represents an output of the Boolean function.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The operational domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_operational_domain_operational_domain = R"doc(Default constructor.)doc";

static const char *__doc_fiction_operational_domain_operational_domain_2 =
R"doc(Standard constructor.

Parameter ``dims``:
    Dimensions.)doc";

static const char *__doc_fiction_operational_domain_params =
R"doc(Parameters for the operational domain computation. The parameters are
used across the different operational domain computation algorithms.)doc";

static const char *__doc_fiction_operational_domain_params_operational_params =
R"doc(The parameters used to determine if a layout is operational or non-
operational.)doc";

static const char *__doc_fiction_operational_domain_params_sweep_dimensions =
R"doc(The dimensions to sweep over together with their value ranges, ordered
by priority. The first dimension is the x dimension, the second
dimension is the y dimension, etc.)doc";

static const char *__doc_fiction_operational_domain_random_sampling =
R"doc(Computes the operational domain of the given SiDB cell-level layout.
The operational domain is the set of all parameter combinations for
which the layout is logically operational. Logical operation is
defined as the layout implementing the given truth table. The input
BDL pairs of the layout are assumed to be in the same order as the
inputs of the truth table.

This algorithm uses random sampling to find a part of the operational
domain that might not be complete. It performs a total of `samples`
uniformly-distributed random samples within the parameter range. For
each sample, the algorithm performs one operational check on the
layout, where each operational check consists of up to :math:`2^n`
exact ground state simulations, where :math:`n` is the number of
inputs of the layout. Each exact ground state simulation has
exponential complexity in of itself. Therefore, the algorithm is only
feasible for small layouts with few inputs.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The operational domain of the layout.

Throws:
    std::invalid_argument if the given sweep parameters are invalid.)doc";

static const char *__doc_fiction_operational_domain_ratio =
R"doc(Calculates the ratio of operational parameter points surrounding a
specified parameter point to the total number of parameter points in
the given parameter space. This function is useful for assessing how
robust a gate design is to variations in its parameters.

A ratio close to 1 indicates that the gate is robust, meaning it
functions correctly across a broad range of parameter values. A ratio
close to 0 indicates that the gate is highly sensitive to parameter
variations and may fail to operate correctly.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    The SiDB layout for which to compute the ratio of operational
    parameter points surrounding a specified parameter point to the
    total number of parameter points.

Parameter ``spec``:
    The expected Boolean function of the layout, provided as a multi-
    output truth table.

Parameter ``params``:
    Parameters.

Parameter ``pp``:
    The specific parameter point around which the operational ratio is
    computed.

Returns:
    The ratio of operational parameter points to the total number of
    parameter points in the parameter space.)doc";

static const char *__doc_fiction_operational_domain_ratio_params =
R"doc(Parameters for computing the ratio of operational parameter points
around a specified parameter point to the total number of parameter
points in the given parameter space.)doc";

static const char *__doc_fiction_operational_domain_ratio_params_op_domain_params = R"doc(Parameters for the operational domain computation.)doc";

static const char *__doc_fiction_operational_domain_stats =
R"doc(Statistics for the operational domain computation. The statistics are
used across the different operational domain computation algorithms.)doc";

static const char *__doc_fiction_operational_domain_stats_duration = R"doc(The total runtime of the operational domain computation.)doc";

static const char *__doc_fiction_operational_domain_stats_num_evaluated_parameter_combinations = R"doc(Number of evaluated parameter combinations.)doc";

static const char *__doc_fiction_operational_domain_stats_num_non_operational_parameter_combinations =
R"doc(Number of parameter combinations, for which the layout is non-
operational.)doc";

static const char *__doc_fiction_operational_domain_stats_num_operational_parameter_combinations = R"doc(Number of parameter combinations, for which the layout is operational.)doc";

static const char *__doc_fiction_operational_domain_stats_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_operational_domain_stats_num_total_parameter_points = R"doc(Total number of parameter points in the parameter space.)doc";

static const char *__doc_fiction_operational_domain_value_range =
R"doc(A range of values for a dimension sweep. The range is defined by a
minimum value, a maximum value and a step size.)doc";

static const char *__doc_fiction_operational_domain_value_range_dimension = R"doc(The sweep parameter of the dimension.)doc";

static const char *__doc_fiction_operational_domain_value_range_max = R"doc(The maximum value of the dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_value_range_min = R"doc(The minimum value of the dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_value_range_step = R"doc(The step size of the dimension sweep.)doc";

static const char *__doc_fiction_operational_input_patterns =
R"doc(This function determines the input combinations for which the layout
is operational.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters to simulate if a input combination is operational.

Returns:
    The operational input combinations.)doc";

static const char *__doc_fiction_operational_input_patterns_2 =
R"doc(This function determines the input combinations for which the layout
is operational.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters to simulate if a input combination is operational.

Parameter ``input_bdl_wire``:
    Optional BDL input wires of lyt.

Parameter ``output_bdl_wire``:
    Optional BDL output wires of lyt.

Parameter ``canvas_lyt``:
    Optional canvas layout.

Returns:
    The count of operational input combinations.)doc";

static const char *__doc_fiction_operational_status = R"doc(Possible operational status of a layout.)doc";

static const char *__doc_fiction_operational_status_NON_OPERATIONAL = R"doc(The layout is non-operational.)doc";

static const char *__doc_fiction_operational_status_OPERATIONAL = R"doc(The layout is operational.)doc";

static const char *__doc_fiction_orthogonal =
R"doc(A scalable placement & routing approach based on orthogonal graph
drawing as originally proposed in \"Scalable Design for Field-coupled
Nanocomputing Circuits\" by M. Walter, R. Wille, F. Sill Torres, D.
Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
can be found in \"Design Automation for Field-coupled
Nanotechnologies\" by M. Walter, R. Wille, F. Sill Torres, and R.
Drechsler published by Springer Nature in 2022.

Via certain restrictions to the degrees of freedom in FCN physical
design, this algorithm achieves a polynomial time complexity. However,
these restrictions lead to an overall approximation of optimal layout
quality within several factors. Therefore, this algorithm produces
valid layouts within a short amount of time, its results are far from
being optimal in terms of area.

The imposed restrictions are that the input logic network has to be a
3-graph, i.e., cannot have any node exceeding degree 3 (combined input
and output), and that the resulting layout is always 2DDWave-clocked.

This algorithm is based on a modification of \"Improved orthogonal
drawings of 3-graphs\" by Therese C. Biedl in Canadian Conference on
Computational Geometry 1996. Biedl's original algorithm works for
undirected graphs only while this modification respects information
flow of directed logic networks. To this end, the edge directions of
the logic network directly used instead of relabeling the edges
according to its DFS tree, ordering the vertices using topological
sorting instead of DFS, and adding an extra placement rule for nodes
without predecessors.

The algorithm works in polynomial time :math:`\mathcal{O}(3|N| + |L|)`
where :math:`|N|` is the number of nodes the given network and
:math:`|L|` is the resulting layout size given by :math:`x \cdot y`,
which approaches :math:`(\frac{|N|}{2})^2` asymptotically.

May throw a high_degree_fanin_exception if `ntk` contains any node
with a fan-in larger than 2.

Template parameter ``Lyt``:
    Desired gate-level layout type.

Template parameter ``Ntk``:
    Network type that acts as specification.

Parameter ``ntk``:
    The network that is to place and route.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.

Returns:
    A gate-level layout of type `Lyt` that implements `ntk` as an FCN
    circuit.)doc";

static const char *__doc_fiction_orthogonal_physical_design_params = R"doc(Parameters for the orthogonal physical design algorithm.)doc";

static const char *__doc_fiction_orthogonal_physical_design_params_number_of_clock_phases = R"doc(Number of clock phases to use. 3 and 4 are supported.)doc";

static const char *__doc_fiction_orthogonal_physical_design_stats = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_duration = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_num_crossings = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_num_gates = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_num_wires = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_report = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_x_size = R"doc()doc";

static const char *__doc_fiction_orthogonal_physical_design_stats_y_size = R"doc()doc";

static const char *__doc_fiction_out_of_cell_names_exception = R"doc()doc";

static const char *__doc_fiction_out_of_cell_names_exception_out_of_cell_names_exception = R"doc()doc";

static const char *__doc_fiction_out_of_place_edge_color_view =
R"doc(A view that extends `mockturtle::out_of_place_color_view` such that it
can color edges, i.e., tuples of nodes as well.

Template parameter ``Ntk``:
    mockturtle network type.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_clear_colors = R"doc(Remove all assigned colors.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_edge_color =
R"doc(Returns the assigned color of a given edge.

Parameter ``e``:
    Edge whose color is desired.

Returns:
    Color of edge `e`.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_edge_colors = R"doc(Out-of-place storage for edge colors.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_eval_edge_color =
R"doc(Evaluates a predicate on a given edge's color.

Template parameter ``Pred``:
    Predicate type.

Parameter ``e``:
    Edge whose color is used as a parameter for `pred`.

Parameter ``pred``:
    Predicate to evaluate on `e`'s color.

Returns:
    Return value of `pred(color(e))`.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_eval_edge_color_2 =
R"doc(Evaluates a predicate on two given edges' colors.

Template parameter ``Pred``:
    Predicate type.

Parameter ``a``:
    Edge whose color is used as the first parameter for `pred`.

Parameter ``b``:
    Edge whose color is used as the second parameter for `pred`.

Parameter ``pred``:
    Predicate to evaluate on `a`'s and `b`'s colors.

Returns:
    Return value of `pred(color(a), color(b))`.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_out_of_place_edge_color_view =
R"doc(Standard constructor.

Parameter ``ntk``:
    Network to color.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_paint_edge =
R"doc(Paint a given edge with the current color.

Parameter ``e``:
    Edge to paint.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_paint_edge_2 =
R"doc(Paint a given edge with the given color.

Parameter ``e``:
    Edge to paint.

Parameter ``clr``:
    Color to paint `e` with.)doc";

static const char *__doc_fiction_out_of_place_edge_color_view_paint_edge_3 =
R"doc(Paint a given edge with the color of another given edge.

Parameter ``e``:
    Edge to paint.

Parameter ``other``:
    Edge whose color is to be used to paint `e`.)doc";

static const char *__doc_fiction_parameter_point = R"doc(The parameter point holds parameter values in the x and y dimension.)doc";

static const char *__doc_fiction_parameter_point_get =
R"doc(Support for structured bindings.

Template parameter ``I``:
    Index of the parameter value to be returned.

Returns:
    The parameter value at the specified index.

Throws:
    std::out_of_range if the index is out of bounds.)doc";

static const char *__doc_fiction_parameter_point_get_parameters =
R"doc(Returns the parameter values for each dimension.

Returns:
    The parameter values for each dimension.)doc";

static const char *__doc_fiction_parameter_point_operator_eq =
R"doc(Equality operator. Checks if this parameter point is equal to another
point within a specified tolerance. The tolerance is defined by
`constants::ERROR_MARGIN`.

Parameter ``other``:
    Other parameter point to compare with.

Returns:
    `true` iff the parameter points are equal.)doc";

static const char *__doc_fiction_parameter_point_operator_ne =
R"doc(Inequality operator.

Parameter ``other``:
    Other parameter point to compare with.

Returns:
    `true` if the parameter points are not equal.)doc";

static const char *__doc_fiction_parameter_point_parameter_point = R"doc(Default constructor.)doc";

static const char *__doc_fiction_parameter_point_parameter_point_2 =
R"doc(Standard constructor.

Parameter ``values``:
    Parameter values for each dimension.)doc";

static const char *__doc_fiction_parameter_point_parameters = R"doc(Parameter values for each dimension.)doc";

static const char *__doc_fiction_path_collection =
R"doc(An ordered collection of multiple paths in a layout.

Template parameter ``Path``:
    Path type.)doc";

static const char *__doc_fiction_path_collection_add = R"doc()doc";

static const char *__doc_fiction_path_collection_contains = R"doc()doc";

static const char *__doc_fiction_path_set =
R"doc(A set of multiple paths in a layout.

Template parameter ``Path``:
    Path type.)doc";

static const char *__doc_fiction_path_set_add = R"doc()doc";

static const char *__doc_fiction_path_set_contains = R"doc()doc";

static const char *__doc_fiction_physical_population_stability =
R"doc(This function simulates the population stability of each physically
valid charge distributions of a given SiDB layout. It determines the
minimum absolute electrostatic potential required to induce a charge
distribution transition. The function also identifies the SiDB for
which this is the case (critical SiDB) and the corresponding charge
state transition (i.e., the change from one charge state to another).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout for which the population stability is simulated.

Parameter ``params``:
    Parameters used to simulate the population stability.

Returns:
    A vector of population stability information for all physically
    valid charge distributions of the given SiDB layout.)doc";

static const char *__doc_fiction_physical_population_stability_params =
R"doc(This struct stores the parameters required to simulate the population
stability.)doc";

static const char *__doc_fiction_physical_population_stability_params_precision_for_distance_corresponding_to_potential =
R"doc(The precision level for the conversion from the minimum potential
difference to the corresponding distance.)doc";

static const char *__doc_fiction_physical_population_stability_params_simulation_parameters = R"doc(Parameters for the electrostatic potential.)doc";

static const char *__doc_fiction_physically_valid_parameters =
R"doc(This function computes the physical parameters necessary for ensuring
the physical validity of a given charge distribution and determines
the corresponding excited state number. The ground state is denoted by
zero, with each subsequent excited state incrementally numbered.

This function is designed to derive the physical parameters from
charge distribution measurements of SiDB layouts, often acquired
through Atomic Force Microscopy (AFM). Given a specific charge
distribution, the function typically yields several physically valid
parameters.

As more SiDB layouts with corresponding charge distributions are
recorded, the number of physically valid parameters for all layouts
decreases. Consequently, this enables a more precise determination of
the physical parameters present on the surface.

Template parameter ``Lyt``:
    The charge distribution surface type.

Parameter ``cds``:
    The charge distribution surface for which physical parameters are
    to be determined.

Parameter ``params``:
    Operational domain parameters.

Returns:
    Physically valid parameters with the corresponding excited state
    number of the given charge distribution surface for each parameter
    point.)doc";

static const char *__doc_fiction_place =
R"doc(Place 0-input gates.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place a 0-input gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Returns:
    Signal pointing to the placed gate in `lyt`.)doc";

static const char *__doc_fiction_place_2 =
R"doc(Place 1-input gates.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place a 1-input gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Parameter ``a``:
    Incoming signal to the newly placed gate in `lyt`.

Returns:
    Signal pointing to the placed gate in `lyt`.)doc";

static const char *__doc_fiction_place_3 =
R"doc(Place 2-input gates.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place a 2-input gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Parameter ``a``:
    First incoming signal to the newly placed gate in `lyt`.

Parameter ``b``:
    Second incoming signal to the newly placed gate in `lyt`.

Parameter ``c``:
    Third optional incoming constant value signal to the newly placed
    gate in `lyt`. Might change the gate function when set, e.g., from
    a MAJ to an AND if `c == false`.

Returns:
    Signal pointing to the placed gate in `lyt`.)doc";

static const char *__doc_fiction_place_4 =
R"doc(Place 3-input gates.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place a 3-input gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Parameter ``a``:
    First incoming signal to the newly placed gate in `lyt`.

Parameter ``b``:
    Second incoming signal to the newly placed gate in `lyt`.

Parameter ``c``:
    Third incoming signal to the newly placed gate in `lyt`.

Returns:
    Signal pointing to the placed gate in `lyt`.)doc";

static const char *__doc_fiction_place_5 =
R"doc(Place any gate from a network. This function automatically identifies
the arity of the passed node and fetches its incoming signals from the
given network and a provided `mockturtle::node_map`. This function
does not update the `mockturtle::node_map`.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place any gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Parameter ``node2pos``:
    Mapping from network nodes to layout signals, i.e., a pointer to
    their position in the layout. The map is used to fetch location of
    the fanins. The `mockturtle::node_map` is not updated by this
    function.

Returns:
    Signal to the newly placed gate in `lyt`.)doc";

static const char *__doc_fiction_place_6 =
R"doc(Place any gate from a network. This function automatically identifies
the arity of the passed node and fetches its incoming signals from the
given network and a provided branching_signal_container
`mockturtle::node_map`. This function does not update the
`mockturtle::node_map`.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Ntk``:
    Logic network type.

Parameter ``lyt``:
    Gate-level layout in which to place any gate.

Parameter ``t``:
    Tile in `lyt` to place the gate onto.

Parameter ``ntk``:
    Network whose node is to be placed.

Parameter ``n``:
    Node in `ntk` to place onto `t` in `lyt`.

Parameter ``node2pos``:
    Mapping from network nodes to layout signals, i.e., a pointer to
    their position in the layout via branches. The map is used to
    fetch location of the fanins. The `mockturtle::node_map` is not
    updated by this function.

Returns:
    Signal to the newly placed gate in `lyt`.)doc";

static const char *__doc_fiction_pointy_top_hex = R"doc(\verbatim / \ / \ | | | | \ / \ / \endverbatim)doc";

static const char *__doc_fiction_population_stability_information =
R"doc(This struct encapsulates information related to the population
stability of a charge distribution. It includes details about the SiDB
closest to a charge transition (critical cell), the specific charge
state transition, the electrostatic potential difference required for
the transition, the corresponding distance, and the total
electrostatic energy of the given charge distribution.

Template parameter ``Lyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_population_stability_information_critical_cell = R"doc(SiDB cell which is closest to a charge transition.)doc";

static const char *__doc_fiction_population_stability_information_distance_corresponding_to_potential =
R"doc(This map collects for all charge transition types, the electrostatic
potential difference which is required to conduct a charge change as a
distance in nanometer. This is possible since the electrostatic
potential is connected to the distance.)doc";

static const char *__doc_fiction_population_stability_information_system_energy = R"doc(Total electrostatic energy (unit: eV) of given charge distribution.)doc";

static const char *__doc_fiction_population_stability_information_transition_potentials =
R"doc(This map collects all charge transition types, the corresponding
critical cells and the required electrostatic potential (unit: V)
required to conduct the transition.)doc";

static const char *__doc_fiction_port_direction =
R"doc(A port direction is a relative (cardinal) direction of a port within a
tile. Useful, when no exact port locations within a tile are needed.)doc";

static const char *__doc_fiction_port_direction_cardinal = R"doc(Cardinal direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_EAST = R"doc(East direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_NONE = R"doc(None direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH = R"doc(North direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH_EAST = R"doc(North-East direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH_WEST = R"doc(North-West direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH = R"doc(South direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH_EAST = R"doc(South-East direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH_WEST = R"doc(South-West direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_WEST = R"doc(West direction.)doc";

static const char *__doc_fiction_port_direction_dir = R"doc(Direction.)doc";

static const char *__doc_fiction_port_direction_operator_eq =
R"doc(Comparator for equality tests.

Parameter ``p``:
    Port to compare to.

Returns:
    `true` iff this port is equal to given port `p`.)doc";

static const char *__doc_fiction_port_direction_operator_lt =
R"doc(Comparator for set insertion.

Parameter ``p``:
    Port to compare to.

Returns:
    `true` iff this port goes before `p` in set.)doc";

static const char *__doc_fiction_port_direction_pi = R"doc(Primary input port.)doc";

static const char *__doc_fiction_port_direction_po = R"doc(Primary output port.)doc";

static const char *__doc_fiction_port_direction_port_direction = R"doc(Default constructor.)doc";

static const char *__doc_fiction_port_direction_port_direction_2 = R"doc(Standard constructor.)doc";

static const char *__doc_fiction_port_direction_to_coordinate =
R"doc(Port directions address coordinates relative to each other by
specifying cardinal directions. This function converts such a relative
direction to an absolute coordinate when given a layout and a
coordinate therein to consider. That is, when presented with, e.g., a
`NORTH_EAST` direction, it will return the coordinate that is to the
`NORTH_EAST` of the given coordinate `c` in the layout `lyt`.

Template parameter ``Lyt``:
    Coordinate layout type.

Parameter ``lyt``:
    Coordinate layout.

Parameter ``c``:
    Coordinate to consider.

Parameter ``port``:
    Port direction.

Returns:
    Absolute coordinate specified by a coordinate `c` in layout `lyt`
    and a port direction.)doc";

static const char *__doc_fiction_port_list =
R"doc(Port lists are collections of input and output ports.

Template parameter ``PortType``:
    A port type, e.g., port_position or port_direction.)doc";

static const char *__doc_fiction_port_list_inp = R"doc(Input and output positions.)doc";

static const char *__doc_fiction_port_list_operator_eq =
R"doc(Comparator for port lists.

Parameter ``p``:
    Ports to compare to.

Returns:
    `true` iff these ports are equal to `p`.)doc";

static const char *__doc_fiction_port_list_operator_iadd =
R"doc(Merges two `port_list` objects together. The given `port_list` might
be altered.

Parameter ``p``:
    Ports to merge.

Returns:
    Merged port lists.)doc";

static const char *__doc_fiction_port_list_out = R"doc()doc";

static const char *__doc_fiction_port_list_port_list = R"doc(Default constructor.)doc";

static const char *__doc_fiction_port_list_port_list_2 = R"doc(Standard constructor.)doc";

static const char *__doc_fiction_port_position = R"doc(A port position is a relative location of a cell within a tile.)doc";

static const char *__doc_fiction_port_position_operator_eq =
R"doc(Comparator for equality tests.

Parameter ``p``:
    Port to compare to.

Returns:
    `true` iff this port is equal to given port `p`.)doc";

static const char *__doc_fiction_port_position_operator_lt =
R"doc(Comparator for set insertion.

Parameter ``p``:
    Port to compare to.

Returns:
    `true` iff this port goes before `p` in set.)doc";

static const char *__doc_fiction_port_position_pi = R"doc(Primary input port.)doc";

static const char *__doc_fiction_port_position_po = R"doc(Primary output port.)doc";

static const char *__doc_fiction_port_position_port_position = R"doc(Default constructor.)doc";

static const char *__doc_fiction_port_position_port_position_2 = R"doc(Standard constructor.)doc";

static const char *__doc_fiction_port_position_x = R"doc(Positions.)doc";

static const char *__doc_fiction_port_position_y = R"doc()doc";

static const char *__doc_fiction_post_layout_optimization =
R"doc(A post-layout optimization algorithm as originally proposed in \"Post-
Layout Optimization for Field-coupled Nanotechnologies\" by S.
Hofmann, M. Walter, and R. Wille in NANOARCH 2023
(https://dl.acm.org/doi/10.1145/3611315.3633247). It can be used to
reduce the area of a given sub-optimal Cartesian gate-level layout
created by heuristics or machine learning. This optimization utilizes
the distinct characteristics of the 2DDWave clocking scheme, which
only allows information flow from top to bottom and left to right,
therefore only aforementioned clocking scheme is supported.

To reduce the layout area, first, gates are moved up and to the left
as far as possible, including rerouting. This creates more compact
layouts by freeing up space to the right and bottom, as all gates were
moved to the top left corner.

After moving all gates, this algorithm also checks if excess wiring
exists on the layout using the `wiring_reduction` algorithm (cf.
`wiring_reduction.hpp`)

As outputs have to lay on the border of a layout for better
accessibility, they are also moved to new borders determined based on
the location of all other gates.

@note This function requires the gate-level layout to be 2DDWave-
clocked!

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    2DDWave-clocked Cartesian gate-level layout to optimize.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_post_layout_optimization_params = R"doc(Parameters for the post-layout optimization algorithm.)doc";

static const char *__doc_fiction_post_layout_optimization_params_max_gate_relocations =
R"doc(Maximum number of relocations to try for each gate. Defaults to the
number of tiles in the given layout if not specified.)doc";

static const char *__doc_fiction_post_layout_optimization_params_optimize_pos_only = R"doc(Only optimize PO positions.)doc";

static const char *__doc_fiction_post_layout_optimization_params_planar_optimization =
R"doc(Disable the creation of crossings during optimization. If set to true,
gates will only be relocated if a crossing-free wiring is found.
Defaults to false.)doc";

static const char *__doc_fiction_post_layout_optimization_params_timeout =
R"doc(Timeout limit (in ms). Specifies the maximum allowed time in
milliseconds for the optimization process. For large layouts, the
actual execution time may slightly exceed this limit because it's
impractical to check the timeout at every algorithm step and the
functional correctness has to be ensured by completing essential
algorithm steps.)doc";

static const char *__doc_fiction_post_layout_optimization_stats =
R"doc(This struct stores statistics about the post-layout optimization
process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_area_improvement = R"doc(Area reduction (in %) after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_duration = R"doc(Runtime of the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_num_crossings_after = R"doc(Number of crossings after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_num_crossings_before = R"doc(Number of crossings before the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_num_wires_after = R"doc(Number of wire segments after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_num_wires_before = R"doc(Number of wire segments before the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_x_size_after = R"doc(Layout width after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_x_size_before = R"doc(Layout width before the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_y_size_after = R"doc(Layout height after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_y_size_before = R"doc(Layout height before the post-layout optimization process.)doc";

static const char *__doc_fiction_potential_bound_top =
R"doc(The respective *top* elements of the lower semi-lattices in which our
potential bounds live, ie., the respective elements of *most*
information. This means that any meet with a top element (weakly)
reduces the information, and thus any meet computed in iteration
through binary application may start out with this element.

Template parameter ``bound``:
    Potential bound domain to return the element of most information
    of.

Returns:
    The element of most information respective to the potential bound
    domain.)doc";

static const char *__doc_fiction_potential_bounds_store =
R"doc(This defines a store in which the bounds on the local electrostatic
potential for an SiDB (index) may be stored. For the *Ground State
Space* algorithm, this is used to keep track of the respective lower
and upper bounds on the partial sum of the potential projected from
SiDBs in a subhierarchy that is local to SiDBs that are also in the
subhierarchy. During *ClusterComplete* simulation, the stored
potential bounds represent information of the complete hierarchy, thus
all SiDB interactions.)doc";

static const char *__doc_fiction_potential_bounds_store_get =
R"doc(Getter for a (partial) potential sum bound local to an SiDB.

Template parameter ``bound``:
    The potential bound to obtain.

Parameter ``sidb_ix``:
    SiDB (index) to obtain the potential bound of.

Returns:
    The potential bound for this SiDB.)doc";

static const char *__doc_fiction_potential_bounds_store_initialize_complete_potential_bounds =
R"doc(Initialize potential bounds for the given number of SiDBs (applicable
to a complete potential bounds store only).

Parameter ``num_sidbs``:
    The number of SiDBs in the layout that is simulated.)doc";

static const char *__doc_fiction_potential_bounds_store_num_sidbs =
R"doc(Getter for the size of the potential bounds store, i.e., the number of
SiDBs considered in this store.

Returns:
    The size of the potential bounds store.)doc";

static const char *__doc_fiction_potential_bounds_store_operator_iadd =
R"doc(Add a complete potential bound store to this (also a complete
potential bound store) through pointwise updates.

Parameter ``other``:
    Other complete potential bound store.

Returns:
    Reference to this.)doc";

static const char *__doc_fiction_potential_bounds_store_operator_isub =
R"doc(Subtract a complete potential bound store to this (also a complete
potential bound store) through pointwise updates, i.e., updates for
each SiDB and for each bound (LB, UB).

Parameter ``other``:
    Other complete potential bound store.

Returns:
    Reference to this.)doc";

static const char *__doc_fiction_potential_bounds_store_set =
R"doc(Setter for a (partial) potential sum bound local to an SiDB.

Template parameter ``bound``:
    The potential bound to obtain.

Parameter ``sidb_ix``:
    SiDB (index) to set the potential bound for.

Parameter ``bound_value``:
    New bound to set.)doc";

static const char *__doc_fiction_potential_bounds_store_set_2 =
R"doc(Setter for (partial) potential sum bounds local to an SiDB.

Parameter ``sidb_ix``:
    SiDB (index) to set the potential bounds for.

Parameter ``min``:
    New lower bound to set.

Parameter ``max``:
    New upper bound to set.)doc";

static const char *__doc_fiction_potential_bounds_store_store =
R"doc(Potential bounds are a map from SiDB indices to two values
respectively representing the lower and upper bound.)doc";

static const char *__doc_fiction_potential_bounds_store_update =
R"doc(Relative setter for a (partial) potential sum bound local to an SiDB.

Template parameter ``bound``:
    The potential bound to update.

Parameter ``sidb_ix``:
    SiDB (index) to update the potential bound of.

Parameter ``bound_diff``:
    Bound difference to apply.)doc";

static const char *__doc_fiction_potential_bounds_store_update_2 =
R"doc(Relative setter for (partial) potential sum bounds local to an SiDB.

Parameter ``sidb_ix``:
    SiDB (index) to update the potential bounds of.

Parameter ``min_diff``:
    Difference in lower bound potential to apply.

Parameter ``max_diff``:
    Difference in upper bound potential to apply.)doc";

static const char *__doc_fiction_potential_projection =
R"doc(This struct defines the type of an electrostatic potential projection,
which pairs a multiset charge configuration with the potential value
(in eV) associated with the potential projection in the given context.
The context is given by potential projection stores in the decorated
cluster hierarchies, which links the projecting cluster with SiDB that
receives this potential projection.)doc";

static const char *__doc_fiction_potential_projection_multiset = R"doc(Associated multiset charge configuration.)doc";

static const char *__doc_fiction_potential_projection_operator_iadd =
R"doc(Defines summation of potential projections through addition of the
potential values and concatenation of the associated multiset charge
configurations. The latter may simply be implemented by addition of
the compressed forms.

Parameter ``other``:
    Other potential projection to sum with the current.

Returns:
    The current potential projection to which the other potential
    projection is now added.)doc";

static const char *__doc_fiction_potential_projection_operator_lt =
R"doc(Defines an ordering of potential projections through comparison of the
potential value. To prevent potential projections of equal potential
value but different associated multiset charge configurations to be
regarded as equal, a comparison of the latter is used as a "fail-
safe".

Parameter ``other``:
    Other potential projection to compare with the current.

Returns:
    `true` if and only if the potential value of the current is lower
    than that of `other`, or if the potential values are equal and the
    compressed form of the multiset charge configuration is strictly
    less than that of `other`.)doc";

static const char *__doc_fiction_potential_projection_order =
R"doc(This struct defines the type of potential projection order, along with
getter and setter operations. Essentially, a potential projection
order is an ordered collection of potential projections, which allow
rapid access and modifications of the potential projection bounds by
relying on the ordering defined on potential projections, and
canonical operations on ordered set containers. In the decorated
cluster hierarchy, each cluster has a potential projection order onto
each SiDB in the layout from which the hierarchy was created.)doc";

static const char *__doc_fiction_potential_projection_order_add =
R"doc(Adds a potential projection to the ordering, automatically placing it
appropriately.

Parameter ``pp``:
    Potential projection to add.)doc";

static const char *__doc_fiction_potential_projection_order_get_bound =
R"doc(A getter for a potential projection bound, which is the first or last
item in the ordered set.

Template parameter ``bound``:
    The bound to obtain.

Returns:
    The potential projection that forms the requested bound on the
    potential projection order.)doc";

static const char *__doc_fiction_potential_projection_order_get_next_bound =
R"doc(A getter for the next potential projection bound, which is the first
potential projection when traversing the ordering either from below or
from above, that differs in its multiset charge configuration from the
relevant potential projection bound.

Template parameter ``bound``:
    The bound to obtain.

Returns:
    The potential projection that would be the requested bound on the
    potential projection order if the current relevant bound would be
    erased.)doc";

static const char *__doc_fiction_potential_projection_order_get_pot_proj_for_m_conf =
R"doc(A getter for the potential projection bound given a multiset charge
configuration specification, which is the first potential projection
in the ordering when traversing either from below or from above, that
matches its multiset charge configuration to the argument.

Template parameter ``bound``:
    The bound to obtain.

Parameter ``m_conf``:
    The multiset charge configuration to match.

Returns:
    The potential projection that forms the requested bound on the
    subset of the potential projection order of potential projections
    that match their multiset charge configuration to the argument.)doc";

static const char *__doc_fiction_potential_projection_order_order = R"doc(The potential projection ordering.)doc";

static const char *__doc_fiction_potential_projection_order_potential_projection_order = R"doc(Default constructor, creating the empty potential projection order.)doc";

static const char *__doc_fiction_potential_projection_order_potential_projection_order_2 =
R"doc(Constructor for a potential projection from a singleton cluster onto
the SiDB contained in it.

Parameter ``loc_ext_pot``:
    The local external potential at the SiDB in the singleton cluster.
    Specifically, this is the sum of the local defect potential and
    the local external potential.

Parameter ``base``:
    The simulation base. This defines whether positive charges are
    considered.

Parameter ``self_projection``:
    Separates the constructor type from inter-SiDB potential
    projections.)doc";

static const char *__doc_fiction_potential_projection_order_potential_projection_order_3 =
R"doc(Constructor for a potential projection from a singleton cluster onto
an SiDB.

Parameter ``inter_sidb_pot``:
    The chargeless potential between the SiDB in the singleton cluster
    and the one projected onto, as found in the potential matrix in an
    associated `charge_distribution_surface` object.

Parameter ``base``:
    The simulation base. This defines whether positive charges are
    considered.)doc";

static const char *__doc_fiction_potential_projection_order_remove_m_conf =
R"doc(Removes all occurrences of potential projections that match their
multiset charge configuration to the argument.

Parameter ``m_conf``:
    The multiset charge configuration of which all occurrences must be
    removed.)doc";

static const char *__doc_fiction_potential_projection_pot_val = R"doc(Potential projection value (unit: eV).)doc";

static const char *__doc_fiction_potential_projection_potential_projection =
R"doc(Default constructor, used as a starting point for an accumulation of
potential projections.)doc";

static const char *__doc_fiction_potential_projection_potential_projection_2 =
R"doc(Trivial copy constructor.

Parameter ``pot``:
    Potential value to copy.

Parameter ``mul``:
    Multiset charge configuration to copy.)doc";

static const char *__doc_fiction_potential_projection_potential_projection_3 =
R"doc(Constructor for a potential projection from a singleton cluster,
thereby lifting a value in the potential matrix to a potential
projection.

Parameter ``inter_sidb_pot``:
    Potential value of which the absolute value may be found in the
    potential matrix in an associated `charge_distribution_surface`
    object.

Parameter ``cs``:
    Charge state associated with the singleton cluster projector for
    this potential projection.)doc";

static const char *__doc_fiction_potential_to_distance_conversion =
R"doc(The electrostatic potential on hydrogen-passivated silicon is
typically modeled using a screened Coulomb potential. This
electrostatic potential is commonly employed to determine the
electrostatic potential for a given distance (between SiDB and point
under consideration) and given physical parameters. However, the
function provided here serves the inverse purpose by calculating the
distance for a given potential and given physical parameters.

@note Runtime depends exponentially on the provided precision.

Parameter ``params``:
    The physical parameters for a given hydrogen-passivated silicon
    surface.

Parameter ``potential``:
    The electrostatic potential (unit: V) to be converted to a
    distance.

Parameter ``precision``:
    The precision level for the conversion, specifying the number of
    decimal places.

Returns:
    The distance (unit: nm) corresponding to the given electrostatic
    potential.)doc";

static const char *__doc_fiction_print_cell_level_layout =
R"doc(Writes a simplified 2D representation of a cell-level layout to an
output stream.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``os``:
    Output stream to write into.

Parameter ``layout``:
    The cell-level layout to print.

Parameter ``io_color``:
    Flag to utilize color escapes for inputs and outputs.

Parameter ``clk_color``:
    Flag to utilize color escapes for clock zones.)doc";

static const char *__doc_fiction_print_gate_level_layout =
R"doc(Writes a simplified 2D representation of a gate-level layout to an
output stream.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``os``:
    Output stream to write into.

Parameter ``layout``:
    The gate-level layout to print.

Parameter ``io_color``:
    Flag to utilize color escapes for inputs and outputs.

Parameter ``clk_color``:
    Flag to utilize color escapes for clock zones.)doc";

static const char *__doc_fiction_print_layout =
R"doc(A unified printer of the versions above. Depending on the passed
layout type, this function will automatically select the appropriate
printer to use. This simplifies printing by enabling the statement
`print_layout(lyt)`.

@note This function will use the respective function's default
settings to print the layout.

Template parameter ``Lyt``:
    Any coordinate layout type.

Parameter ``lyt``:
    The coordinate layout.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_print_sidb_layout =
R"doc(Writes a simplified 2D representation of an SiDB layout (SiDB and
defect charges are supported) to an output stream.

Template parameter ``Lyt``:
    SiDB cell-level layout with charge-information based on SiQAD
    coordinates or defect-information, e.g., a
    `charge_distribution_surface` or `sidb_defect_surface`.

Parameter ``os``:
    Output stream to write into.

Parameter ``lyt``:
    The layout of which the information is to be printed.

Parameter ``lat_color``:
    Flag to utilize color escapes for the lattice, charge states, and
    atomic defects.

Parameter ``crop_layout``:
    Flag to print the 2D bounding box of the layout, while leaving a
    maximum padding of one dimer row and two columns.

Parameter ``draw_lattice``:
    Flag to enable lattice background drawing.)doc";

static const char *__doc_fiction_ptr =
R"doc(Returns a smart pointer to the given scheme.

Template parameter ``Lyt``:
    Layout type.

Parameter ``scheme``:
    Universal reference to a clocking scheme.

Returns:
    A shared pointer to the given `scheme`.)doc";

static const char *__doc_fiction_qca_energy_dissipation =
R"doc(Estimates the energy dissipation of a gate-level layout if it were to
be converted to a QCA cell-level layout via the QCA ONE gate library.
This estimation was proposed in \"An Energy-aware Model for the Logic
Synthesis of Quantum-Dot Cellular Automata\" by Frank Sill Torres,
Robert Wille, Philipp Niemann, and Rolf Drechsler in TCAD 2018.

As done in that publication, energy dissipation values are being given
in meV for slow (25 GHz) and fast (100 GHz) clocking.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The gate-level layout whose energy dissipation is to be
    calculated.

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_qca_one_library =
R"doc(A concrete FCN gate library based on QCA ONE proposed in \"A
Methodology for Standard Cell Design for QCA\" by Dayane Alfenas Reis,
Caio Araújo T. Campos, Thiago Rodrigues B. S. Soares, Omar Paranaiba
V. Neto, and Frank Sill Torres in IEEE International Symposium on
Circuits and Systems, 2016. QCA ONE was originally proposed for the
USE clocking scheme. The version used here is an extension to the
original QCA ONE by also theoretically allowing multiple wires in the
same tile. Furthermore, it can be used for a range of clocking
schemes. Tiles in QCA ONE are :math:`5 \times 5` QCA cells.)doc";

static const char *__doc_fiction_qca_one_library_determine_port_routing = R"doc()doc";

static const char *__doc_fiction_qca_one_library_post_layout_optimization =
R"doc(Post-layout optimization that assigns via cell mode to wire crossings.

Template parameter ``CellLyt``:
    Cell-level layout type.

Parameter ``lyt``:
    The cell-level layout that has been created via application of
    `set_up_gate`.)doc";

static const char *__doc_fiction_qca_one_library_qca_one_library = R"doc()doc";

static const char *__doc_fiction_qca_one_library_set_up_gate =
R"doc(Overrides the corresponding function in fcn_gate_library. Given a tile
`t`, this function takes all necessary information from the stored
grid into account to choose the correct fcn_gate representation for
that tile. May it be a gate or wires. Rotation and special marks like
input and output, const cells etc. are computed additionally.

Template parameter ``GateLyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    Layout that hosts tile `t`.

Parameter ``t``:
    Tile to be realized as a QCA ONE gate.

Returns:
    QCA ONE gate representation of `t` including I/Os, rotation, const
    cells, etc.)doc";

static const char *__doc_fiction_qca_technology =
R"doc(Quantum-dot Cellular Automata (QCA) technology implementation of the
FCN concept.)doc";

static const char *__doc_fiction_qca_technology_cell_mark = R"doc(Possible marks to be applied to a cell to change its type.)doc";

static const char *__doc_fiction_qca_technology_cell_mark_EMPTY = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mark_INPUT = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mark_OUTPUT = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mode = R"doc(Possible cell modes for QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_mode_CROSSOVER = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mode_NORMAL = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mode_ROTATED = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_mode_VERTICAL = R"doc()doc";

static const char *__doc_fiction_qca_technology_cell_type = R"doc(Possible types of QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_CONST_0 = R"doc(Symbol used for constant 0 input QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_CONST_1 = R"doc(Symbol used for constant 1 input QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_EMPTY = R"doc(Symbol used for empty QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_INPUT = R"doc(Symbol used for input QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_NORMAL = R"doc(Symbol used for normal QCA cells.)doc";

static const char *__doc_fiction_qca_technology_cell_type_OUTPUT = R"doc(Symbol used for output QCA cells.)doc";

static const char *__doc_fiction_qca_technology_is_const_0_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_const_1_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_constant_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_crossover_cell_mode = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_empty_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_input_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_normal_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_normal_cell_mode = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_output_cell = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_rotated_cell_mode = R"doc()doc";

static const char *__doc_fiction_qca_technology_is_vertical_cell_mode = R"doc()doc";

static const char *__doc_fiction_qca_technology_qca_technology = R"doc()doc";

static const char *__doc_fiction_quickexact =
R"doc(*QuickExact* is a quick and exact physical simulation algorithm
designed specifically for SiDB layouts. It was proposed in \"The Need
for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic\"
by J. Drewniok, M. Walter, and R. Wille in ASP-DAC 2024
(https://ieeexplore.ieee.org/document/10473946). It determines all
physically valid charge configurations of a given SiDB layout,
providing a significant performance advantage of more than three
orders of magnitude over *ExGS*
(`exhaustive_ground_state_simulation`).

The performance improvement of *QuickExact* can be attributed to the
incorporation of three key ideas:

1. Advanced Negative SiDB Detection: *QuickExact* efficiently
identifies SiDBs that require negative charges in a physically valid
charge distribution. By pre-assigned them in advance, the search space
is pruned by a factor of :math:`2^k`, where k is the number of found
SiDBs.

2. Dependent SiDB Selection: The algorithm selects a dependent SiDB,
whose charge state is always derived from its n-1 neighbors. This
dependency simplifies the computation process and contributes to the
overall efficiency of *QuickExact*.

3. Gray Code Representation: *QuickExact* employs Gray code to
represent and traverse through all charge configurations. By using
Gray code, only one charge state changes at a time, making the
computation of the local electrostatic potential easier.

Additionally, *QuickExact* also considers global and local
electrostatic potentials, as well as existing defects. This holistic
approach ensures an accurate representation of the physical behavior
of the SiDB layout.

In summary, *QuickExact* combines advanced SiDB charge detection,
dependent SiDB selection, and the use of Gray code to achieve
outstanding performance and enable efficient simulations of SiDB
layouts, even in scenarios where positively-charged SiDBs occur due to
small spacing.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout to simulate.

Parameter ``params``:
    Parameter required for the simulation.

Returns:
    Simulation Results.)doc";

static const char *__doc_fiction_quickexact_params =
R"doc(This struct stores the parameters for the *QuickExact* algorithm.

Template parameter ``CellType``:
    Cell type.)doc";

static const char *__doc_fiction_quickexact_params_automatic_base_number_detection = R"doc(Modes to use for the *QuickExact* algorithm.)doc";

static const char *__doc_fiction_quickexact_params_automatic_base_number_detection_OFF =
R"doc(The base number from the physical parameter is used for the
simulation.)doc";

static const char *__doc_fiction_quickexact_params_automatic_base_number_detection_ON =
R"doc(Simulation is conducted with the required base number (i.e., if
positively charged SiDBs can occur, three state simulation is
conducted).)doc";

static const char *__doc_fiction_quickexact_params_base_number_detection =
R"doc(If `ON`, *QuickExact* checks which base number is required for the
simulation, i.e., whether 3-state is necessary or 2-state simulation
is sufficient.)doc";

static const char *__doc_fiction_quickexact_params_global_potential =
R"doc(Global external electrostatic potential. Value is applied on each cell
in the layout.)doc";

static const char *__doc_fiction_quickexact_params_local_external_potential =
R"doc(Local external electrostatic potentials (e.g., locally applied
electrodes).)doc";

static const char *__doc_fiction_quickexact_params_simulation_parameters = R"doc(All parameters for physical SiDB simulations.)doc";

static const char *__doc_fiction_quicksim =
R"doc(The *QuickSim* algorithm which was proposed in \"QuickSim: Efficient
and Accurate Physical Simulation of Silicon Dangling Bond Logic\" by
J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille in IEEE
NANO 2023 (https://ieeexplore.ieee.org/document/10231266) is an
electrostatic ground state simulation algorithm for SiDB layouts. It
determines physically valid charge configurations (with minimal
energy) of a given (already initialized) charge distribution layout.
Depending on the simulation parameters, the ground state is found with
a certain probability after one run.

@note *QuickSim* currently does not support atomic defect simulation.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``ps``:
    QuickSim parameters.

Returns:
    `sidb_simulation_result` is returned if the simulation was
    successful, otherwise `std::nullopt`.)doc";

static const char *__doc_fiction_quicksim_params = R"doc(This struct stores the parameters for the *QuickSim* algorithm.)doc";

static const char *__doc_fiction_quicksim_params_alpha =
R"doc(`alpha` parameter for the *QuickSim* algorithm (should be reduced if
no result is found).)doc";

static const char *__doc_fiction_quicksim_params_iteration_steps = R"doc(Number of iterations to run the simulation for.)doc";

static const char *__doc_fiction_quicksim_params_number_threads =
R"doc(Number of threads to spawn. By default the number of threads is set to
the number of available hardware threads.)doc";

static const char *__doc_fiction_quicksim_params_simulation_parameters = R"doc(Simulation parameters for the simulation of the physical SiDB system.)doc";

static const char *__doc_fiction_quicksim_params_timeout = R"doc(Timeout limit (in ms).)doc";

static const char *__doc_fiction_random_coordinate =
R"doc(Generates a random coordinate within the region spanned by two given
coordinates. The two given coordinates form the top left corner and
the bottom right corner of the spanned region.

Template parameter ``CoordinateType``:
    The coordinate implementation to be used.

Parameter ``coordinate1``:
    Top left Coordinate.

Parameter ``coordinate2``:
    Bottom right Coordinate (coordinate order is not important,
    automatically swapped if necessary).

Returns:
    Randomly generated coordinate.)doc";

static const char *__doc_fiction_random_cost =
R"doc(Random cost between two layout coordinates that returns a random value
between 0 and 1.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Cost``:
    Floating-point type for the cost.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Random cost between `source` and `target`, i.e., a random number
    between 0 and 1.)doc";

static const char *__doc_fiction_random_cost_functor =
R"doc(A pre-defined cost functor that uses random costs.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Cost``:
    Floating-point cost type.)doc";

static const char *__doc_fiction_random_cost_functor_random_cost_functor = R"doc()doc";

static const char *__doc_fiction_range_t =
R"doc(Defines a range type utilizing iterators. It implements `begin()` and
`end()` as well as `cbegin()` and `cend()` as member functions to work
for range based for-loops.

Template parameter ``Iterator``:
    type for which the range should be created.)doc";

static const char *__doc_fiction_range_t_b = R"doc(Iterators pointing to the begin and the end of the represented range.)doc";

static const char *__doc_fiction_range_t_begin =
R"doc(Returns the iterator pointing to the begin of the represented range.

Returns:
    Begin iterator.)doc";

static const char *__doc_fiction_range_t_cbegin =
R"doc(Returns a const iterator pointing to the begin of the represented
range.

Returns:
    const begin iterator.)doc";

static const char *__doc_fiction_range_t_cend =
R"doc(Returns a const iterator pointing to the end of the represented range.

Returns:
    const end iterator.)doc";

static const char *__doc_fiction_range_t_e = R"doc(Iterators pointing to the begin and the end of the represented range.)doc";

static const char *__doc_fiction_range_t_end =
R"doc(Returns the iterator pointing to the end of the represented range.

Returns:
    End iterator.)doc";

static const char *__doc_fiction_range_t_range_t =
R"doc(Standard constructor with forward reference.

Parameter ``range``:
    Begin and end iterator pair.)doc";

static const char *__doc_fiction_read_fgl_layout =
R"doc(Reads a gate-level layout from an FGL file provided as an input
stream.

May throw an `fgl_parsing_error` if the FGL file is malformed.

Template parameter ``Lyt``:
    The layout type to be created from an input.

Parameter ``is``:
    The input stream to read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_fgl_layout_2 =
R"doc(Reads a gate-level layout from an FGL file provided as an input
stream.

May throw an `fgl_parsing_error` if the FGL file is malformed.

This is an in-place version of `read_fgl_layout` that utilizes the
given layout as a target to write to.

Template parameter ``Lyt``:
    The layout type to be used as input.

Parameter ``lyt``:
    The layout to write to.

Parameter ``is``:
    The input stream to read from.)doc";

static const char *__doc_fiction_read_fgl_layout_3 =
R"doc(Reads a gate-level layout from an FGL file provided as a file name.

May throw an `fgl_parsing_error` if the FGL file is malformed.

Template parameter ``Lyt``:
    The layout type to be created from an input.

Parameter ``filename``:
    The file name to open and read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_fgl_layout_4 =
R"doc(Reads a gate-level layout from an FGL file provided as a file name.

May throw an `fgl_parsing_error` if the FGL file is malformed.

This is an in-place version of `read_fgl_layout` that utilizes the
given layout as a target to write to.

Template parameter ``Lyt``:
    The layout type to be used as input.

Parameter ``lyt``:
    The layout to write to.

Parameter ``filename``:
    The file name to open and read from.)doc";

static const char *__doc_fiction_read_fqca_layout =
R"doc(Reads a cell-level QCA layout from an fqca file provided as an input
stream. The format is used by QCA-STACK by Willem Lambooy
(https://github.com/wlambooy/QCA-STACK).

May throw an unsupported_character_exception,
undefined_cell_label_exception, or
unrecognized_cell_definition_exception.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be a clocked
    cell-level QCA layout.

Parameter ``is``:
    The input stream to read from.

Parameter ``layout_name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_fqca_layout_2 =
R"doc(Reads a cell-level QCA layout from an fqca file provided as a file
name. The format is used by QCA-STACK by Willem Lambooy
(https://github.com/wlambooy/QCA-STACK).

May throw an unsupported_character_exception,
undefined_cell_label_exception, or
unrecognized_cell_definition_exception.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be a clocked
    cell-level QCA layout.

Parameter ``filename``:
    The file name to open and read from.

Parameter ``layout_name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_sidb_surface_defects =
R"doc(Reads a defective SiDB surface from a text file provided as an input
stream. The format is rudimentary and consists of a simple 2D array of
integers representing defect indices printed by Python.

May throw a `missing_sidb_position_exception` or an
`unsupported_defect_index_exception`.

@note For testing purposes, a Python script that generates defective
surfaces is provided in the `experiments/defect_aware_physical_design`
directory.

Template parameter ``Lyt``:
    The layout type underlying the SiDB surface. Must be a cell-level
    SiDB layout.

Parameter ``is``:
    The input stream to read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_sidb_surface_defects_2 =
R"doc(Reads a defective SiDB surface from a text file provided as a file
name. The format is rudimentary and consists of a simple 2D array of
integers representing defect indices printed by Python.

May throw a `missing_sidb_position_exception` or an
`unsupported_defect_index_exception`.

@note For testing purposes, a Python script that generates defective
surfaces is provided in the `experiments/defect_aware_physical_design`
directory.

Template parameter ``Lyt``:
    The layout type underlying the SiDB surface. Must be a cell-level
    SiDB layout.

Parameter ``filename``:
    The file name to open and read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_sqd_layout =
R"doc(Reads a cell-level SiDB layout from an sqd file provided as an input
stream. The format is used by SiQAD (https://github.com/siqad/siqad).

If the provided cell-level layout type can represent SiDB defects,
they will be parsed from the sqd file as well.

May throw an `sqd_parsing_exception` if the sqd file is malformed.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be an SiDB
    lattice cell-level SiDB layout.

Parameter ``is``:
    The input stream to read from.

Parameter ``name``:
    The name to give to the generated layout.

Returns:
    The cell-level SiDB layout read from the sqd file.)doc";

static const char *__doc_fiction_read_sqd_layout_2 =
R"doc(Reads a cell-level SiDB layout from an sqd file provided as an input
stream. The format is used by SiQAD (https://github.com/siqad/siqad).

If The provided cell-level layout type can represent SiDB defects,
they will be parsed from the sqd file as well.

May throw an `sqd_parsing_exception` if the sqd file is malformed.

This is an in-place version of read_sqd_layout that utilizes the given
layout as a target to write to.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be an SiDB
    lattice cell-level SiDB layout.

Parameter ``lyt``:
    The layout to write to.

Parameter ``is``:
    The input stream to read from.)doc";

static const char *__doc_fiction_read_sqd_layout_3 =
R"doc(Reads a cell-level SiDB layout from an sqd file provided as a file
name. The format is used by SiQAD (https://github.com/siqad/siqad).

If The provided cell-level layout type can represent SiDB defects,
they will be parsed from the sqd file as well.

May throw an `sqd_parsing_exception` if the sqd file is malformed.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be an SiDB
    lattice cell-level SiDB layout.

Parameter ``filename``:
    The file name to open and read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_sqd_layout_4 =
R"doc(Reads a cell-level SiDB layout from an sqd file provided as a file
name. The format is used by SiQAD (https://github.com/siqad/siqad).

If The provided cell-level layout type can represent SiDB defects,
they will be parsed from the sqd file as well.

May throw an `sqd_parsing_exception` if the sqd file is malformed.

This is an in-place version of `read_sqd_layout` that utilizes the
given layout as a target to write to.

Template parameter ``Lyt``:
    The layout type to be created from an input. Must be an SiDB
    lattice cell-level SiDB layout.

Parameter ``lyt``:
    The layout to write to.

Parameter ``filename``:
    The file name to open and read from.)doc";

static const char *__doc_fiction_relative_to_absolute_cell_position =
R"doc(Converts a relative cell position within a tile to an absolute cell
position within a layout. To compute the absolute position, the layout
topology is taken into account.

Template parameter ``GateSizeX``:
    Horizontal tile size.

Template parameter ``GateSizeY``:
    Vertical tile size.

Template parameter ``GateLyt``:
    Gate-level layout type.

Template parameter ``CellLyt``:
    Cell-level layout type.

Parameter ``gate_lyt``:
    The gate-level layout whose tiles are to be considered.

Parameter ``t``:
    Tile within gate_lyt.

Parameter ``relative_c``:
    Relative cell position within t.

Returns:
    Absolute cell position in a layout.)doc";

static const char *__doc_fiction_res_clocking =
R"doc(Returns the RES clocking as defined in \"An efficient clocking scheme
for quantum-dot cellular automata\" by Mrinal Goswami, Anindan Mondal,
Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in
International Journal of Electronics Letters 2019.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    RES clocking scheme.)doc";

static const char *__doc_fiction_restore_input_names =
R"doc(Assigns input names from one network to another. Matching inputs are
identified by their index. Since gate-level layout's are network types
as well, this function naturally works for them, too.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Parameter ``ntk_src``:
    Source logic network whose input names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose inputs are to be assigned `ntk_src`'s
    names.)doc";

static const char *__doc_fiction_restore_names =
R"doc(Transfers all input and output names as well as the network/layout
name from one network to another. This function calls
`restore_network_name`, `restore_input_names`, and
`restore_output_names`.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Parameter ``ntk_src``:
    Source logic network whose I/O names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose I/O names are to be assigned
    `ntk_src`'s names.)doc";

static const char *__doc_fiction_restore_names_2 =
R"doc(Transfers all signal and output names as well as the network/layout
name from one network to another. This function calls
`restore_network_name`, `restore_signal_names`, and
`restore_output_names`.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Template parameter ``T``:
    Mapping type to identify signals by. Currently,
    `mockturtle::signal<NtkDest>` and
    `branching_signal_container<NtkDest, NtkSrc, fanout_size>` are
    supported.

Parameter ``ntk_src``:
    Source logic network whose signal names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose signal names are to be assigned
    `ntk_src`'s names.

Parameter ``old2new``:
    Mapping of signals from `ntk_src` to `ntk_dest` using a signal
    identifier.)doc";

static const char *__doc_fiction_restore_network_name =
R"doc(Helper function to conveniently assign the name of a source network or
layout to a target network or layout as they use different function
names for the same purpose. This function comes in handy when networks
are translated or layouts are being created from networks that are
supposed to have the same name.

Template parameter ``NtkOrLytSrc``:
    Source network or layout type.

Template parameter ``NtkOrLytDest``:
    Target network or layout type.

Parameter ``ntk_or_lyt_src``:
    Source network or layout whose name is to be assigned to
    `ntk_or_lyt_dest`.

Parameter ``ntk_or_lyt_dest``:
    Target network or layout that is to be assigned `ntk_or_lyt_src`'s
    name.)doc";

static const char *__doc_fiction_restore_output_names =
R"doc(Assigns output names from one network to another. Matching outputs are
identified by their order. Since gate-level layout's are network types
as well, this function naturally works for them, too.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Parameter ``ntk_src``:
    Source logic network whose output names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose outputs are to be assigned `ntk_src`'s
    names.)doc";

static const char *__doc_fiction_restore_signal_names =
R"doc(Assigns all signal names from one network to another. For this
purpose, a mapping between signals is needed in terms of a
`mockturtle::node_map`. Since gate-level layout's are network types as
well, this function naturally works for them, too.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Parameter ``ntk_src``:
    Source logic network whose signal names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose signal names are to be assigned
    `ntk_src`'s names.

Parameter ``old2new``:
    Mapping of signals from `ntk_src` to `ntk_dest`.)doc";

static const char *__doc_fiction_restore_signal_names_2 =
R"doc(Same as the other restore_signal_names function but this overload uses
a `mockturtle::node_map` with a branching_signal_container that is
specifically used for networks or layouts that allow branches to be
distinct, e.g., by their position on the layout.

Template parameter ``NtkSrc``:
    Source network type.

Template parameter ``NtkDest``:
    Target network type.

Template parameter ``fanout_size``:
    Maximum fanout size in the network.

Parameter ``ntk_src``:
    Source logic network whose signal names are to be transferred to
    `ntk_dest`.

Parameter ``ntk_dest``:
    Target logic network whose signal names are to be assigned
    `ntk_src`'s names.

Parameter ``old2new``:
    Mapping of signals from `ntk_src` to `ntk_dest` using a
    branching_signal_container.)doc";

static const char *__doc_fiction_ripple_clocking =
R"doc(Returns the Ripple clocking as defined in \"Ripple Clock Schemes for
Quantum-dot Cellular Automata Circuits\" by Prafull Purohit, Master
Thesis, Rochester Institute of Technology, 2012.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    Ripple clocking scheme.)doc";

static const char *__doc_fiction_round_to_n_decimal_places =
R"doc(Rounds a number to a specified number of decimal places.

Template parameter ``T``:
    The type of the number to round.

Parameter ``number``:
    The number to round.

Parameter ``n``:
    The number of decimal places to round to.

Returns:
    The number rounded to n decimal places.)doc";

static const char *__doc_fiction_route_path =
R"doc(Establishes a wire routing along the given path in the given layout.
To this end, the given path's source and target coordinates are
assumed to be populated by other gates or wires that the new path
shall connect to.

If `path` contains a tile that is allocated already, it will instead
switch to the crossing layer. If path contains exactly source and
target, no wires are created, but the source and target are connected.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``Path``:
    Path type.

Parameter ``lyt``:
    Gate-level layout in which a wire path is to be established.

Parameter ``path``:
    Path to route wires along.)doc";

static const char *__doc_fiction_routing_objective =
R"doc(Routing objectives are source-target pairs.

Template parameter ``Lyt``:
    Layout type whose coordinates are to be used.)doc";

static const char *__doc_fiction_routing_objective_operator_eq =
R"doc(Equality operator.

Template parameter ``OtherLyt``:
    Type of other layout.

Parameter ``other``:
    Routing objective to compare to.

Returns:
    `true` iff the given objective is equal to this one.)doc";

static const char *__doc_fiction_routing_objective_source = R"doc()doc";

static const char *__doc_fiction_routing_objective_target = R"doc()doc";

static const char *__doc_fiction_row_clocking =
R"doc(Returns a 90° rotated linear 1D clocking based on the one originally
introduced in \"A device architecture for computing with quantum
dots\" by C. S. Lent and P. D. Tougaw in the Proceedings of the IEEE
1997.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``n``:
    Number of clocks.

Returns:
    Row-based clocking scheme.)doc";

static const char *__doc_fiction_searchable_priority_queue =
R"doc(An extension of `std::priority_queue` that allows searching the
underlying container. The implementation is based on
https://stackoverflow.com/questions/16749723/how-i-can-find-value-in-
priority-queue.

Template parameter ``T``:
    The type of the stored elements.

Template parameter ``Container``:
    The type of the underlying container.

Template parameter ``Compare``:
    A Compare type providing a strict weak ordering.)doc";

static const char *__doc_fiction_searchable_priority_queue_begin = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_begin_2 = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_cbegin = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_cend = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_contains =
R"doc(Returns `true` if the provided value is stored in the queue and
`false` otherwise.

Parameter ``val``:
    Value to search for.

Returns:
    `true` iff `val` is contained in the priority queue.)doc";

static const char *__doc_fiction_searchable_priority_queue_end = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_end_2 = R"doc()doc";

static const char *__doc_fiction_searchable_priority_queue_find =
R"doc(Returns an iterator to the provided value if it is contained in the
priority queue. Returns an iterator to the end of the stored container
otherwise.

Parameter ``val``:
    Value to search for.

Returns:
    Iterator to the stored value or to the end of the container if
    `val` is not contained.)doc";

static const char *__doc_fiction_searchable_priority_queue_find_2 =
R"doc(Returns a `const_iterator` to the provided value if it is contained in
the priority queue. Returns an iterator to the end of the stored
container otherwise.

Parameter ``val``:
    Value to search for.

Returns:
    Iterator to the stored value or to the end of the container if
    `val` is not contained.)doc";

static const char *__doc_fiction_set_name =
R"doc(Helper function to conveniently assign a name to a layout or network
as they use different function names for the same purpose.

Template parameter ``NtkOrLyt``:
    Network or layout type.

Parameter ``ntk_or_lyt``:
    Network or layout object.

Parameter ``name``:
    Name to assign to given network or layout.)doc";

static const char *__doc_fiction_shifted_cartesian_layout =
R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian layout with shifted coordinates. Its faces are organizes in
an offset coordinate system as provided. These can either be the
horizontal_shift_cartesian or vertical_shift_cartesian orientation.
Based on that, two respectively possible coordinate systems emerge
accordingly: odd_row_cartesian and even_row_cartesian for horizontal
shifts and odd_column_cartesian and even_column_cartesian for vertical
shifts. All are sketched in ASCII above.

Template parameter ``OffsetCoordinateType``:
    The coordinate implementation to be used. Offset coordinates are
    required.

Template parameter ``ShiftedCartesianCoordinateSystem``:
    One of the following: odd_row_cartesian, even_row_cartesian,
    odd_column_cartesian, even_column_cartesian.)doc";

static const char *__doc_fiction_shifted_cartesian_layout_conditional_t = R"doc()doc";

static const char *__doc_fiction_shifted_cartesian_layout_odd_row_hex = R"doc()doc";

static const char *__doc_fiction_shifted_cartesian_layout_shifted_cartesian_layout =
R"doc(Standard constructor. The given aspect ratio points to the highest
possible coordinate in the layout. That means in the
even_column_cartesian ASCII layout representation above `ar = (3,2)`.
Consequently, with `ar = (0,0)`, the layout has exactly one
coordinate.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

static const char *__doc_fiction_shifted_cartesian_layout_shifted_cartesian_layout_2 = R"doc()doc";

static const char *__doc_fiction_sidb_100_lattice = R"doc(H-Si(100)-2x1 surface.)doc";

static const char *__doc_fiction_sidb_100_lattice_sidb_100_lattice = R"doc()doc";

static const char *__doc_fiction_sidb_111_lattice = R"doc(H-Si(111)-1x1 surface.)doc";

static const char *__doc_fiction_sidb_111_lattice_sidb_111_lattice = R"doc()doc";

static const char *__doc_fiction_sidb_bestagon_library =
R"doc(A gate library for the SiDB technology that is based on Y-shaped gates
in hexagonal tiles. Y-shaped gates have been first introduced in
\"Binary Atomic Silicon Logic\" by Taleana Huff, Hatem Labidi,
Mohammad Rashidi, Lucian Livadaru, Thomas Dienel, Roshan Achal, Wyatt
Vine, Jason Pitters, and Robert A. Wolkow in Nature Electronics 2018.
The Bestagon library was later proposed in \"Hexagons are the
Bestagons: Design Automation for Silicon Dangling Bond Logic\" by
Marcel Walter, Samuel Sze Hang Ng, Konrad Walus, and Robert Wille in
Design Automation Conference 2022. The goal of the Bestagon library is
to be as close to physically realizable SiDB circuits as possible by
taking fabrication limitations of, e.g., clocking electrodes into
account while also relying on established gate shape. Thus, the
hexagonal tiles in the Bestagon library are quite large with a lot of
free space to avoid unwanted gate interactions. The Bestagon library
is intended for hexagonal, pointy-top layouts that are clocked with a
row-based clocking scheme, i.e., where the information flow direction
is north to south.)doc";

static const char *__doc_fiction_sidb_bestagon_library_determine_port_routing =
R"doc(Determines the port directions of a given tile.

Template parameter ``GateLyt``:
    Pointy-top hexagonal gate-level layout type.

Parameter ``lyt``:
    Given tile `t` for which the port directions are determined.

Returns:
    port directions of the given tile are returned as `port_list`.)doc";

static const char *__doc_fiction_sidb_bestagon_library_get_functional_implementations =
R"doc(Returns a map of all gate functions supported by the library and their
respectively possible implementations.

This is an optional interface function that is required by some
algorithms.

Returns:
    Map of all gate functions supported by the library and their
    respective implementations as Bestagon gates.)doc";

static const char *__doc_fiction_sidb_bestagon_library_get_gate_ports =
R"doc(Returns a map of all different gate implementations and their
respective port information.

This is an optional interface function that is required by some
algorithms.

Returns:
    Map of all different gate implementations and their respective
    port information.)doc";

static const char *__doc_fiction_sidb_bestagon_library_set_up_gate =
R"doc(Overrides the corresponding function in fcn_gate_library. Given a tile
`t`, this function takes all necessary information from the stored
grid into account to choose the correct fcn_gate representation for
that tile. May it be a gate or wires. Rotation and special marks like
input and output, const cells etc. are computed additionally.

Template parameter ``GateLyt``:
    Pointy-top hexagonal gate-level layout type.

Parameter ``lyt``:
    Layout that hosts tile `t`.

Parameter ``t``:
    Tile to be realized as a Bestagon gate.

Returns:
    Bestagon gate representation of `t` including mirroring.)doc";

static const char *__doc_fiction_sidb_bestagon_library_sidb_bestagon_library = R"doc()doc";

static const char *__doc_fiction_sidb_binary_cluster_hierarchy_node = R"doc(Forward-declaration of sidb_binary_cluster_hierarchy_node.)doc";

static const char *__doc_fiction_sidb_binary_cluster_hierarchy_node_2 = R"doc(Forward-declaration of sidb_binary_cluster_hierarchy_node.)doc";

static const char *__doc_fiction_sidb_binary_cluster_hierarchy_node_c = R"doc()doc";

static const char *__doc_fiction_sidb_binary_cluster_hierarchy_node_sidb_binary_cluster_hierarchy_node = R"doc()doc";

static const char *__doc_fiction_sidb_binary_cluster_hierarchy_node_sub = R"doc(The two children of the node.)doc";

static const char *__doc_fiction_sidb_charge_space_composition =
R"doc(A charge space composition holds a number of projector states of
sibling clusters. Summing the multiset charge configuration associated
with each, we obtain an element of the charge space of their parent.
Additionally, we have a store for the bounds on the partial potential
sum local to each SiDB contained by the parent, i.e., partial in the
sense that SiDBs not contained by the parent are not taken into
account. The potential bounds for each SiDB correspond to the meet on
the potential bounds for each (sub-)composition of the respective
cluster charge states associated with the multiset charge
configuration of each projector state in this composition of siblings.)doc";

static const char *__doc_fiction_sidb_charge_space_composition_pot_bounds =
R"doc(Flattened (hierarchical) potential bounds specific to this
composition.)doc";

static const char *__doc_fiction_sidb_charge_space_composition_proj_states =
R"doc(Projector states associated with charge space elements that make up
the composition.)doc";

static const char *__doc_fiction_sidb_charge_state = R"doc(Charge states of SiDBs.)doc";

static const char *__doc_fiction_sidb_charge_state_NEGATIVE = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_NEUTRAL = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_NONE = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_POSITIVE = R"doc()doc";

static const char *__doc_fiction_sidb_charge_states_for_base_number =
R"doc(Charge states of SiDBs for a given simulation base number. The full
base states are returned for an invalid simulation base.

Parameter ``base``:
    The simulation base number to get the associated SiDB charge
    states for.

Returns:
    NEG, NEUT, POS for base 3 (full base), and NEG, NEUT otherwise,
    associated with base 2 simulation.)doc";

static const char *__doc_fiction_sidb_cluster =
R"doc(Forward declaration of the SiDB cluster hierarchy required for the
mutual recursive structure in this file. Here we define a pointer to a
SiDB cluster to be a shared pointer, which enables us to also have
pointers to parents.)doc";

static const char *__doc_fiction_sidb_cluster_2 =
R"doc(Forward declaration of the SiDB cluster hierarchy required for the
mutual recursive structure in this file. Here we define a pointer to a
SiDB cluster to be a shared pointer, which enables us to also have
pointers to parents.)doc";

static const char *__doc_fiction_sidb_cluster_charge_space = R"doc(The charge state space of the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state =
R"doc(A cluster charge state is a multiset charge configuration. We may
compress it into a 64 bit unsigned integer by putting the number of
negative and positive charges in the upper and lower 32 bits
respectively. The number of neutral charges may then be inferred for a
given cluster by considering its size. Cluster charge states are the
crucial objects of the state spaces, called charge spaces, since they
not only hold information of the multiset charge configuration, but
also the set of compositions, each of which compose into the current
cluster charge state.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_add_charge =
R"doc(Modifier of the cluster charge state, adding a single charge state.

Parameter ``cs``:
    The charge state to add.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_compositions = R"doc(Stored compositions of this cluster charge state.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_neg_count =
R"doc(Number of negative charges in the cluster charge state (32 available
bits).)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_operator_call =
R"doc(Defined a hashing of a cluster charge state. Since we need only
separate cluster charge states by their compressed form, we may
compute a hash over this for optimal performance when used in a hash
set.

Parameter ``m``:
    Cluster charge state to compute the hash of.

Returns:
    The hash of the given cluster charge state.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_operator_eq =
R"doc(Defines the equality operation of cluster charge states, which
disregards the compositions.

Parameter ``other``:
    Other cluster charge state to test for equality with the current.

Returns:
    `true` if and only if the compressed forms are equal.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_operator_iadd =
R"doc(Defines addition of cluster charge states through multiset
concatenation. Disregards compositions.

Parameter ``other``:
    Other cluster charge state to concatenate with the current.

Returns:
    The concatenated cluster charge state, which is the modified
    version of the current.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_operator_isub =
R"doc(Defines subtraction of cluster charge states through multiset
difference. Disregards compositions.

Parameter ``other``:
    Other cluster charge state to take the difference of w.r.t. with
    the current.

Returns:
    The cluster charge state that is their difference, which is the
    modified version of the current.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_operator_unsigned_long =
R"doc(Explicit instructions for the compiler on how to cast a cluster charge
state to an 64-bit unsigned integer.

Returns:
    The 64-bit unsigned integer representing the compressed form of
    the cluster charge state.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_pos_count =
R"doc(Number of positive charges in the cluster charge state (32 available
bits).)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_sidb_cluster_charge_state =
R"doc(Default constructor, creates a cluster charge state without any
negative and positive charges.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_sidb_cluster_charge_state_2 =
R"doc(Constructor for a charge space element of a singleton cluster. It has
a single composition, which is a cluster state of the singleton
cluster and the singleton multiset charge configuration itself.

Parameter ``singleton``:
    Singleton cluster to put in the compositions of this cluster
    charge state.

Parameter ``cs``:
    Charge state to lift to a singleton multiset charge configuration.

Parameter ``loc_ext_pot``:
    The local external potential at the SiDB in the singleton cluster.
    Specifically, this is the sum of the local defect potential and
    the local external potential.

Parameter ``total_num_sidbs``:
    The total number of SiDBs in the layout.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_sidb_cluster_charge_state_3 =
R"doc(Constructor for cluster charge state given a multiset charge
configuration represented in its compressed form. It allows the
compressed form to be lifted to the full type to facilitate equality
checks.

Parameter ``m``:
    The multiset charge configuration to create a cluster charge state
    of.)doc";

static const char *__doc_fiction_sidb_cluster_charge_state_sidb_cluster_charge_state_4 =
R"doc(Constructor of a cluster charge state allowing initializer list
construction.

Parameter ``charge_states``:
    initializer list of charge states to form into a cluster charge
    state.)doc";

static const char *__doc_fiction_sidb_cluster_children = R"doc(The set of children of a cluster is a clustering.)doc";

static const char *__doc_fiction_sidb_cluster_external_sidbs = R"doc(The SiDBs in the layout that are not contained by the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_get_parent =
R"doc(This function returns a shared pointer to the parent of this cluster.

Returns:
    A shared pointer to the parent of this cluster)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy =
R"doc(This function performs the ALGLIB agglomerative clustering algorithm
for a given SiDB layout. By default, the cluster are created by a
minimal positional variance heuristic, also known as Ward's method.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to create a cluster hierarchy of.

Parameter ``linkage_method``:
    The agglomerative clustering linking heuristic that is used by
    ALGLIB.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method =
R"doc(An enumeration of cluster linkage methods. The chosen method defines
how clusters are merged in the agglomerative clustering procedure, by,
e.g., defining an inter-cluster distance to minimize for the cluster
to merge. For more information, visit: https://docs.tibco.com/pub/spot
fire/6.5.1/doc/html/hc/hc_clustering_methods_overview.htm.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method_COMPLETE =
R"doc(Complete linkage takes the maximum distance between nodes in a
cluster.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method_MINIMUM_VARIANCE =
R"doc(Known as Ward's method, this type of linkage merges clusters based on
a minimum variance measure.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method_SINGLE = R"doc(Single linkage takes the minimum distance between nodes in a cluster.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method_UNWEIGHTED_AVERAGE =
R"doc(Unweighted average linkage takes the average distance between nodes in
a cluster, disregarding the number of elements in a node.)doc";

static const char *__doc_fiction_sidb_cluster_hierarchy_linkage_method_WEIGHTED_AVERAGE =
R"doc(Weighted average linkage takes the average distance between nodes in a
cluster, weighing in the number of elements in a node.)doc";

static const char *__doc_fiction_sidb_cluster_initialize_singleton_cluster_charge_space =
R"doc(This function initializes the charge space of a singleton cluster
corresponding with the given simulation base, and sets the initial
bounds on the potential received from outside the cluster as the local
potential bounds. The local potential bounds do not include the local
external potential at the SiDB in the singleton, as this is passed
separately.

Parameter ``loc_pot_min``:
    The minimum local potential for the SiDB in the singleton cluster.

Parameter ``loc_pot_max``:
    The maximum local potential for the SiDB in the singleton cluster.

Parameter ``loc_ext_pot``:
    The local external potential at the SiDB in the singleton cluster.
    Specifically, this is the sum of the local defect potential and
    the local external potential.

Parameter ``base``:
    The simulation base.

Parameter ``self_ptr``:
    Shared pointer to itself.)doc";

static const char *__doc_fiction_sidb_cluster_num_sidbs =
R"doc(Function to return the number of SiDBs contained in the cluster.

Returns:
    The number of SiDBs contained in the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_operator_eq =
R"doc(Equality operation on cluster hierarchies. Checks the respective
unique identifiers for equality.

Parameter ``other``:
    Cluster (hierarchy) to compare to.

Returns:
    `true` if and only if the unique identifiers match.)doc";

static const char *__doc_fiction_sidb_cluster_parent =
R"doc(Every cluster carries a pointer to its parent. For the top cluster,
this is `nullptr`.)doc";

static const char *__doc_fiction_sidb_cluster_pot_projs = R"doc()doc";

static const char *__doc_fiction_sidb_cluster_pot_projs_complete_store = R"doc()doc";

static const char *__doc_fiction_sidb_cluster_projector_state =
R"doc(A projector state pairs the potential projecting cluster with the
associated multiset charge configuration.)doc";

static const char *__doc_fiction_sidb_cluster_projector_state_cluster = R"doc(Projector cluster.)doc";

static const char *__doc_fiction_sidb_cluster_projector_state_get_count =
R"doc(Getter for the number of a given charge state in the multiset
configuration.

Template parameter ``cs``:
    Charge state to count the number of occurrences in the projector
    state of.

Returns:
    The number of occurrences of the given charge state in the
    multiset charge configuration. For a neutral charge, the number of
    occurrences is inferred by considering the size of the cluster in
    the projector state.)doc";

static const char *__doc_fiction_sidb_cluster_projector_state_multiset_conf =
R"doc(Multiset charge configuration. It is an element of the charge space of
the projector cluster.)doc";

static const char *__doc_fiction_sidb_cluster_ptr_hash =
R"doc(This struct defines a hashing of shared pointers to clusters, used to
facilitate quick access in a clustering.)doc";

static const char *__doc_fiction_sidb_cluster_ptr_hash_operator_call =
R"doc(The hashing operation is defined.

Parameter ``c``:
    Shared pointer to a cluster to take the has of.

Returns:
    The hash computed over the unique id associated with the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_received_ext_pot_bounds =
R"doc(The bounds on the electrostatic potential sum of SiDBs external to
this cluster, local to an SiDB in the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_receptor_state =
R"doc(A receptor state pairs the potential receiving cluster with the
identifier of the SiDB.)doc";

static const char *__doc_fiction_sidb_cluster_receptor_state_cluster = R"doc(Receptor cluster.)doc";

static const char *__doc_fiction_sidb_cluster_receptor_state_sidb_ix = R"doc(SiDB index. It is contained in the receptor cluster.)doc";

static const char *__doc_fiction_sidb_cluster_sidb_cluster =
R"doc(SiDB cluster hierarchy constructor.

Parameter ``c``:
    Set of SiDB indices for the cluster to contain.

Parameter ``other_c``:
    Set of SiDB indices in the layout that the cluster will not
    contain.

Parameter ``x``:
    A set of cluster hierarchies to set as the children of this
    cluster.

Parameter ``unique_id``:
    The unsigned integer to identify the cluster hierarchy uniquely
    with. For the case of a singleton cluster, the unique identifier
    is set to be the index of the single SiDB it contains.)doc";

static const char *__doc_fiction_sidb_cluster_sidbs = R"doc(The SiDBs contained by the cluster.)doc";

static const char *__doc_fiction_sidb_cluster_uid = R"doc(Unique identifier. Equal to the SiDB index for singleton clusters.)doc";

static const char *__doc_fiction_sidb_clustering_state =
R"doc(A clustering state is very similar to a cluster state composition,
though it uses unique pointers to the cluster states that may be
moved. Thereby, this is the essential type of the dynamic objects in
*ClusterComplete*'s operation, which always represent information of
the complete layout.)doc";

static const char *__doc_fiction_sidb_clustering_state_operator_assign =
R"doc(Copy assignment operator.

Assigns the contents of another `sidb_clustering_state` instance to
this instance. Performs a deep copy of `proj_states` and a complete
copy of `pot_bounds`.

Parameter ``other``:
    The `sidb_clustering_state` instance to copy from.

Returns:
    A reference to this `sidb_clustering_state` instance after
    assignment.)doc";

static const char *__doc_fiction_sidb_clustering_state_operator_assign_2 =
R"doc(Move assignment operator.

Parameter ``other``:
    Other clustering state to move.)doc";

static const char *__doc_fiction_sidb_clustering_state_pot_bounds =
R"doc(Flattened (hierarchical) potential bounds specific to this clustering
state.)doc";

static const char *__doc_fiction_sidb_clustering_state_proj_states =
R"doc(Projector states associated with charge space elements that make up
the clustering state.)doc";

static const char *__doc_fiction_sidb_clustering_state_sidb_clustering_state =
R"doc(Default constructor.

Parameter ``num_sidbs``:
    Number of SiDBs in the layout that the clustering state should
    consider.)doc";

static const char *__doc_fiction_sidb_clustering_state_sidb_clustering_state_2 =
R"doc(Copy constructor.

Parameter ``other``:
    Other clustering state to copy.)doc";

static const char *__doc_fiction_sidb_clustering_state_sidb_clustering_state_3 =
R"doc(Move constructor.

Parameter ``other``:
    Other clustering state to move.)doc";

static const char *__doc_fiction_sidb_defect =
R"doc(In accordance with the paper mentioned above, the `sidb_defect` struct
is used to represent a specific defect on the H-Si(100) 2x1 surface
that has a charge as well as relative permittivity (`epsilon_r`) and
Thomas-Fermi screening distance (`lambda_tf`) values associated to it.

See \"SiQAD: A Design and Simulation Tool for Atomic Silicon Quantum
Dot Circuits\" by S. S. H. Ng, J. Retallick, H. N. Chiu, R. Lupoiu, L.
Livadaru, T. Huff, M. Rashidi, W. Vine, T. Dienel, R. A. Wolkow, and
K. Walus in IEEE Transactions on Nanotechnology for more details on
these values.)doc";

static const char *__doc_fiction_sidb_defect_charge =
R"doc(Electrical charge in units of the elementary charge e (e.g., 1 ^= 1*e,
-2 ^= -2*e).)doc";

static const char *__doc_fiction_sidb_defect_epsilon_r = R"doc(Electric permittivity (unitless).)doc";

static const char *__doc_fiction_sidb_defect_lambda_tf = R"doc(Thomas-Fermi screening distance in nm.)doc";

static const char *__doc_fiction_sidb_defect_operator_eq =
R"doc(This operator compares two `sidb_defect` instances for equality. It
checks if the `type`, `charge`, `epsilon_r`, and `lambda_tf` members
of the two instances are equal.

Parameter ``rhs``:
    `sidb_defect` instance to compare against.)doc";

static const char *__doc_fiction_sidb_defect_operator_ne =
R"doc(This operator compares two `sidb_defect` instances for inequality. It
uses the `operator==` to check if the two instances are equal and
returns the negation of the result.

Parameter ``rhs``:
    `sidb_defect` instance to compare against.)doc";

static const char *__doc_fiction_sidb_defect_sidb_defect = R"doc(Standard constructor.)doc";

static const char *__doc_fiction_sidb_defect_surface = R"doc()doc";

static const char *__doc_fiction_sidb_defect_surface_2 = R"doc()doc";

static const char *__doc_fiction_sidb_defect_surface_params = R"doc(This struct stores parameters for the `sidb_defect_surface`)doc";

static const char *__doc_fiction_sidb_defect_surface_params_ignore =
R"doc(Specifies which defects are to be ignored, e.g., when they are not
relevant for the current analysis.

@note Ignored defects are not stored in the surface instance!)doc";

static const char *__doc_fiction_sidb_defect_surface_sidb_defect_surface = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type =
R"doc(Specifies the types of fabrication defects that can occur on the
H-Si(100) 2x1 surface according to \"Atomic defect classification of
the H–Si(100) surface through multi-mode scanning probe microscopy\"
by Jeremiah Croshaw, Thomas Dienel, Taleana Huff, and Robert Wolkow in
Journal of Nanotechnology in 2020.)doc";

static const char *__doc_fiction_sidb_defect_type_2 = R"doc(Type of defect.)doc";

static const char *__doc_fiction_sidb_defect_type_DB = R"doc(A stray dangling bond.)doc";

static const char *__doc_fiction_sidb_defect_type_DIHYDRIDE_PAIR =
R"doc(A missing bond between dimers that leads to two double hydrogen
passivations.)doc";

static const char *__doc_fiction_sidb_defect_type_ETCH_PIT = R"doc(A collection of missing dimers.)doc";

static const char *__doc_fiction_sidb_defect_type_GUNK = R"doc(Residual material.)doc";

static const char *__doc_fiction_sidb_defect_type_MISSING_DIMER = R"doc(The dimer is missing altogether.)doc";

static const char *__doc_fiction_sidb_defect_type_NONE = R"doc(Defect-free H-Si.)doc";

static const char *__doc_fiction_sidb_defect_type_ONE_BY_ONE = R"doc(A collection of dihydride pairs.)doc";

static const char *__doc_fiction_sidb_defect_type_RAISED_SI = R"doc(A raised silicon dimer.)doc";

static const char *__doc_fiction_sidb_defect_type_SILOXANE = R"doc(An oxidized dimer.)doc";

static const char *__doc_fiction_sidb_defect_type_SINGLE_DIHYDRIDE = R"doc(Double hydrogen passivation.)doc";

static const char *__doc_fiction_sidb_defect_type_SI_VACANCY = R"doc(A missing silicon atom.)doc";

static const char *__doc_fiction_sidb_defect_type_STEP_EDGE = R"doc(A step edge, which is a break in the surface reconstruction.)doc";

static const char *__doc_fiction_sidb_defect_type_THREE_BY_ONE = R"doc(A collection of 1 by 1's.)doc";

static const char *__doc_fiction_sidb_defect_type_UNKNOWN = R"doc(Unknown defect.)doc";

static const char *__doc_fiction_sidb_lattice =
R"doc(A layout type to layer on top of an SiDB cell-level layout. It
implements an interface for different lattice orientations of the H-Si
crystal.

Template parameter ``LatticeOrientation``:
    Type of the lattice orientation.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``has_sidb_lattice_interface``:
    Automatically determines whether an SiDB lattice interface is
    already present.)doc";

static const char *__doc_fiction_sidb_lattice_2 = R"doc()doc";

static const char *__doc_fiction_sidb_nm_distance =
R"doc(Computes the distance between two SiDB cells in nanometers (unit: nm).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``source``:
    The source cell.

Parameter ``target``:
    The target cell.

Returns:
    The distance between the two cells in nanometers (unit: nm).)doc";

static const char *__doc_fiction_sidb_nm_position =
R"doc(Computes the position of a cell in nanometers from the layout origin
in an SiDB layout (unit: nm).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``c``:
    The cell to compute the position for.

Returns:
    A pair representing the `(x,y)` position of `c` in nanometers from
    the layout origin.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library =
R"doc(A parameterized gate library for SiDB technology. It allows the design
of SiDB gates tailored to given atomic defects, thus enabling the
design of SiDB circuits in the presence of atomic defects. The
skeleton (i.e., the pre-defined input and output wires) are hexagonal
in shape.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_add_defect_to_skeleton =
R"doc(This function takes a defect surface and a skeleton skeleton and adds
defects from the surrounding area to the skeleton. The defects within
a specified distance from the center cell are taken into account. The
resulting skeleton with added defects is returned.

Template parameter ``CellLyt``:
    SiDB defect surface type.

Template parameter ``Params``:
    Type of Parameters.

Parameter ``skeleton``:
    The skeleton to which defects will be added.

Parameter ``center_cell``:
    The coordinates of the center cell.

Parameter ``absolute_cell``:
    The coordinates of the skeleton's absolute cell.

Parameter ``parameters``:
    Parameters for defect handling.

Returns:
    The updated skeleton with added defects from the surrounding area.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_cell_level_layout_to_list =
R"doc(Generates a cell-level layout as a 2D array of characters based on the
provided cell layout information.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``lyt``:
    Cell-level layout

Returns:
    A 2D array of characters representing the cell-level layout.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_cell_list_to_cell_level_layout =
R"doc(The function generates a layout where each cell is assigned a specific
cell type according to the characters in the cell list/input grid.

Template parameter ``Lyt``:
    The type of the cell-level layout to be generated.

Parameter ``cell_list``:
    A 2D grid representing the cells and their types.

Returns:
    The cell-level layout with assigned cell types.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_design_gate =
R"doc(This function designs an SiDB gate for a given Boolean function at a
given tile and a given rotation. If atomic defects exist, they are
incorporated into the design process.

An exception is thrown in case there is no possible gate design.

Template parameter ``LytSkeleton``:
    The cell-level layout of the skeleton.

Template parameter ``TT``:
    Truth table type.

Template parameter ``CellLyt``:
    The cell-level layout.

Template parameter ``GateLyt``:
    The gate-level layout.

Parameter ``skeleton``:
    Skeleton with atomic defects if available.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``parameters``:
    Parameters for the SiDB gate design process.

Parameter ``p``:
    The list of ports and their directions.

Parameter ``tile``:
    The specific tile on which the gate should be designed.

Returns:
    An `fcn_gate` object.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_determine_port_routing =
R"doc(Determines the port directions of a given tile.

Template parameter ``GateLyt``:
    Pointy-top hexagonal gate-level layout type.

Parameter ``lyt``:
    Given tile `t` for which the port directions are determined.

Returns:
    port directions of the given tile are returned as `port_list`.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_is_predefined_bestagon_gate_applicable =
R"doc(This function evaluates whether a predefined Bestagon gate can be
applied to the given node by considering various conditions, including
the presence of defects and spacing requirements.

Template parameter ``CellLyt``:
    SiDB defect surface type.

Template parameter ``TT``:
    Truth table type.

Template parameter ``Params``:
    Type of the parameters used for the parametrized gate library.

Parameter ``bestagon_lyt``:
    The Bestagon gate which is to be applied.

Parameter ``skeleton_with_defects``:
    The skeleton layout with atomic defects.

Parameter ``truth_table``:
    The truth table representing the gate's logic function.

Parameter ``parameters``:
    Parameters for the gate design and simulation.

Returns:
    `true` if the Bestagon gate is applicable to the layout,
    considering the provided conditions; otherwise, returns `false`.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params =
R"doc(This struct encapsulates parameters for the parameterized SiDB gate
library.

Template parameter ``CellType``:
    SiDB cell type.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_canvas_sidb_complex_gates =
R"doc(This variable defines the number of canvas SiDBs dedicated to complex
gates, such as crossing, double wire, and half-adder.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy =
R"doc(This struct represents the policy for complex (i.e., crossing, double
wire, half-adder) gate design.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_DESIGN_ON_THE_FLY = R"doc(Design complex gates on-the-fly.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_USING_PREDEFINED = R"doc(Use predefined complex gates if possible.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_design_gate_params = R"doc(This struct holds parameters to design SiDB gates.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_influence_radius_charged_defects =
R"doc(This variable specifies the radius in nanometers around the center of
the hexagon where atomic defects are incorporated into the gate
design.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_params_using_predefined_crossing_and_double_wire_if_possible = R"doc(This variable specifies the policy for complex gate design.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_set_up_gate =
R"doc(Overrides the corresponding function in fcn_gate_library. Given a tile
`t`, this function takes all necessary information from the stored
grid into account to design the correct fcn_gate representation for
that tile. In case there is no possible SiDB design, the blacklist is
updated and an error fcn gate is returned.

Template parameter ``GateLyt``:
    Pointy-top hexagonal gate-level layout type.

Template parameter ``CellLyt``:
    SiDB cell-level layout type.

Template parameter ``Params``:
    Type of the parameter used for the gate library.

Parameter ``lyt``:
    Layout that hosts tile `t`.

Parameter ``t``:
    Tile to be realized as a Bestagon gate.

Parameter ``parameters``:
    Parameter to design SiDB gates.

Parameter ``defect_surface``:
    Optional atomic defect surface in case atomic defects are present.

Returns:
    Bestagon gate representation of `t` including mirroring.)doc";

static const char *__doc_fiction_sidb_on_the_fly_gate_library_sidb_on_the_fly_gate_library = R"doc()doc";

static const char *__doc_fiction_sidb_simulation_domain =
R"doc(The `sidb_simulation_domain` is designed to represent a generic
simulation domain where keys are associated with values stored as
tuples. It uses a `locked_parallel_flat_hash_map` to ensure thread-
safe access to the stored data. All methods of this class are thread-
safe.

Template parameter ``Key``:
    The type of the key used to identify entries in the domain.

Template parameter ``MappedTypes``:
    Value types stored in the tuple.)doc";

static const char *__doc_fiction_sidb_simulation_domain_add_value =
R"doc(Adds a value to the operational domain. This function is thread-safe
and uses the `try_emplace` method.

Parameter ``key``:
    The key to associate with the value.

Parameter ``value``:
    The value to add, which must be a tuple.)doc";

static const char *__doc_fiction_sidb_simulation_domain_contains =
R"doc(Checks whether a specified key exists in the given map and retrieves
its associated value if present. This function utilizes the
`if_contains` method of the map to ensure thread-safe access.

Parameter ``key``:
    The key to search for in the map.

Returns:
    The value associated with the key if it exists, `std::nullopt`
    otherwise.)doc";

static const char *__doc_fiction_sidb_simulation_domain_domain_values = R"doc(The domain values stored in a thread-safe map.)doc";

static const char *__doc_fiction_sidb_simulation_domain_empty =
R"doc(Checks whether the operational domain is empty.

Returns:
    `true` if the operational domain is empty, `false` otherwise.)doc";

static const char *__doc_fiction_sidb_simulation_domain_for_each =
R"doc(Applies a callable to all key-value pairs in the container. For
thread-safety, this function operates on a copy of the underlying map
created at the time of the function call.

Template parameter ``Fn``:
    Functor type.

Parameter ``fn``:
    Functor to apply to each key-value pair.)doc";

static const char *__doc_fiction_sidb_simulation_domain_sidb_simulation_domain = R"doc(Constructs a new `sidb_simulation_domain` instance.)doc";

static const char *__doc_fiction_sidb_simulation_domain_size =
R"doc(Counts the number of key-value pairs in the operational domain.

Returns:
    The size of the operational domain.)doc";

static const char *__doc_fiction_sidb_simulation_engine = R"doc(Selector for the available SiDB simulation engines.)doc";

static const char *__doc_fiction_sidb_simulation_engine_CLUSTERCOMPLETE =
R"doc(*ClusterComplete* is a novel exact simulation engine that requires
exponential runtime, though, depending on the simulation problem, it
effectively reduces the base number by a real number, thus allowing
problem sizes that were previously considered astronomical in size.
Inherent to the simulation methodology that does not depend on the
simulation base, it simulates very effectively for either base number
(2 or 3).)doc";

static const char *__doc_fiction_sidb_simulation_engine_EXGS =
R"doc(*Exhaustive Ground State Search (EXGS)* is an exact simulation engine
that always has exponential runtime.)doc";

static const char *__doc_fiction_sidb_simulation_engine_QUICKEXACT =
R"doc(*QuickExact* is also an exact simulation engine that requires
exponential runtime, but it scales a lot better than *ExGS* due to its
effective search-space pruning.)doc";

static const char *__doc_fiction_sidb_simulation_engine_QUICKSIM =
R"doc(*QuickSim* is a heuristic simulation engine that only requires
polynomial runtime.)doc";

static const char *__doc_fiction_sidb_simulation_engine_name =
R"doc(Returns the name of the given simulation engine.

Template parameter ``EngineType``:
    The type of the SiDB simulation engine
    (exhaustive/heuristic/generic).

Parameter ``engine``:
    An SiDB simulation engine.

Returns:
    The name of the simulation engine.)doc";

static const char *__doc_fiction_sidb_simulation_parameters =
R"doc(This struct collects all physical parameters for physical SiDB
simulations. It can be useful to adjust them, especially when
experiments create new insights. However, the default values are
commonly used.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_base =
R"doc(`base` can be either 2 or 3 and describes the assumed number of charge
states of one SiDB. It often makes sense to assume only negatively and
neutrally charged SiDBs.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_epsilon_r =
R"doc(`epsilon_r` is the electric permittivity. It is a material specific
number (unit-less).)doc";

static const char *__doc_fiction_sidb_simulation_parameters_k =
R"doc(`k` is the Coulomb constant `K_E` divided by `epsilon_r` (unit:
:math:`N \cdot m^{2} \cdot C^{-2}`).)doc";

static const char *__doc_fiction_sidb_simulation_parameters_lambda_tf = R"doc(`lambda_tf` is the Thomas-Fermi screening distance (unit: nm).)doc";

static const char *__doc_fiction_sidb_simulation_parameters_mu_minus = R"doc(`mu_minus` (µ-) is the energy transition level (0/-) (unit: eV).)doc";

static const char *__doc_fiction_sidb_simulation_parameters_mu_plus = R"doc(`mu_plus` (µ+) is the energy transition level (+/0) (unit: eV).)doc";

static const char *__doc_fiction_sidb_simulation_parameters_sidb_simulation_parameters =
R"doc(Default constructor.

Parameter ``base_number``:
    simulation can be conducted with 2 and 3 charge states. 2 =
    (Negative, Neutral), 3 = (Negative, Neutral, Positive).

Parameter ``mu``:
    (µ-) is the energy transition level (0/-) in eV.

Parameter ``relative_permittivity``:
    it describes the electric field reduction due to polarization.

Parameter ``screening_distance``:
    also known as "Thomas-Fermi screening" and it describes the
    electric field screening due to free charges in nm.)doc";

static const char *__doc_fiction_sidb_simulation_result =
R"doc(This struct defines a unified return type for all SiDB simulation
algorithms. It contains the name of the algorithm, the total
simulation runtime, the charge distributions determined by the
algorithm, the physical parameters used in the simulation, and
(optional) algorithm-specific named simulation parameters.

Template parameter ``Lyt``:
    SiDB cell-level layout type.)doc";

static const char *__doc_fiction_sidb_simulation_result_additional_simulation_parameters =
R"doc(Additional named simulation parameters. This is used to store
algorithm-dependent parameters that are not part of the
`sidb_simulation_parameters` struct.

The key of the map is the name of the parameter, the element is the
value of the parameter.)doc";

static const char *__doc_fiction_sidb_simulation_result_algorithm_name = R"doc(Name of the algorithm used to determine the charge distributions.)doc";

static const char *__doc_fiction_sidb_simulation_result_charge_distributions = R"doc(Charge distributions determined by the algorithm.)doc";

static const char *__doc_fiction_sidb_simulation_result_groundstates =
R"doc(This function computes the ground state of the charge distributions.

@note If degenerate states exist in the simulation result, this
function will return multiple ground states that all possess the same
system energy.

Returns:
    A vector of charge distributions with the minimal energy.)doc";

static const char *__doc_fiction_sidb_simulation_result_sidb_simulation_result =
R"doc(Default constructor. It only exists to allow for the use of
`static_assert` statements that restrict the type of `Lyt`.)doc";

static const char *__doc_fiction_sidb_simulation_result_simulation_parameters = R"doc(Physical parameters used in the simulation.)doc";

static const char *__doc_fiction_sidb_simulation_result_simulation_runtime = R"doc(Total simulation runtime in seconds.)doc";

static const char *__doc_fiction_sidb_skeleton_bestagon_library =
R"doc(This library contains SiDB I/O wires designed for both 1- and 2-input
functions. Each wire comprises 2 BDL pairs. The library contains all
mirrored versions, a double wire and a crossing.)doc";

static const char *__doc_fiction_sidb_skeleton_bestagon_library_determine_port_routing =
R"doc(Determines the port directions of a given tile.

Template parameter ``GateLyt``:
    Pointy-top hexagonal gate-level layout type.

Parameter ``lyt``:
    Given tile `t` for which the port directions are determined.

Returns:
    port directions of the given tile are returned as `port_list`.)doc";

static const char *__doc_fiction_sidb_skeleton_bestagon_library_get_functional_implementations =
R"doc(Returns a map of all gate functions supported by the library and their
respectively possible implementations.

This is an optional interface function that is required by some
algorithms.

Returns:
    Map of all gate functions supported by the library and their
    respective implementations as Bestagon skeletons.)doc";

static const char *__doc_fiction_sidb_skeleton_bestagon_library_get_gate_ports =
R"doc(Returns a map of all different gate implementations and their
respective port information.

This is an optional interface function that is required by some
algorithms.

Returns:
    Map of all different gate implementations and their respective
    port information.)doc";

static const char *__doc_fiction_sidb_skeleton_bestagon_library_sidb_skeleton_bestagon_library = R"doc()doc";

static const char *__doc_fiction_sidb_surface_analysis =
R"doc(Analyzes a given defective SiDB surface and matches it against gate
tiles provided by a library. Any gate type that cannot be realized on
a certain tile due to disturbances caused by defects gets blacklisted
on said tile. The black list is then returned by this function.

@note The given gate library must implement both the
`get_functional_implementations()` and `get_gate_ports()` functions.

Template parameter ``GateLibrary``:
    FCN gate library type to fetch the gate descriptions from.

Template parameter ``GateLyt``:
    Gate-level layout type that specifies the tiling of the SiDB
    surface.

Template parameter ``CellLyt``:
    SiDB cell-level layout type that is underlying to the SiDB defect
    surface.

Parameter ``gate_lyt``:
    Gate-level layout instance that specifies the aspect ratio.

Parameter ``surface``:
    SiDB surface that instantiates the defects.

Parameter ``charged_defect_spacing_overwrite``:
    Override the default influence distance of charged atomic defects
    on SiDBs with an optional pair of horizontal and vertical
    distances.

Parameter ``neutral_defect_spacing_overwrite``:
    Override the default influence distance of neutral atomic defects
    on SiDBs with an optional pair of horizontal and vertical
    distances.

Returns:
    A black list of gate functions associated with tiles.)doc";

static const char *__doc_fiction_sidb_technology =
R"doc(Silicon Dangling Bond (SiDB) technology implementation of the FCN
concept.)doc";

static const char *__doc_fiction_sidb_technology_cell_mark = R"doc(Possible marks to be applied to a cell to change its type.)doc";

static const char *__doc_fiction_sidb_technology_cell_mark_EMPTY = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mark_INPUT = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mark_LOGIC = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mark_OUTPUT = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mode = R"doc(SiDB cells do not have modes.)doc";

static const char *__doc_fiction_sidb_technology_cell_type = R"doc(Possible types of SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_EMPTY = R"doc(Symbol used for empty SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_INPUT = R"doc(Symbol used for input SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_LOGIC = R"doc(Symbol used for logic SiDB cells (e.g. canvas SiDBs).)doc";

static const char *__doc_fiction_sidb_technology_cell_type_NORMAL = R"doc(Symbol used for normal SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_OUTPUT = R"doc(Symbol used for output SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_is_empty_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_input_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_logic_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_normal_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_normal_cell_mode = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_output_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_sidb_technology = R"doc()doc";

static const char *__doc_fiction_sign_to_charge_state =
R"doc(Converts an integer (`-1`, `0`, `1`) into a charge state.

Parameter ``sg``:
    Integer (`-1`, `0`, `1`) representing a charge state.

Returns:
    sidb_charge_state representation of `sg`.)doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer =
R"doc(Base class for a simple gate-level layout DOT drawer.

Template parameter ``Lyt``:
    Gate-level layout type.

Template parameter ``ClockColors``:
    Flag to toggle the drawing of clock colors instead of gate type
    colors.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.)doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_additional_graph_attributes = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_additional_node_attributes = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_columns = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_edge = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_rows = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_same_rank = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_fillcolor = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_id = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_label = R"doc()doc";

static const char *__doc_fiction_singleton_multiset_conf_to_charge_state =
R"doc(Function to convert a singleton cluster charge state in its compressed
form to a charge state.

Parameter ``m``:
    A singleton multiset charge configuration.

Returns:
    The charge state associated with the sole element contained in the
    given multiset charge configuration.)doc";

static const char *__doc_fiction_siqad_coord_t =
R"doc(SiQAD coordinates.

Coordinates span from :math:`(-2^{31}, -2^{31}, 0)` to :math:`(2^{31}
- 1 , 2^{31} - 1, 1)`. `x` is the SiDB's x-coordinate, `y` is the
dimer pair's row number, and `z` represents the two possible SiDB
positions in one SiDB dimer pair. Each coordinate has a dead indicator
`d` that can be used to represent that it is not in use.)doc";

static const char *__doc_fiction_siqad_coord_t_coord_t = R"doc(Default constructor. Creates a dead coordinate at (0, 0, 0).)doc";

static const char *__doc_fiction_siqad_coord_t_coord_t_2 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Template parameter ``Z``:
    Type of z.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.

Parameter ``z_``:
    z position.)doc";

static const char *__doc_fiction_siqad_coord_t_coord_t_3 =
R"doc(Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).

Template parameter ``X``:
    Type of x.

Template parameter ``Y``:
    Type of y.

Parameter ``x_``:
    x position.

Parameter ``y_``:
    y position.)doc";

static const char *__doc_fiction_siqad_coord_t_d = R"doc(MSB acts as dead indicator.)doc";

static const char *__doc_fiction_siqad_coord_t_get_dead =
R"doc(Returns a dead copy of the coordinate, i.e., (1, x, y, z).

Returns:
    A dead copy of the coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_is_dead =
R"doc(Returns whether the coordinate is dead.

Returns:
    True iff coordinate is dead.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_add =
R"doc(Adds another coordinate to this one and returns the result. Does not
modify this coordinate.

Parameter ``other``:
    Coordinate to add.

Returns:
    Sum of both coordinates.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_eq =
R"doc(Compares against another coordinate for equality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff both coordinates are identical.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_ge =
R"doc(Determine whether this coordinate is "greater than or equal to"
another one. This is the case if this one is not "less than" the
other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff this coordinate is "greater than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_gt =
R"doc(Determine whether this coordinate is "greater than" another one. This
is the case if the other one is "less than".

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff this coordinate is "greater than" the other coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_le =
R"doc(Determine whether this coordinate is "less than or equal to" another
one. This is the case if this one is not "greater than" the other.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff this coordinate is "less than or equal to" the other
    coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_lt =
R"doc(Determine whether this coordinate is "less than" another one. This is
the case if y is smaller, or if y is equal but z is smaller, or if z
and y are equal but x is smaller.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff this coordinate is "less than" the other coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_ne =
R"doc(Compares against another coordinate for inequality. Respects the dead
indicator.

Parameter ``other``:
    Right-hand side coordinate.

Returns:
    True iff both coordinates are not identical.)doc";

static const char *__doc_fiction_siqad_coord_t_operator_sub =
R"doc(Subtracts another coordinate from this one and returns the result.
Does not modify this coordinate.

Parameter ``other``:
    Coordinate to subtract.

Returns:
    Difference of both coordinates.)doc";

static const char *__doc_fiction_siqad_coord_t_str =
R"doc(Returns a string representation of the coordinate of the form "(x, y,
z)" that does not respect the dead indicator.

Returns:
    String representation of the form "(x, y, z)".)doc";

static const char *__doc_fiction_siqad_coord_t_wrap =
R"doc(Wraps the coordinate with respect to the given aspect ratio by
iterating over the dimensions in the order defined by the coordinate
type. For any dimension of the coordinate that is strictly larger than
the associated dimension of the aspect ratio, this dimension will be
wrapped to zero, and the next dimension is increased. The resulting
coordinate becomes a dead copy of the aspect ratio if it is not
contained in the aspect ratio after iterating. An example use case of
this function is the coordinate iterator, which implements iterator
advancing by first incrementing the x dimension, then wrapping the
coordinate to the boundary within to enumerate.

Parameter ``aspect_ratio``:
    Aspect ratio to wrap the coordinate to.)doc";

static const char *__doc_fiction_siqad_coord_t_x = R"doc(31 bit for the x coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_y = R"doc(31 bit for the y coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_z = R"doc(1 bit for the z coordinate.)doc";

static const char *__doc_fiction_siqad_to_fiction_coord =
R"doc(Converts SiQAD coordinates to other coordinates (offset, cube).

Template parameter ``CoordinateType``:
    The desired coordinate type.

Parameter ``coord``:
    SiQAD coordinate to convert.

Returns:
    Coordinate of type `CoordinateType`.)doc";

static const char *__doc_fiction_siqad_to_siqad_coord =
R"doc(Converts any coordinate type to SiQAD coordinates.

Template parameter ``CoordinateType``:
    Coordinate type to convert.

Parameter ``coord``:
    Coordinate to convert.

Returns:
    SiQAD coordinate representation of `coord`.)doc";

static const char *__doc_fiction_smart_distance_cache_functor =
R"doc(A distance functor that internally uses a `sparse_distance_map` as a
cache to prevent re-computing distances that have already been
evaluated. In contrast to `distance_map_functor` and
`sparse_distance_map_functor`, this functor does not require a pre-
computed distance map upon construction, but instead will gradually
build up its own cache when queried multiple times. It can be used as
a drop-in replacement for any other distance functor in path-finding
algorithms.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.)doc";

static const char *__doc_fiction_smart_distance_cache_functor_distance_cache = R"doc(Sparse distance map serving as a cache.)doc";

static const char *__doc_fiction_smart_distance_cache_functor_operator_call =
R"doc(Override the call operator to first query the cache instead of the
distance function. Only on a cache miss, the distance function will be
called and the result will be stored in the cache.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Distance between source and target according to the cache or the
    distance function.)doc";

static const char *__doc_fiction_smart_distance_cache_functor_smart_distance_cache_functor =
R"doc(Construct a distance functor from a layout and a distance function.

The internal cache will be initialized empty. Distances will be
computed on the fly and stored in the cache whenever they are queried.

Parameter ``lyt``:
    Layout.

Parameter ``dist_fn``:
    Distance function.)doc";

static const char *__doc_fiction_sparse_distance_map_functor =
R"doc(A distance functor that uses a fully precomputed `sparse_distance_map`
to determine distances between coordinates. It can be used as a drop-
in replacement for any other distance functor in path-finding
algorithms.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Distance type.)doc";

static const char *__doc_fiction_sparse_distance_map_functor_operator_call =
R"doc(Override the call operator to query the sparse distance map instead of
the distance function.

@note This function will throw an exception if the queried distance is
not stored in the sparse distance map.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Distance between source and target according to the stored sparse
    distance map.)doc";

static const char *__doc_fiction_sparse_distance_map_functor_sparse_dist_map = R"doc(Sparse distance map.)doc";

static const char *__doc_fiction_sparse_distance_map_functor_sparse_distance_map_functor =
R"doc(Construct the distance functor from a sparse distance map.

Parameter ``sdm``:
    Sparse distance map.)doc";

static const char *__doc_fiction_sqd_parsing_error = R"doc(Exception thrown when an error occurs during parsing of an SQD file.)doc";

static const char *__doc_fiction_sqd_parsing_error_sqd_parsing_error = R"doc()doc";

static const char *__doc_fiction_squared_euclidean_distance =
R"doc(The squared Euclidean distance :math:`D` between two layout
coordinates :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

:math:`D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}^2 = (x_1 - x_2)^2 +
(y_1 - y_2)^2`

In contrast to the regular Euclidean distance, this function is
differentiable and can be used in optimization algorithms that require
gradients. Additionally, it is computationally cheaper by omitting the
square root operation.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral type for the distance.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Squared euclidean distance between `source` and `target`.)doc";

static const char *__doc_fiction_squared_euclidean_distance_functor =
R"doc(A pre-defined distance functor that uses the squared Euclidean
distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral distance type.)doc";

static const char *__doc_fiction_squared_euclidean_distance_functor_squared_euclidean_distance_functor = R"doc()doc";

static const char *__doc_fiction_srs_clocking =
R"doc(Returns the SRS clocking as defined in \"Simple, robust and systematic
QCA clocking scheme for area-efficient nanocircuits\" by Mrinal
Goswami, Tonmoy Jyoti Sharma, and Arpita Nath Boruah in International
Journal of Electronics Letters 2025.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    SRS clocking scheme.)doc";

static const char *__doc_fiction_static_depth_view =
R"doc(A specialization of `static_depth_view` for networks where
`has_depth_interface` is `true`. When this condition is met,
constructing a new depth view is unnecessary.

Template parameter ``Ntk``:
    The type of the network.

Template parameter ``NodeCostFn``:
    A function to compute the costs associated with nodes.)doc";

static const char *__doc_fiction_static_depth_view_2 =
R"doc(Deduction guide for `fiction::static_depth_view'.

Template parameter ``T``:
    Network type deduced from the construction context of
    `fiction::static_depth_view`.)doc";

static const char *__doc_fiction_static_depth_view_3 =
R"doc(Deduction guide for `fiction::static_depth_view` with two constructor
arguments

Template parameter ``T``:
    Network type deduced from the construction context of
    `fiction::static_depth_view`.)doc";

static const char *__doc_fiction_static_depth_view_static_depth_view = R"doc()doc";

static const char *__doc_fiction_sweep_parameter = R"doc(Possible sweep parameters for the operational domain computation.)doc";

static const char *__doc_fiction_sweep_parameter_EPSILON_R = R"doc(The relative permittivity of the dielectric material.)doc";

static const char *__doc_fiction_sweep_parameter_LAMBDA_TF = R"doc(The Thomas-Fermi screening length.)doc";

static const char *__doc_fiction_sweep_parameter_MU_MINUS = R"doc(The energy transition level.)doc";

static const char *__doc_fiction_synchronization_element_layout =
R"doc(This layout provides synchronization elements on top of a clocked
layout as a technology extension to the FCN concept proposed in
\"Synchronization of Clocked Field-Coupled Circuits\" by F. Sill
Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO
2018. More in-depth information can be found in \"Design Automation
for Field-coupled Nanotechnologies\" by M. Walter, R. Wille, F. Sill
Torres, and R. Drechsler published by Springer Nature in 2022.

A synchronization element is a clock zone whose clock signal is
altered such that it provides an extended Hold phase of a multitude of
full clock cycles. It thereby stalls information in place and acts as
a latch. Additionally, the pipeline-like behavior of FCN circuits
allows clock zones that act as synchronization elements to transmit
information to every clock number instead of just its consecutive one.

The exploration of synchronization elements further allows for
interesting sequential applications like D Latches built from single
MAJ gates as also proposed in the above publications. However, fiction
does not yet support sequential FCN circuits. Therefore, support for
these applications is limited. It is advised to use synchronization
elements only on wire tiles.

Template parameter ``ClockedLayout``:
    The clocked layout type whose clocking system should be extended
    by synchronization elements.)doc";

static const char *__doc_fiction_synchronization_element_layout_assign_synchronization_element =
R"doc(Assigns a synchronization element to the provided clock zone.

Parameter ``cz``:
    Clock zone to turn into a synchronization element.

Parameter ``se``:
    Number of full clock cycles to extend `cz`'s Hold phase by. If
    this value is 0, `cz` is turned back into a normal clock zone.)doc";

static const char *__doc_fiction_synchronization_element_layout_clone =
R"doc(Clones the layout returning a deep copy.

Returns:
    Deep copy of the layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_degree =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Returns the number of incoming plus outgoing
clock zones of the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s incoming plus outgoing clock zones.)doc";

static const char *__doc_fiction_synchronization_element_layout_foreach_incoming_clocked_zone =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Applies a function to all incoming clock
zones of a given one in accordance with `incoming_clocked_zones`.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``cz``:
    Base clock zone.

Parameter ``fn``:
    Functor to apply to each of `cz`'s incoming clock zones.)doc";

static const char *__doc_fiction_synchronization_element_layout_foreach_outgoing_clocked_zone =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Applies a function to all outgoing clock
zones of a given one in accordance with `outgoing_clocked_zones`.

Template parameter ``Fn``:
    Functor type that has to comply with the restrictions imposed by
    `mockturtle::foreach_element`.

Parameter ``cz``:
    Base clock zone.

Parameter ``fn``:
    Functor to apply to each of `cz`'s outgoing clock zones.)doc";

static const char *__doc_fiction_synchronization_element_layout_get_synchronization_element =
R"doc(Returns the Hold phase extension in clock cycles of clock zone `cz`.

Parameter ``cz``:
    Clock zone to check.

Returns:
    Synchronization element value, i.e., Hold phase extension, of
    clock zone `cz`.)doc";

static const char *__doc_fiction_synchronization_element_layout_in_degree =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Returns the number of incoming clock zones
to the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s incoming clock zones.)doc";

static const char *__doc_fiction_synchronization_element_layout_incoming_clocked_zones =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Returns a container with all clock zones
that are incoming to the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    A container with all clock zones that are incoming to `cz`.)doc";

static const char *__doc_fiction_synchronization_element_layout_is_incoming_clocked =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Evaluates whether clock zone `cz2` feeds
information to clock zone `cz1`, i.e., whether `cz2` is clocked with a
clock number that is lower by 1 modulo `num_clocks()` or if one of
them is a synchronization element. Due to their extended Hold phase,
they feed information to any clock number.

Parameter ``cz1``:
    Base clock zone.

Parameter ``cz2``:
    Clock zone to check whether it feeds information to `cz1`.

Returns:
    `true` iff `cz2`'s clock number is lower by 1 modulo
    `num_clocks()` or if `cz1` or `cz2` are synchronization elements.)doc";

static const char *__doc_fiction_synchronization_element_layout_is_outgoing_clocked =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Evaluates whether clock zone `cz1` feeds
information to clock zone `cz2`, i.e., whether `cz2` is clocked with a
clock number that is higher by 1 modulo `num_clocks()` or if one of
them is a synchronization element. Due to their extended Hold phase,
they feed information to any clock number.

Parameter ``cz1``:
    Base clock zone.

Parameter ``cz2``:
    Clock zone to check whether it accepts information from `cz1`.

Returns:
    `true` iff `cz2`'s clock number is higher by 1 modulo
    `num_clocks()` or if `cz1` or `cz2` are synchronization elements.)doc";

static const char *__doc_fiction_synchronization_element_layout_is_synchronization_element =
R"doc(Check whether the provided clock zone is a synchronization element.

Parameter ``cz``:
    Clock zone to check.

Returns:
    `true` iff `cz` is a synchronization element.)doc";

static const char *__doc_fiction_synchronization_element_layout_num_se =
R"doc(Returns the number of clock zones acting as synchronization elements
in the layout.

Returns:
    Number of synchronization elements in the layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_out_degree =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Returns the number of outgoing clock zones
from the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    Number of `cz`'s outgoing clock zones.)doc";

static const char *__doc_fiction_synchronization_element_layout_outgoing_clocked_zones =
R"doc(Overwrites the function from `ClockedLayout` to account for
synchronization elements. Returns a container with all clock zones
that are outgoing from the given one.

Parameter ``cz``:
    Base clock zone.

Returns:
    A container with all clock zones that are outgoing from `cz`.)doc";

static const char *__doc_fiction_synchronization_element_layout_strg = R"doc()doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout =
R"doc(Standard constructor. Creates a clocked synchronization element layout
of the given aspect ratio. To this end, it calls `ClockedLayout`'s
standard constructor.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_2 =
R"doc(Standard constructor. Creates a clocked synchronization element layout
of the given aspect ratio and clocks it via the given clocking scheme.
To this end, it calls `ClockedLayout`'s standard constructor.

Parameter ``ar``:
    Highest possible position in the layout.

Parameter ``scheme``:
    Clocking scheme to apply to this layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_3 =
R"doc(Copy constructor from another layout's storage.

Parameter ``s``:
    Storage of another synchronization_element_layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_4 =
R"doc(Copy constructor from another `ClockedLayout`.

Parameter ``lyt``:
    Clocked layout.)doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage = R"doc()doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage_se_map = R"doc()doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage_synchronization_element_layout_storage = R"doc()doc";

static const char *__doc_fiction_take_meet_of_potential_bounds =
R"doc(This function computes a binary meet, overwriting the first argument
with the result. It takes the minimum in case of a lower bound, and a
maximum in case of an upper bound, each corresponding to the minimal
information common to the arguments.

Template parameter ``bound``:
    The potential bound domain which defines the implementation of the
    meet.

Parameter ``a``:
    First potential bound which is overwritten with the result of the
    meet.

Parameter ``b``:
    Second potential bound.)doc";

static const char *__doc_fiction_technology_constraints = R"doc(Target technologies.)doc";

static const char *__doc_fiction_technology_constraints_NONE = R"doc(No technology-specific constraints.)doc";

static const char *__doc_fiction_technology_constraints_TOPOLINANO = R"doc(ToPoLiNano technology-specific constraints.)doc";

static const char *__doc_fiction_technology_dot_drawer =
R"doc(A DOT drawer that extends mockturtle's one by several additional gate
types.

Template parameter ``Ntk``:
    Logic network type.

Template parameter ``DrawIndexes``:
    Flag to toggle the drawing of node indices.

Template parameter ``DrawHexTT``:
    Flag to toggle the representation of truth tables for unknown
    functions in hexadecimal notation.)doc";

static const char *__doc_fiction_technology_dot_drawer_is_node_number = R"doc()doc";

static const char *__doc_fiction_technology_dot_drawer_node_fillcolor = R"doc()doc";

static const char *__doc_fiction_technology_dot_drawer_node_label = R"doc()doc";

static const char *__doc_fiction_technology_dot_drawer_node_label_callback = R"doc()doc";

static const char *__doc_fiction_technology_mapping =
R"doc(Performs technology mapping on the given network. Technology mapping
is the process of replacing the gates in a network with gates from a
given technology library. This function utilizes `mockturtle::map` to
perform the technology mapping. This function is a wrapper around that
interface to provide a more convenient usage.

Template parameter ``Ntk``:
    Input logic network type.

Parameter ``ntk``:
    Input logic network.

Parameter ``params``:
    Technology mapping parameters.

Parameter ``pst``:
    Technology mapping statistics.

Returns:
    Mapped network exclusively using gates from the provided library.)doc";

static const char *__doc_fiction_technology_mapping_params = R"doc()doc";

static const char *__doc_fiction_technology_mapping_params_and2 = R"doc(2-input AND gate.)doc";

static const char *__doc_fiction_technology_mapping_params_and3 = R"doc(3-input AND gate.)doc";

static const char *__doc_fiction_technology_mapping_params_and_xor = R"doc(3-input AND-XOR gate.)doc";

static const char *__doc_fiction_technology_mapping_params_decay =
R"doc(Enforce the application of at least one constant input to three-input
gates.)doc";

static const char *__doc_fiction_technology_mapping_params_dot = R"doc(3-input DOT gate.)doc";

static const char *__doc_fiction_technology_mapping_params_gamble = R"doc(3-input GAMBLE gate.)doc";

static const char *__doc_fiction_technology_mapping_params_ge2 = R"doc(2-input greater-or-equal gate.)doc";

static const char *__doc_fiction_technology_mapping_params_gt2 = R"doc(2-input greater-than gate.)doc";

static const char *__doc_fiction_technology_mapping_params_inv = R"doc()doc";

static const char *__doc_fiction_technology_mapping_params_le2 = R"doc(2-input less-or-equal gate.)doc";

static const char *__doc_fiction_technology_mapping_params_lt2 = R"doc(2-input less-than gate.)doc";

static const char *__doc_fiction_technology_mapping_params_maj3 = R"doc(3-input MAJ gate.)doc";

static const char *__doc_fiction_technology_mapping_params_mapper_params = R"doc(mockturtle's mapper parameters.)doc";

static const char *__doc_fiction_technology_mapping_params_mux = R"doc(3-input MUX gate.)doc";

static const char *__doc_fiction_technology_mapping_params_nand2 = R"doc(2-input NAND gate.)doc";

static const char *__doc_fiction_technology_mapping_params_nor2 = R"doc(2-input NOR gate.)doc";

static const char *__doc_fiction_technology_mapping_params_onehot = R"doc(3-input ONEHOT gate.)doc";

static const char *__doc_fiction_technology_mapping_params_or2 = R"doc(2-input OR gate.)doc";

static const char *__doc_fiction_technology_mapping_params_or_and = R"doc(3-input OR-AND gate.)doc";

static const char *__doc_fiction_technology_mapping_params_xnor2 = R"doc(2-input XNOR gate.)doc";

static const char *__doc_fiction_technology_mapping_params_xor2 = R"doc(2-input XOR gate.)doc";

static const char *__doc_fiction_technology_mapping_params_xor_and = R"doc(3-input XOR-AND gate.)doc";

static const char *__doc_fiction_technology_mapping_stats = R"doc(Statistics for technology mapping.)doc";

static const char *__doc_fiction_technology_mapping_stats_mapper_stats = R"doc(Statistics for mockturtle's mapper.)doc";

static const char *__doc_fiction_technology_mapping_stats_report = R"doc(Report statistics.)doc";

static const char *__doc_fiction_technology_network =
R"doc(A `mockturtle` logic network type that extends `klut_network`. It
reimplements all node creation functions to disable structural
hashing. Furthermore, `create_buf` is overwritten such that it creates
real buffer nodes that count towards the size of the network and
compute the identity function. In the same way, a buffer with more
than one fanout node is considered a fanout node (or splitter in some
contexts) that copies an incoming signal to two or more data paths.

This network is used internally for most implemented FCN physical
design algorithms and extends `klut_network` by some further
convenience functions but is still fully compatible with the
mockturtle network API. The function convert_network can be used to
convert any other mockturtle network to this type.

Most functions in this class are borrowed and reimplemented directly
from `klut_network` and are, therefore, only sporadically documented
where their behavior might differ. For information consider
`mockturtle/networks/klut.hpp`.)doc";

static const char *__doc_fiction_technology_network_add_additional_functions =
R"doc(Adds some further Boolean functions to the truth table cache for fixed
indexing.)doc";

static const char *__doc_fiction_technology_network_clone = R"doc()doc";

static const char *__doc_fiction_technology_network_clone_node = R"doc()doc";

static const char *__doc_fiction_technology_network_create_and = R"doc()doc";

static const char *__doc_fiction_technology_network_create_buf =
R"doc(Creates a buffer node in the network that computes the identity
function and counts towards the network size and depth. A buffer node
with more than one output is called a fanout.

Parameter ``a``:
    Incoming signal to the newly created buffer node.

Returns:
    Outgoing signal of the newly created buffer node.)doc";

static const char *__doc_fiction_technology_network_create_dot = R"doc()doc";

static const char *__doc_fiction_technology_network_create_ge = R"doc()doc";

static const char *__doc_fiction_technology_network_create_gt = R"doc()doc";

static const char *__doc_fiction_technology_network_create_ite = R"doc()doc";

static const char *__doc_fiction_technology_network_create_le = R"doc()doc";

static const char *__doc_fiction_technology_network_create_lt = R"doc()doc";

static const char *__doc_fiction_technology_network_create_maj = R"doc()doc";

static const char *__doc_fiction_technology_network_create_nand = R"doc()doc";

static const char *__doc_fiction_technology_network_create_nary_and = R"doc()doc";

static const char *__doc_fiction_technology_network_create_nary_or = R"doc()doc";

static const char *__doc_fiction_technology_network_create_nary_xor = R"doc()doc";

static const char *__doc_fiction_technology_network_create_node = R"doc(Disable structural hashing.)doc";

static const char *__doc_fiction_technology_network_create_node_2 = R"doc(Disable structural hashing.)doc";

static const char *__doc_fiction_technology_network_create_nor = R"doc()doc";

static const char *__doc_fiction_technology_network_create_not = R"doc()doc";

static const char *__doc_fiction_technology_network_create_or = R"doc()doc";

static const char *__doc_fiction_technology_network_create_xnor = R"doc()doc";

static const char *__doc_fiction_technology_network_create_xor = R"doc()doc";

static const char *__doc_fiction_technology_network_create_xor3 = R"doc()doc";

static const char *__doc_fiction_technology_network_is_and = R"doc()doc";

static const char *__doc_fiction_technology_network_is_and3 = R"doc()doc";

static const char *__doc_fiction_technology_network_is_and_xor = R"doc()doc";

static const char *__doc_fiction_technology_network_is_buf =
R"doc(Returns whether the given node `n` is a buffer node, i.e., whether `n`
computes the identity function and is not a PI. This also returns
`true` on fanout nodes.

Parameter ``n``:
    Node to consider.

Returns:
    `true` iff `n` is a buffer node.)doc";

static const char *__doc_fiction_technology_network_is_dot = R"doc()doc";

static const char *__doc_fiction_technology_network_is_fanout =
R"doc(Returns whether the given node `n` is a fanout node, i.e., whether `n`
is a buffer node and has more than one output signal.

Parameter ``n``:
    Node to consider.

Returns:
    `true` iff `n` is a fanout node.)doc";

static const char *__doc_fiction_technology_network_is_gamble = R"doc()doc";

static const char *__doc_fiction_technology_network_is_ge = R"doc()doc";

static const char *__doc_fiction_technology_network_is_gt = R"doc()doc";

static const char *__doc_fiction_technology_network_is_inv =
R"doc(Returns whether the given node `n` is an inverter node.

Parameter ``n``:
    Node to consider.

Returns:
    `true` iff `n` is an inverter node.)doc";

static const char *__doc_fiction_technology_network_is_ite = R"doc()doc";

static const char *__doc_fiction_technology_network_is_le = R"doc()doc";

static const char *__doc_fiction_technology_network_is_lt = R"doc()doc";

static const char *__doc_fiction_technology_network_is_maj = R"doc()doc";

static const char *__doc_fiction_technology_network_is_nand = R"doc()doc";

static const char *__doc_fiction_technology_network_is_nor = R"doc()doc";

static const char *__doc_fiction_technology_network_is_onehot = R"doc()doc";

static const char *__doc_fiction_technology_network_is_or = R"doc()doc";

static const char *__doc_fiction_technology_network_is_or_and = R"doc()doc";

static const char *__doc_fiction_technology_network_is_po =
R"doc(Checks whether a node is primary output by considering the signal
pointing to it. This is possible because this network does not use
complemented signals.

Parameter ``n``:
    Node to consider.

Returns:
    `true` iff `n`'s signal is primary output.)doc";

static const char *__doc_fiction_technology_network_is_xnor = R"doc()doc";

static const char *__doc_fiction_technology_network_is_xor = R"doc()doc";

static const char *__doc_fiction_technology_network_is_xor3 = R"doc()doc";

static const char *__doc_fiction_technology_network_is_xor_and = R"doc()doc";

static const char *__doc_fiction_technology_network_substitute_po_signals =
R"doc(Adds additional buffer nodes for each primary output that does not
already point to a buffer.)doc";

static const char *__doc_fiction_technology_network_technology_network = R"doc()doc";

static const char *__doc_fiction_technology_network_technology_network_2 = R"doc()doc";

static const char *__doc_fiction_tile_based_layout =
R"doc(This class provides a tile-based naming scheme for coordinate-based
functions. It does not add any functionality, but it might be useful
to adopt tile_based_layout to provide an intuition of abstraction in
the code. For instance, in a gate_level_layout, the coordinates, in
fact, refer to groups of coordinates in lower-level abstractions.
These are called 'tiles' in the literature. Therefore, it might be
helpful for a reader of the code to provide this abstraction level-
dependent information.

Template parameter ``CoordinateLayout``:
    A coordinate layout type.)doc";

static const char *__doc_fiction_tile_based_layout_adjacent_opposite_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_adjacent_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_clone = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_adjacent_opposite_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_adjacent_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_ground_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_ground_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tile_based_layout = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tile_based_layout_2 = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tile_based_layout_3 = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tiles = R"doc()doc";

static const char *__doc_fiction_time_to_solution =
R"doc(This function determines the time-to-solution (TTS) and the accuracy
(acc) of the *QuickSim* algorithm.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    Layout that is used for the simulation.

Parameter ``quicksim_params``:
    Parameters required for the *QuickSim* algorithm.

Parameter ``tts_params``:
    Parameters used for the time-to-solution calculation.

Parameter ``ps``:
    Pointer to a struct where the results (time_to_solution, acc,
    single runtime) are stored.)doc";

static const char *__doc_fiction_time_to_solution_for_given_simulation_results =
R"doc(This function calculates the Time-to-Solution (TTS) by analyzing the
simulation results of a heuristic algorithm in comparison to those of
an exact algorithm. It provides further statistical metrics, including
the accuracy of the heuristic algorithm, and individual runtimes.

Template parameter ``Lyt``:
    SiDB ell-level layout type.

Parameter ``results_exact``:
    Simulation results of the exact algorithm.

Parameter ``results_heuristic``:
    Simulation of the heuristic for which the TTS is determined.

Parameter ``confidence_level``:
    Confidence level for the TTS computation. The confidence level
    represents the probability that the confidence interval calculated
    from the simulation contains the true value. For example, a 95 %
    (0.95) confidence level means that if the simulation were repeated
    many times, approximately 95 out of 100 of the calculated
    confidence intervals would contain the true value.

Parameter ``ps``:
    Pointer to a struct where the statistics of this function call
    (time_to_solution, acc, single runtime) are to be stored.)doc";

static const char *__doc_fiction_time_to_solution_params = R"doc()doc";

static const char *__doc_fiction_time_to_solution_params_confidence_level =
R"doc(The confidence level represents the probability that the confidence
interval calculated from the simulation contains the true value. For
example, a 99.7 % (0.997) confidence level means that if the
simulation were repeated many times, approximately 997 out of 1000 of
the calculated confidence intervals would contain the true value.)doc";

static const char *__doc_fiction_time_to_solution_params_engine =
R"doc(Exhaustive simulation algorithm used to simulate the ground state as
reference.)doc";

static const char *__doc_fiction_time_to_solution_params_repetitions =
R"doc(Number of iterations of the heuristic algorithm used to determine the
simulation accuracy (`repetitions = 100` means that accuracy is
precise to 1 %).)doc";

static const char *__doc_fiction_time_to_solution_stats =
R"doc(This struct stores the time-to-solution, the simulation accuracy and
the average single simulation runtime of *QuickSim*, the single
runtime of the exact simulator used, and the number of valid charge
configurations found by the exact algorithm.)doc";

static const char *__doc_fiction_time_to_solution_stats_acc = R"doc(Accuracy of the simulation in %.)doc";

static const char *__doc_fiction_time_to_solution_stats_algorithm =
R"doc(Exact simulation algorithm used to simulate the ground state as
reference.)doc";

static const char *__doc_fiction_time_to_solution_stats_mean_single_runtime = R"doc(Average single simulation runtime in seconds.)doc";

static const char *__doc_fiction_time_to_solution_stats_report =
R"doc(Print the results to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_time_to_solution_stats_single_runtime_exact =
R"doc(Single simulation runtime of the exact ground state simulation
algorithm.)doc";

static const char *__doc_fiction_time_to_solution_stats_time_to_solution = R"doc(Time-to-solution in seconds.)doc";

static const char *__doc_fiction_to_sidb_cluster =
R"doc(This function initiates the recursive procedure of converting a binary
cluster hierarchy to our bespoke version.

Parameter ``n``:
    A node from a binary cluster hierarchy, as for instance returned
    by parsing ALGLIB's result.

Returns:
    A uniquely identified node in a decorated cluster hierarchy that
    follows the "general tree" structure.)doc";

static const char *__doc_fiction_to_unique_sidb_cluster =
R"doc(This recursive function is used to convert a binary cluster hierarchy,
as for instance returned by `sidb_cluster_hierarchy` function that
uses ALGLIB's `clusterizer`. The returned structure includes parent
pointers.

Parameter ``n``:
    A node from a binary cluster hierarchy, as for instance returned
    by parsing ALGLIB's result.

Parameter ``uid``:
    Variable reference which is updated in each execution to ensure
    uniqueness.

Returns:
    A uniquely identified node in a decorated cluster hierarchy that
    follows the "general tree" structure.)doc";

static const char *__doc_fiction_transition_type =
R"doc(Possible types of charge transitions that can occur in an SiDB layout.
These transitions represent changes in the charge state of SiDBs,
including transitions from neutral to negative, negative to neutral,
neutral to positive, and positive to neutral.)doc";

static const char *__doc_fiction_transition_type_NEGATIVE_TO_NEUTRAL = R"doc(SiDB is negatively charged, but is closest to being neutrally charged.)doc";

static const char *__doc_fiction_transition_type_NEUTRAL_TO_NEGATIVE = R"doc(SiDB is neutrally charged, but is closest to being negatively charged.)doc";

static const char *__doc_fiction_transition_type_NEUTRAL_TO_POSITIVE = R"doc(SiDB is neutrally charged, but is closest to being positively charged.)doc";

static const char *__doc_fiction_transition_type_POSITIVE_TO_NEUTRAL = R"doc(SiDB is positively charged, but is closest to being neutrally charged.)doc";

static const char *__doc_fiction_tt_reader =
R"doc(Simple reader to parse truth tables from files. The files must be
organized as follows: Each line consists of a truth table in
hexadecimal form plus its corresponding Boolean expression. The two
are separated by a space.

This format is used by, e.g., Alan Mishchenko for his DSD functions:
https://people.eecs.berkeley.edu/~alanmi/temp5/)doc";

static const char *__doc_fiction_tt_reader_elements =
R"doc(Contains the hexadecimal truth tables and their Boolean expressions in
string representation. The two are alternating.)doc";

static const char *__doc_fiction_tt_reader_next =
R"doc(Returns the next truth table parsed from the file or std::nullopt if
no further truth tables are available.

Returns:
    Next truth table from the file.)doc";

static const char *__doc_fiction_tt_reader_pos = R"doc(Current position in the vector.)doc";

static const char *__doc_fiction_tt_reader_read =
R"doc(Reads hexadecimal truth table representations as well as Boolean
expressions from the given stream.

Parameter ``in_stream``:
    Stream of truth table representations.)doc";

static const char *__doc_fiction_tt_reader_tt_reader =
R"doc(Standard constructor. Reads the given stream into a vector of lines.

Parameter ``stream``:
    Stream to parse.)doc";

static const char *__doc_fiction_tt_reader_tt_reader_2 =
R"doc(Standard constructor. Reads the given file into a vector of lines.

Parameter ``filename``:
    File to parse.)doc";

static const char *__doc_fiction_twoddwave_clocking =
R"doc(Returns the 2DDWave clocking as defined in \"Clocking and Cell
Placement for QCA\" by V. Vankamamidi, M. Ottavi, and F. Lombardi in
IEEE Conference on Nanotechnology 2006.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``n``:
    Number of clocks.

Returns:
    2DDWave clocking scheme.)doc";

static const char *__doc_fiction_twoddwave_distance =
R"doc(The 2DDWave distance :math:`D` between two layout coordinates :math:`s
= (x_1, y_1)` and :math:`t = (x_2, y_2)` given by

:math:`D = |x_1 - x_2| + |y_1 - y_2|` iff :math:`s \leq t` and
:math:`\infty`, otherwise.

Thereby, :math:`s \leq t` iff :math:`x_1 \leq x_2` and :math:`y_1 \leq
y_2`.

@note To represent :math:`\infty`,
`std::numeric_limits<uint32_t>::max()` is returned for distances of
infinite length. We are using `uint32_t` to prevent overflows when
adding distances in the default `uint64_t` number range.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral type for the distance.

Parameter ``lyt``:
    Layout.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    2DDWave distance between `source` and `target`.)doc";

static const char *__doc_fiction_twoddwave_distance_functor =
R"doc(A pre-defined distance functor that uses the 2DDWave distance.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Dist``:
    Integral distance type.)doc";

static const char *__doc_fiction_twoddwave_distance_functor_twoddwave_distance_functor = R"doc()doc";

static const char *__doc_fiction_twoddwave_hex_clocking =
R"doc(Returns a hexagonal variation of the 2DDWave clocking as originally
defined in \"Clocking and Cell Placement for QCA\" by V. Vankamamidi,
M. Ottavi, and F. Lombardi in IEEE Conference on Nanotechnology 2006.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``n``:
    Number of clocks.

Returns:
    Hexagonal 2DDWave clocking scheme.)doc";

static const char *__doc_fiction_undefined_cell_label_exception = R"doc()doc";

static const char *__doc_fiction_undefined_cell_label_exception_undefined_cell_label_exception = R"doc()doc";

static const char *__doc_fiction_undefined_cell_label_exception_undefined_label = R"doc()doc";

static const char *__doc_fiction_undefined_cell_label_exception_which = R"doc()doc";

static const char *__doc_fiction_unit_cost =
R"doc(Unit cost between two layout coordinates that always returns 1.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Cost``:
    Integral type for the cost.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Unit cost between `source` and `target`, i.e., 1.)doc";

static const char *__doc_fiction_unit_cost_functor =
R"doc(A pre-defined cost functor that uses unit costs.

Template parameter ``Lyt``:
    Coordinate layout type.

Template parameter ``Cost``:
    Integral cost type.)doc";

static const char *__doc_fiction_unit_cost_functor_unit_cost_functor = R"doc()doc";

static const char *__doc_fiction_unrecognized_cell_definition_exception = R"doc()doc";

static const char *__doc_fiction_unrecognized_cell_definition_exception_line = R"doc()doc";

static const char *__doc_fiction_unrecognized_cell_definition_exception_unrecognized_cell_definition_exception = R"doc()doc";

static const char *__doc_fiction_unrecognized_cell_definition_exception_where = R"doc()doc";

static const char *__doc_fiction_unsuccessful_gate_design_error =
R"doc(Exception thrown if the gate design was unsuccessful. Depending on the
given gate design parameters and the defect density, the gate design
may fail.)doc";

static const char *__doc_fiction_unsuccessful_gate_design_error_unsuccessful_gate_design_error =
R"doc(This explicit constructor initializes the base `std::runtime_error`
class with the provided error message, ensuring that the exception
contains detailed information about the reason for the gate design
failure.

Parameter ``msg``:
    A descriptive message explaining why the gate design failed.)doc";

static const char *__doc_fiction_unsuccessful_pr_error =
R"doc(If the blacklist is overly restrictive, finding a valid placement and
routing becomes impossible, resulting in this exception being thrown.)doc";

static const char *__doc_fiction_unsuccessful_pr_error_unsuccessful_pr_error =
R"doc(This class inherits from `std::runtime_error` and is used to signal
errors related to unsuccessful placement and routing.

Parameter ``msg``:
    The error message describing the unsuccessful placement and
    routing.)doc";

static const char *__doc_fiction_unsupported_cell_type_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_cell_type_exception_coord = R"doc()doc";

static const char *__doc_fiction_unsupported_cell_type_exception_unsupported_cell_type_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_cell_type_exception_where = R"doc()doc";

static const char *__doc_fiction_unsupported_character_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_character_exception_unsupported_char = R"doc()doc";

static const char *__doc_fiction_unsupported_character_exception_unsupported_character_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_character_exception_which = R"doc()doc";

static const char *__doc_fiction_unsupported_clocking_scheme_exception =
R"doc(Exception to be thrown when an unsupported clocking scheme is
requested.)doc";

static const char *__doc_fiction_unsupported_clocking_scheme_exception_unsupported_clocking_scheme_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_clocking_scheme_exception_what = R"doc()doc";

static const char *__doc_fiction_unsupported_defect_index_exception =
R"doc(Exception thrown when an unsupported defect index is encountered in
the parsed file.)doc";

static const char *__doc_fiction_unsupported_defect_index_exception_unsupported_defect_index_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_defect_index_exception_unsupported_index = R"doc()doc";

static const char *__doc_fiction_unsupported_defect_index_exception_which = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception =
R"doc(Exception to be thrown when a layout hosts a gate with an orientation
that is unsupported by the applied gate library.

Template parameter ``CoordinateType``:
    Type of the layout coordinates.

Template parameter ``PortType``:
    Type of the library ports.)doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_coord = R"doc(Coordinate at which the unsupported gate orientation was found.)doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_ports = R"doc(Ports of the unsupported gate orientation.)doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_unsupported_gate_orientation_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_where =
R"doc(Coordinate at which the unsupported gate orientation was found.

Returns:
    Coordinate.)doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_which_ports =
R"doc(Ports of the unsupported gate orientation.

Returns:
    Ports.)doc";

static const char *__doc_fiction_unsupported_gate_type_exception =
R"doc(Exception to be thrown when a layout hosts a gate type that is not
implemented in the applied gate library.

Template parameter ``CoordinateType``:
    Type of the layout coordinates.)doc";

static const char *__doc_fiction_unsupported_gate_type_exception_coord = R"doc(Coordinate at which the unsupported gate type was found.)doc";

static const char *__doc_fiction_unsupported_gate_type_exception_unsupported_gate_type_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_type_exception_where = R"doc()doc";

static const char *__doc_fiction_use_clocking =
R"doc(Returns the USE clocking as defined in \"USE: A Universal, Scalable,
and Efficient Clocking Scheme for QCA\" by Caio Araujo T. Campos,
Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD
2015.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    USE clocking scheme.)doc";

static const char *__doc_fiction_verify_logic_match =
R"doc(Checks if a given charge distribution correctly encodes the expected
logic for a specified input pattern, based on a provided truth table.

@note Kinks are rejected.

Example: In the ground state charge distribution of an AND gate, kinks
are rejected for the gate to be considered operational. Given an input
pattern of `01`, this function will: - Verify that the left input wire
encodes `0`. - Verify that the right input wire encodes `1`. - Verify
that the output wire encodes `0`.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``cds``:
    Charge distribution surface, containing charge state information
    for each SiDB.

Parameter ``params``:
    The parameters used to determine if a layout is `operational` or
    `non-operational`.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``input_pattern``:
    The specific input pattern of the given charge distribution
    surface.

Parameter ``input_wires``:
    Input BDL wires.

Parameter ``output_wires``:
    Output BDL wires.

Returns:
    The operational status indicating if the charge distribution
    matches the logic for the given input pattern.)doc";

static const char *__doc_fiction_vertex_coloring =
R"doc(A vertex coloring is simply a hash map from vertex IDs to Color types
where Color should be constructible/convertible from int.

Template parameter ``Graph``:
    Graph type to color.

Template parameter ``Color``:
    Color type to use. Must be convertible from int.)doc";

static const char *__doc_fiction_vertical_shift_cartesian =
R"doc(\verbatim +-------+ | | | +-------+ | | | +-------+ | | | +-------+
\endverbatim)doc";

static const char *__doc_fiction_virtual_miter =
R"doc(Combines two networks into a combinational miter, similar to
`mockturtle::miter`. Either input network may include virtual primary
inputs (PIs). Virtual PIs are removed during miter construction, and
all edges connected to them are remapped to their corresponding
original PIs, ensuring consistent PI counts when the networks are
equivalent.

The resulting miter connects two networks with the same number of
inputs and produces a single primary output. This output represents
the OR of the XORs of all primary output pairs. Thus, the miter
outputs 1 for any input assignment where the primary outputs of the
two networks differ.

The input networks may have different types. If the two input networks
have mismatched numbers of primary inputs or outputs, the method
returns `std::nullopt`.

Template parameter ``NtkDest``:
    The type of the resulting network.

Template parameter ``NtkSource1``:
    The type of the first input network.

Template parameter ``NtkSource2``:
    The type of the second input network.

Parameter ``ntk1_in``:
    The first input network.

Parameter ``ntk2_in``:
    The second input network.

Returns:
    An `optional` containing the virtual miter network if successful,
    or `std::nullopt` if the networks are incompatible.)doc";

static const char *__doc_fiction_virtual_pi_network = R"doc()doc";

static const char *__doc_fiction_virtual_pi_network_clone = R"doc(Clones the `virtual_pi_network` object.)doc";

static const char *__doc_fiction_virtual_pi_network_create_virtual_pi =
R"doc(Create a virtual PI, which is a mapping to a real PI.

This function adds a PI to the parent network, but marks it as virtual
and stores a mapping to a real PI.

Parameter ``real_pi``:
    The node representing the real PI in the network.

Returns:
    The signal of the newly created virtual PI.)doc";

static const char *__doc_fiction_virtual_pi_network_foreach_real_ci =
R"doc(Iterates over the virtual CIs of the circuit and applies a given
function.

Template parameter ``Fn``:
    The type of the function to be applied.

Parameter ``fn``:
    The function to be applied.)doc";

static const char *__doc_fiction_virtual_pi_network_foreach_real_pi =
R"doc(Iterates over the real PIs of the circuit and applies a given
function.

Template parameter ``Fn``:
    The type of the function to be applied.

Parameter ``fn``:
    The function to be applied.)doc";

static const char *__doc_fiction_virtual_pi_network_foreach_virtual_ci =
R"doc(Iterates over the virtual CIs of the circuit and applies a given
function.

Template parameter ``Fn``:
    The type of the function to be applied.

Parameter ``fn``:
    The function to be applied.)doc";

static const char *__doc_fiction_virtual_pi_network_foreach_virtual_pi =
R"doc(Iterates over the virtual PIs of the circuit and applies a given
function.

Template parameter ``Fn``:
    The type of the function to be applied.

Parameter ``fn``:
    The function to be applied.)doc";

static const char *__doc_fiction_virtual_pi_network_get_real_pi =
R"doc(Get the real PI associated with a virtual PI node.

Parameter ``v_pi``:
    The virtual pi node to retrieve the real PI for.

Returns:
    The real pi associated with the virtual PI node.)doc";

static const char *__doc_fiction_virtual_pi_network_is_real_ci =
R"doc(Check if a given node is a real CI in the `virtual_pi_network`.

Parameter ``n``:
    The node to check.

Returns:
    `true` if the node is a real CI, `false` otherwise.)doc";

static const char *__doc_fiction_virtual_pi_network_is_real_pi =
R"doc(Check if a given node is a real PI. Real PIs are created with
`create_pi()`.

Parameter ``n``:
    The node to check.

Returns:
    `true` if the node is a real PI, `false` otherwise.)doc";

static const char *__doc_fiction_virtual_pi_network_is_virtual_ci =
R"doc(Check if a given node is a virtual CI in the `virtual_pi_network`.

Parameter ``n``:
    The node to check.

Returns:
    `true` if the node is a virtual CI, `false` otherwise.)doc";

static const char *__doc_fiction_virtual_pi_network_is_virtual_pi =
R"doc(Check if a given node is a virtual PI. Virtual PIs are created with
`create_virtual_pi()`.

Parameter ``n``:
    The node to check.

Returns:
    `true` if the node is a virtual PI, `false` otherwise.)doc";

static const char *__doc_fiction_virtual_pi_network_num_real_cis =
R"doc(Get the number of real CIs in the `virtual_pi_network`.

Returns:
    The number of real CIs as a `uint32_t`.)doc";

static const char *__doc_fiction_virtual_pi_network_num_real_pis =
R"doc(Get the number of real PIs in the `virtual_pi_network`.

Returns:
    The number of real PIs as a `uint32_t`.)doc";

static const char *__doc_fiction_virtual_pi_network_num_virtual_cis =
R"doc(Get the number of virtual CIs in the `virtual_pi_network`.

Returns:
    The number of virtual CIs as a `uint32_t`.)doc";

static const char *__doc_fiction_virtual_pi_network_num_virtual_pis =
R"doc(Get the number of virtual PIs in the `virtual_pi_network`.

Returns:
    The number of virtual PIs as a `uint32_t`.)doc";

static const char *__doc_fiction_virtual_pi_network_real_size =
R"doc(Calculate the real size of the `virtual_pi_network`.

The real size of the network is considered the size without virtual
PIs.

Returns:
    The real size of the `virtual_pi_network` as a `uint32_t`.)doc";

static const char *__doc_fiction_virtual_pi_network_v_storage = R"doc(Shared pointer of the virtual PI storage.)doc";

static const char *__doc_fiction_virtual_pi_network_virtual_pi_network =
R"doc(Default constructor for the `virtual_pi_network` class. Initializes
`v_storage` as a shared pointer.)doc";

static const char *__doc_fiction_virtual_pi_network_virtual_pi_network_2 =
R"doc(Constructor for the `virtual_pi_network` class that takes a network
and a shared pointer to a `virtual_storage` object. This is used for
cloning.

Template parameter ``Ntk``:
    Network type.

Parameter ``ntk``:
    Input network.

Parameter ``s``:
    Shared pointer to the `virtual_storage` object to be used by this
    `virtual_pi_network`.)doc";

static const char *__doc_fiction_virtual_pi_network_virtual_storage = R"doc()doc";

static const char *__doc_fiction_virtual_pi_network_virtual_storage_map_virtual_to_real_pi = R"doc(Map from virtual_pis to real_pis.)doc";

static const char *__doc_fiction_virtual_pi_network_virtual_storage_virtual_inputs = R"doc(Vector storing virtual_inputs.)doc";

static const char *__doc_fiction_volume =
R"doc(Computes the volume of a given coordinate assuming its origin is (0,
0, 0). Calculates :math:`(|x| + 1) \cdot (|y| + 1) \cdot (|z| + 1)` by
default. For SiQAD coordinates, which are planar by definition, the
area is returned.

Template parameter ``CoordinateType``:
    Coordinate type.

Parameter ``coord``:
    Coordinate.

Returns:
    Volume of coord.)doc";

static const char *__doc_fiction_wiring_reduction =
R"doc(A scalable wiring reduction algorithm for 2DDWave-clocked layouts
based on A* path finding.

The core concept revolves around the selective removal of excess
wiring by cutting them from a layout, contingent upon the ability to
restore functional correctness by realigning the remaining layout
fragments. Given the complexity of identifying these cuts,
obstructions are strategically inserted into the layout to safeguard
against the inadvertent deletion of standard gates or wire segments
essential for the layout's integrity. Leveraging the obstructed layout
as a basis, A* Search is employed to systematically identify feasible
cuts either from left to right or top to bottom. Subsequently, these
identified cuts are removed from the layout to minimize not only the
number of wire segments, but also the area and critical path length.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    The 2DDWave-clocked layout whose wiring is to be reduced.

Parameter ``ps``:
    Parameters.

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_wiring_reduction_params = R"doc(Parameters for the wiring reduction algorithm.)doc";

static const char *__doc_fiction_wiring_reduction_params_timeout =
R"doc(Timeout limit (in ms). Specifies the maximum allowed time in
milliseconds for the optimization process. For large layouts, the
actual execution time may slightly exceed this limit because it's
impractical to check the timeout at every algorithm step and the
functional correctness has to be ensured by completing essential
algorithm steps.)doc";

static const char *__doc_fiction_wiring_reduction_stats = R"doc(This struct stores statistics about the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_area_improvement = R"doc(Improvement in layout area.)doc";

static const char *__doc_fiction_wiring_reduction_stats_duration = R"doc(Runtime of the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_num_wires_after = R"doc(Number of wire segments after the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_num_wires_before = R"doc(Number of wire segments before the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_wiring_reduction_stats_wiring_improvement = R"doc(Improvement in the number wire segments.)doc";

static const char *__doc_fiction_wiring_reduction_stats_x_size_after = R"doc(Layout width after the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_x_size_before = R"doc(Layout width before the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_y_size_after = R"doc(Layout height before the wiring reduction process.)doc";

static const char *__doc_fiction_wiring_reduction_stats_y_size_before = R"doc(Layout height before the wiring reduction process.)doc";

static const char *__doc_fiction_write_defect_influence_domain =
R"doc(Writes a CSV representation of an defect influence domain to the
specified output stream. The data are written as rows, each
corresponding to one set of simulation parameters and their
corresponding influence status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
Influence STATUS ... subsequent rows for each set of simulation
parameters.

Parameter ``defect_infdom``:
    The defect influence domain to be written. It contains a mapping
    from defect positions to their influence status.

Parameter ``os``:
    The output stream where the CSV representation of the defect
    influence domain is written to.

Parameter ``params``:
    The parameters used for writing, including the influential and
    non-influential tags. Defaults to an empty
    `write_defect_influence_domain_params` object, which provides
    standard tags.)doc";

static const char *__doc_fiction_write_defect_influence_domain_2 =
R"doc(Writes a CSV representation of an defect influence domain to the
specified file. The data are written as rows, each corresponding to
one set of simulation parameters and their corresponding influence
status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
Influence STATUS ... subsequent rows for each set of simulation
parameters.

Parameter ``defect_infdom``:
    The defect influence domain to be written. It contains a mapping
    from defect positions to their influence status.

Parameter ``filename``:
    The filename where the CSV representation of the defect influence
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the influential and
    non-influential tags. Defaults to an empty
    `write_defect_influence_domain_params` object, which provides
    standard tags.)doc";

static const char *__doc_fiction_write_defect_influence_domain_params = R"doc(Parameters for writing a defect influence domain to a CSV file.)doc";

static const char *__doc_fiction_write_defect_influence_domain_params_influential_tag = R"doc(The tag used to represent the influential value of a defect position.)doc";

static const char *__doc_fiction_write_defect_influence_domain_params_non_influential_tag =
R"doc(The tag used to represent the non-influential value of a defect
position.)doc";

static const char *__doc_fiction_write_dot_layout =
R"doc(! Writes layout in DOT format into output stream

An overloaded variant exists that writes the layout into a file.

**Required network functions:** - is_pi - foreach_node - foreach_fanin

Parameter ``lyt``:
    Layout

Parameter ``os``:
    Output stream)doc";

static const char *__doc_fiction_write_dot_layout_2 =
R"doc(! Writes layout in DOT format into a file

**Required network functions:** - is_pi - foreach_node - foreach_fanin

Parameter ``lyt``:
    Layout

Parameter ``filename``:
    Filename)doc";

static const char *__doc_fiction_write_fgl_layout =
R"doc(Writes an FGL layout to a file.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    Layout.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_write_fgl_layout_2 =
R"doc(Writes an FGL layout to a file.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Layout.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    .fgl extension.)doc";

static const char *__doc_fiction_write_fqca_layout =
R"doc(Writes a cell-level QCA layout to an fqca file provided as an output
stream. The format is used by QCA-STACK by Willem Lambooy
(https://github.com/wlambooy/QCA-STACK).

Might throw an out_of_cell_names_exception in case there are more I/O
cells in the layout than lowercase + uppercase letters in the English
alphabet.

Template parameter ``Lyt``:
    The layout type to be written. Must be a clocked cell-level QCA
    layout.

Parameter ``lyt``:
    The cell-level QCA layout.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_fqca_layout_2 =
R"doc(Writes a cell-level QCA layout to an fqca file provided as a file
name. The format is used by QCA-STACK by Willem Lambooy
(https://github.com/wlambooy/QCA-STACK).

Might throw an out_of_cell_names_exception in case there are more I/O
cells in the layout than lowercase + uppercase letters in the English
alphabet.

Template parameter ``Lyt``:
    The layout type to be written. Must be a clocked cell-level QCA
    layout.

Parameter ``lyt``:
    The cell-level QCA layout.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.fqca` extension.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_fqca_layout_params = R"doc(Parameters for writing FQCA layouts.)doc";

static const char *__doc_fiction_write_fqca_layout_params_create_inter_layer_via_cells = R"doc(Create via cells in between each layer.)doc";

static const char *__doc_fiction_write_location_and_ground_state =
R"doc(Writes the coordinates of all SiDBs of a layout together with the
charge distribution of the ground state(s) to a file.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Parameter ``sim_result``:
    The simulation result to write.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_write_location_and_ground_state_2 =
R"doc(Writes the coordinates of all SiDBs of a layout together with the
charge distribution of the ground state(s) to a file.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Template parameter ``sim_result``:
    The simulation result to write.

Parameter ``filename``:
    The file name to create and write into.)doc";

static const char *__doc_fiction_write_operational_domain =
R"doc(Writes a CSV representation of an operational domain to the specified
output stream. The data are written as rows, each corresponding to one
set of simulation parameters and their corresponding operational
status.

The output CSV format is e.g. as follows: \verbatim embed:rst .. code-
block:: RST

epsilon_r, lambda_tf, operational status 0.0, 0.0, 0 0.1, 0.0, 1 ...
subsequent rows for each set of simulation parameters \endverbatim

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Template parameter ``OpDomain``:
    The type of the operational domain.

Parameter ``opdom``:
    The operational domain to be written. It represents a mapping
    between sets of simulation parameters (defined as a pair of sweep
    parameters for the X, Y, and Z dimensions) and a tuple containing
    detailed information about the SiDB layout associated with those
    simulation parameters.

Parameter ``os``:
    The output stream where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_operational_domain_params` object, which provides standard
    tags.

Throws:
    std::invalid_argument if the number of dimensions in the
    operational domain is 0 or greater than 3.)doc";

static const char *__doc_fiction_write_operational_domain_2 =
R"doc(Writes a CSV representation of an operational domain to the specified
file. The data are written as rows, each corresponding to one set of
simulation parameters and their corresponding operational status.

The output CSV format is e.g. as follows: \verbatim embed:rst .. code-
block:: RST

epsilon_r, lambda_tf, operational status 0.0, 0.0, 0 0.1, 0.0, 1 ...
subsequent rows for each set of simulation parameters \endverbatim

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Template parameter ``OpDomain``:
    The type of the operational domain.

Parameter ``opdom``:
    The operational domain to be written. It represents a mapping
    between sets of simulation parameters (defined as a pair of sweep
    parameters for the X, Y, and Z dimensions) and a tuple containing
    detailed information about the SiDB layout associated with those
    simulation parameters.

Parameter ``filename``:
    The filename where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_operational_domain_params` object, which provides standard
    tags.

Throws:
    std::ofstream::failure if the file could not be opened.)doc";

static const char *__doc_fiction_write_operational_domain_params = R"doc(Parameters for writing an operational domain to a CSV file.)doc";

static const char *__doc_fiction_write_operational_domain_params_non_operational_tag =
R"doc(The tag used to represent the non-operational value of a parameter
set.)doc";

static const char *__doc_fiction_write_operational_domain_params_operational_tag = R"doc(The tag used to represent the operational value of a parameter set.)doc";

static const char *__doc_fiction_write_operational_domain_params_sample_writing_mode = R"doc(Mode selector for writing samples to file.)doc";

static const char *__doc_fiction_write_operational_domain_params_sample_writing_mode_ALL_SAMPLES =
R"doc(Write all samples, including non-operational ones. This may lead to
large file sizes.)doc";

static const char *__doc_fiction_write_operational_domain_params_sample_writing_mode_OPERATIONAL_ONLY =
R"doc(Write operational samples only. This can drastically reduce file size
and help with visibility in 3D plots.)doc";

static const char *__doc_fiction_write_operational_domain_params_writing_mode =
R"doc(Whether to write non-operational samples to the CSV file. If set to
`OPERATIONAL_ONLY`, operational samples are written exclusively. This
yields a significantly smaller CSV file. It is recommended to set this
option for 3D plots because the non-operational samples would shadow
the operational samples anyway.)doc";

static const char *__doc_fiction_write_qca_layout =
R"doc(Writes a cell-level QCA layout to a qca file that is used by
QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/), a physical
simulator for the QCA technology platform.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    Cell-level QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qca_layout_2 =
R"doc(Writes a cell-level QCA layout to a qca file that is used by
QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/), a physical
simulator for the QCA technology platform.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Cell-level QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.qca` extension.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qca_layout_params = R"doc(Parameters for writing QCADesigner layouts.)doc";

static const char *__doc_fiction_write_qca_layout_params_create_inter_layer_via_cells = R"doc(Create via cells in between each layer.)doc";

static const char *__doc_fiction_write_qca_layout_svg =
R"doc(Writes an SVG representation of a cell-level QCA layout into an output
stream. Both tile- and cell-based layouts are supported. For tile-
based layouts, QCA layouts of tile size :math:`5 \times 5` are
supported exclusively so far.

The utilized color scheme is based on the standard scheme used in
QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/).

May throw an `unsupported_cell_type_exception` if it encounters
unsupported cell types in the layout.

Template parameter ``Lyt``:
    Cell-level QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qca_layout_svg_2 =
R"doc(Writes an SVG representation of a cell-level QCA layout into a file.
Both tile- and cell-based layouts are supported. For tile-based
layouts, QCA layouts of tile size :math:`5 \times 5` are supported
exclusively so far.

The utilized color scheme is based on the standard scheme used in
QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/).

May throw an `unsupported_cell_type_exception` if it encounters
unsupported cell types in the layout. May throw an
`std::ofstream::failure` if it cannot open the file.

Template parameter ``Lyt``:
    Cell-level QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.svg` extension.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qca_layout_svg_params = R"doc(Parameters for writing SVG QCA layouts.)doc";

static const char *__doc_fiction_write_qca_layout_svg_params_simple = R"doc(Limit details to create smaller file sizes.)doc";

static const char *__doc_fiction_write_qcc_layout =
R"doc(Writes a cell-level iNML layout to a qcc file that is used by
ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
physical simulator for the iNML technology platform.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    Cell-level iNML layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qcc_layout_2 =
R"doc(Writes a cell-level iNML layout to a qcc file that is used by
ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
physical simulator for the iNML technology platform.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Cell-level iNML layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.qcc` extension.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qcc_layout_params = R"doc(Parameters for writing QCC layouts.)doc";

static const char *__doc_fiction_write_qcc_layout_params_filename = R"doc(Filename of the QCC file.)doc";

static const char *__doc_fiction_write_qcc_layout_params_use_filename_as_component_name = R"doc(Use the given filename as the component name inside the QCC file.)doc";

static const char *__doc_fiction_write_qll_layout =
R"doc(Writes a cell-level QCA or iNML layout to a qll file that is used by
ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
physical simulator for the iNML technology platform as well as SCERPA
(https://ieeexplore.ieee.org/document/8935211), a physical simulator
for the mQCA technology platform.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    Cell-level QCA or iNML layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_qll_layout_2 =
R"doc(Writes a cell-level QCA or iNML layout to a qll file that is used by
ToPoliNano & MagCAD (https://topolinano.polito.it/), an EDA tool and a
physical simulator for the iNML technology platform as well as SCERPA
(https://ieeexplore.ieee.org/document/8935211), a physical simulator
for the mQCA technology platform.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Cell-level iNML layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.qll` extension.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_sidb_layout_svg =
R"doc(Writes an SVG representation of an SiDB cell-level SiDB layout into an
output stream.

@note SiDB defects are not supported yet.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_2 =
R"doc(Writes an SVG representation of an SiDB cell-level SiDB layout into a
file.

@note SiDB defects are not supported yet.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into.

Parameter ``ps``:
    Parameters.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params = R"doc(Parameters for writing SiDB layouts to SVG format.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_color_background = R"doc(The color mode of the background for the SVG output.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_color_mode = R"doc(Enumeration to specify the color mode for the SVG output.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_color_mode_DARK = R"doc(Dark mode.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_color_mode_LIGHT = R"doc(Light mode.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_lattice_mode = R"doc(The lattice mode of the SiDB layout.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_lattice_point_size = R"doc(Size of the H-Si lattice points in SVG units.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_sidb_border_width = R"doc(Border width of the SiDB.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_sidb_lattice_mode =
R"doc(Enumeration to specify if the H-Si lattice is plotted in addition to
SiDBs.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_sidb_lattice_mode_HIDE_LATTICE = R"doc(Lattice is hidden. Only SiDBs are shown.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_sidb_lattice_mode_SHOW_LATTICE = R"doc(Lattice is shown.)doc";

static const char *__doc_fiction_write_sidb_layout_svg_params_sidb_size = R"doc(Size of the SiDB in SVG units.)doc";

static const char *__doc_fiction_write_sqd_layout =
R"doc(Writes a cell-level SiDB or QCA layout to an sqd file that is used by
SiQAD (https://github.com/siqad/siqad), a physical simulator for the
SiDB technology platform.

If the provided cell-level layout type can represent SiDB defects,
they will be written to the file as well.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    Cell-level SiDB or QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_write_sqd_layout_2 =
R"doc(Writes a cell-level SiDB or QCA layout to an sqd file that is used by
SiQAD (https://github.com/siqad/siqad), a physical simulator for the
SiDB technology platform.

If the provided cell-level layout type can represent SiDB defects,
they will be written to the file as well.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Cell-level SiDB or QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.sqd` extension.)doc";

static const char *__doc_fiction_write_sqd_sim_result =
R"doc(Writes an SiDB simulation result to an XML file that is used by SiQAD
(https://github.com/siqad/siqad), a physical simulator for the SiDB
technology platform.

This overload uses an output stream to write into.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Parameter ``sim_result``:
    The simulation result to write.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_write_sqd_sim_result_2 =
R"doc(Writes an SiDB simulation result to an XML file that is used by SiQAD
(https://github.com/siqad/siqad), a physical simulator for the SiDB
technology platform.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    SiDB cell-level SiDB layout type.

Parameter ``sim_result``:
    The simulation result to write.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.xml` extension.)doc";

static const char *__doc_fiction_yen_k_shortest_paths =
R"doc(Yen's algorithm for finding up to :math:`k` shortest paths without
loops from a source to a target coordinate. If :math:`k` is larger
than the number of possible paths from source to target, the size of
the returned path collection will be smaller than :math:`k`.

This implementation uses the A* algorithm with the Manhattan distance
function internally.

This function automatically detects whether the given layout
implements a clocking interface (see `clocked_layout`) and respects
the underlying information flow imposed by `layout`'s clocking scheme.
This algorithm does neither generate duplicate nor looping paths, even
in a cyclic clocking scheme. That is, along each path, each coordinate
can occur at maximum once.

If the given layout implements the obstruction interface (see
`obstruction_layout`), paths will not be routed via obstructed
coordinates or connections.

If the given layout is a gate-level layout and implements the
obstruction interface (see obstruction_layout), paths may contain wire
crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

In certain cases it might be desirable to enumerate regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:

```
{.cpp}
using clk_lyt = clocked_layout<cartesian_layout<>>;
using path = layout_coordinate_path<cartesian_layout<>>;
clk_lyt layout = ...;
auto k_paths = yen_k_shortest_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target}, k);
```

The algorithm was originally described in \"An algorithm for finding
shortest routes from all source nodes to a given destination in
general networks\" by Jin Y. Yen in Quarterly of Applied Mathematics,
1970.

Template parameter ``Path``:
    Type of the returned individual paths.

Template parameter ``Lyt``:
    Type of the layout to perform path finding on.

Parameter ``layout``:
    The layout in which the :math:`k` shortest paths are to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``k``:
    Maximum number of shortest paths to find.

Parameter ``params``:
    Parameters.

Returns:
    A collection of up to :math:`k` shortest loop-less paths in
    `layout` from `objective.source` to `objective.target`.)doc";

static const char *__doc_fiction_yen_k_shortest_paths_params = R"doc(Parameters for Yen's :math:`k`-shortest paths algorithm.)doc";

static const char *__doc_fiction_yen_k_shortest_paths_params_astar_params = R"doc(Parameters for the internal A* algorithm.)doc";

static const char *__doc_fmt_formatter = R"doc()doc";

static const char *__doc_fmt_formatter_2 = R"doc()doc";

static const char *__doc_fmt_formatter_format = R"doc()doc";

static const char *__doc_fmt_formatter_format_2 = R"doc()doc";

static const char *__doc_fmt_formatter_parse = R"doc()doc";

static const char *__doc_fmt_formatter_parse_2 = R"doc()doc";

static const char *__doc_fmt_unnamed_struct_at_home_runner_work_fiction_fiction_include_fiction_layouts_coordinates_hpp_1090_8 = R"doc()doc";

static const char *__doc_fmt_unnamed_struct_at_home_runner_work_fiction_fiction_include_fiction_layouts_coordinates_hpp_1106_8 = R"doc()doc";

static const char *__doc_fmt_unnamed_struct_at_home_runner_work_fiction_fiction_include_fiction_technology_cell_ports_hpp_291_8 = R"doc()doc";

static const char *__doc_mockturtle_detail_foreach_element_if_transform = R"doc()doc";

static const char *__doc_mockturtle_edge =
R"doc(Representation of an edge between two network nodes.

Template parameter ``Ntk``:
    `mockturtle` network type.)doc";

static const char *__doc_mockturtle_edge_operator_eq =
R"doc(Equality operator.

Parameter ``other``:
    Edge to compare to.

Returns:
    `true` iff both sources and targets match.)doc";

static const char *__doc_mockturtle_edge_operator_ne =
R"doc(Inequality operator.

Parameter ``other``:
    Edge to compare to.

Returns:
    `true` iff this edge is not equal to other.)doc";

static const char *__doc_mockturtle_edge_source = R"doc()doc";

static const char *__doc_mockturtle_edge_target = R"doc()doc";

static const char *__doc_std_hash = R"doc()doc";

static const char *__doc_std_hash_2 = R"doc()doc";

static const char *__doc_std_hash_3 = R"doc()doc";

static const char *__doc_std_hash_4 = R"doc()doc";

static const char *__doc_std_hash_5 = R"doc()doc";

static const char *__doc_std_hash_6 = R"doc()doc";

static const char *__doc_std_hash_operator_call = R"doc()doc";

static const char *__doc_std_hash_operator_call_2 = R"doc()doc";

static const char *__doc_std_hash_operator_call_3 = R"doc()doc";

static const char *__doc_std_hash_operator_call_4 = R"doc()doc";

static const char *__doc_std_hash_operator_call_5 = R"doc()doc";

static const char *__doc_std_hash_operator_call_6 = R"doc()doc";

static const char *__doc_std_iterator_traits = R"doc()doc";

static const char *__doc_std_tuple_size = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

