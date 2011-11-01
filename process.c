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

void
process( char *line )
{
	request *p_request = parse( line );
	rewrite *p_rewrite = prepare_rewrite( p_request );

	/* count lines in debugmode */
	if ( v.debugmode > 2 ) v.timer.lines++;

	/* If parsing failed or there wasn't a successful rewrite match,
	 * return a blank line to squid to allow the request to pass
	 * through unmolested. */
	if ( p_request == NULL || p_rewrite == NULL ) {
		out( "\n" );
		finish_request( p_request );
		finish_rewrite( p_rewrite );
		return;
	}

	if ( v.debugmode < 4 ) {
		if ( p_rewrite->redir == REDIR_TEMPORARY ) printf( "302:" );
		if ( p_rewrite->redir == REDIR_PERMANENT ) printf( "301:" );

		if ( p_request->scheme || p_rewrite->scheme )
			printf( "%s", p_rewrite->scheme ? p_rewrite->scheme : p_request->scheme );
		printf( "%s", p_rewrite->host   ? p_rewrite->host   : p_request->host );
		printf( "%s", p_rewrite->path   ? p_rewrite->path   : p_request->path );
		if ( p_request->port != 0 || p_rewrite->port != 0 )
			printf( ":%d", p_rewrite->port ? p_rewrite->port : p_request->port );
		printf("\n");
	}
	else {
		debug( 5, LOC, "Rewrite match on %s/%s\n", p_request->host, p_request->path );
		debug( 5, LOC, "    --> %s/%s\n", p_rewrite->host, p_rewrite->path );
	}


	/* unsigned long hst, net; */
	/* hst = inet_lnaof( *(p_request->client_ip) ); */
	/* net = inet_netof( *(p_request->client_ip) ); */
	/* printf("%14s : net=0x%08lX host=0x%08lX\n", inet_ntoa( *(p_request->client_ip) ), net, hst); */
	/* printf("%14s : net=%lu host=%lu\n", inet_ntoa( *(p_request->client_ip) ), net, hst); */

	/*
	 * create function bigint_to_inet(bigint) returns inet as $$
	 * select
	 * (($1>>24&255)||'.'||($1>>16&255)||'.'||($1>>8&255)||'.'||($1>>0&255))::inet
	 * $$ language sql;
	 * */

	/*
	char ip[ INET_ADDRSTRLEN ];
	inet_ntop( AF_INET, p_request->client_ip, ip, INET_ADDRSTRLEN );
	printf( "%s\n", ip );
	*/

	finish_request( p_request );
	finish_rewrite( p_rewrite );
	return;
}

