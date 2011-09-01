PBS=../ParticleBaseSystem

BOARD?=pc232
#SENSORBOARD?=dummy
SENSORBOARD?=ssimp202
#SENSORBOARD?=cateyeclip10
MAC?=5

TARGET=prs_app
IGNORE_WARNINGS?=216,207,201

.PHONY: install

SUFFIX=$(BOARD)_$(SENSORBOARD)_V$(MAC)
SUFFIX_DEFINES=BOARD SENSORBOARD MAC

HEXFILE=$(TARGET)_$(SUFFIX).hex

CFLAGS+=I+=../../SensorValuesService/src-gen/ I+=../../edu.teco.dpws.generator/shared/include/ I+=../../edu.teco.dpws.generator/shared/src/ I+=../../edu.teco.decenc/bitsio/ #+Y=3

bin/$(HEXFILE): $(HEXFILE)
	cp $< $@

all: build

build:$(HEXFILE)
include $(PBS)/Mk/ccsc.mk

#$(HEXFILE): FORCE


%_$(SUFFIX).c:%.c
	@printf '$(foreach D,$(SUFFIX_DEFINES),#define $(D) $(value $(D))\n)#IGNORE_WARNINGS %s\n#include "../%s"\n' "$(IGNORE_WARNINGS)" $< >$(@)

.PRECIOUS: %_$(SUFFIX).c

realclean: clean
	@rm -f $(HEXFILE:.hex=.c) 

distclean: realclean
	@echo "deleting (CTRL-C to interupt):";echo `svn status --no-ignore| awk '$$1=="?"||$$1=="I"{print $$2}'`
	@sleep 3
	@svn status --no-ignore| awk '$$1=="?"||$$1=="I"{print $$2}'|xargs rm -rf

install:$(HEXFILE)
