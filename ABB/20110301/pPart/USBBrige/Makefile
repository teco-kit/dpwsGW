PBS=../ParticleBaseSystem

BOARD?=pc232
MAC?=5

TARGET=usb_bridge
IGNORE_WARNINGS?=216

.PHONY: build install

SUFFIX=$(BOARD)_V$(MAC)
SUFFIX_DEFINES=BOARD MAC

HEXFILE=$(TARGET)_$(SUFFIX).hex

build:$(HEXFILE)
include $(PBS)/Mk/ccsc.mk



%_$(SUFFIX).c:%.c
	@printf '$(foreach D,$(SUFFIX_DEFINES),#define $(D) $(value $(D))\n)#IGNORE_WARNINGS %s\n#include "%s"\n' "$(IGNORE_WARNINGS)" $< >$(@)

.PRECIOUS: %_$(SUFFIX).c

realclean: clean
	@rm -f $(HEXFILE:.hex=.c) 

distclean: realclean
	@echo "deleting (CTRL-C to interupt):";echo `svn status --no-ignore| awk '$$1=="?"||$$1=="I"{print $$2}'`
	@sleep 3
	@svn status --no-ignore| awk '$$1=="?"||$$1=="I"{print $$2}'|xargs rm -rf

install:$(HEXFILE)
