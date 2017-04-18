REM       COMMAND FILE BUILT BY IDEA 
REM       Thu Jan 29 03:16:49 2015
REM
cxcorm -v +proto +split +warn -wa335788 -wb1224 -i"..\..\utaskerserialboot" -i"..\..\..\..\utaskerv1_4" -d_COMPILE_COSMIC -d_KINETIS "..\..\..\hardware\kinetis\kinetis.c"
cxcorm -v +proto +split +warn -wa335788 -wb1224 -i"..\..\utaskerserialboot" -i"..\..\..\..\utaskerv1_4" -d_COMPILE_COSMIC -d_KINETIS "..\webinterface.c"
cxcorm -v +proto +split +warn -wa335788 -wb1224 -i"..\..\utaskerserialboot" -i"..\..\..\..\utaskerv1_4" -d_COMPILE_COSMIC -d_KINETIS "..\..\..\utasker\can_drv.c" "..\..\..\utasker\driver.c" "..\..\..\utasker\eth_drv.c" "..\..\..\utasker\globaltimer.c" "..\..\..\utasker\iic_drv.c" "..\..\..\utasker\low_power.c" "..\..\..\utasker\ssc_drv.c" "..\..\..\utasker\tty_drv.c" "..\..\..\utasker\ufile.c" "..\..\..\utasker\umalloc.c" "..\..\..\utasker\unetwork.c" "..\..\..\utasker\usb_drv.c" "..\..\..\utasker\utasker.c" "..\..\..\utasker\watchdog.c"
clnk -l"C:\Program Files (x86)\COSMIC\CXCORM_64K\LIB" -o "uTaskerSerialLoader.m" -m "uTaskerSerialLoader.map" "KL_128_16.lkf"
clabs "uTaskerSerialLoader.m"
cvdwarf "uTaskerSerialLoader.m"
create_bin.bat
