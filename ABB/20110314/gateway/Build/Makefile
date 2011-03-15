PLATFORM:=ip
TOPDIR=$(dir $(firstword $(MAKEFILE_LIST)))



ifeq ($(PLATFORM),microstrain)
DEVICES=AccelerationModel_static
endif

DEVICES?=SSimpDevice
include $(TOPDIR)../../$(DEVICES)/Build/SERVICES.mk

ifeq ($(TOPDIR),./)
subdir:
	$(foreach service,$(SERVICES),$(MAKE) -C  $(TOPDIR)../../$(service)/Build/)
	$(MAKE) -C  $(TOPDIR)../../$(DEVICES)/Build/
	$(MAKE) -C $(PLATFORM) -f ../$(firstword $(MAKEFILE_LIST))
endif

SRCPATH=$(TOPDIR)../src/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard $(SRCPATH)/*.c) $(wildcard $(SRCPATH)/platform/$(PLATFORM)/*.c)
OBJ_FILES=$(filter-out main.o,$(subst $(SRCPATH)/,,$(SRC_FILES:.c=.o)))

SHAREDDIR?=$(TOPDIR)../../edu.teco.dpws.generator/shared/
WS4DDIR?=$(TOPDIR)../../../DPWS2/ws4d-gsoap-build/lib

CFLAGS=-O0 -g3
CPPFLAGS=-I $(SHAREDDIR)include  $(addsuffix /$(SRCDIR),$(addprefix -I$(TOPDIR)../../,$(SERVICES)) $(addsuffix /$(SRCDIR),$(addprefix -I$(TOPDIR)../../,$(DEVICES))))\
-DWITH_MUTEXES -D_GNU_SOURCE -DDPWS_DEVICE -DWITH_NONAMESPACES -DDEBUG -DABORT_ON_ASSERT -D$(shell echo $(PLATFORM)|tr '[a-z]' '[A-Z]')
LDFLAGS=-L/usr/local/lib -L $(SHAREDDIR)build $(addsuffix /Build,$(addprefix -L$(TOPDIR)../../,$(SERVICES))) $(addsuffix /Build,$(addprefix -L$(TOPDIR)../../,$(DEVICES))) -L $(WS4DDIR) 

LIBS=$(DEVICES) $(SERVICES) dpwsS dpwsDmt ws4d_listLmt bitsio_buf gsoap pthread
LDLIBS=$(addprefix -l,$(LIBS))

include  $(TOPDIR)$(PLATFORM)/platform/$(PLATFORM)/pre

$(PLATFORM)_gateway: $(TOPDIR)../../$(DEVICES)/Build/lib$(DEVICES).a $(foreach service,$(SERVICES),$(TOPDIR)../../$(service)/Build/lib$(service).a) $(OBJ_FILES) $(PLATFORM)_gateway.c  

$(PLATFORM)_gateway.c:
	echo "#include \"$(SRCPATH)main.c\"" >$@


clean:
	rm $(OBJ_FILES) main

$(PLATFORM)/platform/$(PLATFORM)/pre:
	mkdir -p `dirname $@`
	touch $@
