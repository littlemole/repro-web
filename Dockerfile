# This is a comment
FROM littlemole/devenv_clangpp_make
MAINTAINER me <little.mole@oha7.org>

ARG CXX=g++
ENV CXX=${CXX}

ARG BACKEND=
ENV BACKEND=${BACKEND}

ARG BUILDCHAIN=make
ENV BUILDCHAIN=${BUILDCHAIN}

ARG TS=
ENV TS=${TS}

RUN /usr/local/bin/install.sh repro 
RUN /usr/local/bin/install.sh prio 
RUN /usr/local/bin/install.sh repro-curl
RUN /usr/local/bin/install.sh prio-http 

RUN mkdir -p /usr/local/src/reproweb
ADD . /usr/local/src/reproweb

RUN SKIPTESTS=true /usr/local/bin/build.sh reproweb 

