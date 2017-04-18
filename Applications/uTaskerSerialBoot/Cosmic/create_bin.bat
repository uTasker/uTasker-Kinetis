SET PATH=%PATH%;../../../Tools
arm-none-eabi-objcopy --only-section=.vector --only-section=.config --only-section=text --only-section=const --only-section=sconst --output-target=binary uTaskerSerialLoader.elf uTaskerSerialLoader.bin
rem arm-none-eabi-objcopy --output-target=srec uTaskerSerialLoader.elf uTaskerSerialLoader.srec


