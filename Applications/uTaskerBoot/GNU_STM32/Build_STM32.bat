rem - enter your own tool chain path here if needed and check this forum entry in case the make file doesn't work correctly:
rem "https://www.utasker.com/forum/index.php?topic=1988.msg7410#msg7410"
SET PATH=%PATH%;C:\nxp\MCUXpressoIDE_11.4.1_6260\ide\tools\bin
SET PATH=%PATH%;../../../Tools
make -f make_uTaskerBoot_GNU_STM32 all

