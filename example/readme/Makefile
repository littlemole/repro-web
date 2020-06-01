

all:
	g++ main.cpp -std=c++17 $(shell pkg-config --libs --cflags reproweb jsoncpp libnghttp2  openssl zlib libevent_pthreads)

clean:
	-rm a.out

run:
	./a.out
	
	