set toolpath=%1
%toolpath%\arm-none-eabi-objcopy  --output-target=binary uTasker_BM_V1.4.elf uTasker_BM_V1.4.bin
%toolpath%\arm-none-eabi-objcopy  --output-target=srec uTasker_BM_V1.4.elf uTasker_BM_V1.4.srec
%toolpath%\arm-none-eabi-objcopy  --output-target=ihex uTasker_BM_V1.4.elf uTasker_BM_V1.4.hex
