PLATFORM:=ip
SRCPATH=../src/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard ../src/*.c) $(wildcard ../src/platform/$(PLATFORM)/*.c)
OBJ_FILES=$(subst $(SRCPATH),,$(SRC_FILES:.c=.o))


SERVICES=SensorValuesService
DEVICES=SSimpDevice
CPPFLAGS=-I ../../edu.teco.dpws.generator/shared/include  $(addsuffix /src-gen,$(addprefix -I../../,$(SERVICES)) $(addsuffix /src-gen,$(addprefix -I../../,$(DEVICES))))
LDFLAGS=-L ../../edu.teco.dpws.generator/shared/build $(addsuffix /Debug,$(addprefix -L../../,$(SERVICES))) $(addsuffix /Debug,$(addprefix -L../../,$(DEVICES))) -L/usr/local/lib/ws4d-gSOAP

LIBS=$(DEVICES) $(SERVICES) dpwsS dpwsDmt ws4d_listLmt bitsio_buf gsoap pthread
LDLIBS=$(addprefix -l,$(LIBS))

include  platform/$(PLATFORM)/pre

main: $(OBJ_FILES)

platform/$(PLATFORM)/pre:
	mkdir -p `dirname $@`
	touch $@	
