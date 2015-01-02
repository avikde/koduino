# Variants

Assuming the core libraries account for the stupid differences between the Std Periph libraries for different series, the next point of diversity is within-series. These are called *variants*.

#### When do you need a new variant?

1. If a different *startup script* is required, a new variant should be defined. E.g. within the F4 series, F40_41x and F411x need different variants. This is actually a necessity, since if you put different startup `.S` scripts in the variant directory, the Arduino IDE will compile all files in the directory.
1. If you want a new pin layout for an existing board (including different timer assignments that you don't want to do using `pinRemap()`). Copying the variant directory over is a good starting point for this.

#### Different packages (e.g. 64-pin vs 100-pin)

More info coming

#### Pin, timer maps

Start off with a spreadsheet, like ...

There is a python script `variant_codegen.py` which should generate the text for the code that can be pasted into `variant.h` and `variant.cpp`
