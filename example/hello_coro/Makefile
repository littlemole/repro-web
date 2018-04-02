CXX = clang++-5.0
BACKEND=libevent
BUILDCHAIN = make

CONTAINER = $(shell echo "reproweb_coro_example_$(CXX)_$(BACKEND)_$(BUILDCHAIN)" | sed 's/++/pp/')
BASE_CONTAINER = $(shell echo "reproweb_$(CXX)_$(BACKEND)_$(BUILDCHAIN)" | sed 's/++/pp/')
IMAGE = littlemole/$(CONTAINER)
BASE_IMAGE = littlemole\/$(BASE_CONTAINER)

DEBUG = -g -DMOL_PROMISE_DEBUG
release : override DEBUG = -O3

LIBEVENT=$(shell pkg-config libevent_pthreads --libs)
PKG_INC=$(shell pkg-config cryptoneat reprocpp --cflags)
PKG_LIB=$(shell pkg-config jsoncpp cryptoneat libnghttp2 sqlite3 zlib libcurl hiredis --libs)

# if not g++ we assume clang++
DEFAULT_LIBS = -stdlib=libc++ -fcoroutines-ts  -lc++abi -std=c++14 
DEFAULT_OPTS = -std=c++14 -stdlib=libc++ -fcoroutines-ts -D_RESUMABLE_FUNCTIONS_SUPPORTED 

ifeq ($(CXX),g++)
DEFAULT_OPTS = -std=c++14 
DEFAULT_LIBS = 
endif

#backend switch libevent/boost_asio
PROMISE_IMPL = -DPROMISE_USE_LIBEVENT
PROMISE_3DPARTY = $(LIBEVENT) $(DEFAULT_LIBS)

ifeq ($(BACKEND),boost_asio)
        PROMISE_IMPL = -DPROMISE_USE_BOOST_ASIO
        PROMISE_3DPARTY = -lboost_system $(DEFAULT_LIBS)
endif

#final cpp options
OPTIONS = -g -fpic -Wno-write-strings -pthread -D_REENTRANT $(DEFAULT_OPTS) $(PKG_INC)
CPPFLAGS = -Wall -I../include $(DEBUG) $(OPTIONS) $(PROMISE_IMPL)

# target
LIB = -lreprowebd -lpriohttpd  -lreprocurld -lreproredisd -lreprosqlited -lpriocppd
release : override LIB = -lreproweb -lpriohttp  -lreprocurl -lreproredis -lreprosqlite -lpriocpp

TEST_SRC   = .
TEST_BUILD = ./build
TEST_LIBS  = $(LIB)  /usr/lib/libgtest.a -lpthread  $(PKG_LIB) $(PROMISE_3DPARTY) 


# Objects for the test executable
TEST_SRCFILESABS = $(shell ls $(TEST_SRC)/*.cpp)
TEST_SRCFILES =  $(notdir $(TEST_SRCFILESABS))
TEST_OBJFILES = $(TEST_SRCFILES:%.cpp=$(TEST_BUILD)/%.o)
TEST_BINS = $(TEST_SRCFILES:%.cpp=$(TEST_BUILD)/%.bin)

#################################################
# rule to compile all (default rule)
#################################################

all: $(TEST_BINS)

#################################################
# rules to compile .o files from .cpp source
#################################################

$(TEST_BUILD)/%.o: $(TEST_SRC)/%.cpp 
	-mkdir -p $(TEST_BUILD)
	$(CXX) -c $^ -o $@ $(CPPFLAGS) 


#################################################
# rules to compile the artifacts from .o files
#################################################

$(TEST_BUILD)/%.bin: $(TEST_BUILD)/%.o 
	$(CXX) $(LIB_OBJFILES) $^ $(TEST_LIBS) -o $@ 
			
#################################################
# make clean
#################################################

clean:
	-find -name "*.o" -exec rm {} \;
	-find -name "*.bin" -exec rm {} \;
	-find -name "*~" -exec rm {} \;


release : all

update-dockerfile:
	/bin/sed -i "s/FROM .*/FROM ${BASE_IMAGE}/" Dockerfile

build: update-dockerfile ## run the docker image and open a shell
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose build

up: update-dockerfile ## run the docker image and open a shell
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose up -d

stop: ## stop running docker image, if any
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose stop

down: ## stop running docker image, if any
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose down
	
.PHONY: build