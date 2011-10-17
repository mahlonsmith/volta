/* vim: set noet nosta sw=4 ts=4 ft=ragel : */
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

%%{
	machine redirector;

	action success { valid = 1; }
	action error   { valid = 0; }

	action channel_id_found  {
		debug( 1, LOC, "Channel ID found in redirector input.  Set 'url_rewrite_concurrency' to '0' in squid.\n" );
		fbreak;
	}

	action scheme_start  { p_request->tokens.scheme_start  = fpc; }
	action scheme_finish { p_request->tokens.scheme_length = fpc - ( *pe + p_request->tokens.scheme_start ); }
	action scheme_error  { debug( 3, LOC, "Unable to parse scheme.\n" ); }

	action host_start  { p_request->tokens.host_start  = fpc; }
	action host_finish { p_request->tokens.host_length = fpc - ( *pe + p_request->tokens.host_start ); }
	action host_error  { debug( 3, LOC, "Unable to parse hostname.\n" ); }

	action port_start  { p_request->tokens.port_start  = fpc; }
	action port_finish { p_request->tokens.port_length = fpc - ( *pe + p_request->tokens.port_start ); }

	action path_start  { p_request->tokens.path_start  = fpc; }
	action path_finish { p_request->tokens.path_length = fpc - ( *pe + p_request->tokens.path_start ); }

	action meth_start  { p_request->tokens.meth_start  = fpc; }
	action meth_finish { p_request->tokens.meth_length = fpc - ( *pe + p_request->tokens.meth_start ); }
	action meth_error  { debug( 3, LOC, "Unable to parse method.\n" ); }

	action c_ip_start  { p_request->tokens.c_ip_start  = fpc; }
	action c_ip_finish { p_request->tokens.c_ip_length = fpc - ( *pe + p_request->tokens.c_ip_start ); }
	action c_ip_error  { debug( 3, LOC, "Unable to parse the client IP address.\n" ); }

    # 
    # Squid line: URL <SP> client_ip "/" fqdn <SP> user <SP> method [<SP> kvpairs]<NL>
    # 
    # URI Syntax (RFC 3986) misc notes:
    #
    # - Scheme isn't passed to redirectors on CONNECT method requests
    #
    # - Hostname segments aren't supposed to be individually greater than 63 chars,
    #   and the hostname in total shouldn't exceed 255.  They also shouldn't be entirely
    #   made up of digits, or contain underscores.  In practice, these rules appear to
    #   be violated constantly by some pretty big sites. I'm looking at you, facebook.
    #   (( alnum ) | ( alnum . [a-zA-Z0-9\-]{0,63} . alnum )) & !( digit+ );
    #
    # - ipv6 has some utterly insane rules (RFC 5952) in the name of "shortcuts", which
    #   only seem like shortcuts to someone writing IP addresses by hand.  Anyone that
    #   has to parse (or even just read) them has a bunch of seemingly arbitrary work
    #   dumped in their lap.  Heck, it's impossible to even search for an ipv6 address
    #   that contains zeros in a text editor, because you have no idea what how it might
    #   be represented.  Rad!
    #
    #   The parser just trusts any ipv6 squid hands us as being valid, without
    #   any real parsing/validation, other than it consists of hex digits and colons.
    #
    # - This parser originally validated path/query/fragment as well, but there were
    #   enough inconsistencies with unescaped chars and other real-life RFC deviations
    #   that I opted to just accept what we get from squid.
	#
	# - Redirectors aren't handed any userinfo (http://mahlon:password@example.com),
    #   so no need to check for that.
    #

	host_component = alnum | ( alnum [a-zA-Z0-9\-_]* alnum );
	pchar          = ( alnum | [\-._~!$%&'()*+,;=] );
	path_segment   = '/' ( any - space )*;

	hostname       = host_component ( '.' host_component )* '.'?;
	ipv4           = digit{1,3} '.' digit{1,3} '.' digit{1,3} '.' digit{1,3};
	ipv6           = ( xdigit | ':' )+;

	channel_id     = ( digit+ space )      %channel_id_found;
	scheme         = ( alpha{3,5} '://' )  >scheme_start %scheme_finish @!scheme_error;
	host           = ( hostname | ipv4 )   >host_start   %host_finish   @!host_error;
	port           = ( ':' digit{1,5} )    >port_start   %port_finish;
	path           = path_segment*         >path_start   %path_finish;
	client_ip      = ipv4                  >c_ip_start   %c_ip_finish   @!c_ip_error;
	method         = upper+                >meth_start   %meth_finish   @!meth_error;

	Line = (
 		start: (
			channel_id? -> Url
		),

		Url: (
		   	scheme? host port? path? space -> Client
 		),

		Client: (
			client_ip '/' ( hostname | '-' ) space -> User
		),

		User: (
			pchar+ space -> Method
		),

		Method: (
			method -> KVPairs
		),

		KVPairs: (
			( space any+ )? -> final
		)
 	) %success @!error;


	main := Line '\n';
}%%
%% write data;

/* 
 * Tokenize an incoming line from squid, returning a parsed and populated
 * structure to make redirection decisions against.  This pointer should
 * be freed using cleanup_request() after use.
 * 
 * Squid documentation about redirectors:
 * ---------------------------------------------------------------------------
 * TAG: url_rewrite_program
 * Specify the location of the executable for the URL rewriter.
 * Since they can perform almost any function there isn't one included.
 * 
 * For each requested URL rewriter will receive on line with the format
 * 
 * URL <SP> client_ip "/" fqdn <SP> user <SP> method [<SP> kvpairs]<NL>
 * 
 * In the future, the rewriter interface will be extended with
 * key=value pairs ("kvpairs" shown above).  Rewriter programs
 * should be prepared to receive and possibly ignore additional
 * whitespace-separated tokens on each input line.
 * 
 * And the rewriter may return a rewritten URL. The other components of
 * the request line does not need to be returned (ignored if they are).
 * 
 * The rewriter can also indicate that a client-side redirect should
 * be performed to the new URL. This is done by prefixing the returned
 * URL with "301:" (moved permanently) or 302: (moved temporarily).
 * 
 * By default, a URL rewriter is not used.
 * ---------------------------------------------------------------------------
*/
request *
parse( char *line )
{
   	/* machine required vars */
	int  cs   = 0;
	char *p   = line;
	char *pe  = p + strlen(p);
	char *eof = NULL;

	/* the client request pointer */
	unsigned char valid = 0;
	request *p_request = init_request();

	/* enter state machine */
	%% write init;
	%% write exec;

	/* If we were given an invalid line, bail early */
	if ( valid == 0 ) {
		free( p_request ), p_request = NULL;
		debug( 3, LOC, "Invalid line (%d), skipped\n", v.timer.lines + 1 );
		debug( 4, LOC, "%s", line );
		return( NULL );
	}

	(void)populate_request( p_request );
	return( p_request );
}


/*
 * Initialize and return a pointer to a new request object.
 *
 */
request *
init_request( void )
{
	request *p_request = NULL;
	if ( (p_request = malloc( sizeof(request) )) == NULL ) {
		debug( 1, LOC, "Unable to allocate memory for request struct: %s\n", strerror(errno) );
		return( NULL );
	}
	p_request->scheme    = NULL;
	p_request->host      = NULL;
	p_request->port      = NULL;
	p_request->path      = NULL;
	p_request->user      = NULL;
	p_request->method    = NULL;
	p_request->client_ip = NULL;

	p_request->tokens.scheme_start  = NULL;
	p_request->tokens.host_start    = NULL;
	p_request->tokens.port_start    = NULL;
	p_request->tokens.path_start    = NULL;
	p_request->tokens.meth_start    = NULL;
	p_request->tokens.c_ip_start    = NULL;
	p_request->tokens.scheme_length = 0;
	p_request->tokens.host_length   = 0;
	p_request->tokens.port_length   = 0;
	p_request->tokens.path_length   = 0;
	p_request->tokens.meth_length   = 0;
	p_request->tokens.c_ip_length   = 0;

	return p_request;
}


/*
 * Take the previously parsed token locations and copy them into the request struct.
 *
 */
void
populate_request( struct request *p_request )
{
	p_request->scheme =
		copy_string_token( p_request->tokens.scheme_start, p_request->tokens.scheme_length );
	p_request->host =
		copy_string_token( p_request->tokens.host_start, p_request->tokens.host_length );
	p_request->port =
		copy_string_token( p_request->tokens.port_start, p_request->tokens.port_length );
	p_request->path =
		copy_string_token( p_request->tokens.path_start, p_request->tokens.path_length );
	p_request->method =
		copy_string_token( p_request->tokens.meth_start, p_request->tokens.meth_length );
	p_request->client_ip =
		copy_ipv4_token( p_request->tokens.c_ip_start, p_request->tokens.c_ip_length );

	return;
}


/*
 * Release memory used by request struct.
 *
 */
void
cleanup_request( struct request *p_request )
{
	if ( p_request == NULL ) return;

	free( p_request->scheme );
	free( p_request->host );
	free( p_request->port );
	free( p_request->path );
	free( p_request->method );
	free( p_request->client_ip );

	free( p_request ), p_request = NULL;

	return;
}

