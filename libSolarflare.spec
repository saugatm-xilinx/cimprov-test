Name: cimprovider-Solarflare
Version: 0.1
Release:        1%{?dist}
Summary: CIM provider for Solarflare NIC management

Group: Applications/System
License: GPL
URL: http://solarflare.com
Source0: %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

#BuildRequires:
#Requires:

%description

Prefix: /opt/solarflare/lib
Prefix: /opt/solarflare/bin
Prefix: /opt/solarflare/mof

%prep
%setup -q


%build
make PRESET=ibm %{?_smp_mflags}
make PRESET=ibm %{?_smp_mflags} regmod

%install
rm -rf $RPM_BUILD_ROOT
make PRESET=ibm DESTDIR=$RPM_BUILD_ROOT install
make PRESET=ibm DESTDIR=$RPM_BUILD_ROOT install-aux

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
/opt/solarflare/bin/regmod
/opt/solarflare/lib/libSolarflare.so
/opt/solarflare/mof/repository.mof
%doc



%changelog