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
 * Given a redirect +line+ from squid, send it to the parser,
 * perform database lookups, and conditonally perform the rewrite.
 *
 */
void
process( char *line )
{
	parsed *p_request = parse_request( line ), *rule = NULL;
	parsed *results[ DB_RESULTS_MAX ] = { NULL }; /* array of response matches */
	unsigned int rcount = 0;

	/* count lines in debugmode */
	if ( v.debugmode > 2 ) v.timer.lines++;

	/* If request parsing failed, return a blank line to squid
	   to allow the request to pass through unmolested. */
	if ( p_request == NULL ) {
		out( "\n" );
		finish_parsed( p_request );
		return;
	}

	/*
	 * Main rewrite logic.
	 *
	 * First, try and match the host exactly.
	 *
	 * Second, match the TLD of the host, so separate rules aren't needed for
	 * every possible subdomain of one particular domain.
	 *
	 * Finally, look for '*', if for some reason the rules provided don't care to
	 * match specific hosts, and instead just match on any path.
	 *
	 * If DB matches are found at any step above, the rules are tried in order
	 * to attempt a match against the path.  Exact string match attempted
	 * first, then fallback to regexp.
	 * 
	 * First rule match wins, and elements of the URL are rewritten based on
	 * what is present in the rule -- any missing parts just use the original
	 * URL element.  (this way, you can rewrite just the host and leave the
	 * path intact, or redir to https, for example.)
	 *
	 */
	rcount = find_records( p_request->host, results );
	rule = find_matching_rule( results, rcount, p_request );

	if ( rule == NULL ) {
		reset_results( results, rcount );
		rcount = find_records( p_request->tld, results );
		rule = find_matching_rule( results, rcount, p_request );
	}

	if ( rule == NULL ) {
		reset_results( results, rcount );
		rcount = find_records( "*", results );
		rule = find_matching_rule( results, rcount, p_request );
	}

	/* no matching rule still?  no need to rewrite anything. */
	if ( rule == NULL ) {
		out( "\n" );
	}
	/* otherwise, perform the rewrite */
	else {
		rewrite( p_request, rule );
	}

	reset_results( results, rcount );
	finish_parsed( p_request );
	return;
}


/*
 * Output a rewritten URL for squid.
 *
 */
void
rewrite( parsed *request, parsed *rule )
{
	if ( rule == NULL || v.debugmode >= 5 ) return;

	if ( rule->redir ) printf( "%s:", rule->redir );
	printf( "%s%s", (rule->scheme ? rule->scheme : request->scheme), rule->host );
	if ( rule->port ) printf( ":%s", rule->port );
	printf( "%s", rule->path ? rule->path : request->path );

	printf("\n");
	return;
}


/*
 * Search through a result set, and return the first
 * matching path (or NULL).
 *
 */
parsed *
find_matching_rule( parsed **results, unsigned int resultcount, parsed *p_request )
{
	unsigned int i = 0;
	int re_rv;
	regex_t re;
	char re_err[128];
	parsed *rule = NULL;

	if ( resultcount == 0 || p_request->path == NULL ) return( NULL );

	for ( i = 0; i < resultcount; i++ ) {
		/* quick comparison */
		if ( (strcasecmp( results[i]->path_re, p_request->path ) == 0) ||
			 (strcmp( results[i]->path_re, "*" ) == 0) ) {
			debug( 4, LOC, "Rule %d match (non regexp)\n", i+1 );
			rule = results[i];
			break;
		}

		/* compile the regexp */
		if ( (re_rv = regcomp( &re, results[i]->path_re, REG_EXTENDED | REG_NOSUB )) != 0 ) {
			regerror( re_rv, &re, re_err, 128 );
			debug( 4, LOC, "Invalid regex: \"%s\": %s\n", results[i]->path_re, re_err );
			regfree( &re );
			continue;
		}

		/* compare! */
		if ( (regexec( &re, p_request->path, 0, NULL, 0 )) == 0 ) {
			debug( 4, LOC, "Rule %d match (regexp)\n", i+1 );
			rule = results[i];
			regfree( &re );
			break;
		}
	}

	return( rule );
}


/*
 * Clear the results array and free memory.
 *
 */
void
reset_results( parsed **results, unsigned int count )
{
	unsigned int i = 0;

	for ( ; i < count && i < DB_RESULTS_MAX; i++ ) finish_parsed( results[i] );
	memset( results, 0, sizeof(results) );

	return;
}

