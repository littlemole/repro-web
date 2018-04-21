#!/bin/bash

export DESTDIR=/tmp
mkdir -p $DESTDIR/usr/local/lib
mkdir -p $DESTDIR/usr/local/include
mkdir -p $DESTDIR/usr/local/lib/pkgconfig

/usr/local/bin/build.sh cryptoneat
/usr/local/bin/build.sh repro
/usr/local/bin/build.sh prio
/usr/local/bin/build.sh repro-curl
/usr/local/bin/build.sh diy
/usr/local/bin/build.sh prio-http
/usr/local/bin/build.sh reproweb

bash -i
