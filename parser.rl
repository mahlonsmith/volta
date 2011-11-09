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

#define MARK_S( LBL ) p_parsed->tokens.LBL ## _start = p;
#define MARK_E( LBL ) p_parsed->tokens.LBL ## _length = p - ( *pe + p_parsed->tokens.LBL ## _start );

/* 
 * Tokenize an incoming line from squid, returning a parsed and populated
 * structure to make redirection decisions against.  This pointer should
 * be freed using finish_parsed() after use.
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
parsed *
parse_request( char *line )
{
	/* machine required vars */
	unsigned short int cs = 1;
	char *p   = line;
	char *pe  = p + strlen(p);
	char *eof = pe;

	/* the client request pointer */
	parsed *p_parsed = init_parsed();
	p_parsed->type   = REQUEST;

%%{
	machine request_parser;

	action channel_id_found  {
		debug( 1, LOC, "Channel ID found in redirector input.  Set 'url_rewrite_concurrency' to '0' in squid.\n" );
		fbreak;
	}

	action scheme_start  { MARK_S(scheme) }
	action scheme_finish { MARK_E(scheme) }
	action host_start    { MARK_S(host) }
	action host_finish   { MARK_E(host) }
	action port_start    { p_parsed->tokens.port_start = p+1; } # strip leading colon
	action port_finish   { MARK_E(port) }
	action path_start    { MARK_S(path) }
	action path_finish   { MARK_E(path) }
	action meth_start    { MARK_S(meth) }
	action meth_finish   { MARK_E(meth) }
	action c_ip_start    { MARK_S(c_ip) }
	action c_ip_finish   { MARK_E(c_ip) }

	action host_error   { debug( 3, LOC, "Unable to parse the request hostname.\n" ); }
	action scheme_error { debug( 3, LOC, "Unable to parse the request scheme.\n" ); }
	action meth_error   { debug( 3, LOC, "Unable to parse the request method.\n" ); }
	action c_ip_error   { debug( 3, LOC, "Unable to parse the client IP address.\n" ); }

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
	#   be violated constantly by some pretty big sites.
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

	URL = (
		start:   channel_id?                            -> Url,
		Url:     scheme? host port? path?               -> final
 	);

	Squidvars = (
		start:
		Client:  space client_ip '/' ( hostname | '-' ) -> User,
		User:    space pchar+                           -> Method,
		Method:  space method                           -> KVPairs,
		KVPairs: ( space any+ )?                        -> final
 	);

	main := URL Squidvars? '\n';
}%%

	/* state machine */
	%% write exec;

	/* If we were given an invalid line, bail early */
	if ( cs < %%{ write first_final; }%% ) {
		free( p_parsed ), p_parsed = NULL;
		debug( 3, LOC, "Invalid request line (%d), skipped\n", v.timer.lines + 1 );
		debug( 4, LOC, "%s", line );
		return( NULL );
	}

	debug( 6, LOC, "%s", line );
	(void)populate_parsed( p_parsed );
	return( p_parsed );
}


/*
 * Tokenize a value string from a successful database lookup, returning a parsed
 * and populated structure. This pointer should be freed using finish_parsed() after use.
 *
 */
parsed *
parse_rule( char *rewrite )
{
	/* machine required vars */
	unsigned short int cs = 1;
	char *p   = rewrite;
	char *pe  = p + strlen(p);
	char *eof = pe;

	/* the client rule pointer */
	parsed *p_parsed = init_parsed();
	p_parsed->type   = RULE;

%%{
	machine rule_parser;

	action match_start   { MARK_S(path_re) }
	action match_finish  { MARK_E(path_re) }
	action redir_start   { MARK_S(redir) }
	action redir_finish  { p_parsed->tokens.redir_length = 3; } # strip trailing colon
	action negate_finish { p_parsed->negate = 1; }

	action scheme_start  { MARK_S(scheme) }
	action scheme_finish { MARK_E(scheme) }
	action host_start    { MARK_S(host) }
	action host_finish   { MARK_E(host) }
	action port_start    { p_parsed->tokens.port_start = p+1; } # strip leading colon
	action port_finish   { MARK_E(port) }
	action path_start    { MARK_S(path) }
	action path_finish   { MARK_E(path) }

	action match_error { debug( 3, LOC, "Unable to parse the rule path matcher.\n" ); }
	action host_error  { debug( 3, LOC, "Unable to parse the rule hostname.\n" ); }

	host_component  = alnum | ( alnum [a-zA-Z0-9\-_]* alnum );
	path_segment    = '/' ( any - space )*;

	sep       = space+;
	hostname  = host_component ( '.' host_component )* '.'?;
	ipv4      = digit{1,3} '.' digit{1,3} '.' digit{1,3} '.' digit{1,3};
	ipv6      = ( xdigit | ':' )+;

	negate    = ( '-' )                 %negate_finish;
	path_re   = ( any - space )+        >match_start  %match_finish @!match_error;

	redir     = ( ('301' | '302') ':' ) >redir_start  %redir_finish;
	scheme    = ( alpha{3,5} '://' )    >scheme_start %scheme_finish;
	host      = ( hostname | ipv4 )     >host_start   %host_finish   @!host_error;
	port      = ( ':' digit{1,5} )      >port_start   %port_finish;
	path      = path_segment*           >path_start   %path_finish;

	rewrite   = ( redir? scheme? host port? path? );

	main := path_re sep ( rewrite | negate );
}%%

	/* state machine */
	%% write exec;

	/* If we were given an invalid rule, bail early */
	if ( cs < %%{ write first_final; }%% ) {
		free( p_parsed ), p_parsed = NULL;
		debug( 3, LOC, "Invalid rule\n" );
		debug( 4, LOC, "%s\n", rewrite );
		return( NULL );
	}

	(void)populate_parsed( p_parsed );
	return( p_parsed );
}


/*
 * Tokenize a line from an ascii representation of the database, returning
 * a pointer to a parsed struct.  Used for creation of a new cdb file,
 * validating data prior to use.
 *
 */
struct db_input *
parse_dbinput( char *line )
{
	/* machine required vars */
	unsigned short int cs = 1;
	char *p   = line;
	char *pe  = p + strlen(p);
	char *eof = pe;

	/* the db line input pointer */
	struct db_input *dbline = NULL;
	if ( (dbline = malloc( sizeof(struct db_input) )) == NULL ) {
		debug( 5, LOC, "Unable to allocate memory for db input: %s\n", strerror(errno) );
		return( NULL );
	}
	dbline->klen   = 0;
	dbline->vlen   = 0;
	dbline->kstart = NULL;
	dbline->key    = NULL;
	dbline->vstart = NULL;
	dbline->val    = NULL;

%%{
	machine dbinput_parser;

	action key_start  { dbline->kstart = p; }
	action key_finish { dbline->klen = p - ( *pe + dbline->kstart ); }
	action key_error  { debug( 0, LOC, "Invalid key format\n" ); }
	action val_start  { dbline->vstart = p; }
	action val_finish { dbline->vlen = p - ( *pe + dbline->vstart ); }
	action val_error  { debug( 0, LOC, "Invalid rewrite value\n" ); }

	sep            = space+;
	host_component = alnum | ( alnum [a-zA-Z0-9\-_]* alnum );
	hostname       = host_component ( '.' host_component )* '.'?;
	ipv4           = digit{1,3} '.' digit{1,3} '.' digit{1,3} '.' digit{1,3};
	token          = ( any - space )+;
	redir          = ( digit{3} ':' );
	host           = ( hostname | ipv4 );

	key = ( host | '*' )      >key_start %key_finish @!key_error;
	val = ( token sep token ) >val_start %val_finish @!val_error;
	#       regex     rewrite or negate
	
	main:= key sep val '\n';
}%%

	/* state machine */
	%% write exec;

	/* If the input line was invalid, bail early */
	if ( cs < %%{ write first_final; }%% ) {
		free( dbline ), dbline = NULL;
		return( NULL );
	}

	/* populate struct */
	dbline->key = copy_string_token( dbline->kstart, dbline->klen );
	dbline->val = copy_string_token( dbline->vstart, dbline->vlen );

	/* check the val to make sure it is a valid rewrite rule */
	parsed *valstr = NULL;
	valstr = parse_rule( dbline->val );
	if ( valstr == NULL ) {
		free( dbline->key );
		free( dbline->val );
		free( dbline );
		return( NULL );
	}
	finish_parsed( valstr );

	return( dbline );
}



/*
 * Initialize and return a pointer to a new parser object.
 *
 */
parsed *
init_parsed( void )
{
	parsed *p_parsed = NULL;
	if ( (p_parsed = malloc( sizeof(parsed) )) == NULL ) {
		debug( 5, LOC, "Unable to allocate memory for parsed struct: %s\n", strerror(errno) );
		return( NULL );
	}

	p_parsed->type      = 0;
	p_parsed->negate    = 0;
	p_parsed->path_re   = NULL;
	p_parsed->redir     = NULL;
	p_parsed->scheme    = NULL;
	p_parsed->host      = NULL;
	p_parsed->tld       = NULL;
	p_parsed->port      = NULL;
	p_parsed->path      = NULL;
	p_parsed->user      = NULL;
	p_parsed->method    = NULL;
	p_parsed->client_ip = NULL;

	p_parsed->tokens.path_re_start  = NULL;
	p_parsed->tokens.redir_start    = NULL;
	p_parsed->tokens.scheme_start   = NULL;
	p_parsed->tokens.host_start     = NULL;
	p_parsed->tokens.port_start     = NULL;
	p_parsed->tokens.path_start     = NULL;
	p_parsed->tokens.meth_start     = NULL;
	p_parsed->tokens.c_ip_start     = NULL;
	p_parsed->tokens.path_re_length = 0;
	p_parsed->tokens.redir_length   = 0;
	p_parsed->tokens.scheme_length  = 0;
	p_parsed->tokens.host_length    = 0;
	p_parsed->tokens.port_length    = 0;
	p_parsed->tokens.path_length    = 0;
	p_parsed->tokens.meth_length    = 0;
	p_parsed->tokens.c_ip_length    = 0;

	return p_parsed;
}


/*
 * Release memory used by the parsed struct.
 *
 */
void
finish_parsed( parsed *p_parsed )
{
	if ( p_parsed == NULL ) return;

	free( p_parsed->scheme );
	free( p_parsed->host );
	free( p_parsed->path );
	free( p_parsed->port );

	if ( p_parsed->type == REQUEST ) {
		free( p_parsed->tld );
		free( p_parsed->method );
		free( p_parsed->client_ip );
	}

	if ( p_parsed->type == RULE ) {
		free( p_parsed->path_re );
		free( p_parsed->redir );
	}

	free( p_parsed ), p_parsed = NULL;

	return;
}


#define COPY_STR( LBL ) copy_string_token( p_parsed->tokens.LBL ## _start, p_parsed->tokens.LBL ## _length )
/* #define COPY_IP4( LBL ) copy_ipv4_token(   p_request->tokens.LBL ## _start, p_request->tokens.LBL ## _length ) */

/*
 * Take the previously parsed token locations and copy them into the request struct.
 *
 */
void
populate_parsed( parsed *p_parsed )
{
	p_parsed->scheme = COPY_STR( scheme );
	p_parsed->host   = COPY_STR( host );
	p_parsed->path   = COPY_STR( path );
	p_parsed->port   = COPY_STR( port );

	if ( p_parsed->type == REQUEST ) {
		p_parsed->method    = COPY_STR( meth );
		p_parsed->client_ip = COPY_STR( c_ip );
		/* p_request->client_ip = COPY_IP4( c_ip ); */

		(void)lowercase_str( p_parsed->host, p_parsed->tokens.host_length );
		(void)parse_tld( p_parsed );
	}

	if ( p_parsed->type == RULE ) {
		p_parsed->path_re = COPY_STR( path_re );
		p_parsed->redir   = COPY_STR( redir );
	}

	return;
}


/*
 * Pull the top level domain out of the requested hostname.
 *
 */
void
parse_tld( parsed *p_request )
{
	unsigned short int cs = 5, mark = 0;
	char *p  = p_request->host;
	char *pe = p + p_request->tokens.host_length;
	char *ts = 0, *te = 0, *eof = pe;

%%{
    machine tld_parser;

	host_component = alnum | ( alnum [a-zA-Z0-9\-_]* alnum );
	tld = ( host_component '.' host_component );

	main := |*
		tld => { mark = ( p_request->tokens.host_length - (int)strlen(te) ); };
	*|;
}%%

	/* It's far easier (and faster) to scan from left to right rather than
	   backtrack, so start by reversing the requested host string. */
	reverse_str( p_request->host );

	/* scanner */
	%% write exec;

	/* If there was a mark, then allocate memory and copy. */
	if ( mark != 0 ) {
		if ( (p_request->tld = calloc( mark + 1, sizeof(char) )) == NULL ) {
			debug( 5, LOC, "Unable to allocate memory for tld token: %s\n", strerror(errno) );
			reverse_str( p_request->host );
			return;
		}

		memcpy( p_request->tld, p_request->host, mark );
		reverse_str( p_request->tld );
	}

	/* restore the hostname. */
	reverse_str( p_request->host );
	return;
}

