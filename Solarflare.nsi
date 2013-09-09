# This example tests the compile time aspect of the Library macros
# more than the runtime aspect. It is more of a syntax example,
# rather than a usage example.

!include "Library.nsh"
!include "MUI2.nsh"

Name "${PROVIDERNAME}"
OutFile "${INSTALLERNAME}"

InstallDir "$PROGRAMFILES\Solarflare Communications\Solarflare CIM provider"

!insertmacro MUI_PAGE_LICENSE "${TOP}/sf-license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
	     
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

RequestExecutionLevel admin 

!if ${CIM_INTERFACE} == cmpi
!define PegasusPath 'c:\cimom\pegasus_2.11.2\bin'
!define ProviderDLL '"${PegasusPath}\${PROVIDERNAME}.dll"'
!else
!define ProviderDLL '"$SYSDIR\wbem\${PROVIDERNAME}.dll"'
!endif

Section ProviderDLL

!if ${CIM_INTERFACE} == cmpi
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${PROVIDERNAME}.dll ${ProviderDLL} $INSTDIR
!else
!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED ${PROVIDERNAME}.dll ${ProviderDLL} $INSTDIR
!endif

SectionEnd

Section MOFs
SetOutPath $INSTDIR
File libcimobjects/repository.mof
!if ${CIM_INTERFACE} == cmpi
File libcimobjects/interop.mof
File repository.reg
ExecWait '${PegasusPath}\cimmof.exe -n ${NAMESPACE} "$INSTDIR\repository.mof"'
ExecWait '${PegasusPath}\cimmof.exe -n ${INTEROP_NAMESPACE} "$INSTDIR\interop.mof"'
ExecWait '${PegasusPath}\cimmof.exe -n ${INTEROP_NAMESPACE} "$INSTDIR\repository.reg"'
!else
File libcimobjects/schema.mof
File libprovider/register.mof
File libprovider/unregister.mof
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\schema.mof"'
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\repository.mof"'
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\register.mof"'
!endif
SectionEnd

Section
WriteUninstaller $INSTDIR\uninstall.exe
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "DisplayName" "Solarflare CIM"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "InstallLocation" "$INSTDIR"
SectionEnd

Section un.MOFs

!if ${CIM_INTERFACE} == cmpi
ExecWait '${PegasusPath}\cimprovider.exe -r -m ${PROVIDERNAME}_Module'
Delete $INSTDIR\repository.reg
Delete $INSTDIR\interop.mof
!else
ExecWait 'wmic path __Win32Provider.Name="Solarflare" delete'
ExecWait `wmic path __InstanceProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait `wmic path __MethodProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait `wmic path __EventProviderRegistration.Provider="__Win32Provider.Name='Solarflare'" delete`
ExecWait 'mofcomp.exe -N:${NAMESPACE} "$INSTDIR\unregister.mof"'
Delete $INSTDIR\unregister.mof
Delete $INSTDIR\register.mof
Delete $INSTDIR\schema.mof
!endif
Delete $INSTDIR\repository.mof

SectionEnd

Section un.ProviderDLL

!if ${CIM_INTERFACE} == cmpi
!insertmacro UninstallLib DLL NOTSHARED NOREBOOT_PROTECTED  ${ProviderDLL}
!else
!insertmacro UninstallLib REGDLL NOTSHARED NOREBOOT_PROTECTED  ${ProviderDLL}
!endif
Delete ${ProviderDLL}

SectionEnd

Section uninstall

Delete $INSTDIR\uninstall.exe
RmDir $INSTDIR

SectionEnd


