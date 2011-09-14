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
 * Parse command line options, perform actions, and enter accept loop.
 *
 */
int
db_initialize( void )
{
	debug( 1, LOC, "init! init! init!\n" );

	struct sqlite3 *db;
	struct sqlite3_stmt *stmt;

	if ( sqlite3_open( "testing.db", &db ) != SQLITE_OK ) {
		debug( 1, LOC, "Error when initializing database: %s\n", sqlite3_errmsg(db) );
		return( sqlite3_errcode(db) );
	}

	if ( sqlite3_prepare( db, "create table foo(bar int);", -1, &stmt, NULL ) != SQLITE_OK ) {
		debug( 1, LOC, "Error preparing statement: %s\n", sqlite3_errmsg(db) );
		return( sqlite3_errcode(db) );
	}

	if ( sqlite3_step( stmt ) != SQLITE_DONE ) {
		debug( 1, LOC, "Error executing statement: %s\n", sqlite3_errmsg(db) );
		return( sqlite3_errcode(db) );
	}

	sqlite3_finalize( stmt );
	sqlite3_close( db );

	debug( 1, LOC, "okay! okay! okay!\n" );
	return( SQLITE_OK );
}

