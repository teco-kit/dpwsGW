CPPFLAGS+=-I../include 
CXXFLAGS+=-O0 -g3
LDFLAGS+=-L../build -L/usr/local/lib
LDLIBS+=-lgtest -lgtest_main 

all: bitsio_test
test: bitsio_test
	./$<

VPATH= ../shared/bitsio/src/

bitsio_test: LDLIBS += -lbitsio_buf  
