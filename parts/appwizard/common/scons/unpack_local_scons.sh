#!/bin/sh

BOLD="\033[1m"
RED="\033[91m"
GREEN="\033[92m"
YELLOW="\033[93m"
CYAN="\033[96m"
NORMAL="\033[0m"

#bunzip2 scons-mini.tar.bz2
#tar xvf scons-mini.tar

# if kde is there a recent tar should be too
tar xjvf scons-mini.tar.bz2

cat > Makefile << EOF
## Makefile automatically generated by unpack_local_scons.sh

### To use scons when installed globally use
#SCONS=scons
### To run the local version instead, define
SCONS=./scons

# scons            : compile
# scons -c         : clean
# scons install    : install
# scons -c install : uninstall and clean

# default target : use scons to build the programs
all:
	\$(SCONS) -Q

### There are several possibilities to help debugging :
# scons --debug=explain, scons --debug=tree ..
#
### To optimize the runtime, use
# scons --max-drift=1 --implicit-deps-unchanged
debug:
	\$(SCONS) -Q --debug=tree

clean:
	\$(SCONS) -c

install:
	\$(SCONS) install

uninstall:
	\$(SCONS) -c install

## this target creates a tarball of the project
dist:
	\$(SCONS) dist
EOF

echo ""
echo -e $GREEN"A minimum scons distribution has been unpacked right here"
echo -e $GREEN"you can now run : "$NORMAL
echo -e $BOLD"./scons"$NORMAL
echo -e $BOLD"./scons install$NORMAL (as root probably)"

