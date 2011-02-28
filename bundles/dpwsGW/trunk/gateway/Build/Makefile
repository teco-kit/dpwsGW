PLATFORM:=ip
SRCPATH=../src/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard ../src/*.c) $(wildcard ../src/platform/$(PLATFORM)/*.c)

OBJ_FILES=$(filter-out main.o,$(subst $(SRCPATH),,$(SRC_FILES:.c=.o)))

ifeq ($(PLATFORM),microstrain)
SRCDIR=src
DEVICES=AccelerationModel_static
SERVICES=AccelerationService DataLoggingService DeviceInfoService
endif

SRCDIR?=src-gen
DEVICES?=SSimpDevice
SERVICES?=SensorValuesService

CFLAGS=-O0 -g3
CPPFLAGS=-I ../../edu.teco.dpws.generator/shared/include  $(addsuffix /$(SRCDIR),$(addprefix -I../../,$(SERVICES)) $(addsuffix /$(SRCDIR),$(addprefix -I../../,$(DEVICES))))\
-DWITH_MUTEXES -D_GNU_SOURCE -DDPWS_DEVICE -DWITH_NONAMESPACES -DDEBUG -DABORT_ON_ASSERT -D$(shell echo $(PLATFORM)|tr '[a-z]' '[A-Z]')
LDFLAGS=-L/usr/local/lib -L ../../../DPWS2/edu.teco.dpws.generator/shared/build $(addsuffix /Build,$(addprefix -L../../,$(SERVICES))) $(addsuffix /Build,$(addprefix -L../../,$(DEVICES))) -L ../../../DPWS2/ws4d-gsoap-build/lib

LIBS=$(DEVICES) $(SERVICES) dpwsS dpwsDmt ws4d_listLmt bitsio_buf gsoap pthread
LDLIBS=$(addprefix -l,$(LIBS))

include  platform/$(PLATFORM)/pre

#$(PLATFORM)_gateway: LDFLAGS:=$(OBJ_FILES) 

$(PLATFORM)_gateway:  $(OBJ_FILES) $(PLATFORM)_gateway.c  

$(PLATFORM)_gateway.c:
	echo "#include \"$(SRCPATH)main.c\"" >$@


clean:
	rm $(OBJ_FILES) main

platform/$(PLATFORM)/pre:
	mkdir -p `dirname $@`
	touch $@	
