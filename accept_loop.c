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
 * Accept and process lines from squid.
 */
int
accept_loop( void )
{
	/* incoming line (stack) */
	char buf[ LINE_BUFSIZE ];

	/* incoming line (heap) */
	char *line = NULL;

	/* the length of the incoming line buffer */
	unsigned short int bufsize;

	debug( 1, LOC, "Waiting for input...\n" );
	while ( fgets( buf, LINE_BUFSIZE, stdin ) != NULL ) {
		bufsize = strlen( buf );

		/* Common case, or last iteration of loop:
		   End of line was found without filling LINE_BUFSIZE
		 */
		if ( bufsize + 1 < LINE_BUFSIZE ) {
			/* line wasn't concatenated onto in previous loops,
			 * just pass it directly to process() */
			if ( line == NULL ) {
				process( buf );
			}

			/* memory was previously allocated to contain the line, 
			 * append the final chunk, pass to process(), and cleanup. */
			else {
				if ( (line = extend_line( line, buf )) == NULL ) continue;
				process( line );
				free( line ), line = NULL;
			}
		}

		/* If we've filled LINE_BUFSIZE on this iteration, we were likely
		 * passed a very long URL that we'll need to allocate more memory
		 * for.  Rebuild the line in chunks for each loop iteration.
		 *
		 * If we didn't do this, you could thwart proper parsing by
		 * simply sending requests greater than LINE_BUFSIZE characters.
		 */
		else {
			if ( (line = extend_line( line, buf )) == NULL ) continue;

			/* Special case if the current buffer's last character is a
			 * newline. This means that the buffer length is exactly
			 * LINE_BUFSIZE, but we can't simply check for that, since
			 * we don't know if there are more iterations pending
			 * (within the current line) that still need to be appended.
			 */
			if ( buf[ bufsize - 1 ] == '\n' ) {
				process( line );
				free( line ), line = NULL;
			}
		}
	}

	/* stdin closed */
	debug( 1, LOC, "End of stream\n" );
	return( 0 );
}

