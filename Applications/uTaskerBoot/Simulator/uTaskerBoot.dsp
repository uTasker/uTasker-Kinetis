# Microsoft Developer Studio Project File - Name="uTasker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=uTasker - Win32 Kinetis plus GCC build
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "uTaskerBoot.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "uTaskerBoot.mak" CFG="uTasker - Win32 Kinetis plus GCC build"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "uTasker - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "uTasker - Win32 Kinetis plus GCC build" (basierend auf  "Win32 (x86) Application")
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

!ELSEIF  "$(CFG)" == "uTasker - Win32 Kinetis plus GCC build"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "uTasker___Win32_Kinetis_plus_GCC_build"
# PROP BASE Intermediate_Dir "uTasker___Win32_Kinetis_plus_GCC_build"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "uTasker___Win32_Kinetis_plus_GCC_build"
# PROP Intermediate_Dir "uTasker___Win32_Kinetis_plus_GCC_build"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\..\." /I ".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_LPC17XX" /D "ETHEREAL" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\..\." /I ".\..\..\..\WinSim\WpdPack_3_1\WpdPack\Include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_KINETIS" /D "ETHEREAL" /FR /YX /FD /GZ /c
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
# Name "uTasker - Win32 Kinetis plus GCC build"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\uTaskerBootLoader.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\config.h
# End Source File
# Begin Source File

SOURCE=..\types.h
# End Source File
# End Group
# Begin Group "WinSim"

# PROP Default_Filter ""
# Begin Group "SIMKINETIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_port.h
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

SOURCE=..\..\..\WinSim\LCD\LCDSim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinPcap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinSim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\WinSim\WinSimMain.cpp
# End Source File
# End Group
# Begin Group "Hardware"

# PROP Default_Filter ""
# Begin Group "KINETIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Hardware\Kinetis\kinetis_boot.c
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
# Begin Group "Rescource Files"

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

SOURCE=..\GNU_Kinetis\make_uTaskerBoot_GNU_Kinetis
# End Source File
# End Target
# End Project
