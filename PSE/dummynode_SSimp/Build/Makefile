SRCPATH=../src/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard ../src/*.c) $(wildcard ../src/platform/$(PLATFORM)/*.c)
OBJ_FILES=$(subst $(SRCPATH),,$(SRC_FILES:.c=.o))

SERVICES=SensorValuesService
CFLAGS=-O0 -g3
CPPFLAGS=-I ../../edu.teco.dpws.generator/shared/include  $(addsuffix /src-gen,$(addprefix -I../../,$(SERVICES)))

LDFLAGS=-L ../../edu.teco.dpws.generator/shared/build $(addsuffix /Build,$(addprefix -L../../,$(SERVICES))) 

LDLIBS=-lm $(addprefix -l,$(SERVICES)) -lbitsio_buf

dummynode: $(OBJ_FILES)

clean:
	rm $(OBJ_FILES) main
