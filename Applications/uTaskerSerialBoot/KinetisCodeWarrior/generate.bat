set toolpath=%1
%toolpath%\arm-none-eabi-objcopy --output-target=binary uTaskerSerialBoot.elf uTaskerSerialBoot.bin
%toolpath%\arm-none-eabi-objcopy --output-target=srec   uTaskerSerialBoot.elf uTaskerSerialBoot.srec
%toolpath%\arm-none-eabi-objcopy --output-target=ihex   uTaskerSerialBoot.elf uTaskerSerialBoot.hex
