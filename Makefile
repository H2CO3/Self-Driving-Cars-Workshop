OS = $(shell uname | tr '[[:upper:]]' '[[:lower:]]')
CXFLAGS = -std=c++14 -O3 -flto

ifeq ($(OS),darwin)
	CXX = xcrun -sdk macosx clang++
	PKGCONF = $(shell PKG_CONFIG_PATH=/usr/local/opt/opencv3/lib/pkgconfig pkg-config --cflags --libs opencv)
else
	CXX = g++
	PKGCONF = $(shell pkg-config --cflags --libs opencv)
endif


all:
	$(CXX) $(CXFLAGS) $(PKGCONF) -o lane_detection lane_detection.cc

clean:
	rm -rf lane_detection .DS_Store
