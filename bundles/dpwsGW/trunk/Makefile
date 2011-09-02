RELEASE_SUFFIX=
RELEASE_BASE?=https://svn.teco.edu/svn/bundles/dpwsGW/tags/releases/ABB/$(shell date +%y%m%d)$(RELEASE_SUFFIX)

BIN_DIRS=$(shell svn status|awk '/^\\?.*bin$$/{print $$2}')
BINS=$(shell find $(BIN_DIRS))

echo:
	echo $(BINS)


bin-$(HOSTTYPE): $(shell find $(BIN_DIRS))
	rm -rf $@
	mkdir -p $@
	for d in $(BIN_DIRS);do\
		x=$${d%%/bin};\
		cp -R $$d $@/$${x/\//_};\
	done

release:
	svn cp . $(RELEASE_BASE) -m"make release"
	svn -m "adding binaries for release" --config-option config:miscellany:global-ignores="" import  bin $(RELEASE_BASE)/bin;\

unrelease:
	svn delete $(RELEASE_BASE) -m"undo release"
unrelease-bin:
	svn delete $(RELEASE_BASE)/bin -m"undo release"
