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

/*
Squid docs:
---------------------------------------------------------------------------
TAG: url_rewrite_program
Specify the location of the executable for the URL rewriter.
Since they can perform almost any function there isn't one included.

For each requested URL rewriter will receive on line with the format

URL <SP> client_ip "/" fqdn <SP> user <SP> method [<SP> kvpairs]<NL>

In the future, the rewriter interface will be extended with
key=value pairs ("kvpairs" shown above).  Rewriter programs
should be prepared to receive and possibly ignore additional
whitespace-separated tokens on each input line.

And the rewriter may return a rewritten URL. The other components of
the request line does not need to be returned (ignored if they are).

The rewriter can also indicate that a client-side redirect should
be performed to the new URL. This is done by prefixing the returned
URL with "301:" (moved permanently) or 302: (moved temporarily).

By default, a URL rewriter is not used.
---------------------------------------------------------------------------
*/

#include "volta.h"

%%{
	machine redirector;

	action parse_error {
		debug( 2, LOC, "parse error\n" );
		return( NULL );
	}

	action yay {
		printf( "I saw: %s", p+1 );
	}

	# http://, ftp://, https://, etc
	proto = alpha{3,5} . '://';

	# http://mahlon:password@example.com or http://mahlon@example.com
    #       username              optional password
	creds = ( alnum | [+._\-] )+ . ( ':' . any+ )? . '@';

	main  := ( proto . creds ) | proto @yay '\n';
}%%
%% write data;

/*
%%{
	machine redirector;

	action yay {
		printf( "I saw: %s", p+1 );
	}

	# http://, ftp://, https://, etc
	proto = alpha{3,5} . '://';

	# http://mahlon:password@example.com or http://mahlon@example.com
    #       username              optional password
	creds = ( alnum | [+._\-] )+ . ( ':' . any+ )? . '@';

	main := ( proto . creds ) | proto @yay '\n';
}%%
%% write data;
*/


/*
%%{
	machine foo;

	OPEN = 0;
	CLOSE = 1;

	main :=
		start:
		door_closed: (
			OPEN -> door_open -> final
		),
		door_open: (
			CLOSE -> door_closed
		);
}%%
*/

struct request *
parse( char *p )
{
   	/* initial machine state */
	short int cs = 0;

	/* the client request object */
	request c_request;
	request *cp_request = &c_request;

	/*
	char ip[ INET_ADDRSTRLEN ];
	inet_pton( AF_INET, "127.0.0.1", &cp_request->ip );
	inet_ntop( AF_INET, &cp_request->ip, ip, INET_ADDRSTRLEN );
	*/

	/* initalize state machine with current line */
	char *pe = p + strlen(p) + 1;

	/* enter state machine */
	%% write init;
	%% write exec;

	/* reset the request */
	/* c_request = reset_request; */
	return( cp_request );
}

