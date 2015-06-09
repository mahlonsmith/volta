/* vim: set noet nosta sw=4 ts=4 ft=c : */
/*
Copyright (c) 2011-2015, Mahlon E. Smith <mahlon@martini.nu>
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


/*
 * Output basic usage information.
 */
void
usage( char *prg )
{
	printf( "%s [-vh] [-f <filename>] [-d <level>]\n", prg );
	printf( "    -d <level> Show debug information on stderr\n" );
	printf( "    -c <filename> Create the volta database from a rules file\n" );
	printf( "    -f <filename> Specify the database file to use (default is './volta.db')\n");
	printf( "    -h Usage (you're lookin' at it)\n" );
	printf( "    -v Display version\n" );
	printf( "\n" );
	return;
}


/*
 * Debug function, only output to stderr if the debug level is
 * equal or greater to the set output level.
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
	if ( v.debugmode < level ) return;

	char timestamp[20];
	time_t t = time( NULL );
	struct tm *now = localtime( &t );
	strftime( timestamp, 20, "%F %T", now );

	va_list args;
	va_start( args, fmt );
	fprintf( stderr, "%s [%s] #%d %d (%s:%d): ",
			 PROG, timestamp, getpid(), level, file, line );
	vfprintf( stderr, fmt, args );
	va_end( args );
	fflush( stderr );

	return;
}


/*
 * Given a string, reverse it in place.
 */
void
reverse_str( char *str )
{
	int i = 0;
	int tmp = 0;
	int j = strlen( str ) - 1;

	while ( i < j ) {
		tmp    = str[i];
		str[i] = str[j];
		str[j] = tmp;

		i++;
		j--;
	}

	return;
}


/*
 * Lowercase a string in place.
 *
 */
void
lowercase_str( char *str, unsigned short int len )
{
	unsigned short int i = 0;
	char c;

	for ( ; i < len; i++ ) {
		c = str[i];
		str[i] = tolower( c );
	}

	return;
}


/*
 * Append 'buf' to the end of 'line', a la strcat, except dynamically
 * grow memory for the target string.
 *
 * 'buf' should be null terminated.  Returns the modified line.
 */
char *
extend_line( char *line, const char *buf )
{
	char *line_realloc;
	unsigned short int offset;
	size_t new_len;

	/* find offset and lengths, first assignment */
	if ( line == NULL ) {
		offset  = 0;
		new_len = strlen( buf );
	}
	/* find offset and lengths, append to existing string */
	else {
		offset  = strlen( line ); /* not including '\0' */
		new_len = offset + LINE_BUFSIZE;
	}

	debug( 5, LOC, "Extending line %d to %d bytes at offset %d\n", v.timer.lines+1, new_len, offset );
	if ( new_len > LINE_MAX || (line_realloc = realloc(line, sizeof(char) * new_len)) == NULL ) {
		debug( 5, LOC, "Ignoring line %d, error while allocating memory: %s\n",
				v.timer.lines+1, (line_realloc == NULL ? strerror(errno) : "Line too large") );
		free( line ), line = NULL;
		printf( "\n" );
		fflush( stdout );
	}
	else {
		line = line_realloc;
		memcpy( line + offset, buf, LINE_BUFSIZE );
	}

	return( line );
}


/* 
 * Allocate memory and copy +length+ bytes (plus 1 for null) from the given
 * +string+ into a new string, returning a pointer to it.
 * 
 */
char *
copy_string_token( char *string, unsigned short int length )
{
	char *alloc_ptr = NULL;
	if ( string == NULL || length == 0 ) return ( NULL );

	if ( (alloc_ptr = calloc( length + 1, sizeof(char) )) == NULL ) {
		debug( 5, LOC, "Unable to allocate memory for token: %s\n", strerror(errno) );
		return( NULL );
	}

	(void)memcpy( alloc_ptr, string, length );

	return( alloc_ptr );
}


/* 
 * Allocate memory and copy +length+ bytes from the given dotted quad style
 * +ip_string+ into an in_addr struct, returning a pointer to it.
 * 
 */
/*
struct in_addr *
copy_ipv4_token( char *ip_string, unsigned short int length )
{
	struct in_addr *alloc_ptr = NULL;
	char c_ip[ INET_ADDRSTRLEN ];

	if ( ip_string == NULL ) return ( NULL );

	(void)strncpy( c_ip, ip_string, length );
	c_ip[ length ] = '\0';

	if ( (alloc_ptr = calloc( length, sizeof(struct in_addr) )) == NULL ) {
		debug( 5, LOC, "Unable to allocate memory for ip '%s': %s\n",
				c_ip, strerror(errno) );
	}

	if ( inet_pton( AF_INET, c_ip, alloc_ptr ) < 1 ) {
		debug( 1, LOC, "Unable to create in_addr struct for client ip '%s': %s\n",
				c_ip, strerror(errno) );
		free( alloc_ptr ), alloc_ptr = NULL;
	}

	return( alloc_ptr );
}
*/

/*
 * Report how many lines were processed per second.
 *
 */
void
report_speed( void )
{
	if ( v.debugmode < 3 ) return;

	time_t end_time = time( NULL );
	double elapsed  = difftime( end_time, v.timer.start );

	if ( elapsed > 0 )  {
		debug( 3, LOC, "Processed %lu lines in %0.1f seconds. (%0.1f lines/sec)\n",
				v.timer.lines, elapsed, v.timer.lines/elapsed );
	}
	else {
		debug( 3, LOC, "Processed %lu lines in under a second.\n", v.timer.lines );
	}

	return;
}

