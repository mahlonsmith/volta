/* vim: set noet nosta sw=4 ts=4 ft=c : */
/*
Copyright (c) 2011-2012, Mahlon E. Smith <mahlon@martini.nu>
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Mahlon E. Smith nor the names of his
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _VOLTA_H
#define _VOLTA_H

#ifndef PROG
#define PROG    "volta"
#endif
#define VERSION "$Version$" /* expanded by mercurial */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <regex.h>

/*
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
*/

#ifdef DEBUG
#include <google/profiler.h>
#endif

/* Default line size we accept from squid, longer lines (huge URLs?) malloc. */
#define LINE_BUFSIZE 2048
/* Ceiling for how many bytes can be allocated at once for a single line. */
#define LINE_MAX 256000 /* 250k */

/* Parsed line types */
#define REQUEST 1
#define RULE 2

/* Aid debugging */
#define LOC __FILE__, __LINE__

#include <cdb.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


/*
 * a global struct for easy access to common vars 
 *
 */
struct v_globals {
	unsigned short int debugmode; /* debug level */
	char dbname[128];             /* path to database file */
	short int db_fd;              /* opened db file descriptor */
	struct cdb db;                /* the cdb struct */
	lua_State *lua;               /* the lua interpreter */

	struct {
		time_t start;             /* start time */
		unsigned long int lines;  /* line count for determining speed */
		time_t db_lastcheck;      /* reopen db time */
	} timer;
};
extern struct v_globals v;        /* defined in main.c */


/*
 * A line of parsed ascii DB input, for conversion into cdb.
 *
 */
struct db_input {
	unsigned int klen;
	unsigned int vlen;
	char *key;
	char *val;
	char *kstart;
	char *vstart;
};


/*
 * The parsed attributes from the request line as given to us by squid,
 * or from the rule string found in the database.  Unparsed
 * members are just left NULL.
 *
 */
typedef struct parsed {
	unsigned short int type;
	unsigned short int negate;
	unsigned short int lua;
	char   *path_re;
	char   *redir;
	char   *scheme;
	char   *host;
	char   *tld;
	char   *path;
	char   *port;
	/* struct in_addr *client_ip; */
	char   *client_ip;
	char   *user;
	char   *method;
	char   *luapath;

	struct {
		char *path_re_start;
		char *redir_start;
		char *scheme_start;
		char *host_start;
		char *port_start;
		char *path_start;
		char *meth_start;
		char *c_ip_start;
		char *luapath_start;
		unsigned short int path_re_length;
		unsigned short int redir_length;
		unsigned short int scheme_length;
		unsigned short int host_length;
		unsigned short int port_length;
		unsigned int       path_length;
		unsigned short int meth_length;
		unsigned short int c_ip_length;
		unsigned short int luapath_length;
	} tokens;
} parsed;


/*
 *
 * Function prototypes
 *
 */
int  getopt( int, char * const [], const char *);
void shutdown_handler( int );
void shutdown_actions( void );

void usage( char * );
void debug( int, char *, int, const char *, ... );
void reverse_str( char * );
void lowercase_str( char *, unsigned short int );
void report_speed( void );
char *extend_line( char *, const char * );
char *copy_string_token( char *, unsigned short int );
/* struct in_addr *copy_ipv4_token( char *, unsigned short int ); */

int  accept_loop( void );
void process( char * );
parsed *init_parsed( void );
parsed *parse_request( char * );
parsed *parse_rule( char * );
void populate_parsed( parsed * );
void parse_tld( parsed * );
void finish_parsed( parsed * );
void reset_results( parsed **, unsigned int );
unsigned short int check_rule( parsed *, parsed * );
void pass( parsed *, parsed * );
void rewrite( parsed *, parsed * );

#endif

