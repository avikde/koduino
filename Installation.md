
To get started, you can choose between using the Arduino IDE to write your code in, or use a Makefile build system. What you choose should be personal preference, and it's very simple to switch between them for a project should you change your mind later.

| Arduino IDE | Standalone with Makefile |
|---------------------------------------------------------------|------------------------------------------------|
| Only a single .ino file per sketch | Need to keep sketch code along with a Makefile |
| Set options (HSE value, board variant, libraries) through IDE | Set options in Makefile |
| Recompiles every time (bug?) | Faster to recompile |
| Uses stm32flash to upload | Uses stm32loader.py to upload |
|  | Not recommended on Windows |

Choose:

* [Makefile Setup](SetupMakefile.md), or
* [Arduino IDE Setup](SetupIDE.md) (recommended on Windows)