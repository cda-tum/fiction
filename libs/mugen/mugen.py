try:
	from graphviz import Digraph
except ModuleNotFoundError:
	pass
import itertools
from math import log2
from pysat.solvers import Glucose3
from pysat.card import *
import sys
import subprocess
import os
import tempfile
import wrapt_timeout_decorator

class SynthesisException(Exception):
    '''
    Mugen's generic exception class that is thrown whenever something
    unexpected happens during synthesis.
    '''

    def __init__(self, message):
        '''
        :param message: The message to show when the exception is thrown.
        '''
        self.message = message

CARDINAL_DIRECTIONS = set(['NORTH', 'EAST', 'SOUTH', 'WEST'])

OPPOSITE_DIRECTION = {
    'NORTH': 'SOUTH',
    'EAST' :  'WEST',
    'SOUTH': 'NORTH',
    'WEST' :  'EAST'
}

# Mapping gate types to the corresponding number of fanins.
GATE_FANIN_RANGE = {
    'NOT':   1,
    'AND':   2,
    'OR':    2,
    'MAJ':   3,
    'WIRE':  1,
    'EMPTY': 0,
    'CROSS': 2
}

# Some utility functions.
def is_north(coords1, coords2):
    '''
    Returns true if and only if coords1 lies to the north of coords2.  We say
    this is the case if they have the same horizontal position but coords1 has
    a lower vertical position.
    '''
    return coords1[0] == coords2[0] and coords1[1] < coords2[1]

def is_east(coords1, coords2):
    '''
    Returns true if and only if coords1 lies to the east of coords2.  We say
    this is the case if they have the same vertical position but coords1 has a
    higher horizontal position.
    '''
    return coords1[0] > coords2[0] and coords1[1] == coords2[1]

def is_south(coords1, coords2):
    '''
    Returns true if and only if coords1 lies to the south of coords2.  We say
    this is the case if they have the same horizontal position but coords1 has
    a higher vertical position.
    '''
    return coords1[0] == coords2[0] and coords1[1] > coords2[1]

def is_west(coords1, coords2):
    '''
    Returns true if and only if coords1 lies to the west of coords2.  We say
    this is the case if they have the same vertical position but coords1 has
    a lower horizontal position.
    '''
    return coords1[0] < coords2[0] and coords1[1] == coords2[1]

def get_direction(coords1, coords2):
    '''
    Given a pair of coordinates, determines in which cardinal
    direction the information flows. Returns the result as a pair
    (d1, d2), where d1 is the direction in which the signal leaves
    from coords1 and d2 is the direction in which it arrives at
    coords2.
    '''
    if is_north(coords1, coords2):
        return ('SOUTH', 'NORTH')
    elif is_east(coords1, coords2):
        return ('WEST', 'EAST')
    elif is_south(coords1, coords2):
        return ('NORTH', 'SOUTH')
    elif is_west(coords1, coords2):
        return ('EAST', 'WEST')
    else:
        raise SynthesisException('Unknown direction')

def get_coords_in_direction(coords, direction):
    '''
    Given a set of coordinates and a cardinal direction. Returns the
    coordinates of a node immediately adjacent to the current one in
    the given direction. Note that this may result in a set of
    coordinates that are not within the bounds of a clocking scheme
    (e.g. this may result in negative coordinates).
    '''
    if direction == 'NORTH':
        return (coords[0], coords[1] - 1)
    elif direction == 'EAST':
        return (coords[0] + 1, coords[1])
    elif direction == 'SOUTH':
        return (coords[0], coords[1] + 1)
    elif direction == 'WEST':
        return (coords[0] - 1, coords[1])
    else:
        raise SynthesisException("Unknown cardinal direction: '{}'".format(direction))

def eval_gate(gate_type, inputs):
    '''
    Evaluates a certain gate type on a list of binary input values. Returns the
    result.

    :param gate_type: The type of gate to evaluate. Choices are EMPTY, WIRE, NOT, AND, OR, and MAJ.
    :param inputs: List of input values.
    '''
    if gate_type == 'EMPTY':
        return 0
    elif gate_type == 'WIRE':
        return inputs[0]
    elif gate_type == 'NOT':
        return 1 - inputs[0]
    elif gate_type == 'AND':
        return inputs[0] & inputs[1]
    elif gate_type == 'OR':
        return inputs[0] | inputs[1]
    elif gate_type == 'MAJ':
        return (inputs[0] & inputs[1]) | (inputs[0] & inputs[2]) | (inputs[1] & inputs[2])
    else:
        raise SynthesisException("No evaluation support for gate type '{}'".format(gate_type))

class node:
    '''
    A generic node class, used by both clocking scheme graphs
    and logic networks.

    :ivar coords: The grid coordinates of the node in the clocking scheme.
    :ivar is_pi: Is the node a primary input.
    :ivar is_po: Is the node a primary output.
    :ivar fanin: A list of nodes containing the fanin of the current node.
    :ivar fanout: A list of nodes containing the fanin of the current node.
    :ivar is_border_node: This Boolean flag is True iff the node lies on the border of the clocking scheme.
    :ivar gate_type: The gate type of the node.
    :ivar dir_map: This optional attribute is set only for nodes with gate_type CROSS. It is a dictionary which maps fanin directions to output directions. For example, if dir_map = { 'WEST': 'NORTH', 'EAST': 'SOUTH'}, then the western fanin is mapped to the northern fanout port and the eastern fanin is mapped to the southern fanout port.
    '''

    def __init__(self, *, coords=None, is_pi=False, is_po=False):
        '''
        Creates a new logic node.

        :param coords: the grid coordinates of the node in the clocking scheme.
        :param is_pi: is the node a primary input.
        :param is_po: is the node a primary output.

        '''
        self.coords = coords
        self.is_pi = is_pi
        self.virtual_fanin = []
        self.virtual_fanout = []
        self.is_border_node = False
        self.is_po = is_po
        self.fanin = {}
        self.fanout = {}
        self.gate_type = None
        # self.dir_map = {}

    def set_fanin(self, in_dir, innode, out_dir):
        '''
        Sets the fanin port at direction d of this node to innode and
        updates the fanout of innode by appending this node to it.
        '''
        self.fanin[in_dir] = innode
        innode.fanout[out_dir] = self

    def __repr__(self):
        if self.is_pi:
            return 'PI{}'.format(self.coords)
        else:
            return '<node {}>'.format(self.coords)
        
    def __lt__(self, other): 
        return self.coords < other.coords

class logic_network:

    '''
    A logic_network is the result of synthesis. Its design is similar
    to a clocking scheme graph. A major difference is that it cannot
    contain cycles. However, it can also be accessed using the same
    tile coordinate based API.

    :ivar shape: a 2-tuple containing the size of the grid
           (width x height).
    :ivar nr_pis: number of PIs.
    :ivar nr_pos: number of POs.
    :ivar nodes: A list of all the nodes in the logic network, including the PIs.
    :ivar node_map: A map from tile coordinates to nodes nodes in the logic network. E.g. to access the node corresponding to tile (0,0) one refers to node_map[(0,0)].
    :ivar po_map: A list of size nr_pos mapping output indices to nodes in the network. E.g.  to access the first output, one refers to po_map[0].
    '''
    
    def __init__(self, shape, nr_pis, nr_pos):
        '''
        Creates a new logic network.

        :param shape: a 2-tuple containing the size of the grid
               (width x height).
        :param nr_pis: number of PIs.
        :param nr_pos: number of POs.
        '''


        self.shape = shape
        self.nr_pis = nr_pis
        self.nr_pos = nr_pos
        
        self.nodes = [node(coords=i, is_pi=True) for i in range(nr_pis)] 
        self.node_map = {}
        for y in range(shape[1]):
            for x in range(shape[0]):
                n = node(coords=(x, y))
                if x == 0 or x == (shape[0] - 1) or y == 0 or y == (shape[1] - 1):
                    n.is_border_node = True
                self.nodes.append(n)
                self.node_map[(x,y)] = n

        self.po_map = [None] * nr_pos

    def set_output(self, h, coords, d):
        '''
        Marks the output port in direction d for the node at coords as the
        h-th output of the network.
        '''
        n = self.node_map[coords]
        n.fanout[d] = 'PO{}'.format(h)
        n.is_po = True
        self.po_map[h] = (n, d)

    def __repr__(self):
        r = '\n'
        for n in self.nodes:
            if n.is_pi:
                continue
            r += '<node {}, type={}'.format(n.coords, n.gate_type)
            if 'NORTH' in n.fanin:
                r += ', fanin[NORTH]={}'.format(n.fanin['NORTH'])
            if 'EAST' in n.fanin:
                r += ', fanin[EAST]={}'.format(n.fanin['EAST'])
            if 'SOUTH' in n.fanin:
                r += ', fanin[SOUTH]={}'.format(n.fanin['SOUTH'])
            if 'WEST' in n.fanin:
                r += ', fanin[WEST]={}'.format(n.fanin['WEST'])
            if n.gate_type == 'CROSS':
                r += ', dir_map={}'.format(n.dir_map)
            r += '>\n'
        for h in range(self.nr_pos):
            r += 'PO{}={}\n'.format(h, self.po_map[h])
        return r

    def has_border_io(self):
        '''
        Checks if only border nodes are connected to PIs and POs. Returns
        True if this is the case and False otherwise.
        '''
        for n in self.nodes:
            if n.is_pi:
                continue
            if not n.is_border_node:
                for innode in n.fanin.values():
                    if innode.is_pi:
                        return False
        for (n, d) in self.po_map:
            if not n.is_border_node:
                return False
        return True

    def has_designated_pi(self):
        '''
        Checks if only WIREs are connected to PIs. Moreover, verifies that
        those designated PI WIREs have only one fanout. Returns True
        of this is the case and False otherwise.
        '''
        for n in self.nodes:
            if n.is_pi:
                continue
            if n.gate_type != 'WIRE':
                for innode in n.fanin.values():
                    if innode.is_pi:
                        return False
            else:
                nr_fanout = len(n.fanout)
                for innode in n.fanin.values():
                    if innode.is_pi and nr_fanout > 1:
                        return False
        return True

    def verify_designated_pi(self):
        '''
        The same as :func:`has_designated_pi` but raises a
        :class:`SynthesisException` if the spec is not met.
        '''
        for n in self.nodes:
            if n.is_pi:
                continue
            if n.gate_type != 'WIRE':
                for innode in n.fanin.values():
                    if innode.is_pi:
                        raise SynthesisException('{} has gate type {} and fanin PI_{}'.format(
                            n.coords, n.gate_type, innode.coords))
            else:
                nr_fanout = len(n.fanout)
                for innode in n.fanin.values():
                    if innode.is_pi and nr_fanout > 1:
                        raise SynthesisException('{} is designated PI WIRE and has multiple fanout')

    def has_designated_po(self):
        '''
        Checks if only WIREs are connected to POs. Moreover, verifies that
        those designated PO WIREs have no other fanout. Returns True
        of this is the case and False otherwise.
        '''
        for (n, d) in self.po_map:
            if n.gate_type != 'WIRE':
                return False
            if len(n.fanout) > 1:
                return False
        return True

    def verify_designated_po(self):
        '''
        The same as :func:`has_designated_po` but raises a
        :class:`SynthesisException` if the spec is not met.
        '''
        for (n, d) in self.po_map:
            if n.gate_type != 'WIRE':
                raise SynthesisException('{} is designated PO but has gate type {}'.format(
                    n.coords, n.gate_type))
            if len(n.fanout) > 1:
                raise SynthesisException('{} is designated PO but has multiple fanout'.format(
                    n.coords))

    def verify_consecutive_not(self):
        ''' 
        Verifies that the network contains no consecutive NOT gates. Raises a
        :class:`SynthesisException` if it does.
        '''
        for n in self.nodes:
            if n.is_pi:
                continue
            if n.gate_type == 'NOT':
                for _, innode in n.fanin.items():
                    if not innode.is_pi and innode.gate_type == 'NOT':
                        raise SynthesisException('{} is NOT gate and has NOT fanin {}'.format(
                            n.coords, innode.coords))
    
    def verify_no_crossing_io(self):
        '''
        Verifies that the network contains no crossings that are directly
        connected to I/O pins. Raises a :class:`SynthesisException` if it does.
        '''
        for n in self.nodes:
            if n.is_pi:
                continue
            if n.gate_type == 'CROSS':
                if n.is_po:
                    raise SynthesisException('{} is CROSS so cannot be PO'.format(n.coords))
                for _, innode in n.fanin.items():
                    if innode.is_pi:
                        raise SynthesisException('{} is CROSS so cannot have PI fanin {}'.format(
                            n.coords, innode.coords))

    def to_png(self, filename):
        '''
        Creates a PNG of the logic network using Graphviz.
        In the resulting PNG, all border nodes are filled in
        with a gray color. All internal nodes are white.
        PO nodes are marked by a double border. Every non-PI
        node is also marked with its tile-space coordinates
        as well as the function it computes.
        '''
        dot = Digraph()
        dot.attr(newrank='true')
        dot.attr(rankdir='TB')
        dot.attr('node', shape='circle')
        dot.attr('node', fixedsize='true')
        dot.attr('node', width='1.1')
        dot.attr('node', height='1.1')

        # Find I/O coords.
        pi_coords = {}
        po_coords = {}
        for n in self.nodes:
            if n.is_pi:
                continue
            for in_dir, innode in n.fanin.items():
                if innode.is_pi:
                    coords = get_coords_in_direction(n.coords, in_dir)
                    pi_coords[coords] = innode
        for h in range(self.nr_pos):
            n, d = self.po_map[h]
            coords = get_coords_in_direction(n.coords, d)
            po_coords[coords] = h

        # Draw nodes in a grid. Make the grid a bit bigger so we can
        # fit the PI nodes on there neatly.
        y_range = (-1, self.shape[1] + 1) # y coordinate range (top value exclusive)
        x_range = (-1, self.shape[0] + 1) # x " " "
        boundary_counter = 0
        coord_names = {}
        for y in range(y_range[0], y_range[1]):
            with dot.subgraph() as s:
                s.attr(rank='same')
                for x in range(x_range[0], x_range[1]):
                    if (x,y) in pi_coords:
                        n = pi_coords[(x,y)]
                        name = 'PI{}'.format(n.coords)
                        label = 'x[{}]'.format(n.coords)
                        s.node(name, label, fillcolor='deepskyblue1', style='filled')
                    elif (x,y) in po_coords:
                        h = po_coords[(x,y)]
                        name = 'PO{}'.format(h)
                        label = 'f[{}]'.format(h)
                        s.node(name, label, fillcolor='coral1', style='filled')
                    elif (x,y) in self.node_map:
                        n = self.node_map[(x, y)]
                        name = 'N_{}_{}'.format(x, y)
                        label = '{}\n{}'.format(n.coords, n.gate_type)
                        fill = 'gray' if n.is_border_node else 'white'
                        s.node(name, label, fillcolor=fill, style='filled')
                    else: # Empty boundary node
                        name = 'B_{}'.format(boundary_counter)
                        boundary_counter += 1
                        s.node(name, name, style='invis')
                    coord_names[(x,y)] = name

        for coord, name in coord_names.items():
            if coord[0] < self.shape[0]:
                dot.edge(name, coord_names[(coord[0]+1, coord[1])], style='invis')
            if coord[1] < self.shape[1]:
                dot.edge(name, coord_names[(coord[0], coord[1]+1)], style='invis')
        
        for n in self.nodes:
            if n.is_pi:
                continue
            name = 'N_{}_{}'.format(n.coords[0], n.coords[1])
            for in_dir, innode in n.fanin.items():
                if innode.is_pi:
                    inname = 'PI{}'.format(innode.coords)
                else:
                    inname = 'N_{}_{}'.format(innode.coords[0], innode.coords[1])
                dot.edge(inname, name,
                         tailport=OPPOSITE_DIRECTION[in_dir][0:1].lower(),
                         headport=in_dir[0:1].lower())

        for h in range(self.nr_pos):
            n, d = self.po_map[h]
            name = 'N_{}_{}'.format(n.coords[0], n.coords[1])
            oname = 'PO{}'.format(h)
            olabel = 'PO{}'.format(h)
            dot.edge(name, oname, tailport=d[0:1].lower(), headport=OPPOSITE_DIRECTION[d][0:1].lower())

        dot.render(filename=filename, format='png', cleanup=True)

    def rec_simulate(self, n, sim_vals, marked_nodes):
        '''
        Recursive helper method for :func:`simulate`.
        '''
        if n.is_pi:
            return
        for innode in n.fanin.values():
            if not innode in marked_nodes:
                self.rec_simulate(innode, sim_vals, marked_nodes)
        marked_nodes.add(n)
        for out_dir in n.fanout.keys():
            if n.gate_type == 'EMPTY':
                # Empty gates are not referred to by anything.
                continue
            invals = []
            for in_dir, innode in n.fanin.items():
                if innode.is_pi:
                    invals.append(sim_vals[innode][None])
                else:
                    invals.append(sim_vals[innode][OPPOSITE_DIRECTION[in_dir]])
                        
            if n.gate_type == 'WIRE':
                # Only one fanin, retrieve its sim_val and copy it.
                sim_vals[n][out_dir] = invals[0]
            elif n.gate_type == 'NOT':
                # Only one fanin, retrieve its sim_val and negate it.
                sim_vals[n][out_dir] = 1 - invals[0]
            elif n.gate_type == 'AND':
                sim_vals[n][out_dir] = invals[0] & invals[1]
            elif n.gate_type == 'OR':
                sim_vals[n][out_dir] = invals[0] | invals[1]
            elif n.gate_type == 'MAJ':
                sim_vals[n][out_dir] = eval_gate('MAJ', invals)
            elif n.gate_type == 'CROSS':
                # Copy input to direction described in dir_map
                for indir, outdir in n.dir_map.items():
                    if outdir == out_dir:
                        if n.fanin[indir].is_pi:
                            sim_vals[n][out_dir] = sim_vals[n.fanin[indir][None]]
                        else:
                            sim_vals[n][out_dir] = sim_vals[n.fanin[indir]][OPPOSITE_DIRECTION[indir]]
            else:
                raise SynthesisException("Unknown gate type '{}' in simulation".format(n.gate_type))

    def simulate(self):
        '''
        Simulates the logic network and returns a list which contains the
        simulated function for each output.
        '''
        sim_tt = [[0] * (2 ** self.nr_pis) for i in range(self.nr_pos)]
        sim_idx = 0
        for input_pattern in itertools.product('01', repeat=self.nr_pis):
            # Reverse input pattern, since our PI ordering is the
            # inverse of itertools.product.
            input_pattern = input_pattern[::-1]
            sim_vals = {}
            marked_nodes = set()
            for n in self.nodes:
                sim_vals[n] = {}
            for i in range(self.nr_pis):
                n = self.nodes[i]
                sim_vals[n][None] = int(input_pattern[i])
            for i in range(self.nr_pos):
                n, d = self.po_map[i]
                self.rec_simulate(n, sim_vals, marked_nodes)
                sim_tt[i][sim_idx] = sim_vals[n][d]
            sim_idx += 1
        if self.nr_pis == 2:
            for i in range(self.nr_pos):
                sim_tt[i] = sim_tt[i] + sim_tt[i]
        return sim_tt

class scheme_graph:
    '''
    A scheme_graph (short for clocking-scheme graph) is used
    to specify a clocking scheme and to synthesize logic
    networks according to that specification.
    '''

    def __init__(self, *, shape=(1,1),
                 enable_wire=True, enable_not=True, enable_and=True,
                 enable_or=True, enable_maj=True, enable_crossings=True, 
                 designated_pi=False, designated_po=False, nr_threads=1,
                 timeout=0):
        '''
        Creates a new clocking scheme graph.

        :param shape: A 2-tuple specifying the dimensions of the clocking scheme.
        :param enable_not: Enable synthesis of WIREs.
        :param enable_not: Enable synthesis of NOT gates.
        :param enable_and: Enable synthesis of AND gates.
        :param enable_or: Enable synthesis of OR gates.
        :param enable_maj: Enable synthesis of MAJ gates.
        :param enable_crossings: Enable wire crossings.
        :param designated_pi: True iff only WIRES can have PI fanin.
        :param designated_po: True iff only WIRES can have PO fanout.
        :param nr_threads: How many threads to use in parallel solving.
        :param timeout: the timeout for the synthesize call (in seconds)

        '''
        self.shape = shape
        self.node_map = {}
        for y in range(shape[1]):
            for x in range(shape[0]):
                n = node(coords=(x, y))
                if x == 0 or x == (shape[0] - 1) or y == 0 or y == (shape[1] - 1):
                    n.is_border_node = True
                self.node_map[(x,y)] = n

        self.enable_wire = enable_wire
        self.enable_not = enable_not
        self.enable_and = enable_and
        self.enable_or = enable_or
        self.enable_maj = enable_maj
        self.enable_crossings = enable_crossings
        self.designated_pi = designated_pi
        self.designated_po = designated_po
        self.nr_threads = nr_threads
        self.model = None
        self.timeout = timeout

    def add_virtual_edge(self, coords1, coords2):
        '''
        Adds a virtual edge from the node corresponding to the tile at
        coords1 to the node corresponding to the tile at coords2.
        A virtual edge specifies that the node at coords2 may have
        the node at coords1 in its fanin. However, it does not force
        this to happen. Hence, the connection is virtual and may be
        actualized by the synthesis process.
        '''
        node1 = self.node_map[coords1]
        node2 = self.node_map[coords2]
        node1.virtual_fanout.append(node2)
        node2.virtual_fanin.append(node1)

    def _dfs_find_cycles(self, cycles, start, n, path):
        if n in path:
            if n == start:
                cycles.append([n] + path)
            return
        for innode in n.virtual_fanin:
            self._dfs_find_cycles(self, cycles, start, innode, [n] + path)

    def find_cycles(self):
        '''
        Examines the clocking scheme graph and finds any cycles it may
        contain.
        '''
        cycles = []
        for n in self.node_map.values():
            for innode in n.virtual_fanin:
                self._dfs_find_cycles(self, cycles, n, innode, [n])
        return cycles

    def to_png(self, filename):
        '''
        Creates a PNG of the graph underlying the clock scheme 
        using Graphviz.
        '''
        dot = Digraph()
        dot.attr('node', shape='box')
        dot.attr(splines='ortho')

        for y in range(self.shape[1]):
            with dot.subgraph() as s:
                s.attr(rank='same')
                for x in range(self.shape[0]):
                    n = self.node_map[(x, y)]
                    name = 'N_{}_{}'.format(x, y)
                    label = str((x, y))
                    s.node(name, label)
                    if x > 0:
                        prevname = 'N_{}_{}'.format(x-1, y)
                        s.edge(prevname, name, style='invis')


        for y in range(self.shape[1]):
            for x in range(self.shape[0]):
                n1 = self.node_map[(x, y)]
                n1name = 'N_{}_{}'.format(x, y)
                for n2 in n1.virtual_fanout:
                    n2name = 'N_{}_{}'.format(n2.coords[0], n2.coords[1])
                    dot.edge(n1name, n2name)

        dot.render(filename=filename, format='png', cleanup=True)

    def satisfies_spec(self, net, functions):
        '''
        Verifies that a network satisfies the specifications represented by
        this scheme_graph object. Raises a :class:`SynthesisException` if this
        is not the case.
        '''
        # Make sure PIs do not have more than one fanout.
        for n in net.nodes:
            if not n.is_pi:
                continue
            if len(n.fanout) > 1:
                raise SynthesisException('PI_{} has more than one fanin'.format(n.coords))
        if not self.enable_wire:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'WIRE':
                    raise SynthesisException('{} has type WIRE'.format(n.coords))
        if not self.enable_not:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'NOT':
                    raise SynthesisException('{} has type NOT'.format(n.coords))
        if not self.enable_and:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'AND':
                    raise SynthesisException('{} has type AND'.format(n.coords))
        if not self.enable_or:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'OR':
                    raise SynthesisException('{} has type OR'.format(n.coords))
        if not self.enable_maj:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'MAJ':
                    raise SynthesisException('{} has type MAJ'.format(n.coords))
        if not self.enable_crossings:
            for n in net.nodes:
                if not n.is_pi and n.gate_type == 'CROSS':
                    raise SynthesisException('{} has type CROSS'.format(n.coords))
        if not net.has_border_io():
            raise SynthesisException('Net does not have border I/O')
        if self.designated_pi:
            net.verify_designated_pi()
        if self.designated_po:
            net.verify_designated_po()
        net.verify_consecutive_not()
        net.verify_no_crossing_io()
        sim_tts = net.simulate()
        for i in range(len(functions)):
            if functions[i] != sim_tts[i]:
                raise SynthesisException('Specified f[{}] = {}, net out[{}] = {}'.format(
                    i, functions[i], i, sim_tts[i]))

    def _discover_connectivity(self, n, pi_fanin_options):
        # Check which directions support fanouts.
        fanout_directions = set()
        for outnode in n.virtual_fanout:
            outdir, indir = get_direction(n.coords, outnode.coords)
            fanout_directions.add(outdir)
        # Check which directions support fanins.
        fanin_directions = set()
        for innode in n.virtual_fanin:
            outdir, indir = get_direction(innode.coords, n.coords)
            fanin_directions.add(indir)

        # Border nodes may have PI/PO fanins/fanouts coming from any
        # directions that are not used by virtual fanin or fanout.
        io_directions = CARDINAL_DIRECTIONS.difference(fanout_directions).difference(fanin_directions)
        if n.is_border_node and len(io_directions) == 0:
            raise SynthesisException('Unexpected I/O state at border node')
        elif not n.is_border_node and len(io_directions) > 0:
            raise SynthesisException('Unexpected I/O state at internal node')
        # Add I/O directions to potential fanin/fanout directions.
        for direction in io_directions:
            fanout_directions.add(direction)
            fanin_directions.add(direction)

        fanin_options = {}
        for d in io_directions:
            fanin_options[d] = [(pi, None) for pi in pi_fanin_options]
        for innode in n.virtual_fanin:
            outdir, indir = get_direction(innode.coords, n.coords)
            assert(indir not in fanin_options)
            fanin_options[indir] = [(innode, outdir)]
        n.fanout_directions = fanout_directions
        n.fanin_directions = fanin_directions
        n.fanin_options = fanin_options
        n.io_directions = io_directions

    def synthesize(self, functions, verbosity=0):
        ''' 
        Synthesizes the given list of functions. Returns an iterator of
        :class:`logic_network` objects, so the caller may iterate on this
        method to synthesize all networks that satisfy the specifications given
        by the clocking scheme and the functions. 

        :param functions: A list of lists of binary integers. Every list is a function to be synthesized. Every list is to be computed by the resulting logic network and corresponds to one of its outputs. The n-th list corresponds to the n-th logic network output.
        :param verbosity: Parameter to view debugging output.
        '''

        @wrapt_timeout_decorator.timeout(self.timeout)
        def timeout_call(self, functions, verbosity):
            for net in self._synthesize(self, functions, verbosity):
                return net

        if self.timeout <= 0:
            for net in self._synthesize(self, functions, verbosity):
                yield net
        else:
            net = timeout_call(self, functions, verbosity)
            yield net

    def _synthesize(self, functions, verbosity):
        '''
        Synthesizes a logic network according to the clocking scheme
        specifications encoded in the graph and the functional
        specification encoded by the truth tables in the functions
        list.
        
        NOTE: this function may be called multiple times, which
        will result in it generating zero or more logic networks.
        '''
        assert(len(functions) > 0)
        assert(log2(len(functions[0])).is_integer())

        self.nr_pis = round(log2(len(functions[0])))
        self.nr_pos = len(functions)
        var_idx = 1
        
        self.nodes = [node(coords=i,is_pi=True) for i in range(self.nr_pis)] 
        for y in range(self.shape[1]):
            for x in range(self.shape[0]):
                self.nodes.append(self.node_map[(x,y)])

        legend = {}

        # Pre-process specified functions, make sure truth tables are
        # at least size 8.
        if len(functions[0]) == 4:
            for i in range(len(functions)):
                functions[i] = functions[i] + functions[i]

        # Determine the possible local connectivity options for each node
        pi_fanin_options = [self.nodes[x] for x in range(self.nr_pis)]
        for n in self.nodes:
            if n.is_pi:
                continue
            self._discover_connectivity(self, n, pi_fanin_options)

        # Determine what gate types are supported by each tile node.
        for n in self.nodes:
            if n.is_pi:
                continue
            enabled_gates = ['EMPTY']
            if self.enable_wire:
                enabled_gates.append('WIRE')
            if self.enable_not:
                enabled_gates.append('NOT')
            if self.enable_and:
                enabled_gates.append('AND')
            if self.enable_or:
                enabled_gates.append('OR')
            if self.enable_maj and len(n.fanin_options) > 2:
                # assert(self.nr_pis > 2)
                enabled_gates.append('MAJ')
            if self.enable_crossings:
                if not n.is_border_node and (len(n.virtual_fanin) == 2):
                    enabled_gates.append('CROSS')
            n.enabled_gate_types = enabled_gates
            
        # Based on the enabled gates we can determine the simulation
        # variables and the gate type variables.
        nr_local_sim_vars = len(functions[0])
        for n in self.nodes:
            sim_vars = {}
            if n.is_pi:
                varlist = [0] * nr_local_sim_vars
                for i in range(nr_local_sim_vars):
                    varlist[i] = var_idx
                    legend[var_idx] = 'sim_vars[PI{}][None][{}]'.format(n.coords, i)
                    var_idx += 1
                sim_vars[None] = varlist
            else:
                for d in n.fanout_directions:
                    varlist = [0] * nr_local_sim_vars
                    for i in range(nr_local_sim_vars):
                        varlist[i] = var_idx
                        legend[var_idx] = 'sim_var[{}][d][{}]'.format(n.coords, i)
                        var_idx += 1
                    sim_vars[d] = varlist
            n.sim_vars = sim_vars

        for n in self.nodes:
            if n.is_pi:
                continue
            gate_type_vars = []
            gate_type_map = {}
            for t in n.enabled_gate_types:
                gate_type_vars.append(var_idx)
                gate_type_map[t] = var_idx
                legend[var_idx] = 'gate {} has type {}'.format(n.coords, t)
                var_idx += 1
            n.gate_type_vars = gate_type_vars
            n.gate_type_map = gate_type_map

        # Based on the local connectivity and gate types we can
        # determine the selection variables.
        for n in self.nodes:
            # Keeps track of all variables that select this node as
            # fanin.
            n.ref_vars = []
            # Maps ref vars to the node that refers to n.
            n.ref_var_map = {}
            # Maps fanout directions to the selection variables that
            # refer to this node.
            n.ref_var_direction_map = {}
            if n.is_pi:
                n.ref_var_direction_map[None] = []
            else:
                for direction in n.fanout_directions:
                    n.ref_var_direction_map[direction] = []
        for n in self.nodes:
            if n.is_pi:
                continue
            svar_map = {}
            # dir_map tracks to which output direction an fanin gets mapped.
            dir_map = {}
            # Track all selection variables for a given fanin
            # direction.
            svar_direction_map = {} 
            for direction in n.fanin_directions:
                svar_direction_map[direction] = []
            svars = []
            fanin_size_options = set([GATE_FANIN_RANGE[gate] for gate in n.enabled_gate_types])
            for size_option in fanin_size_options:
                if size_option == 0:
                    # Handle 0 as a special case where this node
                    # corresponds to an empty tile.
                    continue
                # Select all possible combinations of size_option fanins.
                svar_map[size_option] = {}
                dir_list = list(n.fanin_options.keys())
                for directions in itertools.combinations(dir_list, size_option):
                    dir_opt_list = []
                    for d in directions:
                        dir_opt_list.append([(d, o) for o in n.fanin_options[d]])
                    fanin_combinations = itertools.product(*dir_opt_list)
# Warning: enabling the print statement below iterates over fanin_combinations, rendering the for loop useless!
#                    print('{} fanin combinations: {}'.format(n.coords, list(fanin_combinations)))
                    for comb in fanin_combinations:
                        # Filter out redundant combinations.
                        if size_option == 2 and comb[0][1][0] == comb[1][1][0]:
                            continue
                        elif size_option == 3 and (
                                (comb[0][1][0] == comb[1][1][0]) or
                                (comb[0][1][0] == comb[2][1][0]) or
                                (comb[1][1][0] == comb[2][1][0])):
                            continue
                        # If designated PIs are enabled, we don't want
                        # gates with more than 1 fanin referring to
                        # PIs.
                        if self.designated_pi and size_option == 2 and (
                                comb[0][1][0].is_pi or comb[1][1][0].is_pi):
                            continue
                        elif self.designated_pi and size_option == 3 and (
                                comb[0][1][0].is_pi or comb[1][1][0].is_pi or comb[2][1][0].is_pi):
                            continue
                        
                        if not self.enable_crossings or size_option != 2:
                            svar_map[size_option][var_idx] = comb
                            legend[var_idx] = '{} has fanin {}'.format(n.coords, comb)
                            svars.append(var_idx)
    #                        print(comb)
                            for direction, option in comb:
                                # option[0] is the node, option[1] is the
                                # output port direction.
                                option[0].ref_vars.append(var_idx)
                                option[0].ref_var_direction_map[option[1]].append(var_idx)
                                option[0].ref_var_map[var_idx] = n
                                svar_direction_map[direction].append(var_idx)
                            var_idx += 1
                        else:
                            # If we have crossings, we must create a separate
                            # selection variable for every possible mapping
                            # from the input directions to the output
                            # directions.
                            input_directions = set([comb[0][0], comb[1][0]])
                            output_directions = list(CARDINAL_DIRECTIONS.difference(input_directions))
                            for i in range(2):
                                svar_map[size_option][var_idx] = comb
                                dir_map[var_idx] = { comb[0][0] : output_directions[i], comb[1][0] : output_directions[1 - i] }
                                legend[var_idx] = '{} has fanin {}'.format(n.coords, comb)
                                svars.append(var_idx)
                                for direction, option in comb:
                                    # option[0] is the node, option[1] is the
                                    # output port direction.
                                    option[0].ref_vars.append(var_idx)
                                    option[0].ref_var_direction_map[option[1]].append(var_idx)
                                    option[0].ref_var_map[var_idx] = n
                                    svar_direction_map[direction].append(var_idx)
                                var_idx += 1
                            
            n.svar_map = svar_map
            n.dir_map = dir_map
            n.svar_direction_map = svar_direction_map
            n.svars = svars

        # Create the output variables.
        nr_outputs = len(functions)
        out_vars = {}
        for h in range(nr_outputs):
            houtvars = {}
            for n in self.nodes:
                if n.is_pi or not n.is_border_node:
                    continue
                for direction in n.io_directions:
                    houtvars[var_idx] = (n, direction)
                    n.ref_var_direction_map[direction].append(var_idx)
                    n.ref_vars.append(var_idx)
                    legend[var_idx] = 'PO_{} points to ({}, {})'.format(h, n.coords, direction)
                    var_idx += 1
            out_vars[h] = houtvars


        '''
        print('{}.enabled_gate_types: {}'.format((0,0), self.node_map[(0,0)].enabled_gate_types))
        print('{}.svar_map: {}'.format((0,0), self.node_map[(0,0)].svar_map))
        print('{}.svar_direction_map: {}'.format((0,0), self.node_map[(0,0)].svar_direction_map))
        print('{}.svars = {}'.format((0,0), self.node_map[(0,0)].svars))
        print('{}.ref_var_map = {}'.format((0,0), self.node_map[(0,0)].ref_var_map))
        print('{}.ref_var_direction_map = {}'.format((0,0), self.node_map[(0,0)].ref_var_direction_map))
        print('{}.ref_vars = {}'.format((0,0), self.node_map[(0,0)].ref_vars))
        '''
        
        # Create graph connection and path variables
        cycles = self.find_cycles(self)
        connection_vars = {}
        for n in self.nodes:
            if n.is_pi:
                continue
            connection_vars[n] = {}
        for n in self.nodes:
            if n.is_pi:
                continue
            for np in n.virtual_fanin:
                connection_vars[np][n] = var_idx
                legend[var_idx] = '{} and {} are connected'.format(np.coords, n.coords)
                var_idx += 1

        # Create the simulation propagation constraints.
        clauses = []
        for n in self.nodes:
            if n.is_pi:
                continue
            for gate_type in n.enabled_gate_types:
                if gate_type == 'EMPTY':
                    # We handle the empty gate as a special case.
                    continue
                elif gate_type == 'CROSS':
                    # CROSS is also handled as a special case.
                    gate_var = n.gate_type_map['CROSS']
                    fanin_options = n.svar_map[2]
                    for svar, fanins in fanin_options.items():
                        inport1, (innode1, outport1) = fanins[0]
                        inport2, (innode2, outport2) = fanins[1]
                        assert(inport1 == OPPOSITE_DIRECTION[outport1])
                        assert(inport2 == OPPOSITE_DIRECTION[outport2])
                        if innode1.is_pi or innode2.is_pi:
                            # Crossings cannot have PI fanin.
                            clauses.append([-gate_var, -svar])
                            continue
                        out_directions = n.dir_map[svar]
                        assert(out_directions[inport1] in n.sim_vars.keys())
                        assert(out_directions[inport2] in n.sim_vars.keys())
                        for tt_idx in range(nr_local_sim_vars):
                            permutations = list(itertools.product('01', repeat=2))
                            for permutation in permutations:
                                clause1 = [0] * 5
                                clause2 = [0] * 5
                                const_vals = [int(permutation[0]), int(permutation[1])]
                                clause1[0] = -svar
                                clause1[1] = -gate_var
                                clause2[0] = -svar
                                clause2[1] = -gate_var
                                for i in range(2):
                                    if const_vals[i] == 1:
                                        clause1[i+2] = -innode1.sim_vars[outport1][tt_idx]
                                        clause2[i+2] = -innode2.sim_vars[outport2][tt_idx]
                                    else:
                                        clause1[i+2] = innode1.sim_vars[outport1][tt_idx]
                                        clause2[i+2] = innode2.sim_vars[outport2][tt_idx]
                                if const_vals[0] == 1:
                                    clause1[4] = n.sim_vars[out_directions[inport1]][tt_idx]
                                else:
                                    clause1[4] = -n.sim_vars[out_directions[inport1]][tt_idx]
                                if const_vals[1] == 1:
                                    clause2[4] = n.sim_vars[out_directions[inport2]][tt_idx]
                                else:
                                    clause2[4] = -n.sim_vars[out_directions[inport2]][tt_idx]
                                    
                                clauses.append(clause1)
                                clauses.append(clause2)
                else:
                    fanin_size = GATE_FANIN_RANGE[gate_type]
                    gate_var = n.gate_type_map[gate_type]
                    fanin_options = n.svar_map[fanin_size]
                    for svar, fanins in fanin_options.items():
                        for fanout_direction in n.fanout_directions:
                            for tt_idx in range(nr_local_sim_vars):
                                permutations = list(itertools.product('01', repeat=(fanin_size)))
                                for permutation in permutations:
                                    const_vals = []
                                    for i in range(fanin_size):
                                        const_val = int(permutation[i])
                                        const_vals.append(const_val)
                                    function_output = eval_gate(gate_type, const_vals)
                                    clause = [0] * (fanin_size + 3)
                                    clause[0] = -svar
                                    clause[1] = -gate_var
                                    for i in range(len(const_vals)):
                                        _, (innode, output_port) = fanins[i]
                                        if const_vals[i] == 1:
                                            clause[i+2] = -innode.sim_vars[output_port][tt_idx]
                                        else:
                                            clause[i+2] = innode.sim_vars[output_port][tt_idx]
                                    if function_output == 1:
                                        clause[fanin_size+2] = n.sim_vars[fanout_direction][tt_idx]
                                    else:
                                        clause[fanin_size+2] = -n.sim_vars[fanout_direction][tt_idx]
                                    clauses.append(clause)


        # Make sure that every I/O port is used at most once, and that
        # PIs are used at most once.
        for n in self.nodes:
            if n.is_pi:
                cnf = CardEnc.atmost(lits=n.ref_vars, encoding=EncType.pairwise)
                for clause in cnf.clauses:
                    clauses.append(clause)
            else:
                cnf = CardEnc.atmost(lits=n.svars, encoding=EncType.pairwise)
                for clause in cnf.clauses:
                        clauses.append(clause)
                for direction, svars in n.ref_var_direction_map.items():
                    cnf = CardEnc.atmost(lits=svars, encoding=EncType.pairwise)
                    for clause in cnf.clauses:
                        clauses.append(clause)

        # Make sure that every node selects at least some fanin
        # option, unless its the EMPTY fanin. We do this based on
        # fanin range, so that e.g. if a node corrsponds to an AND
        # gate it cannot select only a single fanin.
        for n in self.nodes:
            if n.is_pi:
                continue
            empty_var = n.gate_type_map['EMPTY']
            for gate_type in n.enabled_gate_types:
                if gate_type == 'EMPTY':
                    continue
                gate_type_var = n.gate_type_map[gate_type]
                fanin_range = GATE_FANIN_RANGE[gate_type]
                svars = list(n.svar_map[fanin_range].keys())
                clauses.append([empty_var, -gate_type_var] + svars)

        # Create cycle-prevention constraints.
        for n in self.nodes:
            if n.is_pi:
                continue
            # If svar -> n selects innode as fanin, then there is a
            # connection between innode and n. Conversely, if there is
            # a connection between innode and n, one of the selection
            # variables that picks innode as a fanin of n must be
            # true. This converse case is not stricly necessary to
            # prevent cycles, but we add it anyway to break
            # symmetries.
            for svar, outnode in n.ref_var_map.items():
                clause = [0] * 2
                clause[0] = -svar
                clause[1] = connection_vars[n][outnode]
                clauses.append(clause)
            for innode in n.virtual_fanin:
                potential_svars = []
                for svar, outnode in innode.ref_var_map.items():
                    if outnode == n:
                        potential_svars.append(svar)
                clause = [-connection_vars[innode][n]] + potential_svars
                clauses.append(clause)
                
        # For every cycle in the graph, one of the variables
        # representing a step on the cycle must be false.
        for cycle in cycles:
            cycle_steps = zip(cycle, cycle[1:])
            cycle_lits = [-connection_vars[s[0]][s[1]] for s in cycle_steps]
            clauses.append(cycle_lits)

        # Fix input vars
        for var in range(self.nr_pis):
            n = self.nodes[var]
            for idx in range(nr_local_sim_vars):
                if idx & (1 << var):
                    clauses.append([n.sim_vars[None][idx]])
                else:
                    clauses.append([-n.sim_vars[None][idx]])

        # Fix output vars
        for h in range(nr_outputs):
            houtvars = out_vars[h]
            # Ensure that output h points to exactly one gate
            cnf = CardEnc.equals(lits=list(houtvars.keys()), encoding=EncType.pairwise)
            for clause in cnf.clauses:
                clauses.append(clause)
            # If output h points node n at the output port in
            # direction d, then the truth table of n must
            # agree with that of function h at that output
            # port.
            for houtvar, (n, d) in houtvars.items():
                for idx in range(nr_local_sim_vars):
                    if functions[h][idx] == 1:
                        clauses.append([-houtvar, n.sim_vars[d][idx]])
                    else:
                        clauses.append([-houtvar, -n.sim_vars[d][idx]])

        # Add gate constraints: every node must pick exactly one gate
        # type.
        for n in self.nodes:
            if n.is_pi:
                continue
            cnf = CardEnc.equals(lits=n.gate_type_vars, encoding=EncType.pairwise)
            for clause in cnf.clauses:
                clauses.append(clause)

        # Make sure that every non-PI non-empty gate is used at least
        # once. If not, some gates perform useless computations.
        for n in self.nodes:
            if n.is_pi:
                continue
            empty_var = n.gate_type_map['EMPTY']
            clauses.append([empty_var] + n.ref_vars)

        # Add cardinality constraints on gate fanouts. These
        # constraints depend on the gate type. AND/OR/NOT/MAJ gates
        # are restricted to single fanout, while wires may have fanout
        # up to three, and crossings have exactly two fanouts. Note
        # that we count PO references here as well, since we don't
        # want e.g. an AND gate being both a PO and referenced by an
        # internal node.
        for n in self.nodes:
            if n.is_pi:
                continue
            for gate_type in n.enabled_gate_types:
                gate_var = n.gate_type_map[gate_type]
                if gate_type == 'WIRE':
                    cnf = CardEnc.atmost(lits=n.ref_vars, encoding=EncType.pairwise, bound=3)
                    for clause in cnf.clauses:
                        clauses.append([-gate_var] + clause)
                elif gate_type == 'EMPTY':
                    # We'll handle EMPTY gates elsewhere.
                    continue
                elif gate_type == 'CROSS':
                    cnf = CardEnc.equals(lits=n.ref_vars, encoding=EncType.pairwise, bound=2)
                    for clause in cnf.clauses:
                        clauses.append([-gate_var] + clause)
                else:
                    cnf = CardEnc.equals(lits=n.ref_vars, encoding=EncType.pairwise)
                    for clause in cnf.clauses:
                        clauses.append([-gate_var] + clause)


        # If a tile has the EMPTY gate make sure it does not select
        # any fanin and that no gate selects it as fanin. Moreover,
        # set its simulation variables to zero in every output
        # direction.
        for n in self.nodes:
            if n.is_pi:
                continue
            empty_var = n.gate_type_map['EMPTY']
            for svar in n.svars:
                clauses.append([-empty_var, -svar])
            for ref_var in n.ref_vars:
                clauses.append([-empty_var, -ref_var])
            for direction in n.fanout_directions:
                for tt_idx in range(nr_local_sim_vars):
                    clauses.append([-empty_var, -n.sim_vars[direction][tt_idx]])

        # We cannot have a PI and a PO on the same I/O port.
        for n in self.nodes:
            if n.is_pi or not n.is_border_node:
                continue
            for direction in n.io_directions:
                pi_vars = n.svar_direction_map[direction]
                po_vars = []
                for h in range(nr_outputs):
                    houtvars = out_vars[h]
                    for houtvar, (po_n, d) in houtvars.items():
                        if po_n == n and d == direction:
                            po_vars.append(houtvar)
                for pi_var in pi_vars:
                    for po_var in po_vars:
                        clauses.append([-pi_var, -po_var])

        # If designated_io is enabled only WIRE elements can have
        # PI/PO fanin/fanout.
        if self.designated_pi:
            assert(self.enable_wire)
            for n in self.nodes:
                if not n.is_pi:
                    continue
                for svar, out_node in n.ref_var_map.items():
                    # If this svar refers to
                    wire_var = out_node.gate_type_map['WIRE']
                    clauses.append([-svar, wire_var])
                    # A designated PI can only have a single fanout.
                    cnf = CardEnc.atmost(lits=out_node.ref_vars, encoding=EncType.pairwise)
                    for clause in cnf.clauses:
                        clauses.append([-svar] + clause)
    
        if self.designated_po:
            assert(self.enable_wire)
            for n in self.nodes:
                if n.is_pi or not n.is_border_node:
                    continue
                # If one of the POs points to this gate, it has to be
                # a WIRE. Moreover, it cannot have any other fanout.
                wire_type_var = n.gate_type_map['WIRE']
                for h in range(nr_outputs):
                    houtvars = out_vars[h]
                    for houtvar, (out_node, _) in houtvars.items():
                        if out_node == n:
                            clauses.append([-houtvar, wire_type_var])
                            cnf = CardEnc.atmost(lits=out_node.ref_vars, encoding=EncType.pairwise)
                            for clause in cnf.clauses:
                                clauses.append([-houtvar] + clause)

        # Symmetry break: disallow consecutive NOT gates.
        if self.enable_not:
            for n in self.nodes:
                if n.is_pi:
                    continue
                not_type_var = n.gate_type_map['NOT']
                for svar, fanins in n.svar_map[1].items():
                    innode = fanins[0][1][0]
                    if innode.is_pi:
                        continue
                    innode_not_var = innode.gate_type_map['NOT']
                    clauses.append([-not_type_var, -svar, -innode_not_var])

        if self.nr_threads <= 1:
            # Create solver instance, add clauses, and start solving.
            solver = Glucose3()
            for clause in clauses:
                solver.add_clause(clause)

            # Decode network from solutions
            model_idx = 1
            prev_model = None
            for model in solver.enum_models():
                if verbosity > 1:
                    logfile = open('model-{}.log'.format(model_idx), 'w')
                    if prev_model != None:
                        for i in range(len(model)):
                            if model[i] != prev_model[i]:
                                logfile.write('model[{}] = {}, prev_model[{}] = {}\n'.format(
                                    i, 1 if model[i] > 0 else 0, i, 1 if prev_model[i] > 0 else 0
                                ))
                    for v in model:
                        logfile.write('{}\n'.format(v))
                    for v, s in legend.items():
                        logfile.write('{}: {} ({})\n'.format(v, s, True if model[v-1] > 0 else False))
                    logfile.close()
                self.model = model
                prev_model = model
                net = self.model_to_network(self, model, nr_outputs, out_vars, nr_local_sim_vars, verbosity)
                yield net
        else:
            # Use Glucose::MultiSolvers to find a solution.
            # Start by creating the temporary CNF file for it to act on.
            models = []
            while True:
                proc = None
                with tempfile.NamedTemporaryFile(mode='w', delete=False) as f:
                    f.write('p cnf {} {}\n'.format(var_idx - 1, len(clauses) + len(models)))
                    for clause in clauses:
                        for v in clause:
                            f.write('{} '.format(v))
                        f.write('0\n')
                    for model in models:
                        for v in model:
                            f.write('{} '.format(-v))
                        f.write('0\n')
                    f.close()
                    proc = subprocess.run(['../glucose-syrup', '-nthreads={}'.format(self.nr_threads), '-model', f.name], capture_output=True, text=True)
                    os.remove(f.name)
                if proc.returncode == 10:
                    # Glucose returns 10 on SAT
                    output = proc.stdout.split('\n')
                    model = []
                    for line in output:
                        if line[:1] == 'v':
                            modelvals = line.split(' ')[1:]
                            for modelval in modelvals:
                                if modelval != '0':
                                    model.append(int(modelval))
                    models.append(model)
                    net = self.model_to_network(self, model, nr_outputs, out_vars, nr_local_sim_vars, verbosity)
                    yield net
                elif proc.returncode == 20:
                    # Glucose returns 20 on UNSAT
                    break
                elif proc.returncode == 0:
                    # Glucose returns 0 on timeout/interrupt
                    break
                else:
                    # Error in calling SAT solver.
                    raise SynthesisException('Error calling Glucose::MultiSolvers')

    def model_to_network(self, model, nr_outputs, out_vars, nr_local_sim_vars, verbosity):
        '''
        Decodes a SAT model (i.e. a list of integer values) and creates a
        :class:`logic_network` from it.  
        '''
        net = logic_network(self.shape, self.nr_pis, self.nr_pos)
        for h in range(nr_outputs):
            houtvars = out_vars[h]
            out_found = 0
            for houtvar, (n, d) in houtvars.items():
                if model[houtvar - 1] > 0:
                    #if verbosity > 1:
#                        print('out[{}] -> ({}, {}) (houtvar={})'.format(h, n.coords, d, houtvar))
                    out_found += 1
                    net.set_output(h, n.coords, d)
            # Every output must point to exactly one output port.
            assert(out_found == 1)

        for n in self.nodes:
            if n.is_pi:
                continue
            if verbosity > 1:
                for gate_type in n.enabled_gate_types:
                    print('{} gate type {}: {} ({})'.format(
                    n.coords, gate_type, 1 if model[n.gate_type_map[gate_type]-1] > 0 else 0,
                    model[n.gate_type_map[gate_type]-1]))
                for direction in n.fanout_directions:
                    print('{} tt[{}]: '.format(n.coords, direction), end='')
                    for tt_idx in range(nr_local_sim_vars):
                        print('{}'.format(1 if model[n.sim_vars[direction][tt_idx]-1] > 0 else 0), end='')
                    print(' ', end='')
                    for tt_idx in range(nr_local_sim_vars):
                        print('({})'.format(model[n.sim_vars[direction][tt_idx]-1]), end='')
                    print('')
                
            netnode = net.node_map[n.coords]
            # Find out the gate type.
            gate_types_found = 0
            for gate_type, gate_var in n.gate_type_map.items():
                if model[gate_var - 1] > 0:
                    gate_types_found += 1
                    netnode.gate_type = gate_type
            assert(gate_types_found == 1)
#                print('{} is {}-gate'.format(netnode.coords, netnode.gate_type))
            netnode.is_border_node = n.is_border_node
            nr_selected_svars = 0
            nr_fanin = 0
            for size_option in n.svar_map.keys():
                for svar, comb in n.svar_map[size_option].items():
                    if model[svar - 1] > 0:
                        nr_selected_svars += 1
#                            print('{} has fanin {}'.format(n.coords, comb))
                        for i in range(size_option):
                            in_dir = comb[i][0]
                            innode = comb[i][1][0]
                            out_dir = comb[i][1][1]
                            if innode.is_pi:
                                netnode.set_fanin(in_dir, net.nodes[innode.coords], out_dir)
                            else:
                                netnode.set_fanin(in_dir, net.node_map[innode.coords], out_dir)
                        if netnode.gate_type == 'CROSS':
                            netnode.dir_map = n.dir_map[svar]
            assert(nr_selected_svars <= 1) # may be zero if EMPTY
        return net


    def print_model(self):
        '''
        Prints the model of the latest successful SAT call (if any).
        '''
        if self.model == None:
            raise Exception('No model available')
        for lit in self.model:
            print(lit)
