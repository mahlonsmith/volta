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
 *
 */

#include "volta.h"

int
main( int argc, char *argv[] ) {

	int opt;

	/* storage for line received from squid */
	char line[ LINE_BUFSIZE ];

	while ( (opt = getopt( argc, argv, "a:hv" )) != -1 ) {
		switch ( opt ) {
			case 'a':
				printf( "a -> '%s' (no-op at the moment)\n", optarg );
				break;
			case 'h':
				usage( argv[0] );
				return( 0 );
			case 'v':
				printf( "%s version %s\n", PROG, VERSION );
				return( 0 );
			case '?':
			default:
				break;
		}
	}
	argc -= optind;
	argv += optind;

	/* start stdin line loop */
	while( fgets( line, LINE_BUFSIZE, stdin ) != NULL ) parse( line );

	/* stdin closed */
    debug( "End of stream, shutting down.\n" );
    return( 0 );
}

/*
 * Basic usage
 */
void
usage( char *prg )
{
	printf( "%s [-vh] [-a <init>]\n", prg );
	printf( "    -v Display version\n" );
	printf( "    -h Usage (you're lookin' at it)\n" );
	printf( "    -a Perform an action:\n" );
	printf( "         init: Initialize a new database\n" );
	printf( "\n" );
	return;
}


/*
 * Debug function, only output to stderr if -DDEBUG set
 */
void
debug( const char *fmt, ... )
{
    va_list args;
	va_start( args, fmt );
#ifdef DEBUG
	fprintf( stderr, "%s %d: ", PROG, getpid() );
	vfprintf( stderr, fmt, args );
#endif
	va_end( args );
}

