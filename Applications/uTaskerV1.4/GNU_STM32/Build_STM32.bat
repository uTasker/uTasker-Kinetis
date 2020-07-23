rem - enter your own tool chain path here if needed and check this forum entry in case the make file doesn't work correctly:
rem "https://www.utasker.com/forum/index.php?topic=1988.msg7410#msg7410"
SET PATH=%PATH%;C:\nxp\MCUXpressoIDE_11.1.0_3209\ide\tools\bin
SET PATH=%PATH%;../../../Tools

make -f make_uTaskerV1.4_GNU_STM32 all
uTaskerCombine "../../uTaskerBoot/GNU_STM32/uTaskerBoot.bin" uTaskerV1.4_BM.bin 0x4000 uTaskerBM.bin uTaskerBM.hex
uTaskerConvert.exe uTaskerV1.4_BM.bin 1_Upload.bin -0x5432 -622319de22b1
