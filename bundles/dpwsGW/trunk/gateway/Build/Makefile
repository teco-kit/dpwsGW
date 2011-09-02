PLATFORM?=ip pPart microstrain_glink microstrain_vlink 
TOPDIR=$(dir $(firstword $(MAKEFILE_LIST)))

include $(foreach platform,$(PLATFORM),$(TOPDIR)$(platform).mk)
include $(foreach dev,$(DEVICES),$(TOPDIR)../../$(dev)/Build/SERVICES.mk)

SHAREDDIR?=$(TOPDIR)../../edu.teco.dpws.generator/shared/
WS4DDIR?=$(TOPDIR)../../contrib/ws4d-gsoap-build/lib
DPWSLIBS=dpwsS dpwsDmt ws4d_listLmt

ifeq ($(TOPDIR),./)
all:
ifndef NODEP
	$(foreach service,$(SERVICES),$(MAKE) -C  $(TOPDIR)../../$(service)/Build/ &&) true
	$(foreach device,$(DEVICES),$(MAKE) -C  $(TOPDIR)../../$(device)/Build/ &&) true
ifndef NOXDEP
	$(MAKE) -C $(WS4DDIR)/.. $(DPWSLIBS)
endif
endif
	$(foreach platform,$(PLATFORM),$(MAKE) PLATFORM=$(platform) -C $(platform) -f ../$(firstword $(MAKEFILE_LIST))&&) true
	$(RM) -rf $(TOPDIR)../bin
	mkdir $(TOPDIR)../bin
	cp $(foreach platform,$(PLATFORM),$(platform)/$(platform)_gateway) $(TOPDIR)../bin
	cp $(TOPDIR)../default.ini $(TOPDIR)../bin/gateway.ini

include $(foreach platform,$(PLATFORM),$(TOPDIR)$(platform)/platform/$(firstword $(subst _, ,$(platform)))/pre)

else


SRCPATH=$(TOPDIR)../src/
VPATH=$(SRCPATH) $(TOPDIR)../../contrib/minini/dev

SRC_FILES=$(wildcard $(SRCPATH)/*.c) $(wildcard $(SRCPATH)/platform/$(firstword $(subst _, ,$(PLATFORM)))/*.c)

OBJ_FILES=$(filter-out main.o,$(subst $(SRCPATH)/,,$(SRC_FILES:.c=.o))) minIni.o


CFLAGS=-O0 -g3
CPPFLAGS=-I $(SHAREDDIR)include -I$(TOPDIR)../shared/include   $(addsuffix /$(SRCDIR),$(addprefix -I$(TOPDIR)../../,$(SERVICES)) $(addsuffix /$(SRCDIR),$(addprefix -I$(TOPDIR)../../,$(DEVICES))))\
-DWITH_MUTEXES -D_GNU_SOURCE -DDPWS_DEVICE -DWITH_NONAMESPACES -DDEBUG -DABORT_ON_ASSERT -D$(shell echo $(PLATFORM)|tr '[a-z]' '[A-Z]')
LDFLAGS=-L/usr/local/lib -L $(SHAREDDIR)build $(addsuffix /Build,$(addprefix -L$(TOPDIR)../../,$(SERVICES))) $(addsuffix /Build,$(addprefix -L$(TOPDIR)../../,$(DEVICES))) -L $(WS4DDIR) 


LIBS=$(DEVICES) $(SERVICES) $(DPWSLIBS)  bitsio_buf gsoap pthread
LDLIBS=$(addprefix -l,$(LIBS))


all: $(PLATFORM)_gateway

$(PLATFORM)_gateway: $(TOPDIR)../../$(DEVICES)/Build/lib$(DEVICES).a $(foreach lib,$(DPWSLIBS),$(WS4DDIR)/lib$(lib).a) $(foreach service,$(SERVICES),$(TOPDIR)../../$(service)/Build/lib$(service).a) $(OBJ_FILES) $(PLATFORM)_gateway.c  

$(PLATFORM)_gateway.c: $(SRCPATH)main.c
	echo "#include \"$^\"" >$@

endif

clean:
	$(RM) -rf $(foreach platform,$(PLATFORM),$(TOPDIR)$(platform)/)

$(foreach platform,$(PLATFORM),$(TOPDIR)$(platform)/platform/$(firstword $(subst _, ,$(platform)))/pre):
	mkdir -p `dirname $@`
	touch $@
