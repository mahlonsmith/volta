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
	rule = find_rule( p_request->host, p_request );
	if ( rule == NULL ) rule = find_rule( p_request->tld, p_request );
	if ( rule == NULL ) rule = find_rule( "*", p_request );

	/* no matching rule still or whitelist rule?  no need to rewrite anything. */
	if ( rule == NULL || rule->wl ) {
		out( "\n" );
	}
	/* otherwise, perform the rewrite. */
	else {
		rewrite( p_request, rule );
	}

	finish_parsed( rule );
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
	fflush( stdout );
	return;
}


/*
 * Compare a parsed +rule+ against the +request+.
 * Returns 1 on a match, 0 otherwise.
 *
 */
unsigned short int
check_rule( parsed *rule, parsed *p_request )
{
	int re_rv;
	regex_t re;
	char re_err[128];

	if ( rule == NULL || p_request->path == NULL ) return( 0 );

	/* quick comparison */
	if ( (strcasecmp( rule->path_re, p_request->path ) == 0) ||
			(strcmp( rule->path_re, "*" ) == 0) ) {
		debug( 4, LOC, "Rule match \"%s\" (non regexp)\n", rule->path_re );
		return( 1 );
	}

	/* compile the regexp */
	if ( (re_rv = regcomp( &re, rule->path_re, REG_EXTENDED | REG_NOSUB | REG_ICASE )) != 0 ) {
		regerror( re_rv, &re, re_err, 128 );
		debug( 4, LOC, "Invalid regex: \"%s\": %s\n", rule->path_re, re_err );
		regfree( &re );
		return( 0 );
	}

	/* compare! */
	if ( (regexec( &re, p_request->path, 0, NULL, 0 )) == 0 ) {
		debug( 4, LOC, "Rule match \"%s\" (regexp)\n", rule->path_re );
		regfree( &re );
		return( 1 );
	}
	else {
		regfree( &re );
		return( 0 );
	}
}

