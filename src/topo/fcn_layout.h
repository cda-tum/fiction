//
// Created by marcel on 25.07.18.
//

#ifndef FICTION_FCN_LAYOUT_H
#define FICTION_FCN_LAYOUT_H

#include "grid_graph.h"
#include "fcn_clocking_scheme.h"
#include <random>
#include <unordered_map>
#include <itertools.hpp>
#include <boost/functional/hash.hpp>


/**
 * Alias for boost::array needed for constructor call.
 */
using fcn_dimension_xyz = boost::array<std::size_t, 3>;
/**
 * Alias for boost::array needed for constructor call.
 */
using fcn_dimension_xy = boost::array<std::size_t, 2>;
/**
 * Constant referring to ground layer.
 */
constexpr const std::size_t GROUND = 0u;
/**
 * Constant referring to x-dimension value of faces.
 */
constexpr const std::size_t X = 0u;
/**
 * Constant referring to y-dimension value of faces.
 */
constexpr const std::size_t Y = 1u;
/**
 * Constant referring to z-dimension value of faces.
 */
constexpr const std::size_t Z = 2u;
/**
 * Represents general layouts for Field-coupled Nanocomputing (FCN) devices on an arbitrary abstraction level.
 * Layouts are always 3-dimensional grid-like structures consisting of faces. Each face is addressed by a 3-tuple
 * (x, y, z) where axis are named intuitively. Numbering starts in the upper left ground layer with (0, 0, 0) and ends
 * in the lower right highest layer with (x - 1, y - 1, z - 1).
 *
 * They can be accessed by operator(x, y, z) or operator[x][y][z], where the former is more performant. Special
 * constants defined above allow for direct access of X-, Y-, Z-coordinate of faces. Assume f is a face. Then f[X]
 * returns its X-coordinate. Works analogously for the other dimensions.
 *
 * Each layout must have a clocking scheme which can be regular or irregular. For regular clocking schemes, a small
 * cutout is used to extrapolate the whole floorplan. Some pre-defined clocking schemes are provided in
 * fcn_clocking_scheme.h. More can easily be implemented there. Irregular clockings are stored in a special map which
 * assigns a designated clock zone to each face.
 *
 * Besides that, special clock latches are available. See docstrings below for a detailed description of how they work.
 *
 * This class is pure-virtual so any concrete implementation can deal with what it means to be a face as well as names
 * via aliases themselves. View this class as a general framework for all kinds of layouts.
 */
class fcn_layout : protected grid_graph<3>
{
public:
    /**
     * Vertices of FCN layouts are called faces.
     */
    using face = vertex_t;
    /**
     * Same for face_indices.
     */
    using face_index = vertex_index_t;
    /**
     * Alias for a face in ground layer. Since clocking is identical in all layers, third dimension can be neglected.
     */
    using ground = boost::array<std::size_t, 2u>;
    /**
     * Alias for a number that represents delay of a latch face in clock phases (1/n clock cycles).
     */
    using latch_delay = std::size_t;
    /**
     * Standard constructor. Creates an FCN layout by the means of an array determining its size
     * as well as a clocking scheme defining its data flow possibilities.
     *
     * @param lengths 3-dimensional array defining sizes of each dimension (x, y, z) where z - 1 determines
     * the number of crossing layers.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     * @param clocking Clocking scheme defining possible data flow.
     */
    fcn_layout(fcn_dimension_xyz&& lengths, fcn_clocking_scheme&& clocking) noexcept;
    /**
     * Standard constructor. Creates an FCN layout by the means of an array determining its size
     * as well as a clocking scheme defining its data flow possibilities.
     *
     * @param lengths 2-dimensional array defining sizes of dimensions (x, y) where no crossing layer is given.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     * @param clocking Clocking scheme defining possible data flow.
     */
    fcn_layout(fcn_dimension_xy&& lengths, fcn_clocking_scheme&& clocking) noexcept;
    /**
     * Standard constructor. Creates a FCN layout of size 2 x 2 x 2 with the given clocking scheme.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     *
     * @param clocking Clocking scheme defining possible data flow.
     */
    explicit fcn_layout(fcn_clocking_scheme&& clocking) noexcept;
    /**
     * Standard constructor. Creates a FCN layout of size 2 x 2 x 2 with an open clocking scheme.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     */
    fcn_layout() noexcept;
    /**
     * Copy constructor is not available.
     */
    fcn_layout(const fcn_layout& fl) = delete;
    /**
     * Default move constructor.
     */
    fcn_layout(fcn_layout&& fl) = default;
    /**
     * Pure virtual destructor for polymorphism.
     */
    virtual ~fcn_layout() = 0;
    /**
     * Assignment operator is not available.
     */
    fcn_layout& operator=(const fcn_layout& rhs) = delete;
    /**
     * Move operator is not available.
     */
    fcn_layout& operator=(fcn_layout&& rhs) = delete;
    /**
     * Resizes the layout to the dimensions given. The fcn_layout object will not be re-constructed by this
     * function! All maps and other attributes stay untouched.
     *
     * NOTE, that face_index variables might refer to different faces after resizing and that mappings could run out of
     * scope i.e. the faces to which attributes were assigned might not exist in the layout anymore after resizing.
     * Since mappings are done via face positions and not via objects, they can still be accessed but could lead to
     * unwanted behavior.
     *
     * @param lengths 3-dimensional array defining sizes of each dimension (x, y, z) where z - 1 determines the number
     * of crossing layers.
     */
    void resize(fcn_dimension_xyz&& lengths) noexcept;
    /**
     * Resizes the layout to the dimensions given. The fcn_layout object will not be re-constructed by this
     * function! All maps and other attributes stay untouched.
     *
     * NOTE, that face_index variables might refer to different faces after resizing and that mappings could run out of
     * scope i.e. the faces to which attributes were assigned might not exist in the layout anymore after resizing.
     * Since mappings are done via face positions and not via objects, they can still be accessed but could lead to
     * unwanted behavior.
     *
     * @param lengths 2-dimensional array defining sizes of dimensions (x, y) where no crossing layer is given.
     */
    void resize(fcn_dimension_xy&& lengths) noexcept;
    /**
     * Returns the face north of a given face f i.e. the face whose y-dimension is lower by 1.
     * If f's y-dimension is already at minimum, f is returned instead.
     *
     * @param f Face whose north counterpart is desired.
     * @return Face north of f.
     */
    face north(const face& f) const noexcept;
    /**
     * Returns the face east of a given face f i.e. the face whose x-dimension is higher by 1.
     * If f's x-dimension is already at maximum, f is returned instead.
     *
     * @param f Face whose east counterpart is desired.
     * @return Face east of f.
     */
    face east(const face& f) const noexcept;
    /**
     * Returns the face south of a given face f i.e. the face whose y-dimension is higher by 1.
     * If f's y-dimension is already at maximum, f is returned instead.
     *
     * @param f Face whose south counterpart is desired.
     * @return Face south of f.
     */
    face south(const face& f) const noexcept;
    /**
     * Returns the face west of a given face f i.e. the face whose x-dimension is lower by 1.
     * If f's x-dimension is already at minimum, f is returned instead.
     *
     * @param f Face whose west counterpart is desired.
     * @return Face west of f.
     */
    face west(const face& f) const noexcept;
    /**
     * Returns the face above a given face f i.e. the face whose z-dimension is higher by 1.
     * If f's z-dimension is already at maximum, f is returned instead.
     *
     * @param f Face whose above counterpart is desired.
     * @return Face above f.
     */
    face above(const face& f) const noexcept;
    /**
     * Returns the face below a given face f i.e. the face whose z-dimension is lower by 1.
     * If f's z-dimension is already at minimum, f is returned instead.
     *
     * @param f Face whose below counterpart is desired.
     * @return Face below f.
     */
    face below(const face& f) const noexcept;
    /**
     * Returns the face in ground layer below the given one f regardless of its z-dimension.
     *
     * @param f Face whose ground layer counter part is desired.
     * @return Face in ground layer below f.
     */
    ground get_ground(const face& f) const noexcept;
    /**
     * Returns the layout's x-dimension.
     *
     * @return x-dimension.
     */
    std::size_t x() const noexcept;
    /**
     * Returns the layout's y-dimension.
     *
     * @return y-dimension.
     */
    std::size_t y() const noexcept;
    /**
     * Returns the layout's z-dimension.
     *
     * @return z-dimension.
     */
    std::size_t z() const noexcept;
    /**
     * Function alias for get_vertex using perfect forwarding and the name get_by_index to fit naming in fcn_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return get_vertex(args).
     */
    template <typename... ARGS>
    auto get_by_index(ARGS&&... args) const noexcept
    {
        return get_vertex(std::forward<ARGS>(args)...);
    }
    /**
     * Function alias for get_index using perfect forwarding and the name index to fit naming in fcn_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return get_index(args).
     */
    template <typename... ARGS>
    auto index(ARGS&&... args) const noexcept
    {
        return get_index(std::forward<ARGS>(args)...);
    }
    /**
     * Function alias for get_adjacent_vertices using perfect forwarding and the name surrounding_3d to fit naming in
     * fcn_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return get_adjacent_vertices(args).
     */
    template <typename... ARGS>
    auto surrounding_3d(ARGS&&... args) const noexcept
    {
        return get_adjacent_vertices(std::forward<ARGS>(args)...);
    }
    /**
     * Returns a range of all surrounding faces to f in that very layer. For layouts with z == 1, this function
     * is equivalent to surrounding_3d.
     *
     * @param f Face whose surrounding counterparts are desired.
     * @return All faces adjacent to f in the same layer as f.
     */
    auto surrounding_2d(const face& f) const noexcept
    {
        return get_adjacent_vertices(f) | iter::filter([f = f](const face& _f){return f[Z] == _f[Z];});
    }
    /**
     * Returns a range of all faces in ground layer. For layouts with z == 1, this function is equivalent to get_faces.
     *
     * @return All faces in ground layer, i.e. where z-value == 0.
     */
    auto ground_layer() const noexcept
    {
        return get_vertices() | iter::filter([](const face& _f){return _f[Z] == GROUND;});
    }
    /**
     * Returns a range of all faces except for those in ground layer. For layouts with z == 1, this function returns an
     * empty range.
     *
     * @return All faces except for those in ground layer, i.e. where z-value > 0.
     */
    auto crossing_layers() const noexcept
    {
        return get_vertices() | iter::filterfalse([](const face& _f){return _f[Z] == GROUND;});
    }
    /**
     * Returns a range of all faces in nth layer. For layouts with z <= n, this function returns an empty range.
     *
     * @return All faces in nth layer, i.e. where z-value == n.
     */
    auto layer_n(const std::size_t n) const noexcept
    {
        return get_vertices() | iter::filter([n = n](const face& _f){return _f[Z] == n;});
    }
    /**
     * Samples a random face from the layout.
     *
     * @return Randomly sampled face.
     */
    face random_face() const noexcept;
    /**
     * Samples a random face from given layout's layer.
     *
     * @param n Layer from which the face should be sampled.
     * @return Randomly sampled face from layer n.
     */
    face random_face(const std::size_t n) const noexcept;
    /**
     * Function alias for distance with euclidean metric using perfect forwarding and the name euclidean_distance to fit
     * naming in fcn_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return distance<euclidean_metric, double>(args).
     */
    template <typename... ARGS>
    auto euclidean_distance(ARGS&&... args) const noexcept
    {
        return distance<euclidean_metric, double>(std::forward<ARGS>(args)...);
    }
    /**
     * Function alias for distance with manhattan metric using perfect forwarding and the name manhattan_distance to fit
     * naming in fcn_layout.
     *
     * @tparam ARGS Stack of argument types.
     * @param args Stack of arguments.
     * @return distance<manhattan_metric, int>(args).
     */
    template <typename... ARGS>
    auto manhattan_distance(ARGS&&... args) const noexcept
    {
        return distance<manhattan_metric, int>(std::forward<ARGS>(args)...);
    }
    /**
     * Returns area of layout as x * y many faces.
     *
     * @return Area of layout.
     */
    std::size_t area() const noexcept;
    /**
     * Returns the number of clock zones in this layout.
     *
     * @return Number of clock zones given as template argument.
     */
    fcn_clock::number num_clocks() const noexcept;
    /**
     * Returns true iff stored clocking scheme is regular.
     *
     * @return clocking.regular
     */
    bool is_regularly_clocked() const noexcept;
    /**
     * Assigns a clock number c to the given face f. The clock number will only be assigned if the stored clocking
     * scheme is irregular and if c <= clocking.num_clocks hold. Otherwise, this function does nothing.
     *
     * @param f Face to which clock number c should be assigned.
     * @param c Clock number to assign to face f.
     */
    void assign_clocking(const face& f, const fcn_clock::number c) noexcept;
    /**
     * Assigns a clock number c to the given index' face f. The clock number will only be assigned if the stored
     * clocking scheme is irregular and if c <= clocking.num_clocks hold. Otherwise, this function does nothing.
     *
     * @param f Face to which clock number c should be assigned.
     * @param c Clock number to assign to face f.
     */
    void assign_clocking(const face_index f, const fcn_clock::number c) noexcept;
    /**
     * Assigns a latch delay to a given face in the layout. The delay is given in clock phases (i.e. fractions of clock
     * cycles). The latch is virtually added on the clock number (modulo num_clocks()) for information propagation
     * purposes. For every full clock cycle added this way, the signal on this face is stalled and can therefore
     * compensate synchronisation issues. For more information on these kinds of artificial clock latches see
     * "Synchronization of Clocked Field-Coupled Circuits" by Frank Sill Torres, Marcel Walter, Robert Wille, Daniel
     * Große, and Rolf Drechsler in IEEE-NANO 2018.
     *
     * Note that latch delays are bound to the underlying clocking and therefore are valid for all faces with the same
     * Z-dimension value. The function handles this behavior already. That is, a face (x, y, z) is given, it applies the
     * latch to all faces with that x-dimension and y-dimension values without overhead.
     *
     * @param f Face (stack) to which an artificial latch delay should be assigned.
     * @param l Latch in clock phases to be assigned to face (stack) f.
     */
    void assign_latch(const face& f, const latch_delay l) noexcept;
    /**
     * Returns a previously assigned latch delay of the given face f. If no latch delay was previously assigned, 0 is
     * returned as a default value which corresponds to no artificial delay is assumed.
     *
     * @param f Face whose assigned latch delay is desired.
     * @return Assigned latch delay of face f or 0 if no delay was previously assigned.
     */
    latch_delay get_latch(const face& f) const noexcept;
    /**
     * Returns the number of latches in the layout.
     *
     * @return Number of assigned latches.
     */
    auto latch_count() const noexcept
    {
        return l_map.size();
    }
    /**
     * Returns a vector of string representations of the assigned latches. The representation looks similar to
     * "l@{1,2}=2".
     *
     * @return Vector of string representations of the assigned latches.
     */
    std::vector<std::string> latch_str_reprs() const noexcept;

protected:
    /**
     * Clocking scheme representing possible data flow.
     */
    fcn_clocking_scheme clocking;
    /**
     * Alias for a hash map that assigns clock zones to faces. Used for irregular clocking schemes only.
     */
    using clocking_map = std::unordered_map<ground, fcn_clock::zone, boost::hash<ground>>;
    /**
     * Stores a mapping face -> clock number for irregular clocking schemes. Helper functions for access save memory.
     */
    clocking_map c_map{};
    /**
     * Alias for a hash map that assigns a certain delay to a face.
     */
    using latch_map = std::unordered_map<ground, latch_delay, boost::hash<ground>>;
    /**
     * Stores a mapping face -> latch_delay. Helper functions for access save memory.
     * The idea of latch elements is based on the paper "Synchronization of Clocked Field-Coupled Circuits" by
     * Frank Sill Torres, Marcel Walter, Robert Wille, Daniel Große, and Rolf Drechsler in IEEE-NANO 2018.
     * A latch in this case represents a delay of a single face in l clock phases and thereby holds data.
     * Let l == 0 represent the standard case of no artificial delay and therefore normal signal propagation. A face
     * with clock number of 2 and l == 1 for instance, accepts data from faces with clock number of 1 as usual but holds
     * data for 1 phase longer so that it feeds information to faces with clock number 4 instead of 3. It is doubtful
     * that it is technically realizable to hold data for only 1 to 3 phases longer. What is possible however is to hold
     * data for 1 full cycle plus l phases longer. If one want to realize a hold of 2 extra phases, it can be achieved
     * by adding a latch with l == 6 (4 + 2) to hold data for one cycle and 2 phases.
     */
    latch_map l_map{};

    /**
     * This class is a proxy only needed for operator[][][] overloading on fcn_layouts. Since the layouts are
     * structured in a way that (x, y, z) represents coordinates order, operator[] on layouts substitutes the
     * x-coordinate first. This means, only a slice of dimension y and z with fixed x-value remains.
     */
    class yz_slice
    {
    public:
        /**
         * Default constructor is not available.
         */
        yz_slice() = delete;
        /**
         * Standard constructor gets a x-dimension as well as a smart pointer to the actual fcn_layout it was
         * called on. This is necessary to actually address the slices.
         *
         * @param x x-dimension value of the calling operator[].
         * @param ptr Pointer to the layout this slice belongs to.
         */
        yz_slice(const std::size_t x, const fcn_layout* const ptr) noexcept;
        /**
         * Copy constructor is not available.
         */
        yz_slice(const yz_slice& yzs) = delete;
        /**
          * Default move constructor.
          */
        yz_slice(yz_slice&& yzs) = default;
        /**
         * Default destructor.
         */
        ~yz_slice() = default;
        /**
          * Assignment operator is not available.
          */
        yz_slice& operator=(const yz_slice& rhs) = delete;
        /**
          * Move assignment operator is not available.
          */
        yz_slice& operator=(yz_slice&& rhs) = delete;

    private:
        /**
         * Stores the x-dimension value of the calling operator[] in the fcn_layout.
         */
        const std::size_t x_value;
        /**
         * Pointer to the layout this slice belongs to.
         */
        const fcn_layout* const fgl = nullptr;
        /**
         * Another proxy for operator[][] overloading on yz_slice working by the same mechanism. When invoking
         * operator[] on a yz_slice, the y-dimension becomes specified which means that only a "stack" of
         * faces in the z-dimension remains. This class also supports implicit conversion to a face by
         * assuming that if only two operator[]s are called on the layout, the caller may want to have a
         * face in ground level. So implicit conversion returns (x, y, 0).
         */
        class z_stack
        {
        public:
            /**
             * Default constructor is not available.
             */
            z_stack() = delete;
            /**
             * Standard constructor gets a x- and y-dimension together with a smart pointer to the actual
             * fcn_layout it was called on. This is necessary to actually address the stacks.
             *
             * @param x x-dimension value of the calling operator[].
             * @param y y-dimension value of the calling operator[].
             * @param ptr Pointer to the layout this stack belongs to.
             */
            z_stack(const std::size_t x, const std::size_t y, const fcn_layout* const ptr) noexcept;
            /**
             * Copy constructor is not available.
             */
            z_stack(const z_stack& yzs) = delete;
            /**
             * Default move constructor.
             */
            z_stack(z_stack&& yzs) = default;
            /**
             * Default destructor.
             */
            ~z_stack() = default;
            /**
             * Assignment operator is not available.
             */
            z_stack& operator=(const z_stack& rhs) = delete;
            /**
             * Move assignment operator is not available.
             */
            z_stack& operator=(z_stack&& rhs) = delete;
            /**
             * Overload of operator[] to access faces in the layout. This last proxy stage returns the actual face at
             * position (x, y, z) or boost::none if it is located outside of the layout boundaries.
             *
             * @param z z-dimension of the desired face.
             * @return Face at position (x, y, z) or boost::none if it is located outside of layout's boundaries.
             */
            face operator[](const std::size_t z);
            /**
             * Overloaded operator for implicit type conversion. If only two operator[] calls are performed on the
             * layout object, it is assumed that the desired face is located in ground layer. So a call of layout[x][y]
             * returns the face at position (x, y, 0). This behavior is realized by the implicit type conversion
             * conducted by this function. Note that a std::out_of_range will be thrown if (x, y, 0) is located out of
             * the layout's boundaries.
             *
             * @return Face at position (x, y, 0).
             */
            explicit operator face();

        private:
            /**
             * Stores the x-dimension value of the calling operator[] in the fcn_layout.
             */
            const std::size_t x_value;
            /**
             * Stores the y-dimension value of the calling operator[] in the yz_slice.
             */
            const std::size_t y_value;
            /**
             * Pointer to the layout this stack belongs to.
             */
            const fcn_layout* const fgl = nullptr;
        };

    public:
        /**
         * Overload of operator[] to access faces in the layout. This first proxy stage returns a z_stack proxy object
         * onto which the third and final call of operator[] can be performed to finally return the actual face.
         *
         * @param y y-dimension of the desired face.
         * @return A proxy object namely a z_stack onto which consecutive calls of operator[] can be performed.
         */
        z_stack operator[](const std::size_t y) const;
    };

public:
    /**
     * Overload of operator[] to access faces in the layout. Several proxy objects are needed in order to implement
     * the desired functionality properly. A call to a layout object should look like this: layout[x][y][z]. So first
     * call returns a yz_slice onto which second call is conducted which returns a z_stack onto which the third call is
     * executed which then returns the actual face. In case (x, y, z) lay outside the layout bounds, an
     * std::out_of_range object is thrown. Also layout[x][y] can be called to refer to the face at position (x, y, 0)
     * i.e. a face in ground layer. This works by overloading operator face() on z_stack. Sometimes a static_cast<face>
     * is needed for this to work.
     *
     * @param x x-dimension of the desired face.
     * @return A proxy object namely a yz_slice onto which consecutive calls of operator[] can be performed.
     */
    yz_slice operator[](const std::size_t x) const;
    /**
     * Overload for operator(x, y, z) to access faces in the layout. No proxy objects are needed so this non-array-like
     * implementation has a performance advantage over operator[][][] and should be preferred. The other one just exists
     * for intuition reasons.
     *
     * @param x x-dimension of the desired face.
     * @param y y-dimension of the desired face.
     * @param z z-dimension of the desired face.
     * @return face at position (x, y, z).
     */
    face operator()(const std::size_t x, const std::size_t y, const std::size_t z = GROUND) const;
};


#endif //FICTION_FCN_LAYOUT_H
