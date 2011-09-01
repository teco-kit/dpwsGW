RELEASE_SUFFIX=
RELEASE_BASE?=https://svn.teco.edu/svn/bundles/dpwsGW/tags/releases/ABB/$(shell date +%y%m%d)$(RELEASE_SUFFIX)

BIN_DIRS=`find . -type 'd' -name 'bin' 2>/dev/null|grep -v edu.teco|xargs svn status --depth empty|awk '/^\\?/{print $$2}'`

release:
	rm -rf bin
	svn cp . $(RELEASE_BASE) -m"make release"
	for d in $(BIN_DIRS);do\
		mkdir -p bin;\
		x=$${d%%/bin};\
		cp -R $$d bin/$${x/\//_};\
	done
	svn -m "adding binaries for release" import  bin $(RELEASE_BASE)/bin;\
