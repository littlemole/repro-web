#!/bin/bash

export DESTDIR=/tmp/debian
mkdir -p $DESTDIR/usr/local/lib
mkdir -p $DESTDIR/usr/local/include
mkdir -p $DESTDIR/usr/local/lib/pkgconfig
mkdir -p $DESTDIR/DEBIAN/

cp /usr/local/src/reproweb/docker/control $DESTDIR/DEBIAN/

/usr/local/bin/build.sh cryptoneat
/usr/local/bin/build.sh repro
/usr/local/bin/build.sh prio
/usr/local/bin/build.sh repro-curl
/usr/local/bin/build.sh diy
/usr/local/bin/build.sh prio-http
/usr/local/bin/build.sh reproweb

cp /usr/local/src/reproweb/docker/control $DESTDIR/DEBIAN/
cd $DESTDIR

dpkg --build debian

mv debian.deb $TARGET.deb

bash -i
