
Name: chocolate-doom
Summary: Conservative source port
Version: 2.2.0
Release: 1
Source: http://www.chocolate-doom.org/downloads/2.2.0/chocolate-doom-2.2.0.tar.gz
URL: http://www.chocolate-doom.org/
Group: Amusements/Games
BuildRoot: /var/tmp/chocolate-doom-buildroot
License: GNU General Public License, version 2
Packager: Simon Howard <fraggle@gmail.com>
Prefix: %{_prefix}
Autoreq: 0
Requires: libSDL-1.2.so.0, libSDL_mixer-1.2.so.0, libSDL_net-1.2.so.0

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
./configure \
 	--prefix=/usr \
	--exec-prefix=/usr \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	--sysconfdir=/etc \
	--datadir=/usr/share \
	--includedir=/usr/include \
	--libdir=/usr/lib \
	--libexecdir=/usr/lib \
	--localstatedir=/var/lib \
	--sharedstatedir=/usr/com \
	--mandir=/usr/share/man \
	--infodir=/usr/share/info
make

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%description
%(sed -n "/==/ q; p " < README)

See http://www.chocolate-doom.org/ for more information.

%package -n chocolate-heretic
Summary: Conservative source port (Heretic binaries)
Group: Amusements/Games
Requires: libSDL-1.2.so.0, libSDL_mixer-1.2.so.0, libSDL_net-1.2.so.0

%files
%{_mandir}/man5/chocolate-doom.cfg.5*
%{_mandir}/man5/default.cfg.5*
%{_mandir}/man6/chocolate-doom.6*
%{_mandir}/man6/chocolate-setup.6*
%{_mandir}/man6/chocolate-server.6*
/usr/share/doc/chocolate-doom/*
/usr/games/chocolate-doom
/usr/games/chocolate-server
/usr/games/chocolate-doom-setup
/usr/share/icons/*
/usr/share/applications/*

%description -n chocolate-heretic
%(sed -n "/==/ q; p " < README)

These are the Heretic binaries.

See http://www.chocolate-doom.org/ for more information.

%files -n chocolate-heretic
%{_mandir}/man5/chocolate-heretic.cfg.5*
%{_mandir}/man5/heretic.cfg.5*
%{_mandir}/man6/chocolate-heretic.6*
/usr/share/doc/chocolate-heretic/*
/usr/games/chocolate-heretic
/usr/games/chocolate-heretic-setup

%package -n chocolate-hexen
Summary: Conservative source port (Hexen binaries)
Group: Amusements/Games
Requires: libSDL-1.2.so.0, libSDL_mixer-1.2.so.0, libSDL_net-1.2.so.0

%description -n chocolate-hexen
%(sed -n "/==/ q; p " < README)

These are the Hexen binaries.

See http://www.chocolate-doom.org/ for more information.

%files -n chocolate-hexen
%{_mandir}/man5/chocolate-hexen.cfg.5*
%{_mandir}/man5/hexen.cfg.5*
%{_mandir}/man6/chocolate-hexen.6*
/usr/share/doc/chocolate-hexen/*
/usr/games/chocolate-hexen
/usr/games/chocolate-hexen-setup

%package -n chocolate-strife
Summary: Conservative source port (Strife binaries)
Group: Amusements/Games
Requires: libSDL-1.2.so.0, libSDL_mixer-1.2.so.0, libSDL_net-1.2.so.0

%description -n chocolate-strife
%(sed -n "/==/ q; p " < README)

These are the Strife binaries.

See http://www.chocolate-doom.org/ for more information.

%files -n chocolate-strife
%{_mandir}/man5/chocolate-strife.cfg.5*
%{_mandir}/man5/strife.cfg.5*
%{_mandir}/man6/chocolate-strife.6*
/usr/share/doc/chocolate-strife/*
/usr/games/chocolate-strife
/usr/games/chocolate-strife-setup

