SET PATH=%PATH%;../../../Tools
arm-none-eabi-objcopy --output-target=binary uTaskerSerialBoot uTaskerSerialBoot.bin
arm-none-eabi-objcopy --output-target=srec uTaskerSerialBoot uTaskerSerialBoot.srec
