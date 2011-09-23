DEPS         = sqlite3
DEPS_DEBUG   = sqlite3 libprofiler
CFLAGS       = $(shell pkg-config --cflags-only-I --libs-only-L $(DEPS)) -O2
LIBS         = $(shell pkg-config --libs-only-l $(DEPS))
#OBJS         = $(patsubst %.c,%.o,$(wildcard *.c)) parser.o
OBJS         = accept_loop.o db.o main.o parser.o util.o
RAGEL_FLAGS  = -LCe -G2

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

debug: CFLAGS = -ggdb -Wall -DDEBUG -DPROG='"volta (debugmode)"'\
	$(shell pkg-config --cflags-only-I --libs-only-L $(DEPS_DEBUG))
debug: LIBS = $(shell pkg-config --libs-only-l $(DEPS_DEBUG))
debug: volta-debug parser_graph.xml parser_graph.pdf parser_graph.dot

volta-debug: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o volta $(OBJS)

parser_graph.xml parser_graph.pdf parser_graph.dot: parser.rl
#	ragel -Vp parser.rl > parser_graph.dot
#	ragel $(RAGEL_FLAGS) -x parser.rl -o parser_graph.xml
#	dot -Tpdf parser_graph.dot > parser_graph.pdf

# export CPUPROFILE="cpu.prof" before running volta for cpu profiling
# export CPUPROFILE_FREQUENCY=100 (default)
profile:
	pprof --dot ./volta $(CPUPROFILE) | dot -Tpng > $(CPUPROFILE).png
	pprof --text ./volta $(CPUPROFILE)


########################################################################
### U T I L
########################################################################

.PHONY : clean cleanall

cleanall: clean
	rm -f parser.c volta.db

clean:
	-rm -f volta volta_debug* parser_graph.* *.o *.prof*

