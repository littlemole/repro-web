#!/bin/bash
set -e

cd /usr/src/

git clone https://github.com/open-source-parsers/jsoncpp.git

cd jsoncpp

if [ "$CXX" == "g++" ]
then 
	FLAGS=""
else
	FLAGS="-stdlib=libc++"
fi

cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_CXX_FLAGS=$FLAGS . 
make
make install
  
