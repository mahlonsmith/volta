
CFLAGS       = -O2
CFLAGS_DEBUG = -Wall -L /opt/local/lib -I /opt/local/include -DDEBUG -DPROG='"volta (debugmode)"'
LIBS         = -lsqlite3
LIBS_DEBUG   = -lprofiler  # requires proftools
#OBJS         = $(patsubst %.c,%.o,$(wildcard *.c)) parser.o
OBJS         = accept_loop.o db.o main.o parser.o util.o

########################################################################
### P R O D U C T I O N
########################################################################

volta: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJS)
	strip $@

$(OBJS): volta.h

# don't actually depend on parser.rl, so distributions don't require ragel
# ragel -C -T0 parser.rl -o $@
parser.c:
	ragel -L -C -e -G2 parser.rl -o $@


########################################################################
### D E B U G
########################################################################

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LIBS   += $(LIBS_DEBUG)
debug: volta-debug parser_graph.xml parser_graph.png parser_graph.dot

volta-debug: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o volta $(OBJS)

parser_graph.xml parser_graph.png parser_graph.dot: parser.rl
	ragel -Vp parser.rl > parser_graph.dot
	ragel -C -e -G2 -x parser.rl -o parser_graph.xml
	dot -Tpng parser_graph.dot > parser_graph.png

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

