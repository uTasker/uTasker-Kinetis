rem SET PATH=%PATH%;D:\Freescale\KDS_v3\Toolchain\bin
SET PATH=%PATH%;D:\Freescale\MCUXpressoIDE_10.2.1_795\ide\tools\bin
SET PATH=%PATH%;../../../Tools
make -f make_uTaskerV1.4_GNU_STM32 all
uTaskerCombine "../../uTaskerBoot/GNU_STM32/uTaskerBoot.bin" uTaskerV1.4_BM.bin 0x4000 uTaskerBM.bin uTaskerBM.hex
uTaskerConvert.exe uTaskerV1.4_BM.bin 1_Upload.bin -0x5432 -622319de22b1
