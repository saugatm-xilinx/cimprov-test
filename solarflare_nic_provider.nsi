# This example tests the compile time aspect of the Library macros
# more than the runtime aspect. It is more of a syntax example,
# rather than a usage example.

!include "Library.nsh"
!include "MUI2.nsh"

Name "${PROVIDERNAME}"
OutFile "${INSTALLERNAME}"

InstallDir "$PROGRAMFILES\${VENDORNAME}\${PROVIDERDESC}"

!insertmacro MUI_PAGE_LICENSE "${TOP}/sf-license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
	     
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

RequestExecutionLevel admin 

!if ${CIM_INTERFACE} == cmpi
!define PegasusRoot 'c:\pegasus\build'
!define PegasusPath '${PegasusRoot}\bin'
!define ProviderDLL '"${PegasusPath}\${PROVIDERNAME}.dll"'
!else
!define ProviderDLL '"$SYSDIR\wbem\${PROVIDERNAME}.dll"'
!endif

!macro SilentExec Command Args
   nsExec::ExecToLog `"${Command}" ${Args}`
   Pop $0
   StrCmp $0 "error" +2 0
   StrCmp $0 "0" +3 +2
   Abort "${Command} cannot be executed"
   Abort "${Command} returned $0"
!macroend

!macro SilentExecNofail Command Args
   nsExec::ExecToLog `"${Command}" ${Args}`
   Pop $0
   StrCmp $0 "error" 0 +2
   Abort "${Command} cannot be executed"
!macroend

Section ProviderDLL


!if ${CIM_INTERFACE} == cmpi
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${PROVIDERNAME}.dll ${ProviderDLL} $INSTDIR
!else
!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED ${PROVIDERNAME}.dll ${ProviderDLL} $INSTDIR
!endif

SectionEnd

Section MOFs
SetOutPath $INSTDIR
File libcimobjects/namespace.mof
File libcimobjects/schema.mof
!if ${CIM_INTERFACE} == cmpi
File libcimobjects/interop.mof
!if ${NEED_ASSOC_IN_ROOT_CIMV2} == 1
File libcimobjects/root.mof
!endif
File repository.reg
!insertmacro SilentExec '${PegasusPath}\cimmof.exe' '-n ${NAMESPACE} "$INSTDIR\schema.mof"'
!insertmacro SilentExec '${PegasusPath}\cimmof.exe' '-n ${NAMESPACE} "$INSTDIR\namespace.mof"'
!insertmacro SilentExec '${PegasusPath}\cimmof.exe' '-n ${INTEROP_NAMESPACE} "$INSTDIR\interop.mof"'
!if ${NEED_ASSOC_IN_ROOT_CIMV2} == 1
!insertmacro SilentExec '${PegasusPath}\cimmof.exe' '-n ${ROOT_NAMESPACE} "$INSTDIR\root.mof"'
!endif
!insertmacro SilentExec '${PegasusPath}\cimmof.exe' '-n ${INTEROP_NAMESPACE} "$INSTDIR\repository.reg"'
!else
File ${TOP}/schemas/windows/cimwin32.mof
File libprovider/register.mof
File libprovider/unregister.mof
!insertmacro SilentExec mofcomp.exe '-N:${NAMESPACE} "$INSTDIR\cimwin32.mof"'
!insertmacro SilentExec mofcomp.exe '-N:${NAMESPACE} "$INSTDIR\schema.mof"'
!insertmacro SilentExec mofcomp.exe '-N:${NAMESPACE} "$INSTDIR\namespace.mof"'
!insertmacro SilentExec mofcomp.exe '-N:${NAMESPACE} "$INSTDIR\register.mof"'
!endif
SectionEnd

Section
WriteUninstaller $INSTDIR\uninstall.exe
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "DisplayName" "${PROVIDERDESC}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}" "InstallLocation" "$INSTDIR"
SectionEnd

Section un.MOFs

!if ${CIM_INTERFACE} == cmpi
!insertmacro SilentExec '${PegasusPath}\cimprovider.exe' '-r -m ${PROVIDERNAME}_Module'
!insertmacro SilentExec 'net' 'stop cimserver'
RMDir /r '${PegasusRoot}\repository\root#solarflare'
!insertmacro SilentExec 'net' 'start cimserver'
Delete $INSTDIR\repository.reg
Delete $INSTDIR\interop.mof
!if ${NEED_ASSOC_IN_ROOT_CIMV2} == 1
Delete $INSTDIR\root.mof
!endif
!else
!insertmacro SilentExecNofail wmic `/namespace:${WMIC_NAMESPACE} path__InstanceProviderRegistration.Provider="__Win32Provider.Name='${PROVIDERNAME}'" delete`
!insertmacro SilentExecNofail wmic `/namespace:${WMIC_NAMESPACE} path __MethodProviderRegistration.Provider="__Win32Provider.Name='${PROVIDERNAME}'" delete`
!insertmacro SilentExecNofail wmic `/namespace:${WMIC_NAMESPACE} path __EventProviderRegistration.Provider="__Win32Provider.Name='${PROVIDERNAME}'" delete`
!insertmacro SilentExecNofail wmic '/namespace:${WMIC_NAMESPACE} path __Win32Provider.Name="${PROVIDERNAME}" delete'
!insertmacro SilentExecNofail mofcomp.exe '-N:${NAMESPACE} "$INSTDIR\unregister.mof"'
!insertmacro SilentExecNofail wmic '/namespace:\\root path __Namespace.Name="${NAMESPACE_INSTANCE}" delete'
Delete $INSTDIR\unregister.mof
Delete $INSTDIR\register.mof
!endif
Delete $INSTDIR\namespace.mof
Delete $INSTDIR\schema.mof
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

DeleteRegKey HKLM "Software\${VENDORNAME}\${PROVIDERDESC}"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROVIDERNAME}"
Delete $INSTDIR\uninstall.exe
RmDir $INSTDIR

SectionEnd


