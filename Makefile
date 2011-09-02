RELEASE_SUFFIX=
RELEASE_BASE?=https://svn.teco.edu/svn/bundles/dpwsGW/tags/releases/ABB/$(shell date +%y%m%d)$(RELEASE_SUFFIX)

BIN_DIRS:=$(shell svn status|awk '/^\\?.*bin$$/{print $$2}')
BINS:=$(shell find $(BIN_DIRS))

bin-$(HOSTTYPE): $(BINS)
	rm -rf $@
	mkdir -p $@
	for d in $(BIN_DIRS);do\
		x=$${d%%/bin};\
		cp -R $$d $@/$${x/\//_};\
	done

release: release-base release-bin

release-base:
	svn cp . $(RELEASE_BASE) -m"make release"

release-bin: release-base
	svn -m "adding binaries for release" --config-option config:miscellany:global-ignores="" import  bin-$(HOSTTYPE) $(RELEASE_BASE)/bin-$(HOSTTYPE);\

unrelease:
	svn delete $(RELEASE_BASE) -m"undo release"

unrelease-bin:
	svn delete $(RELEASE_BASE)/ -m"undo release"
