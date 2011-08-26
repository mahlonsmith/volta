
CFLAGS=-O2 -Wall
CFLAGS_DEBUG=-Wall -DDEBUG -DPROG='"volta (debugmode)"'
LIBS=-lsqlite3

volta: parser.c volta.c volta.h
	$(CC) $(CFLAGS) $(LIBS) -o $@ *.c
	strip $@

parser.c: parser.rl
	ragel -L -C -e -G2 parser.rl -o $@

debug: volta_debug

volta_debug: parser_debug.c volta.h
	$(CC) $(CFLAGS_DEBUG) $(LIBS) -o volta *.c

parser_debug.c: parser.c
	ragel -V parser.rl > parser_state.dot
	ragel -C -e -G2 -V -x parser.rl -o parser_state.xml
	dot -Tpng parser_state.dot > parser_state.png

clean:
	@rm -rf volta volta_debug* parser.c parser_state.*

