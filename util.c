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

/*
 * Output basic usage information.
 */
void
usage( char *prg )
{
	printf( "%s [-vh] [-f <filename>] [-d <level>]\n", prg );
	printf( "    -v Display version\n" );
	printf( "    -d <level> Show debug information on stderr\n" );
	printf( "    -h Usage (you're lookin' at it)\n" );
	printf( "    -f <filename> Specify the database to use (default is 'volta.db' in the cwd.)\n");
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
	fprintf( stderr, "%s [%s] #%d (%s:%04d): ",
			 PROG, timestamp, getpid(), file, line );
	vfprintf( stderr, fmt, args );
	va_end( args );

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

	debug( 3, LOC, "Extending line to %d bytes at offset %d...\n", new_len, offset );
	if ( (line_realloc = realloc(line, sizeof(char) * new_len)) == NULL ) {
		/* cleanup on allocation errors */
		debug( 3, LOC, "Ignoring line, error while allocating memory: %s\n", strerror(errno) );
		if ( line != NULL ) free( line );
		line = NULL;
	}
	else {
		line = line_realloc;
		memcpy( line + offset, buf, LINE_BUFSIZE - 1 );
	}

	return( line );
}


/*
 * Read an entire file into memory, returning a pointer the contents.
 * Returns NULL on error.
 *
 */
char *
slurp_file( char *file )
{
	FILE *fh       = NULL;
	char *contents = NULL;
	struct stat sb;

	if ( stat( file, &sb ) != 0 ) {
		debug( 1, LOC, "Unable to stat() file '%s': %s\n",
				file, strerror(errno) );
		return( NULL );
	}

	if ( (contents = malloc( sb.st_size + 1 )) == NULL ) {
		debug( 1, LOC, "Unable to allocate memory for file '%s': %s\n",
				file, strerror(errno) );
		return( NULL );
	}

	if ( (fh = fopen( file, "r" )) == NULL ) {
		debug( 1, LOC, "Could not open file for reading '%s': %s\n",
				file, strerror(errno) );
		return( NULL );
	}

	if ( fread( contents, sizeof(char), sb.st_size, fh ) != sb.st_size ) {
		debug( 1, LOC, "Short read for file '%s'?: %s\n", file );
		fclose( fh );
		return( NULL );
	}

	fclose( fh );
	return( contents );
}



