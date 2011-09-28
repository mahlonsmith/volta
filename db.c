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

const unsigned short int DB_VERSION = 1;


/*
 * Connect to the database specified in the 'v' global struct,
 * and populate v.db with a handle to it.
 *
 * If required, automatically handle initializing/upgrading a DB.
 *
 */
int
db_attach( void )
{
	if ( v.db != NULL )          return( SQLITE_OK );    /* already attached */
	if ( strlen(v.dbname) == 0 ) return( SQLITE_ERROR ); /* db filename not set? */

	debug( 2, LOC, "Attaching to database '%s'\n", v.dbname );
	if ( sqlite3_open( v.dbname, &v.db ) != SQLITE_OK ) {
		debug( 1, LOC, "Error when attaching to database: %s\n", sqlite3_errmsg(v.db) );
		return( sqlite3_errcode(v.db) );
	}

	/* check DB version */
	unsigned short int version = db_version();
	debug( 2, LOC, "Database version: %d\n", version );
	if ( version != DB_VERSION ) {
		debug( 2, LOC, "Database version mismatch: expected %hu\n", DB_VERSION );

		/* We're in need of a DB initialization, or just behind.
		 * Attempt to "stair step" upgrade to the current version. */
		if ( version < DB_VERSION ) {
			return( db_upgrade(version) );
		}

		/* Something else is wack. */
		else {
			return( SQLITE_ERROR );
		}
	}

	return( SQLITE_OK );
}


/*
 * Perform automatic stair-step upgrades of DB versions
 * to get up to the current version expected from code.
 *
 */
int
db_upgrade( unsigned short int current_version )
{
	unsigned short int i = 0;
	char user_pragma[30];
	char sql_file[30];
	char *upgrade_sql = NULL;

	for ( i = current_version + 1; i <= DB_VERSION; i++ ) {
		if ( i == 1 ) {
			debug( 2, LOC, "Initializing new database.\n" );
		}
		else {
			debug( 2, LOC, "Upgrading database version from %hu to %hu\n", current_version, i );
		}

		sprintf( sql_file, "sql/%d.sql", i );
		upgrade_sql = slurp_file( sql_file );
		if ( upgrade_sql == NULL ) return( SQLITE_ERROR );

		/* If there is SQL to execute, do so and then reset for more */
		if ( sqlite3_exec( v.db, upgrade_sql, NULL, NULL, NULL ) != SQLITE_OK ) {
			debug( 2, LOC, "Error %s database: %s\n",
					(i == 1 ? "initalizing" : "upgrading"), sqlite3_errmsg(v.db) );
			return( sqlite3_errcode(v.db) );
		}
		free( upgrade_sql );
		upgrade_sql = NULL;

		/* update version metadata in DB if update was successful */
		current_version = i;
		sprintf( user_pragma, "PRAGMA user_version = %hu;", current_version );
		if ( sqlite3_exec( v.db, user_pragma, NULL, NULL, NULL ) != SQLITE_OK ) {
			debug( 2, LOC, "Error setting version: %s\n", sqlite3_errmsg(v.db) );
			return( sqlite3_errcode(v.db) );
		}
	}

	return( SQLITE_OK );
}


/*
 * Fetch and return the database's current version. or -1 on error.
 * The database should already be attached before calling this function.
 *
 */
short int
db_version( void )
{
	struct sqlite3_stmt *stmt;
	int version = -1;

	if ( sqlite3_prepare_v2( v.db, "PRAGMA user_version", -1, &stmt, NULL ) != SQLITE_OK ) {
		debug( 2, LOC, "Error finding DB version: %s\n", sqlite3_errmsg(v.db) );
		return( -1 );
	}

	if ( sqlite3_step( stmt ) == SQLITE_ROW )
		version = sqlite3_column_int( stmt, 0 );

	sqlite3_finalize( stmt );
	return( version );
}

