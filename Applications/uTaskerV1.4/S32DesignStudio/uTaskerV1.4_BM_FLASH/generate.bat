arm-none-eabi-objcopy  --output-target=binary uTaskerV1.4_BM.elf ..\Applications\uTaskerV1.4\S32DesignStudio\uTaskerV1.4_BM_FLASH\uTaskerV1.4_BM.bin
arm-none-eabi-objcopy  --output-target=srec uTaskerV1.4_BM.elf ..\Applications\uTaskerV1.4\S32DesignStudio\uTaskerV1.4_BM_FLASH\uTaskerV1.4_BM.srec
arm-none-eabi-objcopy  --output-target=ihex uTaskerV1.4_BM.elf ..\Applications\uTaskerV1.4\S32DesignStudio\uTaskerV1.4_BM_FLASH\uTaskerV1.4_BM.hex
rem Convert the binary file to an SD card upload file
..\Tools\uTaskerConvert ..\Applications\uTaskerV1.4\S32DesignStudio\uTaskerV1.4_BM_FLASH\uTaskerV1.4_BM.bin ..\Applications\uTaskerV1.4\S32DesignStudio\uTaskerV1.4_BM_FLASH\software.bin -0x1234 -a748b6531124

