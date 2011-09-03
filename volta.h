
#ifndef _VOLTA_H
#define _VOLTA_H

#ifndef PROG
#define PROG    "volta"
#endif
#define VERSION "$Version$" /* expanded by mercurial */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sqlite3.h>


/* Maximum length per line from Squid */
#define LINE_BUFSIZE 2048

/* Aid debugging */
#define LOC __FILE__, __LINE__
/* Global debug toggle */
extern unsigned short int debugmode;

/* The parsed attributes from the request line, as given to us by squid.
 * URL <SP> client_ip "/" fqdn <SP> user <SP> method [<SP> kvpairs]<NL> */
typedef struct request {
	char   *url;
	char   *host;
	struct sockaddr_in ip;
	char   *ip_fqdn;
	char   *user;
	char   *method;
	char   *kvpairs;
} request;

/* An "empty" request struct used for re-assignment */
static const struct request reset_request;

/* Function prototypes */
void usage( char *prg );
void debug( int level, char *file, int line, const char *fmt, ... );
int  parse( char *p );

#endif

