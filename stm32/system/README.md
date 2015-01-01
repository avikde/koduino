# Vendor libraries

Includes the Cortex CMSIS, and ST standard peripheral library.

To compile a static lib to use with the arduino IDE, 

1. Define a variant in the series (the `variant.mk` defines the series, check existing ones for examples)
1. Type `make lib` in this directory
1. A `libSERIES.a` should appear here.

*WARNING:* the stmxxx_rcc.c file uses the HSE_VALUE for some functions, so those will be
wrong if the HSE value is changed.
