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

static const char *__doc_fiction_all_coordinates_in_spanned_area =
R"doc(Generates a vector of all coordinates within an area spanned by two
coordinates.

This function calculates and returns a vector of all coordinates that
span the area between the northwest (cell_nw) and southeast (cell_se)
cells, inclusive. The cells are generated in a top-down, left-to-right
fashion within the specified area.

Template parameter ``CoordinateType``:
    Coordinate Type.

Parameter ``cell_nw``:
    The northwest cell defining the starting point of the area.

Parameter ``cell_se``:
    The southeast cell defining the ending point of the area.

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

static const char *__doc_fiction_assess_physical_population_stability =
R"doc(This function assesses the population stability of each physically
valid charge distributions of a given SiDB layout. It determines the
minimum absolute electrostatic potential required to induce a charge
distribution transition. The function also identifies the SiDB for
which this is the case (critical SiDB) and the corresponding charge
state transition (i.e., the change from one charge state to another).

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout for which the population stability is assessed.

Parameter ``params``:
    Parameters used to assess the population stability.

Returns:
    A vector of population stability information for all physically
    valid charge distributions of the given SiDB layout.)doc";

static const char *__doc_fiction_assess_physical_population_stability_params =
R"doc(This struct stores the parameters required to assess the population
stability.)doc";

static const char *__doc_fiction_assess_physical_population_stability_params_precision_for_distance_corresponding_to_potential =
R"doc(The precision level for the conversion from the minimum potential
difference to the corresponding distance.)doc";

static const char *__doc_fiction_assess_physical_population_stability_params_simulation_parameters = R"doc(Parameters for the electrostatic potential.)doc";

static const char *__doc_fiction_bancs_clocking =
R"doc(Returns the BANCS clocking as defined in \"BANCS: Bidirectional
Alternating Nanomagnetic Clocking Scheme\" by Ruan Evangelista
Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI
2018.

Template parameter ``Lyt``:
    Clocked layout type.

Returns:
    BANCS clocking scheme.)doc";

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

Parameter ``params``:
    Parameters for the BDL pair detection.)doc";

static const char *__doc_fiction_bdl_input_iterator_current_input_index =
R"doc(The current input index. There are :math:`2^n` possible input states
for an :math:`n`-input BDL layout.)doc";

static const char *__doc_fiction_bdl_input_iterator_get_number_of_inputs =
R"doc(Returns the total number of input BDL pairs of the given SiDB gate
layout.

Returns:
    The number of input BDL pairs.)doc";

static const char *__doc_fiction_bdl_input_iterator_input_pairs = R"doc(The detected input BDL pairs.)doc";

static const char *__doc_fiction_bdl_input_iterator_layout = R"doc(The layout to iterate over.)doc";

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

static const char *__doc_fiction_bdl_pair_lower =
R"doc(The lower SiDB of the pair. Upper and lower are defined relative to
each other via the `operator<` overload.)doc";

static const char *__doc_fiction_bdl_pair_type =
R"doc(The type of the SiDBs in the pair. BDL SiDBs must be of the same type.
They can either be normal, input, or output SiDBs.)doc";

static const char *__doc_fiction_bdl_pair_upper =
R"doc(The upper SiDB of the pair. Upper and lower are defined relative to
each other via the `operator<` overload.)doc";

static const char *__doc_fiction_binomial_coefficient =
R"doc(Calculates the binomial coefficient :math:`\binom{n}{k}`.

Parameter ``n``:
    The total number of items.

Parameter ``k``:
    The number of items to choose from n.

Returns:
    The binomial coefficient :math:`\binom{n}{k}`.)doc";

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

static const char *__doc_fiction_bounding_box_2d_is_empty_coordinate = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_layout = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_max = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_min = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_update_bounding_box =
R"doc(The bounding box is not automatically updated when the layout changes.
This function recomputes the bounding box.)doc";

static const char *__doc_fiction_bounding_box_2d_x_size = R"doc()doc";

static const char *__doc_fiction_bounding_box_2d_y_size = R"doc()doc";

static const char *__doc_fiction_calculate_energy_and_state_type =
R"doc(This function takes in an SiDB energy distribution. For each charge
distribution, the state type is determined (i.e. erroneous,
transparent).

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

static const char *__doc_fiction_calculate_min_bbr_for_all_inputs =
R"doc(Calculates the minimum potential required to induce charge changes in
an SiDB layout for all input combinations.

Template parameter ``Lyt``:
    Type representing the SiDB cell-level layout.

Template parameter ``TT``:
    Type representing the truth table.

Parameter ``lyt``:
    The SiDB layout object.

Parameter ``spec``:
    Expected Boolean function of the layout, provided as a multi-
    output truth table.

Parameter ``params``:
    Parameters for assessing physical population stability.

Parameter ``charge_state_change``:
    Optional parameter indicating the direction of the considered
    charge state change (default is 1).

Returns:
    The minimum potential required for charge change across all input
    combinations.)doc";

static const char *__doc_fiction_calculate_min_bbr_for_all_inputs_params =
R"doc(This struct stores the parameters required to assess the population
stability of an SiDB gate.)doc";

static const char *__doc_fiction_calculate_min_bbr_for_all_inputs_params_assess_population_stability_params =
R"doc(Parameters for the `assessing physical population stability`
simulation)doc";

static const char *__doc_fiction_calculate_min_bbr_for_all_inputs_params_detect_pair_params = R"doc(Parameters to identify the output pairs used to read the output.)doc";

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

static const char *__doc_fiction_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_2 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_3 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_4 = R"doc()doc";

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
cartesian_layout, hexagonal_layout, or tile_based_layout. This type
extends the layout by providing a notion of FCN clocking. To this end,
it utilizes a clocking scheme that assigns each coordinate in the
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

static const char *__doc_fiction_convert_potential_to_distance =
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

static const char *__doc_fiction_convert_to_fiction_coordinates =
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

static const char *__doc_fiction_convert_to_siqad_coordinates =
R"doc(Converts the coordinates of a given cell-level layout (cds and defect
surface can be layered on top) to SiQAD coordinates. A new equivalent
layout based on SiQAD coordinates is returned.

Template parameter ``Lyt``:
    Cell-level layout type based on fiction coordinates, e.g.,
    `offset::ucoord_t` or `cube::coord_t`.

Parameter ``lyt``:
    The layout that is to be converted to a new layout based on SiQAD
    coordinates.

Returns:
    A new equivalent layout based on SiQAD coordinates.)doc";

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

static const char *__doc_fiction_count_gate_types_stats_num_inv = R"doc()doc";

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
    The type of the truth table specifying the gate behavior.

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
engine == APPROXIMATE).)doc";

static const char *__doc_fiction_critical_temperature_params_bdl_params = R"doc(Parameters for the BDL pair detection algorithms.)doc";

static const char *__doc_fiction_critical_temperature_params_confidence_level =
R"doc(Probability threshold for ground state population. The temperature at
which the simulation finds the ground state to be populated with a
probability of less than the given percentage, is determined to be the
critical temperature. For gate-based simulation, this is the
probability of erroneous calculations of the gate.)doc";

static const char *__doc_fiction_critical_temperature_params_engine = R"doc(Simulation mode to determine the *Critical Temperature*.)doc";

static const char *__doc_fiction_critical_temperature_params_iteration_steps =
R"doc(Number of iteration steps for the *QuickSim* algorithm (only
applicable if engine == APPROXIMATE).)doc";

static const char *__doc_fiction_critical_temperature_params_max_temperature =
R"doc(Maximum simulation temperature beyond which no simulation will be
conducted (~ 126 °C by default) (unit: K).)doc";

static const char *__doc_fiction_critical_temperature_params_simulation_engine =
R"doc(An enumeration of simulation modes (exact vs. approximate) to use for
the *Critical Temperature* Simulation.)doc";

static const char *__doc_fiction_critical_temperature_params_simulation_engine_APPROXIMATE =
R"doc(This simulation engine quickly calculates the *Critical Temperature*.
However, there may be deviations from the exact *Critical
Temperature*. This mode is recommended for larger layouts (> 40
SiDBs).)doc";

static const char *__doc_fiction_critical_temperature_params_simulation_engine_EXACT =
R"doc(This simulation engine computes *Critical Temperature* values with 100
% accuracy.)doc";

static const char *__doc_fiction_critical_temperature_params_simulation_parameters = R"doc(All parameters for physical SiDB simulations.)doc";

static const char *__doc_fiction_critical_temperature_stats = R"doc(This struct stores the result of the temperature simulation.)doc";

static const char *__doc_fiction_critical_temperature_stats_algorithm_name =
R"doc(Name of the algorithm used to compute the physically valid charge
distributions.)doc";

static const char *__doc_fiction_critical_temperature_stats_critical_temperature = R"doc(*Critical Temperature* of the given layout (unit: K).)doc";

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

static const char *__doc_fiction_defect_avoidance_distance =
R"doc(Calculates the defect avoidance distance of a given gate layout by a
given atomic defect. This means that a defect must be further away
than this distance for the SiDB gate to be operational. This function
requires both the defect operational domain and the layout as input.

Template parameter ``Lyt``:
    Type representing the SiDB cell-level layout.

Parameter ``lyt``:
    The cell-level layout for which the defect operational domain was
    computed.

Parameter ``defect_opdomain``:
    The defect operational domain associated with the layout.

Returns:
    The maximum minimum defect influence distance.)doc";

static const char *__doc_fiction_defect_avoidance_distance_result =
R"doc(Results of the defect avoidance distance calculation.

Template parameter ``CellType``:)doc";

static const char *__doc_fiction_defect_avoidance_distance_result_max_distance_postion_of_non_operational_defect =
R"doc(Maximum position at which the placement of a SiDB defect still causes
the gate to fail.)doc";

static const char *__doc_fiction_defect_avoidance_distance_result_max_min_distance =
R"doc(Minimum distance between a SiDB of the gate and the defect at the
maximum distance that causes the gate to fail.)doc";

static const char *__doc_fiction_defect_extent =
R"doc(Returns the extent of a defect as a pair of SiDB distances in
horizontal and vertical direction. If `defect` has the `NONE` defect
type, `{0, 0}` is returned.

Parameter ``defect``:
    Defect to evaluate.

Returns:
    Number of horizontal and vertical SiDBs that are affected by the
    given defect.)doc";

static const char *__doc_fiction_defect_influence_operational_domain =
R"doc(A defect operational domain defines the positions where a specific
atomic defect may exist along with corresponding information
indicating whether the SiDB layout remains logically operational.
Logically operational implies that the layout's ground state aligns
with a designated Boolean function at the layout's outputs for all
feasible input combinations. This implementation assumes the presence
of :math:` n ` BDL input wires and a single BDL output wire for a
given layout. Any algorithm for computing the defect influence
operational domain iterates through all :math:` 2^n ` input
combinations, evaluating the layout's output behavior based on the
specified Boolean function. The layout is classified as operational
for a particular defect position only if its output behavior is
correct across all input combinations.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_contour_tracing =
R"doc(Computes the defect influence operational domain of the given SiDB
cell-level layout. The defect influence operational domain is the set
of all defect positions for which the layout is logically operational.
Logical operation is defined as the layout implementing the given
truth table. The input BDL pairs of the layout are assumed to be in
the same order as the inputs of the truth table.

This algorithm uses contour tracing to identify operational defect
locations within the SiDB gate layout. It starts by searching for
defect locations on the left side of the bounding box, with an
additional distance of the SiDB gate where the SiDB gate remains
operational. The y-coordinate for these positions is chosen randomly.
The number of samples is determined by the `samples` parameter.

Then the algorithm moves each defect position to the right, searching
for the last operational defect position. This position is selected as
the starting point for the contour trace. The contour tracing process
checks whether the contour includes the SiDB layout. If it does not,
the next random sample point is is selected as the starting point and
the process is repeated.

@Note This algorithm is an approximation to determine the defect
influence operational domain. Therefore, it is recommended to analyze
the result afterwards to assess whether the contour reflects the
desired contour.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of samples to perform.

Parameter ``params``:
    Defect influence operational domain computation parameters.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The (partial) defect influence operational domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_grid_search =
R"doc(Computes the defect influence operational domain of the given SiDB
layout. The defect influence operational domain is the set of all
defect positions for which the layout is logically operational.
Logical operation is defined as the layout implementing the given
truth table.

This algorithm uses a grid search to determine the defect influence
operational domain. The grid search is performed by exhaustively
sweeping all possible atomic defect positions in x and y dimensions.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``step_size``:
    The parameter specifying the interval between consecutive defect
    positions to be evaluated.

Parameter ``params``:
    Defect influence operational domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The defect influence operational domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_operational_values = R"doc()doc";

static const char *__doc_fiction_defect_influence_operational_domain_params = R"doc(Parameters to determine the defect influence operational domain.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_params_defect_influence_params = R"doc(Parameters to simulate the influence of the atomic defect.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_params_operational_params = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_random_sampling =
R"doc(Computes the `defect influence operational domain` of the given SiDB
cell-level layout. The defect influence operational domain is the set
of all defect positions for which the layout is logically operational.
Logical operation is defined as the layout implementing the given
truth table. The input BDL pairs of the layout are assumed to be in
the same order as the inputs of the truth table.

This algorithm uses random sampling to find a part of the defect
influence operational domain that might not be complete. It performs a
total of `samples` uniformly-distributed random samples within the
specified area.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the defect influence operational domain for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``samples``:
    Number of random samples to perform.

Parameter ``params``:
    Defect influence operational domain computation parameters.

Parameter ``stats``:
    Statistics.

Returns:
    The (partial) defect influence operational domain of the layout.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats = R"doc(Statistics.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats_duration = R"doc(The total runtime of the operational domain computation.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats_num_evaluated_defect_positions = R"doc(Number of evaluated parameter combinations.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats_num_non_operational_defect_positions =
R"doc(Number of parameter combinations, for which the layout is non-
operational.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats_num_operational_defect_positions = R"doc(Number of parameter combinations, for which the layout is operational.)doc";

static const char *__doc_fiction_defect_influence_operational_domain_stats_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_dependent_cell_mode = R"doc(An enumeration of modes for the dependent cell.)doc";

static const char *__doc_fiction_dependent_cell_mode_FIXED =
R"doc(The charge state of the dependent cell is not changed based on the
local electrostatic potential at its position.)doc";

static const char *__doc_fiction_dependent_cell_mode_VARIABLE =
R"doc(The charge state of the dependent cell is changed based on the local
electrostatic potential at its position.)doc";

static const char *__doc_fiction_design_sidb_gates =
R"doc(The *SiDB Gate Designer* designs SiDB gate implementations based on a
specified Boolean function, a skeleton layout (can hold defects),
canvas size, and a predetermined number of canvas SiDBs. Two different
design modes are implemented: `exhaustive` and `random`.

The `exhaustive design` is composed of three steps: 1. In the initial
step, all possible distributions of `number_of_sidbs` SiDBs within a
given canvas are exhaustively determined. This ensures exhaustive
coverage of every potential arrangement of `number_of_sidbs` SiDBs
across the canvas. 2. The calculated SiDB distributions are then
incorporated into the skeleton, resulting in the generation of
distinct SiDB layouts. 3. The generated SiDB layouts then undergo an
extensive simulation process. All input combinations possible for the
given Boolean function are used to verify if the logic is fulfilled.

The `random design` is composed of four steps: 1. A specified number
of canvas SiDBs (`number_of_sidbs`) are randomly added to the skeleton
layout. 2. The operation status of the layout is simulated based on a
given Boolean function. 3. If the layout is `operational`, it is
returned as the result, and the process terminates successfully. 4. If
the layout is `non-operational`, the process is repeated from the
first step until an operational layout is found.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``skeleton``:
    The skeleton layout used as a starting point for gate design.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the *SiDB Gate Designer*.

Parameter ``design_mode``:
    The design mode to use.

Parameter ``stats``:
    The design statistics.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing =
R"doc(This function designs SiDB gates to minimize the cost function
:math:`\chi`, considering a layout skeleton, a set of truth tables,
and specified parameters for gate design and simulated annealing.
Currently, only the critical temperature and the operational domain
are incorporated into the cost function.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``skeleton``:
    The layout skeleton used as the basis for gate design.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    The parameters for gate design.

Parameter ``sa_params``:
    Parameters for simulated annealing.

Parameter ``stats``:
    Statistics for gate design.

Returns:
    A layout with SiDB gates designed to minimize the cost function.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params = R"doc(Parameters for Simulated Annealing-based gate design.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_ct_params = R"doc(Parameters for simulating the Critical Temperature.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_final_temperature = R"doc(Final temperature for Simulated Annealing.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_initial_temperature = R"doc(Initial temperature for Simulated Annealing.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_number_of_cycles = R"doc(Number of iterations per temperature in Simulated Annealing.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_op_params = R"doc(Parameters for simulating the Operational Domain.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_schedule = R"doc(Type of temperature schedule used in Simulated Annealing.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_weight_non_operational =
R"doc(Weight assigned to the non-operational behavior in the overall cost
function. This weight affects the cost function when the design is
non-operational.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_weight_operational_domain =
R"doc(Weight assigned to the operational domain in the overall cost
function. A negative value indicates that this weight is not used.)doc";

static const char *__doc_fiction_design_sidb_gates_metric_driven_simulated_annealing_params_weight_temperature =
R"doc(Weight assigned to the critical temperature in the overall cost
function. A negative value indicates that a high critical temperature
is preferred.)doc";

static const char *__doc_fiction_design_sidb_gates_mode = R"doc(Selector for the available design approaches.)doc";

static const char *__doc_fiction_design_sidb_gates_mode_EXHAUSTIVE = R"doc(All gate layouts are designed exhaustively.)doc";

static const char *__doc_fiction_design_sidb_gates_mode_RANDOM = R"doc(Gate layouts are designed randomly.)doc";

static const char *__doc_fiction_design_sidb_gates_params =
R"doc(This struct contains parameters and settings to design SiDB gates.

Template parameter ``CellType``:
    Cell type.)doc";

static const char *__doc_fiction_design_sidb_gates_params_canvas = R"doc(Canvas spanned by the northwest and southeast cell.)doc";

static const char *__doc_fiction_design_sidb_gates_params_maximal_random_solutions =
R"doc(The number of gate implementations found before the random design
process is stopped.)doc";

static const char *__doc_fiction_design_sidb_gates_params_number_of_sidbs = R"doc(Number of SiDBs placed in the canvas to create a working gate.)doc";

static const char *__doc_fiction_design_sidb_gates_params_sim_engine =
R"doc(The simulation engine to be used for the operational domain
computation.)doc";

static const char *__doc_fiction_design_sidb_gates_params_simulation_parameters = R"doc(All Parameters for physical SiDB simulations.)doc";

static const char *__doc_fiction_design_sidb_gates_stats = R"doc(Statistics for the design of SiDB gates.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_duration = R"doc(The total runtime of SiDB gate design process.)doc";

static const char *__doc_fiction_design_sidb_gates_stats_gate_cost = R"doc(The cost value of the final gate designed with Simulated Annealing.)doc";

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

static const char *__doc_fiction_detail_add_fanin_to_route =
R"doc(This helper function is used to add a fanin coordinate to the
appropriate route based on whether it belongs to the the route from
the first or second fanin to the gate.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``fanin``:
    The fanin coordinate to be added to the route.

Parameter ``is_first_fanin``:
    A boolean indicating whether this is part of the route from the
    first fanin to the gate.

Parameter ``ffd``:
    Reference to the fanin_fanout_data structure containing the
    routes.)doc";

static const char *__doc_fiction_detail_add_fanout_to_route =
R"doc(This helper function is used to add a fanout coordinate to the
appropriate route based on whether it belongs to the the route from
the gate to the first or second fanout.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``fanout``:
    The fanout coordinate to be added to the route.

Parameter ``is_first_fanout``:
    A boolean indicating whether it belongs to the route from the gate
    to the first fanout.

Parameter ``ffd``:
    Reference to the fanin_fanout_data structure containing the
    routes.)doc";

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

static const char *__doc_fiction_detail_apply_gate_library_impl_assign_gate = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_cell_lyt = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_gate_lyt = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl = R"doc()doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_assess_physical_population_stability_impl =
R"doc(Constructor for assess_physical_population_stability_impl.

Parameter ``lyt``:
    SiDB layout.

Parameter ``parameters``:
    The simulation parameters used for the assessment.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_collect_energy_and_charge_index =
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

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_energy_and_charge_index =
R"doc(This struct represents the electrostatic energy and charge index of a
charge distribution.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_energy_and_charge_index_charge_index = R"doc(Charge index of the charge distribution.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_energy_and_charge_index_energy = R"doc(Electrostatic energy of the charge distribution.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_handle_negative_charges =
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

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_handle_neutral_charges =
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

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_handle_positive_charges =
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

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_layout = R"doc(Layout to analyze.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_params = R"doc(Parameters required to assess the population stability.)doc";

static const char *__doc_fiction_detail_assess_physical_population_stability_impl_run =
R"doc(Runs a population stability assessment for a given SiDB layout using
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

static const char *__doc_fiction_detail_critical_temperature_impl_critical_temperature_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_temperature_impl_determine_critical_temperature =
R"doc(The *Critical Temperature* is determined.

Parameter ``energy_state_type``:
    All energies of all physically valid charge distributions with the
    corresponding state type (i.e. transparent, erroneous).)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_energy_between_ground_state_and_first_erroneous =
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

static const char *__doc_fiction_detail_critical_temperature_impl_gate_based_simulation =
R"doc(*Gate-based Critical Temperature* Simulation of a SiDB layout for a
given Boolean function.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_get_critical_temperature =
R"doc(Returns the critical temperature.

Returns:
    The critical temperature (unit: K).)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_layout = R"doc(SiDB cell-level layout.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_non_gate_based_simulation =
R"doc(*Gate-based Critical Temperature* Simulation of a SiDB layout for a
given Boolean function.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_params = R"doc(Parameters for the critical_temperature algorithm.)doc";

static const char *__doc_fiction_detail_critical_temperature_impl_physical_simulation_of_layout =
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

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_contour_tracing = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_current_defect_position = R"doc(The current defect position.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_defect_influence_operational_domain_impl = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_defect_operational_domain = R"doc(The operational domain of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_determine_nw_se_cells =
R"doc(This function determines the northwest and southeast cells based on
the gate layout and the additional scan area specified.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_find_last_operational_defect_position_moving_right =
R"doc(This function identifies the most recent operational defect position
while traversing from left to right towards the SiDB gate.

Parameter ``starting_defect_position``:
    The starting position of the defect, from which the traversal
    towards the right is conducted while maintaining gate operability.

Returns:
    The last operational defect position.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_find_operational_defect_position_at_left_side =
R"doc(This function aims to identify an operational defect position within
the layout. It does so by selecting a defect position with the
leftmost x-coordinate and a randomly selected y-coordinate limited the
layout's bounding box.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_grid_search = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_has_already_been_sampled =
R"doc(This function verifies whether the layout has already been analyzed
for the specified defect position `c`.

Parameter ``c``:
    Position of the defect.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_is_defect_position_operational =
R"doc(This function evaluates the operational status of the SiDB gate when a
defect is placed at position `c`.

Parameter ``c``:
    Position of the defect.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_layout = R"doc(The SiDB cell-level layout to investigate.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_log_stats =
R"doc(Helper function that writes the the statistics of the defect
operational domain computation to the statistics object.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_moore_neighborhood =
R"doc(Computes the Moore neighborhood of a given cell within the SiDB
layout. The Moore neighborhood consists of the eight cells surrounding
the central cell in horizontal, vertical, and diagonal directions.

Parameter ``c``:
    The cell for which the Moore neighborhood is computed.

Returns:
    A vector containing the cells in the Moore neighborhood that are
    empty. If a cell is outside the layout boundaries or occupied, it
    is not included in the result.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_num_evaluated_defect_positions = R"doc(Number of evaluated defect positions.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_nw_bb_layout = R"doc(The north-west bounding box of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_nw_cell = R"doc(North-west cell.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_params = R"doc(The parameters for the operational domain computation.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_previous_defect_position = R"doc(The previous defect position.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_random_sampling = R"doc()doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_se_bb_layout = R"doc(The south-east bounding box of the layout.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_se_cell = R"doc(South-east cell.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_stats = R"doc(The statistics of the operational domain computation.)doc";

static const char *__doc_fiction_detail_defect_influence_operational_domain_impl_truth_table = R"doc(The specification of the layout.)doc";

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

static const char *__doc_fiction_detail_design_sidb_gates_impl_all_sidbs_in_canvas = R"doc(All cells within the canvas.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_are_sidbs_too_close =
R"doc(Checks if any SiDBs within the specified cell indices are located too
closely together, with a distance of less than 0.5 nanometers.

This function iterates through the provided cell indices and compares
the distance between SiDBs. If it finds any pair of SiDBs within a
distance of 0.5 nanometers, it returns `true` to indicate that SiDBs
are too close; otherwise, it returns `false`.

Parameter ``cell_indices``:
    A vector of cell indices to check for SiDB proximity.

Returns:
    `true` if any SiDBs are too close; otherwise, `false`.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_canvas_sidbs = R"doc(All SiDBs within the canvas.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_canvas_sidbs_before_move = R"doc(Canvas SiDBs before move.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_design_sidb_gates_impl =
R"doc(This constructor initializes an instance of the *SiDB Gate Designer*
implementation with the provided skeleton layout and configuration
parameters.

Parameter ``skeleton``:
    The skeleton layout used as a basis for gate design.

Parameter ``tt``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``ps``:
    Parameters and settings for the gate designer.

Parameter ``st``:
    Statistics for the gate designer.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_determine_canvas_sidbs =
R"doc(This function iterates over each cell in the provided layout `lyt` and
checks if the cell corresponds to a canvas SiDB. Canvas SiDBs are
defined as SiDBs that are part of the canvas region. It populates a
vector with the canvas SiDBs found in the layout and returns it.

Parameter ``lyt``:
    The layout from which canvas SiDBs are to be determined.

Returns:
    A vector containing the canvas SiDBs found in the layout.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_generator = R"doc(A random-number generator.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_move_sidb =
R"doc(This function randomly selects a canvas cell from the layout `lyt` and
a canvas SiDB to replace it with. It then moves the selected canvas
SiDB to the randomly chosen canvas cell, updating the layout
accordingly. If the randomly chosen canvas cell is not empty, the
layout remains unchanged.

Parameter ``lyt``:
    The layout from which a canvas SiDB is to be moved.

Returns:
    The layout after the canvas SiDB has been moved, or the original
    layout if the randomly chosen canvas cell was not empty.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_params = R"doc(Parameters for the *SiDB Gate Designer*.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_random_canvas_cell_functor = R"doc()doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_random_canvas_sidb_functor = R"doc()doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_exhaustive_design =
R"doc(Design gates exhaustively and in parallel.

This function adds each cell combination to the given skeleton, and
determines whether the layout is operational based on the specified
parameters. The design process is parallelized to improve performance.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_metric_driven_design_process =
R"doc(Design gates with Simulated Annealing.

This function designs gates with Simulated Annealing. The cost
function involves the critical temperature and the operational domain.
The importance of the individual figures of merit can be adjusted by
the weights.

Parameter ``sa_params``:
    Simulated Annealing parameters.

Returns:
    Designed SiDB gate with minimal cost.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_run_random_design =
R"doc(Design gates randomly and in parallel.

This function adds cells randomly to the given skeleton, and
determines whether the layout is operational based on the specified
parameters. The design process is parallelized to improve performance.

Returns:
    A vector of designed SiDB gate layouts.)doc";

static const char *__doc_fiction_detail_design_sidb_gates_impl_sidb_moved_from_to =
R"doc(Canvas SiDB was moved from one cell (first cell) to another cell
(second cell).)doc";

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

static const char *__doc_fiction_detail_fix_wires =
R"doc(Utility function to move wires that cross over empty tiles down one
layer. This can happen if the wiring of a gate is deleted.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    Gate-level layout.

Parameter ``deleted_coords``:
    Tiles that got deleted.)doc";

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

static const char *__doc_fiction_detail_get_fanin_and_fanouts =
R"doc(Utility function to trace back fanins and fanouts of a gate. Based on
the gate to be moved, this function returns the location of the fanins
and fanouts, as well as the wiring in between them. Additionally, all
wire tiles between fanins and the gate, as well as between the gate
and fanouts are collected for deletion.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

Parameter ``lyt``:
    Cartesian Gate-level layout.

Parameter ``op``:
    coordinate of the gate to be moved.

Returns:
    fanin and fanout gates, wires to be deleted and old routing paths.)doc";

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

static const char *__doc_fiction_detail_get_path_and_obstruct =
R"doc(This helper function computes a path between two coordinates using the
A* algorithm. It then obstructs the tiles along the path in the given
layout.

Template parameter ``Lyt``:
    Cartesian gate-level layout type.

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

static const char *__doc_fiction_detail_improve_gate_location =
R"doc(Utility function that moves gates to new coordinates and checks if
routing is possible. This includes:

- removing the old wiring between fanins, the gate and fanouts -
updating the incoming signals - determining coordinates that would
improve the layout - testing all those coordinates by moving the gate
to each one and checking if a new wiring can be found - if a new
coordinate is found and wiring is possible, it is applied and incoming
signals are updated - if no better coordinate is found, the old wiring
is restored

Template parameter ``Lyt``:
    Cartesian obstruction gate-level layout type.

Parameter ``lyt``:
    2DDWave-clocked cartesian obstruction gate-level layout.

Parameter ``old_pos``:
    Old position of the gate to be moved.

Returns:
    `true` if the gate was moved successfully, `false` otherwise.)doc";

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
R"doc(Implementation of the `is_operational` algorithm for a given gate
layout.

This class provides an implementation of the `is_operational`
algorithm for a specified gate layout and parameters. It checks
whether the gate layout is operational by simulating its behavior for
different input combinations and comparing the results to expected
outputs from a truth table.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the gate behavior.)doc";

static const char *__doc_fiction_detail_is_operational_impl_bii = R"doc(Iterator that iterates over all possible input states.)doc";

static const char *__doc_fiction_detail_is_operational_impl_determine_operational_input_patterns =
R"doc(Determines the input combinations yielding the correct output.

Returns:
    All inputs (e.g. 2-input Boolean function: 00 ^= 0; 10 ^= 2) for
    which the correct output is computed.)doc";

static const char *__doc_fiction_detail_is_operational_impl_get_number_of_simulator_invocations =
R"doc(Returns the total number of simulator invocations.

Returns:
    The number of simulator invocations.)doc";

static const char *__doc_fiction_detail_is_operational_impl_is_operational_impl =
R"doc(Constructor to initialize the algorithm with a layout and parameters.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_detail_is_operational_impl_layout = R"doc(SiDB cell-level layout.)doc";

static const char *__doc_fiction_detail_is_operational_impl_output_bdl_pairs = R"doc(Output BDL pairs.)doc";

static const char *__doc_fiction_detail_is_operational_impl_parameters = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_detail_is_operational_impl_physical_simulation_of_layout =
R"doc(This function conducts physical simulation of the given layout (gate
layout with certain input combination). The simulation results are
stored in the `sim_result` variable.

Parameter ``bdl_iterator``:
    A reference to a BDL input iterator representing the gate layout
    at a given input combination. The simulation is performed based on
    the configuration represented by the iterator.

Returns:
    Simulation results.)doc";

static const char *__doc_fiction_detail_is_operational_impl_run =
R"doc(Run the `is_operational` algorithm.

This function executes the operational status checking algorithm for
the gate layout and parameters provided during initialization.

Returns:
    The operational status of the gate layout (either `OPERATIONAL` or
    `NON_OPERATIONAL`).)doc";

static const char *__doc_fiction_detail_is_operational_impl_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_is_operational_impl_truth_table = R"doc(The specification of the layout.)doc";

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

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl =
R"doc(A class for simulating the maximum influence distance of defects
within an SiDB layout.

This class is responsible for simulating the distance at which defects
placed within an SiDB layout still influence the ground state of the
layout. It conducts simulations at various defect positions,
identifying the position that maximally impacts the layout and
calculating the associated influence distance.

The class provides a `run` method to initiate the simulation and
compute the maximum influence distance and corresponding defect
position. It utilizes multithreading for efficient defect position
simulations.)doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_collect_all_defect_cells =
R"doc(Collects all possible defect cell positions within a given layout
while avoiding SiDB cells.

This function calculates a bounding box around the provided layout,
encompassing the area where defect cells can be placed. It then
iterates through this bounding box, scanning from top to bottom and
left to right, and identifies all valid positions for defect cells. A
defect cell can only be placed in locations where there are no SiDB
cells.)doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_defect_cells = R"doc(All allowed defect positions.)doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_layout = R"doc(SiDB cell-level layout to simulate.)doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_maximum_defect_influence_position_and_distance_impl = R"doc()doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_params = R"doc(Parameters used for the simulation.)doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_maximum_defect_influence_position_and_distance_impl_stats = R"doc(The statistics of the maximum defect influence position.)doc";

static const char *__doc_fiction_detail_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_depth = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_topo = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_operational_domain_impl = R"doc()doc";

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

Parameter ``initial_parameter``:
    Optional initial point in the parameter space for contour tracing.

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
step in x and y dimension of the first operational point found. If no
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

Parameter ``initial_parameter``:
    Optional initial point in the parameter space for flood fill.

Returns:
    The (partial) operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_generate_random_step_points =
R"doc(Generates (potentially repeating) random `step_points` in the stored
parameter range. The number of generated points is exactly equal to
`samples`.

Parameter ``samples``:
    Number of random `step_point`s to generate.

Returns:
    A set of random `step_point`s in the stored parameter range.)doc";

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

static const char *__doc_fiction_detail_operational_domain_impl_has_already_been_sampled =
R"doc(Determines whether the point at step position `(x, y)` has already
been sampled and returns the operational value at `(x, y)` if it
already exists. Here, `x` and `y` represent steps in the x and y
dimension, respectively, not the actual values of the parameters.

Parameter ``sp``:
    Step point to check.

Returns:
    The operational status of the point at step position `sp = (x, y)`
    or `std::nullopt` if `(x, y)` has not been sampled yet.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_is_step_point_operational =
R"doc(Logs and returns the operational status at the given point `sp = (x,
y)`. If the point has already been sampled, it returns the cached
value. Otherwise, a ground state simulation is performed for all input
combinations of the stored layout using the given simulation
parameters. It terminates as soon as a non-operational state is found.
In the worst case, the function performs :math:`2^n` simulations,
where :math:`n` is the number of inputs of the layout. This function
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

static const char *__doc_fiction_detail_operational_domain_impl_moore_neighborhood =
R"doc(Returns the Moore neighborhood of the step point at `sp = (x, y)`. The
Moore neighborhood is the set of all points that are adjacent to `(x,
y)` including the diagonals. Thereby, the Moore neighborhood contains
up to 8 points as points outside of the parameter range are not
gathered. The points are returned in clockwise order starting from the
right neighbor.

Parameter ``sp``:
    Step point to get the Moore neighborhood of.

Returns:
    The Moore neighborhood of the step point at `sp = (x, y)`.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_evaluated_parameter_combinations = R"doc(Number of evaluated parameter combinations.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_simulator_invocations = R"doc(Number of simulator invocations.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_x_steps =
R"doc(Calculates the number of steps in the x dimension based on the
provided parameters.

Returns:
    The number of steps in the x dimension.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_num_y_steps =
R"doc(Calculates the number of steps in the y dimension based on the
provided parameters.

Returns:
    The number of steps in the y dimension.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_op_domain = R"doc(The operational domain of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_operational_domain_impl =
R"doc(Standard constructor. Initializes the lyt, the truth table, the
parameters and the statistics. Also detects the output BDL pair, which
is necessary for the operational domain computation. The lyt must have
exactly one output BDL pair.

Parameter ``lyt``:
    SiDB cell-level lyt to be evaluated.

Parameter ``spec``:
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

static const char *__doc_fiction_detail_operational_domain_impl_output_bdl_pairs = R"doc(The output BDL pair of the layout.)doc";

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

static const char *__doc_fiction_detail_operational_domain_impl_set_x_dimension_value =
R"doc(Helper function that sets the value of the x dimension in the
simulation parameters.

Parameter ``sim_params``:
    Simulation parameter object to set the x dimension value of.

Parameter ``val``:
    Value to set the x dimension to.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_set_y_dimension_value =
R"doc(Helper function that sets the value of the y dimension in the
simulation parameters.

Parameter ``sim_params``:
    Simulation parameter object to set the y dimension value of.

Parameter ``val``:
    Value to set the y dimension to.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_stats = R"doc(The statistics of the operational domain computation.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point =
R"doc(A step point represents a point in the x and y dimension from 0 to the
maximum number of steps. A step point does not hold the actual
parameter values, but the step values in the x and y dimension,
respectively.

See `operational_domain::parameter_point` for a point that holds the
actual parameter values.)doc";

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

Parameter ``x_step``:
    X dimension step value.

Parameter ``y_step``:
    Y dimension step value.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_x = R"doc(X dimension step value.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_step_point_y = R"doc(Y dimension step value.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_sweep_dimension = R"doc(Potential sweep dimensions.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_sweep_dimension_X = R"doc(Sweep dimension X.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_sweep_dimension_Y = R"doc(Sweep dimension Y.)doc";

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

static const char *__doc_fiction_detail_operational_domain_impl_truth_table = R"doc(The specification of the layout.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_x_indices = R"doc(X dimension steps.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_x_values = R"doc(All x dimension values.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_y_indices = R"doc(Y dimension steps.)doc";

static const char *__doc_fiction_detail_operational_domain_impl_y_values = R"doc(All y dimension values.)doc";

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

static const char *__doc_fiction_detail_post_layout_optimization_impl = R"doc()doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_plyt = R"doc(2DDWave-clocked Cartesian gate-level layout to optimize.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_post_layout_optimization_impl = R"doc()doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_ps = R"doc(Post-layout optimization parameters.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_pst = R"doc(Statistics about the post-layout optimization process.)doc";

static const char *__doc_fiction_detail_post_layout_optimization_impl_run = R"doc()doc";

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
    The type of Charge Layout.

Template parameter ``ChargeLyt``:
    The type representing the charge layout to simulate.

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

static const char *__doc_fiction_detail_quickexact_impl_real_placed_defects = R"doc(Collection of defects that are placed in addition to the SiDBs.)doc";

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

static const char *__doc_fiction_detail_wire_east = R"doc()doc";

static const char *__doc_fiction_detail_wire_south = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_impl = R"doc()doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_plyt = R"doc(The 2DDWave-clocked layout whose wiring is to be reduced.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_pst = R"doc(Statistics about the wiring_reduction process.)doc";

static const char *__doc_fiction_detail_wiring_reduction_impl_run = R"doc()doc";

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
R"doc(Returns an SVG string representing the given cell-based clocked cell
layout

Parameter ``fcl``:
    The cell layout to generate an SVG representation for.

Parameter ``simple``:
    Flag to indicate that the SVG representation should be generated
    with less details. Recommended for large layouts.

Returns:
    The SVG string containing a visual representation of the given
    layout.)doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_generate_description_color = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_generate_tile_based_svg =
R"doc(Returns an SVG string representing the given tile-based clocked cell
layout.

Parameter ``fcl``:
    The cell layout to generate an SVG representation for.

Parameter ``simple``:
    Flag to indicate that the SVG representation should be generated
    with less details. Recommended for large layouts.

Returns:
    The SVG string containing a visual representation of the given
    layout.)doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_os = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_write_qca_layout_svg_impl_write_qca_layout_svg_impl = R"doc()doc";

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
    The type of the SiDBs to detect BDL pairs for, e.g., `INPUT`,
    `OUTPUT`, `NORMAL`.

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
R"doc(The minimum distance between two dots to be considered a BDL pair.
This is useful to prevent, e.g., SiDBs of atomic wires to be
considered BDL pairs. (unit: nm).)doc";

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
R"doc(Determines clock numbers for the given gate-level layout. This
algorithm parses the layout's gate and wire connections, disregarding
any existing clocking information, and constructs a SAT instance to
find a valid clock number assignment under which the information flow
is respected. It then assigns these clock numbers as an irregular
clock map to the given layout via the `assign_clock_number` function,
overriding any existing clocking scheme.

If no valid clock number assignment exists for `lyt`, this function
returns `false` and does not modify `lyt`.

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

static const char *__doc_fiction_determine_groundstate_from_simulation_results =
R"doc(This function calculates the ground state charge distributions from
the provided simulation results. The ground state charge distributions
are those with energy closest to the minimum energy found in the
simulation results.

@note When degenerate states exist, there are multiple ground states
with the same energy.

Template parameter ``Lyt``:
    The layout type used in the simulation results.

Parameter ``simulation_results``:
    The simulation results containing charge distributions.

Returns:
    A vector of charge distributions with the minimal energy.)doc";

static const char *__doc_fiction_determine_physically_valid_parameters =
R"doc(This function computes the parameters necessary for ensuring the
physical validity of a given charge distribution and determines the
corresponding excited state number. The ground state is denoted by
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
    number of the given cds for each parameter point.)doc";

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
R"doc(This function takes in a vector of `charge_distribution_surface`
objects and returns a map containing the system energy and the number
of occurrences of that energy in the input vector. To compare two
energy values for equality, the comparison uses a tolerance specified
by `physical_constants::POP_STABILITY_ERR`.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``input_vec``:
    A vector of `charge_distribution_surface` objects for which
    statistics are to be computed.

Returns:
    A map containing the system energy as the key and the number of
    occurrences of that energy in the input vector as the value.)doc";

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

static const char *__doc_fiction_exact_physical_design_stats_num_gates = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_num_wires = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_report = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_x_size = R"doc()doc";

static const char *__doc_fiction_exact_physical_design_stats_y_size = R"doc()doc";

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
replaced by *QuickExact* due to the much better runtimes and more
functionality.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``params``:
    Simulation parameters.

Parameter ``ps``:
    Simulation statistics.

Returns:
    sidb_simulation_result is returned with all results.)doc";

static const char *__doc_fiction_exhaustive_sidb_simulation_engine = R"doc(Selector exclusively for exhaustive SiDB simulation engines.)doc";

static const char *__doc_fiction_exhaustive_sidb_simulation_engine_EXGS =
R"doc(*Exhaustive Ground State Search* (EXGS) is an exact simulation engine
that always has exponential runtime.)doc";

static const char *__doc_fiction_exhaustive_sidb_simulation_engine_QUICKEXACT =
R"doc(*QuickExact* is also an exact simulation engine that requires
exponential runtime, but it scales a lot better than ExGS due to its
effective search-space pruning.)doc";

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
`fiction::physical_constants::POP_STABILITY_ERR`. Each key in the map
is compared to the specified key within this tolerance.

Template parameter ``MapType``:
    The type of the map containing parameter points as keys.

Parameter ``map``:
    The map containing parameter points as keys and associated values.

Parameter ``key``:
    The parameter point to search for in the map.

Returns:
    An iterator to the found parameter point in the map, or
    `map.cend()` if not found.)doc";

static const char *__doc_fiction_find_parameter_point_with_tolerance =
R"doc(This function searches for a parameter point, specified by the `key`,
in the provided map `map` with tolerance.

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
R"doc(Generates multiple unique random SiDB layouts by adding them to the
provided layout skeleton. The layout skeleton serves as the starting
layout to which SiDBs are added to create unique SiDB layouts.

Template parameter ``Lyt``:
    Cell-level SiDB layout type.

Parameter ``lyt_skeleton``:
    A layout to which random SiDBs are added to create unique layouts.

Parameter ``params``:
    The parameters for generating the random SiDB layouts.

Returns:
    A vector containing the unique randomly generated SiDB layouts.)doc";

static const char *__doc_fiction_generate_random_sidb_layout =
R"doc(Generates a random layout of SiDBs by adding them to the provided
layout skeleton. The layout skeleton serves as the starting layout to
which SiDBs are added to create the final layout.

Template parameter ``Lyt``:
    Cell-level SiDB layout type.

Parameter ``lyt_skeleton``:
    A layout to which random cells are added to create the final
    layout.

Parameter ``params``:
    The parameters for generating the random layout.

Returns:
    A randomly-generated layout of SiDBs.)doc";

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

static const char *__doc_fiction_generate_random_sidb_layout_params_minimal_spacing =
R"doc(If positively charged SiDBs should be prevented, SiDBs are not placed
closer than this value (Euclidean distance of two cells).)doc";

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

static const char *__doc_fiction_generate_random_sidb_layout_params_positive_sidbs =
R"doc(If positively charged SiDBs should be prevented, SiDBs are not placed
closer than the minimal_spacing.)doc";

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

static const char *__doc_fiction_get_name =
R"doc(Helper function to conveniently fetch the name from a layout or
network as they use different function names for the same purpose.

Template parameter ``NtkOrLyt``:
    Network or layout type.

Parameter ``ntk_or_lyt``:
    Network or layout object.

Returns:
    Name of given network or layout.)doc";

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

static const char *__doc_fiction_has_above = R"doc()doc";

static const char *__doc_fiction_has_assign_charge_state = R"doc()doc";

static const char *__doc_fiction_has_assign_sidb_defect = R"doc()doc";

static const char *__doc_fiction_has_below = R"doc()doc";

static const char *__doc_fiction_has_cardinal_operations = R"doc()doc";

static const char *__doc_fiction_has_create_dot = R"doc()doc";

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

static const char *__doc_fiction_has_foreach_sidb_defect = R"doc()doc";

static const char *__doc_fiction_has_foreach_tile = R"doc()doc";

static const char *__doc_fiction_has_get_charge_state = R"doc()doc";

static const char *__doc_fiction_has_get_functional_implementations = R"doc()doc";

static const char *__doc_fiction_has_get_gate_ports = R"doc()doc";

static const char *__doc_fiction_has_get_layout_name = R"doc()doc";

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

static const char *__doc_fiction_has_is_incoming_clocked = R"doc()doc";

static const char *__doc_fiction_has_is_inv = R"doc()doc";

static const char *__doc_fiction_has_is_mux = R"doc()doc";

static const char *__doc_fiction_has_is_nand = R"doc()doc";

static const char *__doc_fiction_has_is_nor = R"doc()doc";

static const char *__doc_fiction_has_is_obstructed_connection = R"doc()doc";

static const char *__doc_fiction_has_is_obstructed_coordinate = R"doc()doc";

static const char *__doc_fiction_has_is_onehot = R"doc()doc";

static const char *__doc_fiction_has_is_or_and = R"doc()doc";

static const char *__doc_fiction_has_is_outgoing_clocked = R"doc()doc";

static const char *__doc_fiction_has_is_po = R"doc()doc";

static const char *__doc_fiction_has_is_wire_tile = R"doc()doc";

static const char *__doc_fiction_has_is_xnor = R"doc()doc";

static const char *__doc_fiction_has_is_xor_and = R"doc()doc";

static const char *__doc_fiction_has_north = R"doc()doc";

static const char *__doc_fiction_has_north_east = R"doc()doc";

static const char *__doc_fiction_has_north_west = R"doc()doc";

static const char *__doc_fiction_has_ordinal_operations = R"doc()doc";

static const char *__doc_fiction_has_post_layout_optimization = R"doc()doc";

static const char *__doc_fiction_has_set_layout_name = R"doc()doc";

static const char *__doc_fiction_has_south = R"doc()doc";

static const char *__doc_fiction_has_south_east = R"doc()doc";

static const char *__doc_fiction_has_south_west = R"doc()doc";

static const char *__doc_fiction_has_synchronization_elements = R"doc()doc";

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
R"doc(This function checks if the ground state is found by the *QuickSim*
algorithm.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``heuristic_results``:
    All found physically valid charge distribution surfaces obtained
    by a heuristic algorithm.

Parameter ``exhaustive_results``:
    All valid charge distribution surfaces determined by ExGS.

Returns:
    Returns `true` if the relative difference between the lowest
    energies of the two sets is less than :math:`0.00001`, `false`
    otherwise.)doc";

static const char *__doc_fiction_is_hexagonal_layout = R"doc()doc";

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

This function checks the operational status of a given gate layout
using the `is_operational` algorithm. It determines whether the gate
layout is operational and returns the correct result for all
:math:`2^n` input combinations.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``TT``:
    The type of the truth table specifying the layout behavior.

Parameter ``lyt``:
    The SiDB cell-level layout to be checked.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the `is_operational` algorithm.

Returns:
    A pair containing the operational status of the gate layout
    (either `OPERATIONAL` or `NON_OPERATIONAL`) and the number of
    input combinations tested.)doc";

static const char *__doc_fiction_is_operational_params = R"doc(Parameters for the `is_operational` algorithm.)doc";

static const char *__doc_fiction_is_operational_params_bdl_params = R"doc(Parameters for the BDL pair detection algorithms.)doc";

static const char *__doc_fiction_is_operational_params_sim_engine =
R"doc(The simulation engine to be used for the operational domain
computation.)doc";

static const char *__doc_fiction_is_operational_params_simulation_parameters =
R"doc(The simulation parameters for the physical simulation of the ground
state.)doc";

static const char *__doc_fiction_is_positively_charged_defect =
R"doc(Checks whether the given defect has a positive charge value assigned
to it. This function is irrespective of the associated defect type.

Parameter ``defect``:
    Defect to check.

Returns:
    `true` iff `defect` has a positive charge value.)doc";

static const char *__doc_fiction_is_shifted_cartesian_layout = R"doc()doc";

static const char *__doc_fiction_is_sidb_defect_surface = R"doc()doc";

static const char *__doc_fiction_is_sidb_lattice = R"doc()doc";

static const char *__doc_fiction_is_sidb_lattice_100 = R"doc()doc";

static const char *__doc_fiction_is_sidb_lattice_111 = R"doc()doc";

static const char *__doc_fiction_is_tile_based_layout = R"doc()doc";

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

static const char *__doc_fiction_maximum_defect_influence_position_and_distance =
R"doc(Calculates the maximum distance at which a given defect can influence
the layout's ground state.

This function simulates the influence of defects on a SiDB cell-level
layout. It computes the maximum influence distance, defined as the
minimum distance between any SiDB cell and the given defect, at which
the defect can still affect the layout's ground state, potentially
altering its behavior, such as gate functionality.

Parameter ``lyt``:
    The SiDB cell-level layout for which the influence distance is
    being determined.

Parameter ``sim_params``:
    Parameters used to calculate the defect's maximum influence
    distance.

Parameter ``pst``:
    Statistics of the maximum defect influence distance.

Returns:
    Pair with the first element describing the position with maximum
    distance to the layout where a placed defect can still affect the
    ground state of the layout. The second entry describes the
    distance of the defect from the layout.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_of_sidb_gate =
R"doc(This function calculates the maximum influence position and distance
of a defect on the ground state of an SiDB gate layout. It iterates
over all input combinations and finds the defect position at maximum
position that affects the gate's ground state.

@Note The `defect influence distance` describes the distance at which
an defect influences the ground state. It does not check when the
successful operation starts to fail, since a change in the ground
state can still lead to an operational gate.

Template parameter ``Lyt``:
    Lyt SiDB cell-level layout type.

Template parameter ``TT``:
    Truth table type.

Parameter ``lyt``:
    Layout to compute the maximum defect influence position and
    distance for.

Parameter ``spec``:
    Expected Boolean function of the layout given as a multi-output
    truth table.

Parameter ``params``:
    Parameters for the defect influence simulation and BDL pair
    detection.

Returns:
    A pair containing the maximum influence defect position and its
    distance from the layout/gate.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_of_sidb_gate_params =
R"doc(Parameters for the
`maximum_defect_influence_position_and_distance_of_sidb_gate`
algorithm.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_of_sidb_gate_params_bdl_pairs_params = R"doc(Parameters for the detection of BDL pairs.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_of_sidb_gate_params_defect_influence_params = R"doc(Parameters for the defect influence simulation.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_params =
R"doc(This struct stores the parameters for the
maximum_defect_influence_position_and_distance algorithm.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_params_additional_scanning_area =
R"doc(The pair describes the width and height of the area around the gate,
which is also used to place defects.

@note If SiQAD coordinates are used, the second entry describes the
number of dimer rows.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_params_defect = R"doc(The defect to calculate the maximum defect influence distance for.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_params_simulation_parameters = R"doc(Physical simulation parameters.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_stats = R"doc(Statistics for the maximum defect influence simulation.)doc";

static const char *__doc_fiction_maximum_defect_influence_position_and_distance_stats_duration = R"doc(The total runtime of the maximum defect influence simulation.)doc";

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
    Cell-level layout type.

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

static const char *__doc_fiction_num_clks_FOUR = R"doc()doc";

static const char *__doc_fiction_num_clks_THREE = R"doc()doc";

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

static const char *__doc_fiction_number_of_operational_input_combinations =
R"doc(This function calculates the count of input combinations for which the
SiDB-based logic, represented by the provided layout (`lyt`) and truth
table specifications (`spec`), produces the correct output.

Template parameter ``Lyt``:
    Type of the cell-level layout.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters to simualte if a input combination is operational.

Returns:
    The count of operational input combinations.)doc";

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
implemented.

Template parameter ``Key``:
    The type representing the key. Defaults to `parameter_point`.

Template parameter ``Value``:
    The type representing the value. Defaults to `operational_status`.)doc";

static const char *__doc_fiction_operational_domain_contour_tracing =
R"doc(Computes the operational domain of the given SiDB cell-level layout.
The operational domain is the set of all parameter combinations for
which the layout is logically operational. Logical operation is
defined as the layout implementing the given truth table. The input
BDL pairs of the layout are assumed to be in the same order as the
inputs of the truth table.

This algorithm first uses random sampling to find a single operational
point within the parameter range. From there, it traverses outwards to
find the edge of the operational area and performs Moore neighborhood
contour tracing to explore the contour of the operational domain. If
the operational domain is connected, the algorithm is guaranteed to
find the contours of the entire operational domain within the
parameter range if the initial random sampling found an operational
point.

It performs up to `samples` uniformly-distributed random samples
within the parameter range until an operational point is found. From
there, it performs another number of samples equal to the distance to
an edge of the operational area. Finally, it performs up to 8 samples
for each contour point (however, the actual number is usually much
lower). For each sample, the algorithm performs one operational check
on the layout, where each operational check consists of up to
:math:`2^n` exact ground state simulations, where :math:`n` is the
number of inputs of the layout. Each exact ground state simulation has
exponential complexity in of itself. Therefore, the algorithm is only
feasible for small layouts with few inputs.

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

Parameter ``initial_parameter_point``:
    Optional initial point in the parameter space for contour tracing.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The (partial) operational domain of the layout.)doc";

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
guaranteed to find all operational areas in their entirety if the
initial random sampling found at least one operational point within
them. Thereby, this algorithm works for disconnected operational
domains.

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

Parameter ``initial_parameter_point``:
    Optional initial point in the parameter space for flood fill.

Parameter ``stats``:
    Operational domain computation statistics.

Returns:
    The (partial) operational domain of the layout.)doc";

static const char *__doc_fiction_operational_domain_get_value =
R"doc(This function retrieves the value associated with the provided
parameter point from the operational domain. If the parameter point is
found in the domain, its corresponding value is returned. Otherwise, a
runtime error is thrown.

Parameter ``pp``:
    The parameter point to look up.

Returns:
    The value associated with the parameter point.)doc";

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
    The operational domain of the layout.)doc";

static const char *__doc_fiction_operational_domain_operational_values =
R"doc(This can store different information depending on the use case. If the
operational domain is simulated for different physical parameters, the
parameters are stored with the corresponding operating status.)doc";

static const char *__doc_fiction_operational_domain_params =
R"doc(Parameters for the operational domain computation. The parameters are
used across the different operational domain computation algorithms.)doc";

static const char *__doc_fiction_operational_domain_params_bdl_params =
R"doc(The parameters for the BDL pair detection, which is necessary during
the operational domain computation to detect input and output BDL
pairs.)doc";

static const char *__doc_fiction_operational_domain_params_sim_engine =
R"doc(The simulation engine to be used for the operational domain
computation.)doc";

static const char *__doc_fiction_operational_domain_params_simulation_parameters =
R"doc(The simulation parameters for the operational domain computation. Most
parameters will be kept constant across sweeps, but the sweep
parameters are adjusted in each simulation step and thus overwritten
in this object.)doc";

static const char *__doc_fiction_operational_domain_params_x_dimension = R"doc(The sweep parameter for the x dimension.)doc";

static const char *__doc_fiction_operational_domain_params_x_max = R"doc(The maximum value of the x dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_params_x_min = R"doc(The minimum value of the x dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_params_x_step = R"doc(The step size of the x dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_params_y_dimension = R"doc(The sweep parameter for the y dimension.)doc";

static const char *__doc_fiction_operational_domain_params_y_max = R"doc(The maximum value of the y dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_params_y_min = R"doc(The minimum value of the y dimension sweep.)doc";

static const char *__doc_fiction_operational_domain_params_y_step = R"doc(The step size of the y dimension sweep.)doc";

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
    The (partial) operational domain of the layout.)doc";

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

static const char *__doc_fiction_operational_domain_stats_percentual_operational_area =
R"doc(The ratio of operational parameter pairs to all possible parameter
pairs. Value is between 0 and 1.)doc";

static const char *__doc_fiction_operational_domain_x_dimension = R"doc(X dimension sweep parameter.)doc";

static const char *__doc_fiction_operational_domain_y_dimension = R"doc(Y dimension sweep parameter.)doc";

static const char *__doc_fiction_operational_input_patterns =
R"doc(This function determines the input combinations for which the SiDB-
based logic, represented by the provided layout (`lyt`) and truth
table specifications (`spec`), produces the correct output.

Template parameter ``Lyt``:
    Type of the cell-level layout.

Template parameter ``TT``:
    Type of the truth table.

Parameter ``lyt``:
    The SiDB layout.

Parameter ``spec``:
    Vector of truth table specifications.

Parameter ``params``:
    Parameters to simualte if a input combination is operational.

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
    The parameter value at the specified index.)doc";

static const char *__doc_fiction_parameter_point_operator_eq =
R"doc(Equality operator. Checks if this parameter point is equal to another
point within a specified tolerance. The tolerance is defined by
`physical_constants::POP_STABILITY_ERR`.

Parameter ``other``:
    Other parameter point to compare with.

Returns:
    `true` iff the parameter points are equal.)doc";

static const char *__doc_fiction_parameter_point_operator_ne =
R"doc(Inequality operator. Checks if this parameter point is unequal to
another point within a specified tolerance. The tolerance is defined
by `physical_constants::POP_STABILITY_ERR`.

Parameter ``other``:
    Other parameter point to compare with.

Returns:
    `true` iff the parameter points are not equal.)doc";

static const char *__doc_fiction_parameter_point_parameter_point = R"doc(Standard default constructor.)doc";

static const char *__doc_fiction_parameter_point_parameter_point_2 =
R"doc(Standard constructor.

Parameter ``x_val``:
    X dimension parameter value.

Parameter ``y_val``:
    Y dimension parameter value.)doc";

static const char *__doc_fiction_parameter_point_x = R"doc(X dimension parameter value.)doc";

static const char *__doc_fiction_parameter_point_y = R"doc(Y dimension parameter value.)doc";

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

static const char *__doc_fiction_place =
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

static const char *__doc_fiction_population_stability_information_transition_from_to_with_cell_and_required_pot =
R"doc(This map collects all charge transition types, the corresponding
critical cells and the required electrostatic potential required to
conduct the transition.)doc";

static const char *__doc_fiction_port_direction =
R"doc(A port direction is a relative (cardinal) direction of a port within a
tile. Useful, when no exact port locations within a tile are needed.)doc";

static const char *__doc_fiction_port_direction_cardinal = R"doc(Cardinal direction.)doc";

static const char *__doc_fiction_port_direction_cardinal_EAST = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH_EAST = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_NORTH_WEST = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH_EAST = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_SOUTH_WEST = R"doc()doc";

static const char *__doc_fiction_port_direction_cardinal_WEST = R"doc()doc";

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
R"doc(Comparator for unordered_set/map.

Parameter ``p``:
    Ports to compare to.

Returns:
    `true` iff these ports are equal to `p`.)doc";

static const char *__doc_fiction_port_list_operator_iadd =
R"doc(Merges two port_list objects together. The given port_list might be
altered.

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
Hofmann, M. Walter, and R. Wille in NANOARCH 2023. It can be used to
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

static const char *__doc_fiction_post_layout_optimization_stats =
R"doc(This struct stores statistics about the post-layout optimization
process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_area_improvement = R"doc(Area reduction (in %) after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_duration = R"doc(Runtime of the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_report =
R"doc(Reports the statistics to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_x_size_after = R"doc(Layout width after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_x_size_before = R"doc(Layout width before the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_y_size_after = R"doc(Layout height after the post-layout optimization process.)doc";

static const char *__doc_fiction_post_layout_optimization_stats_y_size_before = R"doc(Layout height before the post-layout optimization process.)doc";

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

static const char *__doc_fiction_quickexact_params = R"doc(This struct stores the parameters for the *QuickExact* algorithm.)doc";

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

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``ps``:
    Physical parameters. They are material-specific and may vary from
    experiment to experiment.

Returns:
    sidb_simulation_result is returned with all results.)doc";

static const char *__doc_fiction_quicksim_params = R"doc(This struct stores the parameters for the *QuickSim* algorithm.)doc";

static const char *__doc_fiction_quicksim_params_alpha =
R"doc(`alpha` parameter for the *QuickSim* algorithm (should be reduced if
no result is found).)doc";

static const char *__doc_fiction_quicksim_params_iteration_steps = R"doc(Number of iterations to run the simulation for.)doc";

static const char *__doc_fiction_quicksim_params_number_threads =
R"doc(Number of threads to spawn. By default the number of threads is set to
the number of available hardware threads.)doc";

static const char *__doc_fiction_quicksim_params_simulation_parameters = R"doc(Simulation parameters for the simulation of the physical SiDB system.)doc";

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

If The provided cell-level layout type can represent SiDB defects,
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

static const char *__doc_fiction_sidb_bestagon_library_determine_port_routing = R"doc()doc";

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

static const char *__doc_fiction_sidb_charge_state = R"doc(Charge states of SiDBs.)doc";

static const char *__doc_fiction_sidb_charge_state_NEGATIVE = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_NEUTRAL = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_NONE = R"doc()doc";

static const char *__doc_fiction_sidb_charge_state_POSITIVE = R"doc()doc";

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

static const char *__doc_fiction_sidb_simulation_engine = R"doc(Selector for the available SiDB simulation engines.)doc";

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
    Cell-level layout type.)doc";

static const char *__doc_fiction_sidb_simulation_result_additional_simulation_parameters =
R"doc(Additional named simulation parameters. This is used to store
algorithm-dependent parameters that are not part of the
`sidb_simulation_parameters` struct.

The key of the map is the name of the parameter, the element is the
value of the parameter.)doc";

static const char *__doc_fiction_sidb_simulation_result_algorithm_name = R"doc(Name of the algorithm used to determine the charge distributions.)doc";

static const char *__doc_fiction_sidb_simulation_result_charge_distributions = R"doc(Charge distributions determined by the algorithm.)doc";

static const char *__doc_fiction_sidb_simulation_result_sidb_simulation_result =
R"doc(Default constructor. It only exists to allow for the use of
`static_assert` statements that restrict the type of `Lyt`.)doc";

static const char *__doc_fiction_sidb_simulation_result_simulation_parameters = R"doc(Physical parameters used in the simulation.)doc";

static const char *__doc_fiction_sidb_simulation_result_simulation_runtime = R"doc(Total simulation runtime.)doc";

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
    Cell-level layout type that is underlying to the SiDB surface.

Parameter ``gate_lyt``:
    Gate-level layout instance that specifies the aspect ratio.

Parameter ``surface``:
    SiDB surface that instantiates the defects.

Returns:
    A black list of gate functions associated with tiles.)doc";

static const char *__doc_fiction_sidb_technology =
R"doc(Silicon Dangling Bond (SiDB) technology implementation of the FCN
concept.)doc";

static const char *__doc_fiction_sidb_technology_cell_mark = R"doc(Possible marks to be applied to a cell to change its type.)doc";

static const char *__doc_fiction_sidb_technology_cell_mark_EMPTY = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mark_INPUT = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mark_OUTPUT = R"doc()doc";

static const char *__doc_fiction_sidb_technology_cell_mode = R"doc(SiDB cells do not have modes.)doc";

static const char *__doc_fiction_sidb_technology_cell_type = R"doc(Possible types of SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_EMPTY = R"doc(Symbol used for empty SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_INPUT = R"doc(Symbol used for input SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_NORMAL = R"doc(Symbol used for normal SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_cell_type_OUTPUT = R"doc(Symbol used for output SiDB cells.)doc";

static const char *__doc_fiction_sidb_technology_is_empty_cell = R"doc()doc";

static const char *__doc_fiction_sidb_technology_is_input_cell = R"doc()doc";

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

static const char *__doc_fiction_technology_mapping_params_inv = R"doc()doc";

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

static const char *__doc_fiction_technology_network_is_inv =
R"doc(Returns whether the given node `n` is an inverter node.

Parameter ``n``:
    Node to consider.

Returns:
    `true` iff `n` is an inverter node.)doc";

static const char *__doc_fiction_technology_network_is_ite = R"doc()doc";

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

static const char *__doc_fiction_temperature_schedule = R"doc(Available temperature schedule types.)doc";

static const char *__doc_fiction_temperature_schedule_GEOMETRIC = R"doc(Logarithmically decreasing temperature schedule.)doc";

static const char *__doc_fiction_temperature_schedule_LINEAR = R"doc(Linearly decreasing temperature schedule.)doc";

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
    Cell-level layout type.

Parameter ``lyt``:
    Layout that is used for the simulation.

Parameter ``quicksim_params``:
    Parameters required for the *QuickSim* algorithm.

Parameter ``tts_params``:
    Parameters used for the time-to-solution calculation.

Parameter ``ps``:
    Pointer to a struct where the results (time_to_solution, acc,
    single runtime) are stored.)doc";

static const char *__doc_fiction_time_to_solution_params = R"doc()doc";

static const char *__doc_fiction_time_to_solution_params_confidence_level = R"doc(Confidence level.)doc";

static const char *__doc_fiction_time_to_solution_params_engine =
R"doc(Exhaustive simulation algorithm used to simulate the ground state as
reference.)doc";

static const char *__doc_fiction_time_to_solution_params_repetitions =
R"doc(Number of iterations of the heuristic algorithm used to determine the
simulation accuracy (`repetitions = 100` means that accuracy is
precise to 1%).)doc";

static const char *__doc_fiction_time_to_solution_stats =
R"doc(This struct stores the time-to-solution, the simulation accuracy and
the average single simulation runtime of *QuickSim*, the single
runtime of the exact simulator used, and the number of valid charge
configurations found by the exact algorithm.)doc";

static const char *__doc_fiction_time_to_solution_stats_acc = R"doc(Accuracy of the simulation.)doc";

static const char *__doc_fiction_time_to_solution_stats_algorithm =
R"doc(Exhaustive simulation algorithm used to simulate the ground state as
reference.)doc";

static const char *__doc_fiction_time_to_solution_stats_mean_single_runtime = R"doc(Average single simulation runtime in seconds.)doc";

static const char *__doc_fiction_time_to_solution_stats_report =
R"doc(Print the results to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_time_to_solution_stats_single_runtime_exhaustive =
R"doc(Single simulation runtime of the exhaustive ground state searcher in
seconds.)doc";

static const char *__doc_fiction_time_to_solution_stats_time_to_solution = R"doc(Time-to-solution in seconds.)doc";

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

static const char *__doc_fiction_unsupported_gate_orientation_exception_coord = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_ports = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_unsupported_gate_orientation_exception = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_where = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_orientation_exception_which_ports = R"doc()doc";

static const char *__doc_fiction_unsupported_gate_type_exception =
R"doc(Exception to be thrown when a layout hosts a gate type that is not
implemented in the applied gate library.

Template parameter ``CoordinateType``:
    Type of the layout coordinates.)doc";

static const char *__doc_fiction_unsupported_gate_type_exception_coord = R"doc()doc";

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

Parameter ``pst``:
    Pointer to a `wiring_reduction_stats` object to record runtime
    statistics.)doc";

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

static const char *__doc_fiction_write_defect_influence_operational_domain =
R"doc(Writes a CSV representation of an operational domain to the specified
output stream. The data are written as rows, each corresponding to one
set of simulation parameters and their corresponding operational
status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
OPERATIONAL STATUS ... subsequent rows for each set of simulation
parameters.

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Parameter ``opdom``:
    The operational domain to be written. It contains a mapping from
    sets of simulation parameters (represented as a pair of sweep
    parameters for the X and Y dimensions) to their operational
    status.

Parameter ``os``:
    The output stream where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_defect_operational_domain_params` object, which provides
    standard tags.)doc";

static const char *__doc_fiction_write_defect_influence_operational_domain_2 =
R"doc(Writes a CSV representation of an operational domain to the specified
file. The data are written as rows, each corresponding to one set of
simulation parameters and their corresponding operational status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
OPERATIONAL STATUS ... subsequent rows for each set of simulation
parameters.

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Parameter ``opdom``:
    The operational domain to be written. It contains a mapping from
    sets of simulation parameters (represented as a pair of sweep
    parameters for the X and Y dimensions) to their operational
    status.

Parameter ``filename``:
    The filename where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_defect_operational_domain_params` object, which provides
    standard tags.)doc";

static const char *__doc_fiction_write_defect_operational_domain_params = R"doc(Parameters for writing an operational domain to a CSV file.)doc";

static const char *__doc_fiction_write_defect_operational_domain_params_non_operational_tag =
R"doc(The tag used to represent the non-operational value of a parameter
set.)doc";

static const char *__doc_fiction_write_defect_operational_domain_params_operational_tag = R"doc(The tag used to represent the operational value of a parameter set.)doc";

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
    Cell-level SiDB layout type.

Parameter ``sim_result``:
    The simulation result to write.

Parameter ``os``:
    The output stream to write into.)doc";

static const char *__doc_fiction_write_location_and_ground_state_2 =
R"doc(Writes the coordinates of all SiDBs of a layout together with the
charge distribution of the ground state(s) to a file.

This overload uses a file name to create and write into.

Template parameter ``Lyt``:
    Cell-level SiDB layout type.

Template parameter ``sim_result``:
    The simulation result to write.

Parameter ``filename``:
    The file name to create and write into.)doc";

static const char *__doc_fiction_write_operational_domain =
R"doc(Writes a CSV representation of an operational domain to the specified
output stream. The data are written as rows, each corresponding to one
set of simulation parameters and their corresponding operational
status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
OPERATIONAL STATUS ... subsequent rows for each set of simulation
parameters.

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Parameter ``opdom``:
    The operational domain to be written. It contains a mapping from
    sets of simulation parameters (represented as a pair of sweep
    parameters for the X and Y dimensions) to their operational
    status.

Parameter ``os``:
    The output stream where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_operational_domain_params` object, which provides standard
    tags.)doc";

static const char *__doc_fiction_write_operational_domain_2 =
R"doc(Writes a CSV representation of an operational domain to the specified
file. The data are written as rows, each corresponding to one set of
simulation parameters and their corresponding operational status.

The output CSV format is as follows: X_DIMENSION, Y_DIMENSION,
OPERATIONAL STATUS ... subsequent rows for each set of simulation
parameters.

The operational status is a binary value represented by specified tags
in `params` indicating whether the simulation parameters are within
the operational domain or not.

Parameter ``opdom``:
    The operational domain to be written. It contains a mapping from
    sets of simulation parameters (represented as a pair of sweep
    parameters for the X and Y dimensions) to their operational
    status.

Parameter ``filename``:
    The filename where the CSV representation of the operational
    domain is written to.

Parameter ``params``:
    The parameters used for writing, including the operational and
    non-operational tags. Defaults to an empty
    `write_operational_domain_params` object, which provides standard
    tags.)doc";

static const char *__doc_fiction_write_operational_domain_params = R"doc(Parameters for writing an operational domain to a CSV file.)doc";

static const char *__doc_fiction_write_operational_domain_params_non_operational_tag =
R"doc(The tag used to represent the non-operational value of a parameter
set.)doc";

static const char *__doc_fiction_write_operational_domain_params_operational_tag = R"doc(The tag used to represent the operational value of a parameter set.)doc";

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

May throw an unsupported_cell_type_exception.

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

May throw an unsupported_cell_type_exception.

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
    Cell-level SiDB layout type.

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
    Cell-level SiDB layout type.

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

