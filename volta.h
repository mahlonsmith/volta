/* vim: set noet nosta sw=4 ts=4 ft=c : */
/*
Copyright (c) 2011, Mahlon E. Smith <mahlon@martini.nu>
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
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef DEBUG
#include <google/profiler.h>
#endif

/* Default line size we accept from squid, longer lines (huge URLs?) malloc. */
#define LINE_BUFSIZE 2048

/* Aid debugging */
#define LOC __FILE__, __LINE__

/* a global struct for easy access to common vars */
struct v_globals {
	unsigned short int debugmode; /* debug level */
	char dbname[128];             /* path to database file */
	struct sqlite3 *db;           /* database handle */
};
extern struct v_globals v;        /* defined in main.c */

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

/*
 *
 * Function prototypes
 *
 */

void usage( char *prg );
void debug( int level, char *file, int line, const char *fmt, ... );
char *slurp_file( char *file );
char *extend_line( char *line, const char *buf );

int accept_loop( void );
struct request *parse( char *p );

#endif

