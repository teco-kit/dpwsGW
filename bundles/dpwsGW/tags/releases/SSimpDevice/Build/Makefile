SRCPATH=../src-gen/
VPATH=$(SRCPATH)

SRC_FILES=$(wildcard $(SRCPATH)*.c)
OBJ_FILES=$(subst $(SRCPATH),,$(SRC_FILES:.c=.o))

LIBNAME=SSimpDevice

CFLAGS=-O0 -g3

CPPFLAGS=-I ../../edu.teco.dpws.generator/shared/include  $(addsuffix /src-gen,$(addprefix -I../../,$(SERVICES)) $(addsuffix /src-gen,$(addprefix -I../../,$(DEVICES))))\
-DWITH_MUTEXES -D_GNU_SOURCE -DDPWS_DEVICE -DWITH_NONAMESPACES -DDEBUG -DABORT_ON_ASSERT
#LDFLAGS=-L ../../edu.teco.dpws.generator/shared/build $(addsuffix /Debug,$(addprefix -L../../,$(SERVICES))) $(addsuffix /Debug,$(addprefix -L../../,$(DEVICES))) -L/usr/local/lib/ws4d-gSOAP

#LIBS=$(DEVICES) $(SERVICES) dpwsS dpwsDmt ws4d_listLmt bitsio_buf gsoap pthread
#LDLIBS=$(addprefix -l,$(LIBS))

AR=ar 
RANLIB=ranlib

lib$(LIBNAME).a: $(OBJ_FILES)
	$(AR) cru $@ $^
	$(RANLIB) $@


clean:
	rm $(OBJ_FILES) lib$(LIBNAME).a
