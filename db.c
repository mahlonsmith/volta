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


/*
 * Open the database specified in the 'v' global struct,
 * setting the file descriptor.  Returns 0 on success.
 *
 */
short int
db_attach( void )
{
	/* only re-open the db at most every 10 seconds */
	time_t now = time( NULL );
	if ( v.timer.db_lastcheck > 0 ) {
		if ( now - v.timer.db_lastcheck >= 10 ) {
			cdb_free( &v.db );
			close( v.db_fd );
		}
		else {
			return( 0 );
		}
	}

	debug( 2, LOC, "(Re)attaching to database '%s'\n", v.dbname );

	/* db filename not set? */
	if ( strlen(v.dbname) == 0 ) {
		debug( 1, LOC, "Error when attaching to database: DB filename unset?\n" );
		return( -1 );
	}

	if ( (v.db_fd = open( v.dbname, O_RDONLY )) == -1 ) {
		debug( 1, LOC, "Error when attaching to database: %s\n", strerror(errno) );
		return( -1 );
	}

	v.timer.db_lastcheck = now;
	cdb_init( &v.db, v.db_fd );

	return( 0 );
}


/*
 * Given a rule file in ascii specified by the -c command line arg,
 * convert it to a cdb after checking rules for validity.
 *
 */
unsigned short int
db_create_new( char *txt )
{
	FILE *txt_f  = NULL;
	parsed *rule = NULL;
	char buf[ LINE_BUFSIZE*10 ], tmpfile[25];
	int  tmp_fd, linenum = 0, parsed = 0, error = 0;
	struct cdb_make cdbm;
	struct db_input *dbline;

	/* open temporary file */
	debug( 0, LOC, "Creating/updating database (%s) using rules in \"%s\"\n", v.dbname, txt );
	sprintf( tmpfile, "volta-db-%d.tmp", getpid() );
	if ( (tmp_fd = open( tmpfile,
						 O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH )) == -1 ) {
		debug( 0, LOC, "Error writing temporary file: %s\n", strerror(errno) );
		return( 1 );
	}

	/* open rules file */
	if ( (txt_f = fopen( txt, "r" )) == NULL ) {
		debug( 0, LOC, "Error reading rules file: %s\n", strerror(errno) );
		return( 1 );
	}

	/* init struct and start parsing lines */
	cdb_make_start( &cdbm, tmp_fd );
	while ( fgets( buf, LINE_BUFSIZE*10, txt_f ) != NULL ) {
		linenum++;

		/* skip blank lines and comments */
		if ( strlen(buf) == 1 || buf[0] == '#' ) continue;

		/* validate line */
		dbline = parse_dbinput( buf );
		if ( dbline == NULL ) {
			debug( 0, LOC, "Invalid rule (line %d), stopping: %s", linenum, buf );
			error = 1;
			break;
		}

		/* validate rule */
		rule = parse_rule( dbline->val );
		if ( rule == NULL ||
			( rule->negate == 1 && rule->host != NULL ) ||
			( rule->negate == 0 && rule->host == NULL )) {

			debug( 0, LOC, "Invalid rule (line %d), stopping: %s", linenum, buf );
			error = 1;
			break;
		}

		/* looking good, add rule */
		cdb_make_add( &cdbm, dbline->key, dbline->klen, dbline->val, dbline->vlen );
		parsed++;

		free( dbline->key );
		free( dbline->val );
		free( dbline ), dbline = NULL;
		finish_parsed( rule ), rule = NULL;
	}

	/* write indexes */
	fclose( txt_f );
	cdb_make_finish( &cdbm );
	close( tmp_fd );

	if ( error == 1 ) {
		/* delete the tmp db on errors */
		if ( (unlink( tmpfile )) != 0 ) {
			debug( 0, LOC, "Unable to remove temp cdb: %s", strerror(errno) );
			return( 1 );
		}
	}
	else {
		/* move cdb into place */
		if ( (rename( tmpfile, v.dbname )) == -1 ) {
			debug( 0, LOC, "Unable to move temp cdb into place: %s", strerror(errno) );
			return( 1 );
		}
		debug( 0, LOC, "Added %d rules to %s.\n", parsed, v.dbname );
	}

	return( 0 );
}


/* 
 * Search the CDB for all occurances of the given +key+, checking
 * each one against the +p_request+.  First match wins and is
 * returned.  NULL on no match.
 *
 */
parsed *
find_rule( char *key, parsed *p_request )
{
	if ( key == NULL ) return( NULL );

	struct cdb_find cdbf; /* structure to hold current find position */

	parsed *rule = NULL;
	char *val    = NULL;
	unsigned int vlen, vpos;

	/* initialize search structs */
	if ( db_attach() == -1 ) return( NULL );
	cdb_findinit( &cdbf, &v.db, key, (int)strlen(key) );

	while ( cdb_findnext( &cdbf ) > 0 ) {
		vpos = cdb_datapos( &v.db );
		vlen = cdb_datalen( &v.db );

		/* pull the value from the db */
		if ( (val = calloc( vlen+1, sizeof(char) )) == NULL ) {
			debug( 5, LOC, "Unable to allocate memory for DB value storage: %s\n",
					strerror(errno) );
			return( NULL );
		}
		cdb_read( &v.db, val, vlen, vpos );

		/* check it against the request */
		debug( 4, LOC, "DB match for key '%s': %s\n", key, val );
		rule = parse_rule( val );

		free( val );
		if ( rule != NULL ) {
			if ( check_rule( rule, p_request ) == 0 ) {
				finish_parsed( rule ), rule = NULL;
			}
			else {
				break;
			}
		}
	}

	return( rule );
}

