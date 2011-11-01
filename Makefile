
########################################################################
### S E T U P
########################################################################

UNAME       := $(shell uname)
DEPS_DEBUG   = libprofiler
CFLAGS       = -O2 -DSQLITE_THREADSAFE=0 -DSQLITE_TEMP_STORE=2
CFLAGS_DEBUG = -DSQLITE_DEBUG -DDEBUG -DPROG='"volta (debugmode)"' -ggdb -ansi -Wall
LIBS         = 
OBJS         = $(patsubst %.c,%.o,$(wildcard *.c))

# not using pkg-config for sqlite3 anymore
#DEPS        = sqlite3
#CFLAGS      = -O2 -ansi $(shell pkg-config --cflags-only-I --libs-only-L $(DEPS))
#LIBS        = $(shell pkg-config --libs-only-l $(DEPS))

.PHONY : parsegraph profile clean clobber release

# Ubuntu: perftools doesn't currently register a .pc file, and
# sqlite amalgamated requires -ldl
ifeq ($(UNAME), Linux)
volta: LIBS += -ldl
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LIBS = -lprofiler -ldl
else
debug: CFLAGS += $(CFLAGS_DEBUG)\
	$(shell pkg-config --cflags-only-I --libs-only-L $(DEPS_DEBUG))
debug: LIBS = $(shell pkg-config --libs-only-l $(DEPS_DEBUG))
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

parsegraph: squidline_graph.xml squidline_graph.pdf squidline_graph.dot tld_graph.xml tld_graph.pdf tld_graph.dot
squidline_graph.xml squidline_graph.pdf squidline_graph.dot tld_graph.xml tld_graph.pdf tld_graph.dot: parser.rl
	ragel -Vp -S squidline_parser parser.rl > squidline_graph.dot
	ragel -Vp -S tld_parser parser.rl > tld_graph.dot
	ragel $(RAGEL_FLAGS) -S squidline_parser -x parser.rl -o squidline_graph.xml
	ragel $(RAGEL_FLAGS) -S tld_parser -x parser.rl -o tld_graph.xml
	dot -Tpdf squidline_graph.dot > squidline_graph.pdf
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

