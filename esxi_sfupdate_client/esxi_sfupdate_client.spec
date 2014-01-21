Name: esxi_sfupdate
Version: 1.0
Release:        %{RELEASE_TAG}
Summary: Remote firmware update tool for ESXi

Group: Applications/System
License: Property of Solarflare
URL: http://solarflare.com
Source0: esxi_sfupdate-1.0.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description

%prep
%setup -q -n esxi_sfupdate-1.0

%build
./configure
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/sbin/
cp ./sfupdate_esxi $RPM_BUILD_ROOT/usr/sbin/

%pre

%post


%preun

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
/usr/sbin/sfupdate_esxi
%doc

%changelog
