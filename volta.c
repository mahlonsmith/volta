/* vim: set noet nosta sw=4 ts=4 ft=c : */
/* Squid docs:
   ---------------------------------------------------------------------------
TAG: url_rewrite_program
Specify the location of the executable for the URL rewriter.
Since they can perform almost any function there isn't one included.

For each requested URL rewriter will receive on line with the format

URL <SP> client_ip "/" fqdn <SP> user <SP> method [<SP> kvpairs]<NL>

In the future, the rewriter interface will be extended with
key=value pairs ("kvpairs" shown above).  Rewriter programs
should be prepared to receive and possibly ignore additional
whitespace-separated tokens on each input line.

And the rewriter may return a rewritten URL. The other components of
the request line does not need to be returned (ignored if they are).

The rewriter can also indicate that a client-side redirect should
be performed to the new URL. This is done by prefixing the returned
URL with "301:" (moved permanently) or 302: (moved temporarily).

By default, a URL rewriter is not used.
--------------------------------------------------------------------------- */

/*
 * TODO
 *
 * flush stdout on writes
 * empty struct not necessary?
 * inet_pton( AF_INET, *char src, dest )
 * an option to run the DB out of memory?
 * PRAGMA user_version = 1;
 *
 */

#include "volta.h"
#include <time.h>
unsigned short int debugmode;


int
main( int argc, char *argv[] ) {

	/* opt action flags */
	struct {
		unsigned short int init;
	} actions = {0};

#ifdef DEBUG
	/* debugmode set at compile time, default to display everything */
	debugmode = 99;
#else
	debugmode = 0;
#endif

	/* get_opt vars */
	int opt = 0;
	opterr  = 0;

	/* parse options */
	while ( (opt = getopt( argc, argv, "a:d:hv" )) != -1 ) {
		switch ( opt ) {

			/* action */
			case 'a':
				if ( strcmp( optarg, "init" ) == 0 ) actions.init++;
				break;

			/* debug */
			case 'd':
				if ( optarg[0] == '-' ) {
					argc++; argv -= 1;
					debugmode = 1;
				}
				sscanf( optarg, "%hu", &debugmode );
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
						debugmode = 1;
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

	/* perform actions */
	if ( actions.init ) {
		debug( 1, LOC, "init! init! init!\n" );
		return( 0 );
	}

	/* start stdin parsing loop */
	char line[ LINE_BUFSIZE ];
	debug( 1, LOC, "Waiting for input...\n" );
	while( fgets( line, LINE_BUFSIZE, stdin ) != NULL ) parse( line );

	/* stdin closed */
	debug( 1, LOC, "End of stream, shutting down.\n" );
	return( 0 );
}


/*
 * Basic usage
 */
void
usage( char *prg )
{
	printf( "%s [-vh] [-d <level>] [-a <init>]\n", prg );
	printf( "    -v Display version\n" );
	printf( "    -d <level> Show debug information on stderr\n" );
	printf( "    -h Usage (you're lookin' at it)\n" );
	printf( "    -a Perform an action, being one of:\n" );
	printf( "         init: Initialize a new, empty database\n" );
	printf( "\n" );
	return;
}


/*
 * Debug function, only output to stderr if the debug level is
 * equal or greated to the output level.
 *
 * 	level: The minimum debug level that must be set for the 
 * 	       line to be logged.
 * 	file:  The current code file that is emitting the log
 * 	line:  The line number of the code file that is emitting the log
 * 	... :  any printf style strings and formats that constitute the log message
 */
void
debug( int level, char *file, int line, const char *fmt, ... )
{
	if ( debugmode < level ) return;

	char timestamp[20];
	time_t t = time( NULL );
	struct tm *now = localtime( &t );
	strftime( timestamp, 20, "%F %T", now );

	va_list args;
	va_start( args, fmt );
	fprintf( stderr, "%s [%s] #%d (%s:%04d): ", PROG, timestamp, getpid(), file, line );
	vfprintf( stderr, fmt, args );
	va_end( args );

	return;
}

