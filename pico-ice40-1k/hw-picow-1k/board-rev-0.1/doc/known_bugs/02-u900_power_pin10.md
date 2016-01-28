W7500 schematic error
=====================

The schematic has an error for IC W7500. Pin number 10 is a VCC input but
into the library this pin is a ground.

Fix for prototype
-----------------

The pin can be raised from the pad, and a wire can be added to connect it 
to VCC. Note: the chip seems to work without pin 10.

Fix for next release
--------------------

The part must be fixed into library, then the routing must be updated.
