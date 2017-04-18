set toolpath=%1
%toolpath%\arm-none-eabi-objcopy  --output-target=binary uTaskerV1_4.elf uTasker_V1.4.bin
%toolpath%\arm-none-eabi-objcopy  --output-target=srec uTaskerV1_4.elf uTasker_V1.4.srec
%toolpath%\arm-none-eabi-objcopy  --output-target=ihex uTaskerV1_4.elf uTasker_V1.4.hex
