OLED reset not driven
=====================

As in datasheet reset can be left unconnected ... it seems that display
can't be started without it :(

A first test with a 10k resistor between reset and VCC has shown that
communication works ... but init fails 50% of time (a strange pattern is
displayed)

Proposed fix
------------

The reset pin must be driven by a GPIO from U900. A wire to TP906 has
been added to the prototype.

A wire must be added into the next revision.