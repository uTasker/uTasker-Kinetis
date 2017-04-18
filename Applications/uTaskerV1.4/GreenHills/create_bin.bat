SET PATH=%PATH%;../../../Tools
arm-none-eabi-objcopy --output-target=binary uTaskerV1.4 uTaskerV1.4.bin
arm-none-eabi-objcopy --output-target=srec uTaskerV1.4 uTaskerV1.4.srec
