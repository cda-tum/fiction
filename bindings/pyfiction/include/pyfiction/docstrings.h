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
R"doc(The A* path finding algorithm for shortest loopless paths between a
given source and target coordinate in a clocked layout. A* is an
extension of Dijkstra's algorithm for shortest paths but offers better
average complexity. It uses a heuristic distance function that
estimates the remaining costs towards the target in every step. Thus,
this heuristic function should neither be complex to calculate nor
overestimating the remaining costs. Common heuristics to be used are
the Manhattan and the Euclidean distance functions. See
distance_functor for implementations.

If the given layout implements the obstruction interface (see
obstruction_layout), paths will not be routed via obstructed
coordinates and connections.

If the given layout is a gate-level layout and implements the
obstruction interface (see obstruction_layout), paths may contain wire
crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

A* was introduced in \"A Formal Basis for the Heuristic Determination
of Minimum Cost Paths\" by Peter E. Hart, Nils J. Nilsson, and Bertram
Raphael in IEEE Transactions on Systems Science and Cybernetics 1968,
Volume 4, Issue 2.

This implementation is based on the pseudocode from
https://en.wikipedia.org/wiki/A_star_search_algorithm.

Template parameter ``Path``:
    Path type to create.

Template parameter ``Lyt``:
    Clocked layout type.

Template parameter ``Dist``:
    Distance value type to be used in the heuristic estimation
    function.

Template parameter ``Cost``:
    Cost value type to be used when determining moving cost between
    coordinates.

Parameter ``layout``:
    The clocked layout in which the shortest path between `source` and
    `target` is to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``dist_fn``:
    A distance functor that implements the desired heuristic
    estimation function.

Parameter ``cost_fn``:
    A cost functor that implements the desired cost function.

Parameter ``ps``:
    Parameters.

Returns:
    The shortest loopless path in `layout` from `source` to `target`.)doc";

static const char *__doc_fiction_a_star_distance =
R"doc(A distance function that does not approximate but compute the actual
minimum path length on the given layout via A* traversal. Naturally,
this function cannot be evaluated in :math:` O(1) `, but has the
polynomial complexity of A*.

If no path between source and target exists in `lyt`, the returned
distance is `std::numeric_limits<Dist>::infinity()` if that value is
supported by `Dist`, or `std::numeric_limits<Dist>::max()`, otherwise.

Template parameter ``Lyt``:
    Clocked layout type.

Template parameter ``Dist``:
    Distance type.

Parameter ``layout``:
    The clocked layout in which the distance between `source` and
    `target` is to be determined.

Parameter ``source``:
    Source coordinate.

Parameter ``target``:
    Target coordinate.

Returns:
    Minimum path length between `source` and `target`.)doc";

static const char *__doc_fiction_a_star_distance_functor =
R"doc(A pre-defined distance functor that uses the A* distance.

Template parameter ``Lyt``:
    Clocked layout type.

Template parameter ``Dist``:
    Distance type.)doc";

static const char *__doc_fiction_a_star_distance_functor_a_star_distance_functor = R"doc()doc";

static const char *__doc_fiction_a_star_params = R"doc(Parameters for the A* algorithm.)doc";

static const char *__doc_fiction_a_star_params_crossings =
R"doc(Allow paths to cross over obstructed tiles if they are occupied by
wire segments.)doc";

static const char *__doc_fiction_all_incoming_edge_paths =
R"doc(Returns a vector of all possible paths to reach the given node from
the primary inputs within the given network.

Each node without predecessors is considered a terminal and a path is
defined as a sequence of edges.

Parameter ``n``:
    Node whose paths are desired.

Returns:
    A vector of all possible edge paths leading from terminals to `v`.)doc";

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
0). Calculates :math:` (x + 1) \cdot (y + 1) `.

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
    The cell-level layout whose area is desired.)doc";

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
ratios via factorization, starting from a number of faces :math:` n `.
After iterating over all possible factorizations of n, the next step
increases :math:` n ` and continues with the factorization. Thereby, a
sequence of aspect ratios starting from :math:` n = 4 ` faces looks
like this: :math:` 1 \times 4, 4 \times 1, 2 \times 2, 1 \times 5, 5
\times 1, 1 \times 6, 6 \times 1, 2 \times 3, 3 \times 2, \dots `

Template parameter ``AspectRatio``:
    Aspect ratio type.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_aspect_ratio_iterator =
R"doc(Standard constructor. Takes a starting value and computes an initial
factorization. The value `n` represents the amount of faces in the
desired aspect ratios. For example, :math:` n = 1 ` will yield aspect
ratios with exactly :math:` 1 ` face, i.e. :math:` 1 \times 1 ` which
is equal to `ucoord_t{0, 0}`. If :math:` n = 2 `, the aspect ratios
:math:` 1 \times 2 ` and :math:` 2 \times 1 ` will result, which are
equal to `ucoord_t{0, 1}` and `ucoord_t{1, 0}`. Both examples with
`AspectRatio == offset::ucoord_t`.

Parameter ``n``:
    Starting value of the aspect ratio iteration.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_factorize =
R"doc(Factorizes the current `num` into all possible factors :math:` (x, y)
` with :math:` x \cdot y = num `. The result is stored as a vector of
`AspectRatio` objects in the attribute factors.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_factors = R"doc(Factors of num.)doc";

static const char *__doc_fiction_aspect_ratio_iterator_next =
R"doc(Computes the next possible `num` where a factorization :math:` (x, y)
` with :math:` x \cdot y = num ` exists.)doc";

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

static const char *__doc_fiction_cartesian_layout =
R"doc(A layout type that utilizes offset coordinates to represent a
Cartesian grid. Its faces are organized in the following way:

\verbatim +-------+-------+-------+-------+ | | | | | | (0,0) | (1,0)
| (2,0) | (3,0) | | | | | | +-------+-------+-------+-------+ | | | |
| | (0,1) | (1,1) | (2,1) | (3,1) | | | | | |
+-------+-------+-------+-------+ | | | | | | (0,2) | (1,2) | (2,2) |
(3,2) | | | | | | +-------+-------+-------+-------+ \endverbatim

Template parameter ``OffsetCoordinateType``:
    The coordinate implementation to be used. Offset coordinates are
    required.)doc";

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
R"doc(Returns the layout's number of faces which are equal to :math:` (x +
1) \cdot (y + 1) `.

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
    Last coordinate to include in the range of all coordinates.

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
    Last coordinate to include in the range of all coordinates.)doc";

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
    Last coordinate to include in the range of all ground coordinates.)doc";

static const char *__doc_fiction_cartesian_layout_ground_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout's ground
layer between `start` and `stop`. The iteration order is the same as
for the coordinates function but without the z dimension.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate to include in the range of all ground coordinates.

Returns:
    An iterator range from `start` to `stop`. If they are not
    provided, the first/last coordinate in the ground layer is used as
    a default.)doc";

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

static const char *__doc_fiction_cartesian_to_hexagonal =
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

static const char *__doc_fiction_cartesian_to_hexagonal_cartesian_to_hexagonal =
R"doc(Standard constructor that computes an initial bounding box.

Parameter ``lyt``:
    Gate-level or cell-level layout whose bounding box is desired.)doc";

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
that they form larger zones, e.g., of :math:` 5 \times 5 ` cells.
These dimensions can be specified in the constructor. They affect the
way, clock numbers are fetched from the underlying clocked layout.

The de-facto standard of cell-level FCN design is to group multiple
cells into tiles large enough to be addressable by individual clocking
electrodes buried in the layout substrate. Cell-based clocking, i.e.,
clock zones of size :math:` 1 \times 1 ` cells are not recommended as
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

static const char *__doc_fiction_cell_level_layout_cell_level_layout_3 = R"doc()doc";

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

static const char *__doc_fiction_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_2 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_3 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_4 = R"doc()doc";

static const char *__doc_fiction_charge_distribution_surface_charge_distribution_surface = R"doc()doc";

static const char *__doc_fiction_charge_state_to_sign =
R"doc(Converts the charge state into an integer (`-1`, `0`, `1`).

Parameter ``cs``:
    SiDB charge state.

Returns:
    Integer representing the SiDB's charge state.)doc";

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

static const char *__doc_fiction_clocked_layout_clocked_layout_storage = R"doc()doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_storage_clocked_layout_storage = R"doc()doc";

static const char *__doc_fiction_clocked_layout_clocked_layout_storage_clocking = R"doc()doc";

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
can be set that restricts the number of paths to the :math:` k `
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

@note In contrast to `mockturtle::cleanup_dangling`, this function
returns `ntk` if `NtkDest` and `NtkSrc` are of the same type.

Template parameter ``NtkDest``:
    Type of the returned logic network.

Template parameter ``NtkSrc``:
    Type of the input logic network.

Parameter ``ntk``:
    The input logic network.

Returns:
    A logic network of type `NtkDest` that is logically equivalent to
    `ntk`.)doc";

static const char *__doc_fiction_convert_to_fiction_coordinates =
R"doc(Converts the coordinates of a given cell-level layout to fiction
coordinates, e.g., `offset::ucoord_t` or `cube::coord_t`. A new
equivalent layout based on fiction coordinates is returned.

Template parameter ``Lyt``:
    Cell-level layout type based on fiction coordinates.

Parameter ``lyt``:
    The layout that is to be converted to a new layout based on
    fiction coordinates.

Returns:
    A new equivalent layout based on fiction coordinates.)doc";

static const char *__doc_fiction_convert_to_siqad_coordinates =
R"doc(Converts the coordinates of a given cell-level layout to SiQAD
coordinates. A new equivalent layout based on SiQAD coordinates is
returned.

Template parameter ``Lyt``:
    Cell-level layout type based on fiction coordinates, e.g.,
    `offset::ucoord_t` or `cube::coord_t`.

Parameter ``lyt``:
    The layout that is to be converted to a new layout based on SiQAD
    coordinates.

Returns:
    A new equivalent layout based on SiQAD coordinates.)doc";

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

static const char *__doc_fiction_create_and_tt =
R"doc(Creates and returns a truth table that implements the conjunction in
two variables.

Returns:
    Conjunction in two variables.)doc";

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

static const char *__doc_fiction_create_id_tt =
R"doc(Creates and returns a truth table that implements the identity
function in one variable.

Returns:
    Identity function in one variable.)doc";

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

The throughput is defined as :math:` \frac{1}{x} ` where :math:` x `
is the highest path length difference between any sets of paths that
lead to the same gate. This function provides only the denominator
:math:` x `, as the numerator is always :math:` 1 `. Furthermore,
:math:` x ` is given in clock cycles rather than clock phases because
it is assumed that a path length difference smaller than
`lyt.num_clocks()` does not lead to any delay. Contrary, for any
throughput value :math:` \frac{1}{x} ` with :math:` x > 1 `, the
layout computes its represented Boolean function only every :math:` x
` full clock cycles after the first inputs have been propagated
through the design. Thereby, all PIs need to be held constant for
:math:` x ` clock phases to ensure proper computation.

For more information on the concept of throughput and delay see
\"Synchronization of Clocked Field-Coupled Circuits\" by F. Sill
Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO
2018; or \"Design Automation for Field-coupled Nanotechnologies\" by
M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
Springer Nature in 2022.

The complexity of this function is :math:` O(|T|) ` where :math:` T `
is the set of all occupied tiles in `lyt`.

Template parameter ``Lyt``:
    Gate-level layout type.

Parameter ``lyt``:
    The gate-level layout whose CP and TP are desired.

Parameter ``pst``:
    Statistics.)doc";

static const char *__doc_fiction_critical_path_length_and_throughput_stats = R"doc()doc";

static const char *__doc_fiction_critical_path_length_and_throughput_stats_critical_path_length = R"doc()doc";

static const char *__doc_fiction_critical_path_length_and_throughput_stats_throughput = R"doc()doc";

static const char *__doc_fiction_csv_writer = R"doc()doc";

static const char *__doc_fiction_csv_writer_add_delimiter =
R"doc(Writes the given argument to the stored file and returns a delimiter.

Template parameter ``T``:
    Type of the given parameter.

Parameter ``arg``:
    Argument to write to the file.

Returns:
    Delimiter.)doc";

static const char *__doc_fiction_csv_writer_csv_writer =
R"doc(Standard constructor. Opens a file.

Parameter ``filename``:
    CSV file to write into.)doc";

static const char *__doc_fiction_csv_writer_file = R"doc(CSV file to write into.)doc";

static const char *__doc_fiction_csv_writer_write_line =
R"doc(Writes a single line of comma-separated values to the stored file.
Note that no escape checks are performed.

Template parameter ``Ts``:
    Types of the variadic parameter pack.

Parameter ``args``:
    Arguments to write to the file.)doc";

static const char *__doc_fiction_cube_coord_t =
R"doc(Signed cube coordinates.

Each coordinate has a dead indicator that can be used to represent
that it is not in use.)doc";

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

static const char *__doc_fiction_cube_coord_t_x = R"doc(x coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_y = R"doc(y coordinate.)doc";

static const char *__doc_fiction_cube_coord_t_z = R"doc(z coordinate.)doc";

static const char *__doc_fiction_debug_print_node_to_tile_assignments = R"doc()doc";

static const char *__doc_fiction_debug_print_tile_to_node_assignments = R"doc()doc";

static const char *__doc_fiction_debug_write_dot_layout = R"doc()doc";

static const char *__doc_fiction_debug_write_dot_network = R"doc()doc";

static const char *__doc_fiction_defect_extent =
R"doc(Returns the extent of a defect as a pair of SiDB distances in
horizontal and vertical direction. If defect is the `NONE` defect
type, `{0, 0}` is returned.

Parameter ``defect``:
    Defect type to evaluate.

Returns:
    Number of horizontal and vertical SiDBs that are affected by the
    given defect type.)doc";

static const char *__doc_fiction_detail_a_star_impl = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_a_star_impl = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_closed_list = R"doc(Closed list that acts as a set of already visited coordinates.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f = R"doc(Stores a coordinate and its f-value.)doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_coord = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_coordinate_f_f = R"doc()doc";

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

static const char *__doc_fiction_detail_a_star_impl_cost = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_distance = R"doc()doc";

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

static const char *__doc_fiction_detail_a_star_impl_layout = R"doc()doc";

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

static const char *__doc_fiction_detail_a_star_impl_open_list =
R"doc(Open list that contains all coordinates to process next sorted by
their f-value.)doc";

static const char *__doc_fiction_detail_a_star_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_reconstruct_path =
R"doc(Reconstruct the final path from the origin map that was created during
the path finding algorithm.

Returns:
    The shortest path connecting source and target.)doc";

static const char *__doc_fiction_detail_a_star_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_set_g =
R"doc(Updates the g-value of the given coordinate to the given value.

Parameter ``c``:
    Coordinate whose g-value is to be updated to g_val.

Parameter ``g_val``:
    New g-value for c.)doc";

static const char *__doc_fiction_detail_a_star_impl_source = R"doc()doc";

static const char *__doc_fiction_detail_a_star_impl_target = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_apply_gate_library_impl = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_assign_gate = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_cell_lyt = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_gate_lyt = R"doc()doc";

static const char *__doc_fiction_detail_apply_gate_library_impl_run = R"doc()doc";

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

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_critical_path_length_and_throughput_impl = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_delay_cache = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_delay = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_diff = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_length = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_path_info = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_path_info_path_info_2 = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_critical_path_length_and_throughput_impl_signal_delay = R"doc()doc";

static const char *__doc_fiction_detail_determine_layout_size = R"doc()doc";

static const char *__doc_fiction_detail_east_south_edge_coloring = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_collection = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_enumerate_all_clocking_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_is_visited = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_layout = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_mark_unvisited = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_mark_visited = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_objective = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_recursively_enumerate_all_paths = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_enumerate_all_clocking_paths_impl_visited = R"doc()doc";

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

static const char *__doc_fiction_detail_jump_point_search_impl = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_closed_list = R"doc(Closed list that acts as a set of already visited coordinates.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f = R"doc(Stores a coordinate and its f-value.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_coord = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_coordinate_f_f = R"doc()doc";

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

static const char *__doc_fiction_detail_jump_point_search_impl_distance = R"doc()doc";

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
is if the coordinates are not identical, if the clocking checks out,
and if there is no obstacle in the way.

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
traversable from successor. That is, there must be a valid clocking
path through non-obstructed tiles.

Parameter ``current``:
    Parent coordinate.

Parameter ``successor``:
    Coordinate from which to look for the next jump point.

Returns:
    The next jump point or std::nullopt if no such coordinate is
    available.)doc";

static const char *__doc_fiction_detail_jump_point_search_impl_jump_point_search_impl = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_layout = R"doc()doc";

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

static const char *__doc_fiction_detail_jump_point_search_impl_source = R"doc()doc";

static const char *__doc_fiction_detail_jump_point_search_impl_target = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_network_balancing_impl = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_depth = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ntk_topo = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_network_balancing_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_orthogonal_impl = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_po_counter = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_ps = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_pst = R"doc()doc";

static const char *__doc_fiction_detail_orthogonal_impl_run = R"doc()doc";

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

static const char *__doc_fiction_detail_read_sqd_layout_impl_is = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_lyt = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_max_cell_pos = R"doc(The maximum position of a cell in the layout.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_db_dot =
R"doc(Parses a <dbdot> element from the SQD file and adds the respective dot
to the layout.

Parameter ``db_dot``:
    The <dbdot> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_db_dot_siqad = R"doc()doc";

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

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_latcoord =
R"doc(Parses a <latcoord> element from the SQD file and returns its
specified cell position.

Parameter ``latcoord``:
    The <latcoord> element.

Returns:
    The cell position specified by the <latcoord> element.)doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_parse_latcoord_siqad = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_read_sqd_layout_impl = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_read_sqd_layout_impl_2 = R"doc()doc";

static const char *__doc_fiction_detail_read_sqd_layout_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_recursively_paint_edges = R"doc()doc";

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

static const char *__doc_fiction_detail_wire_east = R"doc()doc";

static const char *__doc_fiction_detail_wire_south = R"doc()doc";

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

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl = R"doc()doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_k_shortest_paths = R"doc(The list of k shortest paths that is created during the algorithm.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_layout =
R"doc(The layout in which k shortest paths are to be found extended by an
obstruction functionality layer.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_num_shortest_paths = R"doc(The number of paths to determine, i.e., k.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_objective = R"doc(Source and target coordinates.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_path_cost =
R"doc(Computes the cost of a path. This function can be adjusted to fetch
paths of differing costs.

Currently, the cost is equal to its length.

Parameter ``p``:
    Path whose costs are to be calculated.

Returns:
    Costs of path p.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_ps = R"doc(Parameters.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_reset_temporary_obstructions = R"doc(Resets all temporary obstructions.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_run = R"doc()doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_shortest_path_candidates = R"doc(A set of potential shortest paths.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_temporarily_obstructed_coordinates =
R"doc(A temporary storage for coordinates that are obstructed during the
algorithm.)doc";

static const char *__doc_fiction_detail_yen_k_shortest_paths_impl_yen_k_shortest_paths_impl = R"doc()doc";

static const char *__doc_fiction_determine_vertex_coloring =
R"doc(This function provides an interface to call various vertex coloring
algorithms on the given graph. A vertex coloring is the assignment of
colors to graph vertices such that no two vertices that share an edge
receive the same color. If a graph is colorable with :math:` k `
colors, the graph is said to be :math:` k `-colorable. The minimum
value of :math:` k ` for a graph is called its chromatic number. To
determine the chromatic number of a graph is :math:` NP `-complete in
general. The provided algorithms attempt to get as close to the
optimum coloring as possible. However, no heuristic can give an
optimality guarantee. If the exact chromatic number is required, the
SAT-based engine must be used. This may require exponential runtime in
the worst case but is, on average, a lot faster due to the smart
traversal of search spaces that SAT solvers provide.

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
R"doc(:math:` k `-color value for :math:` k `-coloring algorithms, e.g.,
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

static const char *__doc_fiction_energy_dissipation_stats = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_fast = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_report = R"doc()doc";

static const char *__doc_fiction_energy_dissipation_stats_slow =
R"doc(Energy dissipation information in meV for slow (25 GHz) and fast (100
GHz) clocking.)doc";

static const char *__doc_fiction_energy_dissipation_stats_unknown = R"doc()doc";

static const char *__doc_fiction_energy_distribution =
R"doc(This function takes in a vector of charge_distribution_surface objects
and returns a map containing the system energy and the number of
occurrences of that energy in the input vector.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``input_vec``:
    A vector of charge_distribution_surface objects for which
    statistics are to be computed.

Returns:
    A map containing the system energy as the key and the number of
    occurrences of that energy in the input vector as the value.)doc";

static const char *__doc_fiction_enumerate_all_clocking_paths =
R"doc(Enumerates all possible paths in a clocked layout that start at
coordinate source and lead to coordinate target while respecting the
information flow imposed by the clocking scheme. This algorithm does
neither generate duplicate nor looping paths, even in a cyclic
clocking scheme. That is, along each path, each coordinate can occur
at maximum once.

If the given layout implements the obstruction interface (see
obstruction_layout), paths will not be routed via obstructed
coordinates and connections.

If the given layout is a gate-level layout and implements the
obstruction interface (see obstruction_layout), paths may contain wire
crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

Template parameter ``Path``:
    Type of the returned individual paths.

Template parameter ``Lyt``:
    Type of the clocked layout to perform path finding on.

Parameter ``layout``:
    The clocked layout whose paths are to be enumerated.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``ps``:
    Parameters.

Returns:
    A collection of all unique paths in `layout` from `source` to
    `target`.)doc";

static const char *__doc_fiction_enumerate_all_clocking_paths_params =
R"doc(Parameters for the algorithm that enumerates all paths in a clocked
layout.)doc";

static const char *__doc_fiction_enumerate_all_clocking_paths_params_crossings =
R"doc(Allow paths to cross over obstructed tiles if they are occupied by
wire segments.)doc";

static const char *__doc_fiction_eq_type = R"doc(The different equivalence types possible.)doc";

static const char *__doc_fiction_eq_type_NO = R"doc(`Spec` and `Impl` are logically not equivalent OR `Impl` has DRVs.)doc";

static const char *__doc_fiction_eq_type_STRONG =
R"doc(`Spec` and `Impl` are logically equivalent AND `Impl` has a throughput
of :math:` \frac{1}{1} `.)doc";

static const char *__doc_fiction_eq_type_WEAK =
R"doc(`Spec` and `Impl` are logically equivalent BUT `Impl` has a throughput
of :math:` \frac{1}{x} ` with :math:` x > 1 `.)doc";

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
with TP of :math:` \frac{1}{x} ` with :math:` x > 1 ` or both of them
are gate-level layouts with TP of :math:` \frac{1}{x} ` and :math:`
\frac{1}{y} `, respectively, where :math:` x \neq y `. - `STRONG`
equivalence: Spec and Impl are logically equivalent and all involved
gate-level layouts have TP of :math:` \frac{1}{1} `.

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
    Statistics.)doc";

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
R"doc(The Euclidean distance :math:` D ` between two layout coordinates
:math:` (x_1, y_1) ` and :math:` (x_2, y_2) ` given by

:math:` D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2} `

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
logic network will be found under constraints. Starting with :math:` n
` tiles, where :math:` n ` is the number of logic network nodes, each
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
R"doc(Investigate only aspect ratios with the number of tiles given as upper
bound.)doc";

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

static const char *__doc_fiction_exgs_stats = R"doc()doc";

static const char *__doc_fiction_exgs_stats_duration = R"doc()doc";

static const char *__doc_fiction_exgs_stats_report = R"doc()doc";

static const char *__doc_fiction_exgs_stats_valid_lyts = R"doc()doc";

static const char *__doc_fiction_exhaustive_ground_state_simulation =
R"doc(All metastable and physically valid charge distribution layouts are
computed, stored in a vector and returned.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``params``:
    Simulation parameters.

Parameter ``ps``:
    Simulation statistics.)doc";

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

static const char *__doc_fiction_fanout_substitution_params_substitution_strategy_BREADTH = R"doc()doc";

static const char *__doc_fiction_fanout_substitution_params_substitution_strategy_DEPTH = R"doc()doc";

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

Parameter ``n``:
    Node to check.

Returns:
    Number of fanins to `n`.)doc";

static const char *__doc_fiction_gate_level_layout_fanout_size =
R"doc(Returns the number of outgoing, adjacently placed, and properly
clocked signals of the given node.

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

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in eastern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_input_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_name_2 = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_no_incoming_signal =
R"doc(Checks whether the given tile has no incoming tiles.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` does not have incoming tiles.)doc";

static const char *__doc_fiction_gate_level_layout_has_no_outgoing_signal =
R"doc(Checks whether the given tile has no outgoing tiles.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` does not have outgoing tiles.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_eastern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in north-eastern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in north-eastern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in north-western
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_north_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in north-western
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north_west(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_northern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in northern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `north(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_northern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in northern
direction.

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

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `t` has incoming and outgoing signals on opposite
    sides.)doc";

static const char *__doc_fiction_gate_level_layout_has_output_name = R"doc()doc";

static const char *__doc_fiction_gate_level_layout_has_south_eastern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in south-eastern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_east(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_eastern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in south-eastern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_east(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in south-western
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_south_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in south-western
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south_west(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_southern_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in southern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_southern_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in southern
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `south(t)` is outgoing from `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_western_incoming_signal =
R"doc(Checks whether the given tile has an incoming one in western
direction.

Parameter ``t``:
    Base tile.

Returns:
    `true` iff `west(t)` is incoming to `t`.)doc";

static const char *__doc_fiction_gate_level_layout_has_western_outgoing_signal =
R"doc(Checks whether the given tile has an outgoing one in western
direction.

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

static const char *__doc_fiction_generate_edge_intersection_graph_stats_duration = R"doc(Runtime measurement.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_num_edges = R"doc(Stores the size of the generated edge intersection graph.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_num_vertices = R"doc(Stores the size of the generated edge intersection graph.)doc";

static const char *__doc_fiction_generate_edge_intersection_graph_stats_number_of_unroutable_objectives =
R"doc(For each routing objective that cannot be fulfilled in the given
layout, this counter is incremented.)doc";

static const char *__doc_fiction_geometric_temperature_schedule =
R"doc(A logarithmically decreasing temperature schedule. The temperature is
altered by multiplying it with `0.99`.

Parameter ``t``:
    The current temperature.

Returns:
    The next temperature, i.e. :math:` \texttt{t} \cdot 0.99 `.)doc";

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
R"doc(A :math:` k `-coloring algorithm using tabu search proposed in \"Using
Tabu Search Techniques for Graph Coloring\" by A. Hertz and D. de
Werra in Computing 1987. The authors claim that it significantly
outperforms simulated annealing. However, since it is a :math:` k
`-coloring algorithm, it is required to set `k_color_value` in
`determine_vertex_coloring_params` to the chromatic number that is to
be checked for.)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic =
R"doc(An enumeration of search tactics to use for the SAT-based graph
coloring to determine a min-coloring.)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_BINARY_SEARCH =
R"doc(First ascend exponentially by checking for :math:` k = 2^0, 2^1, 2^2,
\dots ` until SAT, then perform binary search in the window :math:`
[2^{h-1}, 2^h] `, where :math:` 2^h ` was the first SAT. If at least
one clique is passed, :math:` k ` starts at the largest clique size
:math:` |C| ` instead with :math:` k = 2^0 \cdot |C|, 2^1 \cdot |C|,
2^2 \cdot |C|, \dots `)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_LINEARLY_ASCENDING =
R"doc(Ascend linearly by checking for :math:` k = 1, 2, 3, \dots ` until
SAT. If at least one clique is passed, :math:` k ` starts at the
largest clique size :math:` |C| ` instead with :math:` k = |C|, |C| +
1, |C| + 2, \dots `)doc";

static const char *__doc_fiction_graph_coloring_sat_search_tactic_LINEARLY_DESCENDING =
R"doc(Descend linearly by checking for :math:` k = |G|, |G| - 1, |G| - 2,
\dots ` until UNSAT.)doc";

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

static const char *__doc_fiction_has_foreach_charge_state = R"doc()doc";

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
R"doc(Returns the layout's number of faces which are equal to :math:` (x +
1) \cdot (y + 1) `.

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
    Last coordinate to include in the range of all coordinates.

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
    Last coordinate to include in the range of all coordinates.)doc";

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
    Last coordinate to include in the range of all ground coordinates.)doc";

static const char *__doc_fiction_hexagonal_layout_ground_coordinates =
R"doc(Returns a range of all coordinates accessible in the layout's ground
layer between `start` and `stop`. The iteration order is the same as
for the coordinates function but without the z dimension.

Parameter ``start``:
    First coordinate to include in the range of all ground
    coordinates.

Parameter ``stop``:
    Last coordinate to include in the range of all ground coordinates.

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

static const char *__doc_fiction_high_degree_fanin_exception =
R"doc(Exception class that can be thrown if some network exceeds a legal
number of fanins.)doc";

static const char *__doc_fiction_high_degree_fanin_exception_what = R"doc()doc";

static const char *__doc_fiction_horizontal_shift_cartesian =
R"doc(\verbatim +-------+ | | | | | | +---+---+---+ | | | | | | +-------+
\endverbatim)doc";

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
the ToPoliNano library with :math:` 4 \times 4 ` magnet positions with
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

static const char *__doc_fiction_is_charged_defect =
R"doc(Checks whether the given defect is charged. Charged defects are to be
avoided by a larger distance.

Parameter ``defect``:
    Defect type to check.

Returns:
    `true` iff defect is charged.)doc";

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

Parameter ``quicksim_results``:
    All found physically valid charge distribution surfaces obtained
    by the *QuickSim* algorithm.

Parameter ``exhaustive_results``:
    All valid charge distribution surfaces determined by ExGS.

Returns:
    Returns `true` if the relative difference between the lowest
    energies of the two sets is less than :math:` 0.00001 `, `false`
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

static const char *__doc_fiction_is_neutral_defect =
R"doc(Checks whether the given defect is not charged. Neutral defects are to
be avoided but not by such a large distance. Even though the `NONE`
defect type is technically neutral, it is not a defect per se which is
why this function returns false on the `NONE` defect input.
Additionally, the `UNKNOWN` defect cannot be guaranteed to be either
neutral or charged which is why this function returns false for
`UNKNOWN` defect inputs as well.

Parameter ``defect``:
    Defect type to check.

Returns:
    `true` iff defect is not charged.)doc";

static const char *__doc_fiction_is_shifted_cartesian_layout = R"doc()doc";

static const char *__doc_fiction_is_tile_based_layout = R"doc()doc";

static const char *__doc_fiction_jump_point_search =
R"doc(The Jump Point Search (JPS) path finding algorithm for shortest
loopless paths between a given source and target coordinate in a
clocked layout. JPS was proposed as an optimization of A* for shortest
paths and offers better average complexity on uniform-cost grids that
allow diagonal connections. It uses a heuristic distance function that
estimates the remaining costs towards the target in every step. Thus,
this heuristic function should neither be complex to calculate nor
overestimating the remaining costs. Common heuristics to be used are
the Manhattan and the Euclidean distance functions. See
distance_functor for implementations. Since JPS assumes a unit-cost
grid, the use of cost functions together with JPS is not possible.

If the given layout implements the obstruction interface (see
obstruction_layout), paths will not be routed via obstructed
coordinates and connections.

JPS was introduced in \"Online Graph Pruning for Pathfinding on Grid
Maps\" by Daniel Harabor and Alban Grastien in AAAI 2011.

Parts of this implementation are based on
https://github.com/qiao/PathFinding.js.

@note The original JPS highly relies on diagonal paths in the grid
which are not possible in most Cartesian grid-based FCN technologies.
Therefore, this implementation disallows diagonal paths. Consequently,
and due to non-uniform clocking schemes, JPS might perform worse than
A* in terms of runtime. It is recommended to use A* (see a_star).

@note JPS does not support wire crossings.

Template parameter ``Path``:
    Path type to create.

Template parameter ``Lyt``:
    Clocked layout type.

Template parameter ``Dist``:
    Distance value type to be used in the heuristic estimation
    function.

Parameter ``layout``:
    The clocked layout in which the shortest path between `source` and
    `target` is to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``dist_fn``:
    A distance functor that implements the desired heuristic
    estimation function.

Parameter ``ps``:
    Parameters.

Returns:
    The shortest loopless path in `layout` from `source` to `target`.)doc";

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
    The next temperature, i.e. :math:` \texttt{t} - 10 `.)doc";

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
R"doc(The Manhattan distance :math:` D ` between two layout coordinates
:math:` (x_1, y_1) ` and :math:` (x_2, y_2) ` given by

:math:` D = |x_1 - x_2| + |y_1 - y_2| `

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
R"doc(Computes the minimum energy of a vector of charge_distribution_surface
objects.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``result``:
    Vector of charge_distribution_surface objects.

Returns:
    Value of the minimum energy found in the input vector.)doc";

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

static const char *__doc_fiction_obstruction_layout = R"doc()doc";

static const char *__doc_fiction_obstruction_layout_2 = R"doc()doc";

static const char *__doc_fiction_obstruction_layout_obstruction_layout = R"doc()doc";

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

static const char *__doc_fiction_offset_coord_iterator =
R"doc(An iterator type that allows to enumerate coordinates in order within
a boundary.

Template parameter ``CoordinateType``:
    Type of coordinate to enumerate.)doc";

static const char *__doc_fiction_offset_coord_iterator_aspect_ratio = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_coord = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_coord_iterator =
R"doc(Standard constructor. Initializes the iterator with a starting
position and the boundary within to enumerate.

With `dimension = (1, 2, 1)` and `start = (0, 0, 0)`, the following
order would be enumerated:

- (0, 0, 0) - (1, 0, 0) - (0, 1, 0) - (1, 1, 0) - (0, 2, 0) - (1, 2,
0) - (0, 0, 1) - (1, 0, 1) - (0, 1, 1) - (1, 1, 1) - (0, 2, 1) - (1,
2, 1)

coord_iterator is compatible with the STL forward_iterator category.

Parameter ``dimension``:
    Boundary within to enumerate. Iteration wraps at its limits.

Parameter ``start``:
    Starting coordinate to enumerate first.)doc";

static const char *__doc_fiction_offset_coord_iterator_operator_eq = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_operator_inc =
R"doc(Increments the iterator.

Returns:
    Reference to the incremented iterator.)doc";

static const char *__doc_fiction_offset_coord_iterator_operator_inc_2 = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_operator_le = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_operator_lt = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_operator_mul = R"doc()doc";

static const char *__doc_fiction_offset_coord_iterator_operator_ne = R"doc()doc";

static const char *__doc_fiction_offset_operator_lshift = R"doc()doc";

static const char *__doc_fiction_offset_ucoord_t =
R"doc(Unsigned offset coordinates.

The implementation is optimized for memory-efficiency and fits within
64 bits. Coordinates span from :math:` (0, 0, 0) ` to :math:` (2^{31}
- 1, 2^{31} - 1, 1) `. Each coordinate has a dead indicator `d` that
can be used to represent that it is not in use.)doc";

static const char *__doc_fiction_offset_ucoord_t_d = R"doc(MSB acts as dead indicator.)doc";

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

The algorithm works in polynomial time :math:` O(3|N| + |L|) ` where
:math:` |N| ` is the number of nodes in the given network and :math:`
|L| ` is the resulting layout size given by :math:` x \cdot y `, which
approaches :math:` (\frac{|N|}{2})^2 ` asymptotically.

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

static const char *__doc_fiction_out_of_cell_names_exception_what = R"doc()doc";

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
schemes. Tiles in QCA ONE are :math:` 5 \times 5 ` QCA cells.)doc";

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

static const char *__doc_fiction_quicksim =
R"doc(The *QuickSim* algorithm is an electrostatic ground state simulation
algorithm for SiDB layouts. It determines physically valid charge
configurations (with minimal energy) of a given (already initialized)
charge distribution layout. Depending on the simulation parameters,
the ground state is found with a certain probability after one run.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``lyt``:
    The layout to simulate.

Parameter ``ps``:
    Physical parameters. They are material-specific and may vary from
    experiment to experiment.

Parameter ``pst``:
    Statistics. They store the simulation results (simulation runtime
    as well as all physically valid charge distribution layouts).)doc";

static const char *__doc_fiction_quicksim_params = R"doc(This struct stores the parameters for the *QuickSim* algorithm.)doc";

static const char *__doc_fiction_quicksim_params_alpha =
R"doc(`alpha` parameter for the *QuickSim* algorithm (should be reduced if
no result is found).)doc";

static const char *__doc_fiction_quicksim_params_interation_steps = R"doc(Number of iterations to run the simulation for.)doc";

static const char *__doc_fiction_quicksim_params_number_threads =
R"doc(Number of threads to spawn. By default the number of threads is set to
the number of available hardware threads.)doc";

static const char *__doc_fiction_quicksim_params_phys_params = R"doc(General parameters for the simulation of the physical SiDB system.)doc";

static const char *__doc_fiction_quicksim_stats =
R"doc(This struct stores the simulation runtime and all physically valid
charge layouts gained by the *QuickSim* algorithm.

Parameter ``Lyt``:
    Cell-level layout type.)doc";

static const char *__doc_fiction_quicksim_stats_duration = R"doc(Total simulation runtime.)doc";

static const char *__doc_fiction_quicksim_stats_report =
R"doc(Report the simulation statistics in a human-readable fashion.

Parameter ``out``:
    Output stream to write to.)doc";

static const char *__doc_fiction_quicksim_stats_valid_lyts = R"doc(Vector of all physically valid charge layouts.)doc";

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
    The layout type to be created from an input. Must be a cell-level
    SiDB layout.

Parameter ``is``:
    The input stream to read from.

Parameter ``name``:
    The name to give to the generated layout.)doc";

static const char *__doc_fiction_read_sqd_layout_2 =
R"doc(Reads a cell-level SiDB layout from an sqd file provided as an input
stream. The format is used by SiQAD (https://github.com/siqad/siqad).

If The provided cell-level layout type can represent SiDB defects,
they will be parsed from the sqd file as well.

May throw an `sqd_parsing_exception` if the sqd file is malformed.

This is an in-place version of read_sqd_layout that utilizes the given
layout as a target to write to.

Template parameter ``Lyt``:
    The layout type to be used as input. Must be a cell-level SiDB
    layout.

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
    The layout type to be created from an input. Must be a cell-level
    SiDB layout.

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
    The layout type to be used as input. Must be a cell-level SiDB
    layout.

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
cartesian layout with shifted coordinates. Its faces are organizes in
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

static const char *__doc_fiction_shifted_cartesian_layout_odd_row_hex = R"doc()doc";

static const char *__doc_fiction_shifted_cartesian_layout_shifted_cartesian_layout =
R"doc(Standard constructor. The given aspect ratio points to the highest
possible coordinate in the layout. That means in the
even_column_cartesian ASCII layout representation above `ar = (3,2)`.
Consequently, with `ar = (0,0)`, the layout has exactly one
coordinate.

Parameter ``ar``:
    Highest possible position in the layout.)doc";

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
R"doc(In accordance with the paper mentioned above, the sidb_defect struct
is used to represent a specific defect on the H-Si(100) 2x1 surface
that has a charge as well as relative permittivity (`epsilon_r`) and
Thomas-Fermi screening distance (`lambda_tf`) values associated to it.

See \"SiQAD: A Design and Simulation Tool for Atomic Silicon Quantum
Dot Circuits\" by S. S. H. Ng, J. Retallick, H. N. Chiu, R. Lupoiu, L.
Livadaru, T. Huff, M. Rashidi, W. Vine, T. Dienel, R. A. Wolkow, and
K. Walus in IEEE Transactions on Nanotechnology for more details on
these values.)doc";

static const char *__doc_fiction_sidb_defect_charge = R"doc(Electrical charge.)doc";

static const char *__doc_fiction_sidb_defect_epsilon_r = R"doc(Electric permittivity.)doc";

static const char *__doc_fiction_sidb_defect_lambda_tf = R"doc(Thomas-Fermi screening distance in nm.)doc";

static const char *__doc_fiction_sidb_defect_sidb_defect = R"doc(Standard constructor.)doc";

static const char *__doc_fiction_sidb_defect_type =
R"doc(Specifies the types of fabrication defects that can occur on the
H-Si(100) 2x1 surface according to \"Atomic defect classification of
the H–Si(100) surface through multi-mode scanning probe microscopy\"
by Jeremiah Croshaw, Thomas Dienel, Taleana Huff, and Robert Wolkow in
Journal of Nanotechnology in 2020.)doc";

static const char *__doc_fiction_sidb_defect_type_2 = R"doc(Type of defect.)doc";

static const char *__doc_fiction_sidb_defect_type_DB = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_DIHYDRIDE_PAIR = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_ETCH_PIT = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_GUNK = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_MISSING_DIMER = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_NONE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_ONE_BY_ONE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_RAISED_SI = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_SILOXANE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_SINGLE_DIHYDRIDE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_SI_VACANCY = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_STEP_EDGE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_THREE_BY_ONE = R"doc()doc";

static const char *__doc_fiction_sidb_defect_type_UNKNOWN = R"doc()doc";

static const char *__doc_fiction_sidb_nanometer_distance =
R"doc(Computes the distance between two SiDB cells in nanometers.

Template parameter ``Lyt``:
    SiDB cell-level layout type.

Template parameter ``Dist``:
    Floating-point type for the distance.

Parameter ``c1``:
    The first cell.

Parameter ``c2``:
    The second cell.

Returns:
    The distance between the two cells in nanometers.)doc";

static const char *__doc_fiction_sidb_nm_position =
R"doc(Computes the position of a cell in nanometers from the layout origin.

Template parameter ``Lyt``:
    The layout type.

Parameter ``sp``:
    The simulation parameters (required for the lattice constants).

Parameter ``c``:
    The cell to compute the position for.

Returns:
    A pair representing the `(x,y)` position of `c` in nanometers from
    the layout origin.)doc";

static const char *__doc_fiction_sidb_simulation_parameters =
R"doc(This struct collects all physical parameters for physical SiDB
simulations. It can be useful to adjust them, especially when
experiments create new insights. However, the default values are
commonly used.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_base =
R"doc(base can be either 2 or 3 and describes the assumed number of charge
states of one SiDB. It often makes sense to assume only negatively and
neutrally charged SiDBs.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_epsilon_r =
R"doc(epsilon_r is the electric permittivity. It is a material specific
number.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_k =
R"doc(k is the Coulomb constant and is inversely proportional to the
electric permittivity.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_lambda_tf = R"doc(lambda_tf is the Thomas-Fermi screening distance.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_lat_a = R"doc(lat_a is the lattice vector in x-direction.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_lat_b = R"doc(lat_b is the lattice vector in y-direction.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_lat_c = R"doc(lat_c is the dimer pair separation.)doc";

static const char *__doc_fiction_sidb_simulation_parameters_mu = R"doc(µ- is the energy transition level (0/-))doc";

static const char *__doc_fiction_sidb_simulation_parameters_mu_p = R"doc(µ+ is the energy transition level (+/0))doc";

static const char *__doc_fiction_sidb_simulation_parameters_sidb_simulation_parameters =
R"doc(Default constructor.

Parameter ``base_number``:
    simulation can be conducted with 2 and 3 charge states. 2 =
    (Negative, Neutral), 3 = (Negative, Neutral, Positive).

Parameter ``mu_minus``:
    it is the energy transition level (0/-).

Parameter ``relative_permittivity``:
    it describes the electric field reduction due to polarization.

Parameter ``screening_distance``:
    also known as "Thomas-Fermi screening" and it describes the
    electric field screening due to free charges.

Parameter ``a``:
    lattice constant.

Parameter ``b``:
    lattice constant.

Parameter ``c``:
    lattice constant.)doc";

static const char *__doc_fiction_sidb_surface = R"doc()doc";

static const char *__doc_fiction_sidb_surface_2 = R"doc()doc";

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

static const char *__doc_fiction_sidb_surface_params = R"doc()doc";

static const char *__doc_fiction_sidb_surface_params_ignore =
R"doc(Specifies which defects are to be ignored, e.g., when they are not
relevant for the current analysis.

@note Ignored defects are not stored in the surface instance!)doc";

static const char *__doc_fiction_sidb_surface_sidb_surface = R"doc()doc";

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

static const char *__doc_fiction_sign_to_charge_state =
R"doc(Converts an integer (`-1`, `0`, `1`) into a charge state.

Parameter ``sg``:
    Integer (`-1`, `0`, `1`) representing a charge state.

Returns:
    sidb_charge_state representation of `sg`.)doc";

static const char *__doc_fiction_sim_acc_tts =
R"doc(This function determines the time-to-solution (TTS) and the accuracy
(acc) of the *QuickSim* algorithm.

Template parameter ``Lyt``:
    Cell-level layout type.

Parameter ``lyt``:
    Layout that is used for the simulation.

Parameter ``sidb_params``:
    Physical SiDB parameters which are used for the simulation.

Parameter ``ps``:
    Pointer to a struct where the results (time_to_solution, acc,
    single runtime) are stored.

Parameter ``repetitions``:
    Number of repetitions to determine the simulation accuracy
    (`repetitions = 100` means that accuracy is precise to 1%).

Parameter ``confidence_level``:
    The time-to-solution also depends on the given confidence level
    which can be set here.)doc";

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

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_fillcolor = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_id = R"doc()doc";

static const char *__doc_fiction_simple_gate_layout_tile_drawer_tile_label = R"doc()doc";

static const char *__doc_fiction_siqad_coord_t =
R"doc(SiQAD coordinates.

Coordinates span from :math:` (-2^{31}, -2^{31}, 0) ` to :math:`
(2^{31} - 1 , 2^{31} - 1, 1) `. `x` is the SiDB's x-coordinate, `y` is
the dimer pair's row number, and `z` represents the two possible SiDB
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
the case if z is smaller, or if z is equal but y is smaller, or if z
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

static const char *__doc_fiction_siqad_coord_t_x = R"doc(31 bit for the x coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_y = R"doc(31 bit for the y coordinate.)doc";

static const char *__doc_fiction_siqad_coord_t_z = R"doc(1 bit for the z coordinate.)doc";

static const char *__doc_fiction_siqad_to_fiction_coord =
R"doc(Converts SiQAD coordinates to other coordinates (offset, cube).

Template parameter ``CoordinateType``:
    Coordinate type to convert to.

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

static const char *__doc_fiction_sqd_parsing_error = R"doc(Exception thrown when an error occurs during parsing of an SQD file.)doc";

static const char *__doc_fiction_sqd_parsing_error_sqd_parsing_error = R"doc()doc";

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

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage = R"doc()doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage_se_map = R"doc()doc";

static const char *__doc_fiction_synchronization_element_layout_synchronization_element_layout_storage_synchronization_element_layout_storage = R"doc()doc";

static const char *__doc_fiction_technology_constraints = R"doc(Target technologies.)doc";

static const char *__doc_fiction_technology_constraints_NONE = R"doc()doc";

static const char *__doc_fiction_technology_constraints_TOPOLINANO = R"doc()doc";

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

static const char *__doc_fiction_tile_based_layout_adjacent_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_adjacent_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_ground_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_foreach_tile = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_ground_tiles = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tile_based_layout = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tile_based_layout_2 = R"doc()doc";

static const char *__doc_fiction_tile_based_layout_tiles = R"doc()doc";

static const char *__doc_fiction_time_to_solution_stats =
R"doc(This struct stores the time-to-solution, the simulation accuracy and
the average single simulation runtime of *QuickSim* (see
quicksim.hpp).)doc";

static const char *__doc_fiction_time_to_solution_stats_acc = R"doc(Accuracy of the simulation.)doc";

static const char *__doc_fiction_time_to_solution_stats_mean_single_runtime = R"doc(Average single simulation runtime in seconds.)doc";

static const char *__doc_fiction_time_to_solution_stats_report =
R"doc(Print the results to the given output stream.

Parameter ``out``:
    Output stream.)doc";

static const char *__doc_fiction_time_to_solution_stats_single_runtime_exhaustive = R"doc(Single simulation runtime of the exhaustive ground state searcher.)doc";

static const char *__doc_fiction_time_to_solution_stats_time_to_solution = R"doc(Time-to-solution in seconds.)doc";

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
0, 0). Calculates :math:` (x + 1) \cdot (y + 1) \cdot (z + 1) `.

Template parameter ``CoordinateType``:
    Coordinate type.

Parameter ``coord``:
    Coordinate.

Returns:
    Volume of coord.)doc";

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
based layouts, QCA layouts of tile size :math:` 5 \times 5 ` are
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
layouts, QCA layouts of tile size :math:` 5 \times 5 ` are supported
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

If The provided cell-level layout type can represent SiDB defects,
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

If The provided cell-level layout type can represent SiDB defects,
they will be written to the file as well.

This overload uses file name to create and write into.

Template parameter ``Lyt``:
    Cell-level SiDB or QCA layout type.

Parameter ``lyt``:
    The layout to be written.

Parameter ``filename``:
    The file name to create and write into. Should preferably use the
    `.sqd` extension.)doc";

static const char *__doc_fiction_yen_k_shortest_paths =
R"doc(Yen's algorithm for finding up to :math:` k ` shortest paths without
loops from source to target. This implementation works on clocked
layouts and uses the A* algorithm with the Manhattan distance function
internally. The algorithm was originally described in \"An algorithm
for finding shortest routes from all source nodes to a given
destination in general networks\" by Jin Y. Yen in Quarterly of
Applied Mathematics, 1970.

If :math:` k ` is larger than the number of possible paths from source
to target, the size of the returned path collection will be smaller
than :math:` k `.

If the given layout is a gate-level layout and implements the
obstruction interface (see obstruction_layout), paths may contain wire
crossings if specified in the parameters. Wire crossings are only
allowed over other wires and only if the crossing layer is not
obstructed. Furthermore, it is ensured that crossings do not run along
another wire but cross only in a single point (orthogonal crossings +
knock-knees/double wires).

Template parameter ``Path``:
    Path type to create.

Template parameter ``Lyt``:
    Clocked layout type.

Parameter ``layout``:
    The clocked layout in which the :math:` k ` shortest paths between
    `source` and `target` are to be found.

Parameter ``objective``:
    Source-target coordinate pair.

Parameter ``k``:
    Maximum number of shortest paths to find.

Parameter ``ps``:
    Parameters.

Returns:
    A collection of up to :math:` k ` shortest loopless paths in
    `layout` from `source` to `target`.)doc";

static const char *__doc_fiction_yen_k_shortest_paths_params = R"doc(Parameters for Yen's :math:` k `-shortest paths algorithm.)doc";

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

static const char *__doc_std_hash_3 =
R"doc(Provides a hash implementation for `std::array<T, N>`.

Template parameter ``T``:
    Object type in `std::array`.

Template parameter ``N``:
    Size of the array.)doc";

static const char *__doc_std_hash_4 =
R"doc(Provides a hash implementation for `mockturtle::edge<Ntk>`.

Template parameter ``Ntk``:
    Network type of edge.)doc";

static const char *__doc_std_hash_operator_call = R"doc()doc";

static const char *__doc_std_hash_operator_call_2 = R"doc()doc";

static const char *__doc_std_hash_operator_call_3 = R"doc()doc";

static const char *__doc_std_iterator_traits = R"doc()doc";

static const char *__doc_std_iterator_traits_2 = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

