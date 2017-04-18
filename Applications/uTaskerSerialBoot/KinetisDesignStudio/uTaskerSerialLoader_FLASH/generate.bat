arm-none-eabi-objcopy  --output-target=binary uTaskerSerialLoader.elf ..\Applications\uTaskerSerialBoot\KinetisDesignStudio\uTaskerSerialLoader_FLASH\uTaskerSerialLoader.bin
arm-none-eabi-objcopy  --output-target=srec uTaskerSerialLoader.elf ..\Applications\uTaskerSerialBoot\KinetisDesignStudio\uTaskerSerialLoader_FLASH\uTaskerSerialLoader.srec
arm-none-eabi-objcopy  --output-target=ihex uTaskerSerialLoader.elf ..\Applications\uTaskerSerialBoot\KinetisDesignStudio\uTaskerSerialLoader_FLASH\uTaskerSerialLoader.hex
