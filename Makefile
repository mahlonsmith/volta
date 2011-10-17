UNAME       := $(shell uname)
DEPS         = sqlite3
DEPS_DEBUG   = sqlite3 libprofiler
CFLAGS       = -O2 -ansi $(shell pkg-config --cflags-only-I --libs-only-L $(DEPS))
LIBS         = $(shell pkg-config --libs-only-l $(DEPS))
#OBJS         = $(patsubst %.c,%.o,$(wildcard *.c)) parser.o
OBJS         = accept_loop.o db.o main.o parser.o process.o util.o
RAGEL_FLAGS  = -LCe -G2

.PHONY : parsegraph profile clean cleanall release

########################################################################
### P R O D U C T I O N
########################################################################

volta: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJS)
	strip $@

$(OBJS): volta.h

# don't actually depend on parser.rl, so distributions don't require ragel
parser.c:
	ragel $(RAGEL_FLAGS) -s parser.rl -o $@


########################################################################
### D E B U G
########################################################################

# proftools doesn't currently register a .pc file on Ubuntu, hence these
# Makefile gymnastics
ifeq ($(UNAME), Linux)
debug: CFLAGS = -ggdb -ansi -Wall -DDEBUG -DPROG='"volta (debugmode)"'
debug: LIBS = -lsqlite3 -lprofiler
else
debug: CFLAGS = -ggdb -ansi -Wall -DDEBUG -DPROG='"volta (debugmode)"'\
	$(shell pkg-config --cflags-only-I --libs-only-L $(DEPS_DEBUG))
debug: LIBS = $(shell pkg-config --libs-only-l $(DEPS_DEBUG))
endif

debug: RAGEL_FLAGS = -Ce -G2
debug: $(OBJS)
	ctags *.h *.c
	$(CC) $(CFLAGS) $(LIBS) -o volta $(OBJS)

parsegraph: parser_graph.xml parser_graph.pdf parser_graph.dot
parser_graph.xml parser_graph.pdf parser_graph.dot: parser.rl
	ragel -Vp parser.rl > parser_graph.dot
	ragel $(RAGEL_FLAGS) -x parser.rl -o parser_graph.xml
	dot -Tpdf parser_graph.dot > parser_graph.pdf

# export CPUPROFILE="cpu.prof" before running volta for cpu profiling
# export CPUPROFILE_FREQUENCY=100 (default)
profile:
	pprof --dot ./volta $(CPUPROFILE) | dot -Tpng > $(CPUPROFILE).png
	pprof --text ./volta $(CPUPROFILE)


########################################################################
### U T I L
########################################################################

cleanall: clean
	rm -f parser.c volta.db ChangeLog tags

clean:
	-rm -f volta volta_debug* parser_graph.* *.o *.prof*

release: VERSION = $(shell hg id -t | awk '{ print $$1 }')
release: cleanall parser.c
	hg log --style changelog > ChangeLog
	tar -C .. --exclude misc --exclude .\*  -s '/^volta/volta-$(VERSION)/' -czvf /tmp/volta-$(VERSION).tgz volta
	mv /tmp/volta-$(VERSION).tgz .

