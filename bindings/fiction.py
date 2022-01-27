from pyfiction import *

# create 2DDWave-clocked layout of size 4 x 3 tiles (max. coordinate is (3,2))
layout = gate_level_layout((3, 2), "Layout", "2DDWave")

# create primary inputs
x1 = layout.create_pi("x1", tile(1, 0))
x2 = layout.create_pi("x2", tile(0, 1))
x3 = layout.create_pi("x3", tile(2, 0))
x4 = layout.create_pi("x4", tile(0, 2))

# create gates and wires (buffers)
a1 = layout.create_and(x1, x2, tile(1, 1))

b1 = layout.create_buf(x3, tile(2, 1))
b2 = layout.create_buf(x4, tile(1, 2))

a2 = layout.create_and(b1, b2, tile(2, 2))

c = layout.create_buf(a1, tile(2, 1, 1))

# create primary outputs
layout.create_po(c, "f1", tile(3, 1))
layout.create_po(a2, "f2", tile(3, 2))

print("The layout's area is", area(layout), "tiles")

# simulate the layout's Boolean functions
tts = simulate(layout)

print("The layout's truth tables are:")
print("f1:", tts[0])
print("f2:", tts[1])
