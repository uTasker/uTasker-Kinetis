REM "uVision creates strange binary outputs which are often split into multiple files and of no use to most programming tools
REM so we use GCC objcopy to allow a single output that can be used in all cases"
..\..\..\Tools\arm-none-eabi-objcopy --input-target=ihex --output-target=binary Objects/uTaskerV1_4.hex uTaskerV1_4.bin
..\..\..\Tools\arm-none-eabi-objcopy --input-target=ihex --output-target=srec Objects/uTaskerV1_4.hex uTaskerV1_4.srec