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

#include "volta.h"
#include "db.h"

struct v_globals v;

/*
 * Parse command line options, perform actions, and enter accept loop.
 *
 */
int
main( int argc, char *argv[] )
{
#ifdef DEBUG
	/* debugmode set at compile time?  default to crankin' up the level */
	v.debugmode = 99;
#else
	v.debugmode = 0;
#endif

	/* defaults and global initalizations */
	strcpy( v.dbname, "volta.db" );
	char create_db[30]   = "";
	v.timer.db_lastcheck = 0;
	v.timer.start        = time( NULL );
	v.timer.lines        = 0;

	/* get_opt vars */
	int opt = 0;
	extern char *optarg;
	extern int opterr, optind, optopt;

	/* parse options */
	opterr = 0;
	while ( (opt = getopt( argc, argv, "c:d:f:hv" )) != -1 ) {
		switch ( opt ) {

			/* create a new cdb file from ascii input */
			case 'c':
				strncpy( create_db, optarg, sizeof(create_db) );
				break;

			/* debug */
			case 'd':
				if ( optarg[2] == '-' ) {
					argc++; argv -= 1;
					v.debugmode = 1;
				}
				sscanf( optarg, "%hu", &v.debugmode );
				break;

			/* database filename */
			case 'f':
				strncpy( v.dbname, optarg, sizeof(v.dbname) );
				break;

			/* help */
			case 'h':
				usage( argv[0] );
				return( 0 );

			/* version */
			case 'v':
				printf( "%s %s\n", PROG, VERSION );
				return( 0 );

			/* unknown option or option argument missing */
			case '?':
				switch( optopt ) {
					case 'd': /* no debug argument, default to level 1 */
						v.debugmode = 1;
						break;
					default:
						usage( argv[0] );
						return( 1 );
				}

			default:
				break;
		}
	}
	argc -= optind;
	argv += optind;

	/* create a new database if requested */
	if ( (int)strlen(create_db) > 0 ) {
		return( db_create_new( create_db ) );
	}

	/* enter stdin parsing loop */
	(void)signal( SIGINT, shutdown_handler );
	unsigned char exitval = accept_loop();
	shutdown_actions();
	return( exitval );
}


/*
 * Perform actions in preparation for a graceful shutdown.
 *
 */
void
shutdown_actions( void )
{
	close( v.db_fd );
	report_speed();
}


/*
 * Signal handler for shutting things down.
 *
 */
void
shutdown_handler( int sig )
{
	debug( 1, LOC, "Exiting via signal %d.\n", sig );
	shutdown_actions();
	exit( 0 );
}

