# This is a comment
FROM littlemole/reproweb_gpp_libevent_make
MAINTAINER me <little.mole@oha7.org>

ARG CXX=g++
ENV CXX=${CXX}

ARG BACKEND=
ENV BACKEND=${BACKEND}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

ENV USER_SERVICE_HOST=user
ENV SESSION_SERVICE_HOST=session

RUN mkdir -p /usr/local/src/reproweb-example
ADD . /usr/local/src/reproweb-example

RUN cd /usr/local/src/reproweb-example && (if [ "$BUILDCHAIN" != "cmake" ] ; then make -e; else (mkdir -p build && cd build && cmake .. && make) fi)

CMD bash -c " cd /usr/local/src/reproweb-example && build/hello_world.bin"




