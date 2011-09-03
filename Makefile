
CFLAGS       = -O2
LIBS         = -lsqlite3
#OBJS         = $(patsubst %.c,%.o,$(wildcard *.c)) parser.o
OBJS         =  volta.o parser.o

########################################################################
### M A I N
########################################################################

volta: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJS)
	strip $@

$(OBJS): volta.h

parser.c: parser.rl
	ragel -L -C -e -G2 parser.rl -o $@


########################################################################
### D E B U G
########################################################################

debug: CFLAGS = -Wall -DDEBUG -DPROG='"volta (debugmode)"'
debug: volta parser_state.xml parser_state.png parser_state.dot

parser_state.xml parser_state.png parser_state.dot: parser.rl
	ragel -Vp parser.rl > parser_state.dot
	ragel -C -e -G2 -x parser.rl -o parser_state.xml
	dot -Tpng parser_state.dot > parser_state.png


########################################################################
### U T I L
########################################################################

.PHONY : clean
clean:
	-rm -f volta volta_debug* parser.c parser_state.* *.o

