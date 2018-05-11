It is possible to work with CodeWarrior10.x, Atollic or Kinetis Design Studio.
These are however all based on Eclipse and need their project configuration files to be located at the top of the project directory structure, meaning that only one can be used at a time.


To work with the one of your choice, delete the original project files at the top of the project directory. Then copy the ones in this folder to that location, import into the Eclipse based IDE that you are to work with (matching this folder's contents).
Update the copy of the project files here if needed so that they can later be restored when working with the project again, after having changed to a different IDE in the meantime.

When working with Cortex-m4 parts with FPU and KDS select Float ABI selection to "FP instructions (hard)" plus FPU Type to "fpv6-sp-d16" for optimal performance, and remove when no FPU is available!
