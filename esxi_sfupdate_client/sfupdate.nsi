!include "MUI2.nsh"
!if ${BITNESS} == 64
!include "x64.nsh"
!endif

Name "${TOOLNAME}"
OutFile "${INSTALLERNAME}"

!if ${BITNESS} == 64
InstallDir "$PROGRAMFILES64\${VENDORNAME}\${TOOLNAME}"
!else
InstallDir "$PROGRAMFILES\${VENDORNAME}\${TOOLNAME}"
!endif

!insertmacro MUI_PAGE_LICENSE "${TOP}/sf-license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
	     
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

RequestExecutionLevel admin 

Section Binary
!if ${BITNESS} == 64
${IfNot} ${RunningX64}
MessageBox MB_OK|MB_ICONEXCLAMATION "This version of ${TOOLNAME} requires 64-bit Windows"
Abort
${EndIf}
${DisableX64FSRedirection}
!endif
SetOutPath $INSTDIR
File ${TOOLFILE}
!if ${BITNESS} == 64
${EnableX64FSRedirection}
!endif
SectionEnd

Section
!if ${BITNESS} == 64
${DisableX64FSRedirection}
!endif
WriteUninstaller $INSTDIR\uninstall.exe
!if ${BITNESS} == 64
${EnableX64FSRedirection}
!endif
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TOOLNAME}" "DisplayName" "${TOOLDESC}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TOOLNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TOOLNAME}" "InstallLocation" "$INSTDIR"
SectionEnd

Section un.Binary

!if ${BITNESS} == 64
${DisableX64FSRedirection}
!endif
Delete $INSTDIR\${TOOLFILE}
!if ${BITNESS} == 64
${EnableX64FSRedirection}
!endif
SectionEnd

Section uninstall

DeleteRegKey HKLM "Software\${VENDORNAME}\${TOOLDESC}"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TOOLNAME}"
!if ${BITNESS} == 64
${DisableX64FSRedirection}
!endif
Delete $INSTDIR\uninstall.exe
RmDir $INSTDIR
!if ${BITNESS} == 64
${EnableX64FSRedirection}
!endif

SectionEnd


