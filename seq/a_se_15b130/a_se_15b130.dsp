# Microsoft Developer Studio Project File - Name="se_15b130" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=se_15b130 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "a_se_15b130.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "a_se_15b130.mak" CFG="se_15b130 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "se_15b130 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "se_15b130"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\n4\x86\prod\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W4 /GR /GX /Z7 /Od /I "\n4\pkg" /I "\n4\tool" /I "\n4\opensource\STLport\stlport" /I "\n4\x86\prod\include" /I "\n4\x86\delivery\include" /I "\n4\x86\extsw\MedCom\SyngoACE\include" /I "\n4\opensource\boost" /I "\MR\public" /I "\SWF_extern\SDK\include" /I "c:\msvs6\common\msdev98\..\..\VC98\Include" /I "c:\msvs6\common\msdev98\..\..\VC98\MFC\Include" /I "c:\msvs6\common\msdev98\..\..\VC98\ATL\Include" /I "\n4\pkg\MrServers\MrImaging\seq\MiniFLASH" /I "\n4_extsw\x86\extsw\MedCom\include"  /I "\n4\x86\extsw\include" /D "BUILD_SEQU" /D "BUILD_se_15b130" /D "SEQUENCE_CLASS" /D "SEQUENCE_CLASS_se_15b130" /D SEQ_NAMESPACE=SEQ_SE_15B130 /D "MFCDebug" /D "_DEBUG" /D "RWDEBUG" /D "WIN32" /D WINVER=0x500 /D _WIN32_WINNT=0x500 /D "_AFXDLL" /D "_WINDOWS" /D "WinNT400" /D "MSVC60" /D AFX_NOVTABLE= /D PARC_MODULE_NAME=se_15b130 /D "DEBUG" /D "_RWTOOLSDLL" /D "_UNICODE" /D "UNICODE" /D "O_DLL_PCLASS_ONLY" /D "_CONSOLE" /D "_WINDLL" /D "CSA_HAS_DLL" /D "ACE_HAS_DLL" /FD /GF /EHa /Zm500 /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rwtoold.lib aced.lib liboscfe.lib mscchk.lib mscvcch.lib i18nd.lib MrProtd.lib Sequenced.lib libMESd.lib MeasSectionsd.lib MeasNucleiBased.lib libSeqSysPropd.lib MeasPatientd.lib libSBBd.lib libSSLd.lib libRTd.lib libGSLd.lib SeqBufferd.lib libSeqUTd.lib libSeqUtild.lib MdhProxyd.lib libUILinkd.lib libUICtrld.lib MrTraced.lib MeasNucleiIFd.lib MrParcd.lib MrGenericDCd.lib /nologo /dll /debug /machine:I386 /out:"\n4\x86\prod\bin/se_15b130d.dll" /pdbtype:sept /libpath:"\n4\x86\prod\lib" /libpath:"\n4\x86\extsw\MedCom\lib" /libpath:"\n4\x86\delivery\lib" /libpath:"\SWF_extern\SDK\lib"
# Begin Target

# Name "se_15b130 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\a_se_b130_15000.cpp
# End Source File
# Begin Source File

SOURCE=.\a_se_b130_15000_UI.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\a_se_b130_15000.h
# End Source File
# Begin Source File

SOURCE=.\a_se_b130_15000_UI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
