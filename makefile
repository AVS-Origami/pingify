XFT_CFLAGS=$(shell pkg-config --cflags freetype2)
LDFLAGS=-lX11
XFT_LDFLAGS=$(shell pkg-config --libs freetype2) -lXft

build: pingify.cpp
	g++ $(XFT_CFLAGS) $(LDFLAGS) $(XFT_LDFLAGS) pingify.cpp -o pingify
