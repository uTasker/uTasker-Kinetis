SET PATH=%PATH%;D:\Freescale\KDS_v3\Toolchain\bin
SET PATH=%PATH%;../../../Tools
make -f make_uTaskerV1.4_GNU_Kinetis all
uTaskerCombine "../../uTaskerBoot/GNU_Kinetis/uTaskerBoot.bin" uTaskerV1.4_BM.bin 0x8080 uTaskerBM.bin uTaskerBM.hex
uTaskerCombine "../../uTaskerSerialBoot/GNU_Kinetis/uTaskerSerialBoot.bin" uTaskerV1.4_BM.bin 0x8080 uTaskerSA.bin
uTaskerConvert.exe uTaskerV1.4_BM.bin z_Upload.bin -0x1234 -a748b6531124
arm-none-eabi-objcopy --input-target=ihex --output-target=binary software.hex application.bin
uTaskerConvert.exe application.bin software.bin -0x1234 -a748b6531124
uTaskerConvert.exe uTaskerV1.4_BM.bin sd_card_enc.bin -0x1235 -b748b6531124 -ff25a788f2e681338777 -afe1 -c298




