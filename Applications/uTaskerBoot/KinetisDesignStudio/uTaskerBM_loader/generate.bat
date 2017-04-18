arm-none-eabi-objcopy  --output-target=binary uTaskerBM_loader.elf ..\Applications\uTaskerBoot\KinetisDesignStudio\uTaskerBM_Loader\uTaskerBM_loader.bin
arm-none-eabi-objcopy  --output-target=srec uTaskerBM_loader.elf ..\Applications\uTaskerBoot\KinetisDesignStudio\uTaskerBM_Loader\uTaskerBM_loader.srec
arm-none-eabi-objcopy  --output-target=ihex uTaskerBM_loader.elf ..\Applications\uTaskerBoot\KinetisDesignStudio\uTaskerBM_Loader\uTaskerBM_loader.hex
