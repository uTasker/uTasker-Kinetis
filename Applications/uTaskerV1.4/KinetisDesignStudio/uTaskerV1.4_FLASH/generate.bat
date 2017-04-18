arm-none-eabi-objcopy  --output-target=binary uTaskerV1.4.elf ..\Applications\uTaskerV1.4\KinetisDesignStudio\uTaskerV1.4_FLASH\uTaskerV1.4.bin
arm-none-eabi-objcopy  --output-target=srec uTaskerV1.4.elf ..\Applications\uTaskerV1.4\KinetisDesignStudio\uTaskerV1.4_FLASH\uTaskerV1.4.srec
arm-none-eabi-objcopy  --output-target=ihex uTaskerV1.4.elf ..\Applications\uTaskerV1.4\KinetisDesignStudio\uTaskerV1.4_FLASH\uTaskerV1.4.hex

