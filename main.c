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
	/* debugmode set at compile time,
	 * default to display everything */
	v.debugmode = 99;
#else
	v.debugmode = 0;
#endif

	/* default database file name */
	v.db = NULL;
	strcpy( v.dbname, "volta.db" );

	/* get_opt vars */
	int opt = 0;
	extern char *optarg;
	extern int opterr, optind, optopt;

	/* parse options */
	opterr = 0;
	while ( (opt = getopt( argc, argv, "d:f:hv" )) != -1 ) {
		switch ( opt ) {

			/* database filename */
			case 'f':
				strncpy( v.dbname, optarg, sizeof(v.dbname) );
				break;

			/* debug */
			case 'd':
				if ( optarg[2] == '-' ) {
					argc++; argv -= 1;
					v.debugmode = 1;
				}
				sscanf( optarg, "%hu", &v.debugmode );
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

	/* get the initial database handle or bomb immediately. */
	if ( db_attach() != SQLITE_OK ) exit( 1 );

	/* enter stdin parsing loop */
	return accept_loop();
}

