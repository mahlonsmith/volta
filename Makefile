
########################################################################
### S E T U P
########################################################################

UNAME       := $(shell uname)
DEPS_DEBUG   = libprofiler
CFLAGS       = -O2
CFLAGS_DEBUG = -DDEBUG -DPROG='"volta (debugmode)"' -ggdb -ansi -Wall
LIBS         = -lcdb
OBJS         = $(patsubst %.c,%.o,$(wildcard *.c))

.PHONY : parsegraph profile clean clobber release


########################################################################
### P L A T F O R M  T A R G E T S
########################################################################

# I hate this.  Tools like pkg-config are supposed to make finding
# libraries easy across platforms.  They only work when everyone names
# the libraries the same thing, unfortunately.  (Why name the libs with
# the version number, when pkg-config supports built in versioning??)
# And no, I'm not going to use autoconf, which just takes your build
# problems and makes a whole bunch of new baby problems for you to
# deal with.  Gaaarrgghghh.
#
# If you have problems building volta, manually pass the correct CFLAGS
# and LIBS to the 'make' command line for your platform.
#
# The following works for OSX with macports or homebrew (10.6/10.7),
# FreeBSD 8.x and 9.x, and Ubuntu 11.10 and 12.04.

# Ubuntu
#  - perftools doesn't currently register a .pc file at all
#  - lua is called 'lua5.1'
ifeq ($(UNAME), Linux)
volta: CFLAGS += -L/usr/lib -I/usr/include
volta: CFLAGS += $(shell pkg-config --cflags-only-I --libs-only-L lua5.1)
volta: LIBS   += $(shell pkg-config --libs-only-l lua5.1)
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: CFLAGS += $(CFLAGS_DEBUG)\
	$(shell pkg-config --cflags-only-I --libs-only-L lua5.1)
debug: LIBS   += $(shell pkg-config --libs-only-l lua5.1) -lprofiler

# FreeBSD
# - lua is called 'lua-5.1'
else ifeq ($(UNAME), FreeBSD)
volta: CFLAGS += -L/usr/local/lib -I/usr/local/include
volta: CFLAGS += $(shell pkg-config --cflags-only-I --libs-only-L lua-5.1)
volta: LIBS   += $(shell pkg-config --libs-only-l lua-5.1)
debug: CFLAGS += $(CFLAGS_DEBUG)\
    $(shell pkg-config --cflags-only-I --libs-only-L lua-5.1 $(DEPS_DEBUG))
debug: LIBS += $(shell pkg-config --libs-only-l lua-5.1 $(DEPS_DEBUG))

# Darwin, everyone else (best guess?)
# - lua is called 'lua', hopefully!
else
volta: CFLAGS += $(shell pkg-config --cflags-only-I --libs-only-L lua)
volta: LIBS   += $(shell pkg-config --libs-only-l lua) 
debug: CFLAGS += $(CFLAGS_DEBUG)\
    $(shell pkg-config --cflags-only-I --libs-only-L lua $(DEPS_DEBUG))
debug: LIBS += $(shell pkg-config --libs-only-l lua $(DEPS_DEBUG))
endif


# Fix parser line number display in debug mode
ifeq (,$(findstring debug,$(MAKECMDGOALS)))
	RAGEL_FLAGS = -LCe -G2
else
	RAGEL_FLAGS = -Ces -G2
endif

# Ensure the parser is included in the objs list
# (patsubst wouldn't have found it if parser.c wasn't pre-generated.)
ifneq (parser.o,$(findstring parser.o,$(OBJS)))
	OBJS += parser.o
endif


########################################################################
### B U I L D
########################################################################

volta: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	strip $@

$(OBJS): volta.h
db.o: db.h

# don't actually depend on parser.rl, so distributions don't require ragel
parser.c:
	ragel $(RAGEL_FLAGS) parser.rl -o $@

debug: $(OBJS)
	$(CC) $(CFLAGS) -o volta $(OBJS) $(LIBS)


########################################################################
### U T I L
########################################################################

parsegraph: \
	request_graph.xml request_graph.pdf request_graph.dot \
	rule_graph.xml rule_graph.pdf rule_graph.dot \
	dbinput_graph.xml dbinput_graph.pdf dbinput_graph.dot \
	tld_graph.xml tld_graph.pdf tld_graph.dot

request_graph.xml request_graph.pdf request_graph.dot \
rule_graph.xml rule_graph.pdf rule_graph.dot \
dbinput_graph.xml dbinput_graph.pdf dbinput_graph.dot \
tld_graph.xml tld_graph.pdf tld_graph.dot: parser.rl
	ragel -Vp -S request_parser parser.rl > request_graph.dot
	ragel -Vp -S rule_parser parser.rl > rule_graph.dot
	ragel -Vp -S dbinput_parser parser.rl > dbinput_graph.dot
	ragel -Vp -S tld_parser parser.rl > tld_graph.dot
	ragel $(RAGEL_FLAGS) -S request_parser -x parser.rl -o request_graph.xml
	ragel $(RAGEL_FLAGS) -S rule_parser -x parser.rl -o rule_graph.xml
	ragel $(RAGEL_FLAGS) -S dbinput_parser -x parser.rl -o dbinput_graph.xml
	ragel $(RAGEL_FLAGS) -S tld_parser -x parser.rl -o tld_graph.xml
	dot -Tpdf request_graph.dot > request_graph.pdf
	dot -Tpdf rule_graph.dot > rule_graph.pdf
	dot -Tpdf dbinput_graph.dot > dbinput_graph.pdf
	dot -Tpdf tld_graph.dot > tld_graph.pdf

# export CPUPROFILE="cpu.prof" before running volta for cpu profiling
# export CPUPROFILE_FREQUENCY=100 (default)
profile:
	pprof --dot ./volta $(CPUPROFILE) | dot -Tpng > $(CPUPROFILE).png
	pprof --text ./volta $(CPUPROFILE)

tags:
	ctags *.h *.c

clobber: clean
	rm -f parser.c volta.db ChangeLog tags

clean:
	-rm -f volta *_graph.* *.o *.prof*

# requires BSD tar
release: VERSION = $(shell hg id -t | awk '{ print $$1 }')
release: clobber parser.c
	hg log --style changelog > ChangeLog
	tar -C .. --exclude misc --exclude .\* --exclude \*.rl -s '/^volta/volta-$(VERSION)/' -czvf /tmp/volta-$(VERSION).tgz volta
	mv /tmp/volta-$(VERSION).tgz .

