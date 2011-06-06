RELEASE_SUFFIX=
RELEASE_BASE?=https://svn.teco.edu/svn/bundles/dpwsGW/tags/releases/ABB/$(shell date +%y%m%d)$(RELEASE_SUFFIX)

BIN_DIRS=$(shell find . -type 'd' -name 'bin' 2>/dev/null|xargs svn status --depth empty|awk '/^\\?/{print $$2}')

release:
	svn cp . $(RELEASE_BASE)
	for d in $(BIN_DIRS);do\
		svn -m "adding binaries for release" import  $$d $(RELEASE_BASE)/$$d;\
	done
