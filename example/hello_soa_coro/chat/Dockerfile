# This is a comment
FROM littlemole/reproweb_clangpp_boost_asio_make
MAINTAINER me <little.mole@oha7.org>

ARG CXX=g++
ENV CXX=${CXX}

ARG BACKEND=
ENV BACKEND=${BACKEND}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

ARG TS=
ENV TS=${TS}

RUN mkdir -p /usr/local/src/reproweb-example
ADD . /usr/local/src/reproweb-example

ENV USER_SERVICE_HOST=user
ENV SESSION_SERVICE_HOST=session


#RUN cd /usr/local/src/reproweb-example && (if [ "$BUILDCHAIN" != "cmake" ] ; then make -e; else (mkdir -p build && cd build && cmake .. && make) fi)

RUN bash -c 'cd /usr/local/src/reproweb-example && SKIPTESTS=true VERSION="${CXX}_${BACKEND}_${BUILDCHAIN}" /usr/local/bin/compile.sh reproweb-example'

CMD bash -c 'cd /usr/local/src/reproweb-example && (if [ "$BUILDCHAIN" != "cmake" ] ; then build/hello_world.bin; else Release/hello_world.bin; fi )'


