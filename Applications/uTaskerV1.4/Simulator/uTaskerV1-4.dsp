# Microsoft Developer Studio Project File - Name="uTasker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=uTasker - Win32 uTasker kinetis plus GNU build
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "uTaskerV1-4.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "uTaskerV1-4.mak" CFG="uTasker - Win32 uTasker kinetis plus GNU build"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "uTasker - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "uTasker - Win32 uTasker kinetis plus GNU build" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uTasker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\..\." /I ".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Include" /I "." /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_KINETIS" /D "ETHEREAL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x807 /d "_DEBUG"
# ADD RSC /l 0x809 /i "..\..\..\WinSim\\" /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wpcap.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Lib"

!ELSEIF  "$(CFG)" == "uTasker - Win32 uTasker kinetis plus GNU build"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "uTasker___Win32_uTasker_kinetis_plus_GNU_build"
# PROP BASE Intermediate_Dir "uTasker___Win32_uTasker_kinetis_plus_GNU_build"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "uTasker___Win32_uTasker_kinetis_plus_GNU_build"
# PROP Intermediate_Dir "uTasker___Win32_uTasker_kinetis_plus_GNU_build"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\..\." /I ".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Include" /I "." /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LPC17XX" /D "ETHEREAL" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\..\." /I ".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Include" /I "." /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_KINETIS" /D "ETHEREAL" /D "_EXE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /i "..\..\..\WinSim\\" /d "_DEBUG"
# SUBTRACT BASE RSC /x
# ADD RSC /l 0x809 /i "..\..\..\WinSim\\" /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wpcap.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wpcap.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=cd ..\GNU_Kinetis	Build_Kinetis.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "uTasker - Win32 Debug"
# Name "uTasker - Win32 uTasker kinetis plus GNU build"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\application.c
# End Source File
# Begin Source File

SOURCE=..\debug.c
# End Source File
# Begin Source File

SOURCE=..\KeyScan.c
# End Source File
# Begin Source File

SOURCE=..\modbus_app.c
# End Source File
# Begin Source File

SOURCE=..\NetworkIndicator.c
# End Source File
# Begin Source File

SOURCE=..\snmp_mib_handlers.c
# End Source File
# Begin Source File

SOURCE=..\usb_application.c
# End Source File
# Begin Source File

SOURCE=..\webInterface.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\ADC_Timers.h
# End Source File
# Begin Source File

SOURCE=..\app_hw_kinetis.h
# End Source File
# Begin Source File

SOURCE=..\app_user_files.h
# End Source File
# Begin Source File

SOURCE=..\application.h
# End Source File
# Begin Source File

SOURCE=..\application_lcd.h
# End Source File
# Begin Source File

SOURCE=..\can_tests.h
# End Source File
# Begin Source File

SOURCE=..\config.h
# End Source File
# Begin Source File

SOURCE=..\debug_hal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\driver.h
# End Source File
# Begin Source File

SOURCE=..\iic_tests.h
# End Source File
# Begin Source File

SOURCE=..\Port_Interrupts.h
# End Source File
# Begin Source File

SOURCE=..\slcd_time.h
# End Source File
# Begin Source File

SOURCE=..\TaskConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Stack\tcpip.h
# End Source File
# Begin Source File

SOURCE=..\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\usb.h
# End Source File
# Begin Source File

SOURCE=..\user_glcd_fonts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uTasker.h
# End Source File
# Begin Source File

SOURCE=..\widgets.h
# End Source File
# End Group
# Begin Group "uTasker"

# PROP Default_Filter ""
# Begin Group "MODBUS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\uTasker\MODBUS\MODBUS.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\MODBUS\modbus.h
# End Source File
# End Group
# Begin Group "uGLCDLIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\GLCD.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_cglcd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_fonts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_nokia.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_oled.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_samsung.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_st7565s.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_tft.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\glcd_toshiba.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uGLCDLIB\LCD.c
# End Source File
# End Group
# Begin Group "utFAT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\uTasker\utFAT\mass_storage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\utFAT\mass_storage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\utFAT\NAND_driver.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\uTasker\can_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\Driver.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\eth_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\GlobalTimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\iic_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\low_power.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\SSC_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\Tty_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uFile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uMalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uNetwork.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\USB_drv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\uTasker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\uTasker\Watchdog.c
# End Source File
# End Group
# Begin Group "TCP/IP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\stack\arp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\dhcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\dns.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\Ethernet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\ftp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\ftp_client.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\http.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\icmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\igmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\ip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\ip_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\NetBIOS.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\pop3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\ppp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\smtp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\snmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\tcp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\telnet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\tftp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\udp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\webutils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\stack\zero_config.c
# End Source File
# End Group
# Begin Group "Hardware"

# PROP Default_Filter ""
# Begin Group "KINETIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_asm.s
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\spi_flash_kinetis_atmel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\spi_flash_kinetis_sst25.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\spi_flash_kinetis_stmicro.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\Hardware\hardware.h
# End Source File
# End Group
# Begin Group "WinSim"

# PROP Default_Filter ""
# Begin Group "SIMkinetis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k10.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k20.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k30.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k40.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k50.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k51.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k52.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k53.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k61.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_k70.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_ke.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port_kl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_ports.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetisSim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\simkinetis.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\WinSim\Ethereal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\FileToDisk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\IIC_devices\IIC_dev.c
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\LCD\KeyPadSim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\Komodo\komodo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\Komodo\komodo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\LCD\lcd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\LCD\LCDSim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\PortSim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\SDcard_sim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinPcap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinSim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinSim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinSimMain.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# Begin Source File

SOURCE=.\uTasker.ico
# End Source File
# Begin Source File

SOURCE=.\uTasker.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\cursor10.cur
# End Source File
# Begin Source File

SOURCE=.\cursor7.cur
# End Source File
# Begin Source File

SOURCE=.\cursor8.cur
# End Source File
# Begin Source File

SOURCE=.\cursor9.cur
# End Source File
# Begin Source File

SOURCE=..\GNU_Kinetis\make_uTaskerV1.4_GNU_Kinetis
# End Source File
# End Target
# End Project
