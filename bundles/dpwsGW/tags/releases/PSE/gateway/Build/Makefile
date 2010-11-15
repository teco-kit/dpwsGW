PLATFORM:=pPart
SRCPATH=../src/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard ../src/*.c) $(wildcard ../src/platform/$(PLATFORM)/*.c)
OBJ_FILES=$(subst $(SRCPATH),,$(SRC_FILES:.c=.o))


SERVICES=SensorValuesService
DEVICES=SSimpDevice
CFLAGS=-O0 -g3
CPPFLAGS=-I ../../edu.teco.dpws.generator/shared/include  $(addsuffix /src-gen,$(addprefix -I../../,$(SERVICES)) $(addsuffix /src-gen,$(addprefix -I../../,$(DEVICES))))\
-DWITH_MUTEXES -D_GNU_SOURCE -DDPWS_DEVICE -DWITH_NONAMESPACES -DDEBUG -DABORT_ON_ASSERT
LDFLAGS=-L ../../edu.teco.dpws.generator/shared/build $(addsuffix /Build,$(addprefix -L../../,$(SERVICES))) $(addsuffix /Build,$(addprefix -L../../,$(DEVICES))) -L/usr/local/lib/ws4d-gSOAP

LIBS=$(DEVICES) $(SERVICES) dpwsS dpwsDmt ws4d_listLmt bitsio_buf gsoap pthread
LDLIBS=$(addprefix -l,$(LIBS))

include  platform/$(PLATFORM)/pre

$(PLATFORM)_gateway: main
	cp $< $@

main: $(OBJ_FILES)

clean:
	rm $(OBJ_FILES) main

platform/$(PLATFORM)/pre:
	mkdir -p `dirname $@`
	touch $@	
