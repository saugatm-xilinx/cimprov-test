# This example tests the compile time aspect of the Library macros
# more than the runtime aspect. It is more of a syntax example,
# rather than a usage example.

!include "Library.nsh"
!include "MUI2.nsh"

Name "${PROVIDERNAME}"
OutFile "${INSTALLERNAME}"

InstallDir "$PROGRAMFILES\${PROVIDERNAME}"

!insertmacro MUI_PAGE_LICENSE "sf-license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
	     
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

RequestExecutionLevel admin 

!define ProviderDLL '"$SYSDIR\wbem\${PROVIDERNAME}.dll"'

Section ProviderDLL

!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED ${PROVIDERNAME}.dll ${ProviderDLL} $INSTDIR

SectionEnd

Section MOFs
SetOutPath $INSTDIR
File libcimobjects/schema.mof
File libcimobjects/repository.mof
File libprovider/register.mof
File libprovider/unregister.mof
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\schema.mof"'
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\repository.mof"'
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\register.mof"'
SectionEnd

Section
WriteUninstaller $INSTDIR\uninstall.exe
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "DisplayName" "${PROVIDERNAME}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "InstallLocation" "$INSTDIR"
SectionEnd

Section un.MOFs

ExecWait 'wmic path __Win32Provider.Name="Solarflare" delete'
ExecWait `wmic path __InstanceProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait `wmic path __MethodProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait `wmic path __EventProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\unregister.mof"'
Delete $INSTDIR\unregister.mof
Delete $INSTDIR\register.mof
Delete $INSTDIR\repository.mof
Delete $INSTDIR\schema.mof

SectionEnd

Section un.ProviderDLL

!insertmacro UninstallLib REGDLL NOTSHARED NOREBOOT_PROTECTED  ${ProviderDLL}
Delete ${ProviderDLL}

SectionEnd

Section uninstall

Delete $INSTDIR\uninstall.exe
RmDir $INSTDIR

SectionEnd


