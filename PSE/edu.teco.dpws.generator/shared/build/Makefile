include bitsio/dummy 

VPATH=../src


CPPFLAGS += -I ../include 
CFLAGS += -Wall -pedantic -g3 -O0

BITSIO_BUF_OBJ = bitsio/read_bits_buf.o bitsio/write_bits_buf.o  

LIB_BUF = libbitsio_buf.a 

all: ${LIB} ${LIB_BUF}

${LIB_BUF}: ${BITSIO_BUF_OBJ}
	ar r $@ $?
	ranlib $@
	
clean:
	rm -rf bitsio ${LIB_BUF}
bitsio/dummy:
	mkdir -p bitsio
	touch $@ 
