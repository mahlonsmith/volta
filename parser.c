
#line 1 "parser.rl"
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


#line 80 "parser.rl"


#line 67 "parser.c"
static const int redirector_start = 1;
static const int redirector_first_final = 13;
static const int redirector_error = 0;

static const int redirector_en_main = 1;


#line 82 "parser.rl"

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
	
#line 137 "parser.c"
	{
	cs = redirector_start;
	}

#line 143 "parser.rl"
	
#line 144 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else if ( (*p) >= 65 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st3;
	} else if ( (*p) >= 65 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st4;
	} else if ( (*p) >= 65 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 58 )
		goto st5;
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st11;
	} else if ( (*p) >= 65 )
		goto st11;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 47 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 47 )
		goto tr7;
	goto st0;
tr7:
#line 68 "parser.rl"
	{
		printf( "I saw: %s", p+1 );
	}
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 216 "parser.c"
	switch( (*p) ) {
		case 10: goto st13;
		case 43: goto st8;
		case 95: goto st8;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st8;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st8;
		} else if ( (*p) >= 65 )
			goto st8;
	} else
		goto st8;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 43: goto st8;
		case 58: goto st9;
		case 64: goto st13;
		case 95: goto st8;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st8;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st8;
		} else if ( (*p) >= 65 )
			goto st8;
	} else
		goto st8;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 64 )
		goto st14;
	goto st10;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 64 )
		goto st14;
	goto st10;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 58 )
		goto st5;
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st12;
	} else if ( (*p) >= 65 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 58 )
		goto st5;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 144 "parser.rl"

	/* reset the request */
	/* c_request = reset_request; */
	return( cp_request );
}

