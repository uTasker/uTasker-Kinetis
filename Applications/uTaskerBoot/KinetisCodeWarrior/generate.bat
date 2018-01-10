set toolpath=%1
%toolpath%\arm-none-eabi-objcopy --output-target=binary uTaskerBMBoot.elf uTaskerBMBoot.bin
%toolpath%\arm-none-eabi-objcopy --output-target=srec   uTaskerBMBoot.elf uTaskerBMBoot.srec
%toolpath%\arm-none-eabi-objcopy --output-target=ihex   uTaskerBMBoot.elf uTaskerBMBoot.hex
