# This is a comment
FROM littlemole/reproweb_clangpp_libevent_make
MAINTAINER me <little.mole@oha7.org>

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-client libmysqlclient-dev

ARG CXX=g++
ENV CXX=${CXX}

ARG BACKEND=
ENV BACKEND=${BACKEND}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

#ENV REDIS_HOST=redis

RUN /usr/local/bin/install.sh  repro-mysql 

RUN mkdir -p /usr/local/src/reproweb-example
ADD . /usr/local/src/reproweb-example

#RUN cd /usr/local/src/reproweb-example && (if [ "$BUILDCHAIN" != "cmake" ] ; then make -e; else (mkdir -p build && cd build && cmake .. && make) fi)

RUN bash -c 'cd /usr/local/src/reproweb-example && SKIPTESTS=true VERSION="${CXX}_${BACKEND}_${BUILDCHAIN}" /usr/local/bin/compile.sh reproweb-example'

CMD bash -c 'cd /usr/local/src/reproweb-example &&  (if [ "$BUILDCHAIN" != "cmake" ] ; then build/hello_soa_userservice.bin; else Release/hello_soa_userservice.bin; fi )'




