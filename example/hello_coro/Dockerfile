# This is a comment
FROM littlemole/reproweb_clangpp_libevent_make
MAINTAINER me <little.mole@oha7.org>

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y redis-server

ARG CXX=g++
ENV CXX=${CXX}

ARG BACKEND=
ENV BACKEND=${BACKEND}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

RUN /usr/local/bin/install.sh  repro-redis

RUN /usr/local/bin/install.sh  repro-sqlite

RUN mkdir -p /usr/local/src/reproweb-example
ADD . /usr/local/src/reproweb-example

RUN cd /usr/local/src/reproweb-example && make -e 

ENV REDIS_HOST=redis
CMD bash -c "cd /usr/local/src/reproweb-example &&  build/test.bin"

#CMD bash -c "/etc/init.d/redis-server start && cd /usr/local/src/reproweb-example && build/test.bin"


