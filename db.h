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

#ifndef _DB_H
#define _DB_H

#include "sqlite3.h"

#define DBSQL_MATCH_REQUEST "                                       \
	SELECT rewrite_rule, (                                          \
		CASE WHEN scheme       IS null THEN 1 ELSE 0 END +          \
		CASE WHEN host         IS null THEN 1 ELSE 0 END +          \
		CASE WHEN tld          IS null THEN 1 ELSE 0 END +          \
		CASE WHEN path         IS null THEN 1 ELSE 0 END +          \
		CASE WHEN port         IS null THEN 1 ELSE 0 END +          \
		CASE WHEN ip           IS null THEN 1 ELSE 0 END +          \
		CASE WHEN user         IS null THEN 1 ELSE 0 END +          \
		CASE WHEN method       IS null THEN 1 ELSE 0 END +          \
		CASE WHEN rewrite_rule IS null THEN 1 ELSE 0 END ) as nullc \
	FROM requests                                                   \
	WHERE                                                           \
		( scheme IS NULL OR scheme          = lower(?1) ) AND       \
		( host   IS NULL OR lower( host )   = lower(?2) ) AND       \
		( tld    IS NULL OR lower( tld )    = lower(?3) ) AND       \
		( path   IS NULL OR lower( path ) LIKE '?4%'    ) AND       \
		( port   IS NULL OR port            = ?5        ) AND       \
		( ip     IS NULL OR ip              = ?6        ) AND       \
		( user   IS NULL OR lower( user )   = lower(?7) ) AND       \
		( method IS NULL OR lower( method ) = lower(?8) ) AND       \
		rewrite_rule IS NOT null                                    \
	ORDER BY                                                        \
		length(path) DESC,                                          \
		nullc ASC                                                   \
	LIMIT 1"

/* Pull the entire rewrite rule row. */
#define DBSQL_GET_REWRITE_RULE "\
	SELECT * \
	FROM rewrite_rules \
	WHERE id = ?1"

extern const unsigned short int DB_VERSION;

/*
 * Function prototypes
 *
 */
int db_attach( void );
int db_upgrade( unsigned short int );
unsigned short int prepare_statements( void );
short int db_version( void );
rewrite *init_rewrite( void );
rewrite *prepare_rewrite( request * );
void finish_rewrite( rewrite * );

#endif

