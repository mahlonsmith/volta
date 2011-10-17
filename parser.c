
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

#include "volta.h"


#line 141 "parser.rl"


#line 40 "parser.c"
static const int redirector_start = 1;
static const int redirector_first_final = 85;
static const int redirector_error = 0;

static const int redirector_en_main = 1;


#line 143 "parser.rl"

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
	
#line 95 "parser.c"
	{
	cs = redirector_start;
	}

#line 189 "parser.rl"
	
#line 102 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr1;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr2;
	} else
		goto tr2;
	goto tr0;
tr0:
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr3:
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr11:
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 64 "parser.rl"
	{ debug( 3, LOC, "Unable to parse the client IP address.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr14:
#line 64 "parser.rl"
	{ debug( 3, LOC, "Unable to parse the client IP address.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr35:
#line 60 "parser.rl"
	{ debug( 3, LOC, "Unable to parse method.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr49:
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
tr92:
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	goto st0;
#line 164 "parser.c"
st0:
cs = 0;
	goto _out;
tr1:
#line 48 "parser.rl"
	{ p_request->tokens.host_start  = p; }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 176 "parser.c"
	switch( (*p) ) {
		case 32: goto tr4;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr4;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st2;
	goto tr3;
tr4:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
#line 56 "parser.rl"
	{ p_request->tokens.path_length = p - ( *pe + p_request->tokens.path_start ); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 209 "parser.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr12;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr13;
	} else
		goto tr13;
	goto tr11;
tr12:
#line 39 "parser.rl"
	{
		debug( 1, LOC, "Channel ID found in redirector input.  Set 'url_rewrite_concurrency' to '0' in squid.\n" );
		{p++; cs = 4; goto _out;}
	}
#line 48 "parser.rl"
	{ p_request->tokens.host_start  = p; }
#line 62 "parser.rl"
	{ p_request->tokens.c_ip_start  = p; }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 234 "parser.c"
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st42;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st75;
	goto tr14;
tr15:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
#line 56 "parser.rl"
	{ p_request->tokens.path_length = p - ( *pe + p_request->tokens.path_start ); }
	goto st5;
tr51:
#line 56 "parser.rl"
	{ p_request->tokens.path_length = p - ( *pe + p_request->tokens.path_start ); }
	goto st5;
tr53:
#line 53 "parser.rl"
	{ p_request->tokens.port_length = p - ( *pe + p_request->tokens.port_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
#line 56 "parser.rl"
	{ p_request->tokens.path_length = p - ( *pe + p_request->tokens.path_start ); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 279 "parser.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr18;
	goto tr14;
tr18:
#line 62 "parser.rl"
	{ p_request->tokens.c_ip_start  = p; }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 291 "parser.c"
	if ( (*p) == 46 )
		goto st7;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st30;
	goto tr14;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st8;
	goto tr14;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 46 )
		goto st9;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st28;
	goto tr14;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st10;
	goto tr14;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 46 )
		goto st11;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st26;
	goto tr14;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st12;
	goto tr14;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 47 )
		goto tr28;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st24;
	goto tr3;
tr28:
#line 63 "parser.rl"
	{ p_request->tokens.c_ip_length = p - ( *pe + p_request->tokens.c_ip_start ); }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 353 "parser.c"
	if ( (*p) == 45 )
		goto st14;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st21;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto tr3;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 32 )
		goto st15;
	if ( 9 <= (*p) && (*p) <= 13 )
		goto st15;
	goto tr3;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st16;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr3;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr3;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( 65 <= (*p) && (*p) <= 90 )
		goto tr36;
	goto tr35;
tr36:
#line 58 "parser.rl"
	{ p_request->tokens.meth_start  = p; }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 439 "parser.c"
	switch( (*p) ) {
		case 10: goto tr39;
		case 32: goto tr37;
	}
	if ( (*p) > 13 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st18;
	} else if ( (*p) >= 9 )
		goto tr37;
	goto st0;
tr37:
#line 59 "parser.rl"
	{ p_request->tokens.meth_length = p - ( *pe + p_request->tokens.meth_start ); }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 458 "parser.c"
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 10 )
		goto tr42;
	goto st20;
tr42:
#line 36 "parser.rl"
	{ valid = 1; }
	goto st85;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
#line 475 "parser.c"
	if ( (*p) == 10 )
		goto tr42;
	goto st20;
tr39:
#line 59 "parser.rl"
	{ p_request->tokens.meth_length = p - ( *pe + p_request->tokens.meth_start ); }
#line 36 "parser.rl"
	{ valid = 1; }
	goto st86;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 489 "parser.c"
	goto st20;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	switch( (*p) ) {
		case 32: goto st15;
		case 45: goto st22;
		case 46: goto st23;
		case 95: goto st22;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto st15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st21;
		} else if ( (*p) >= 65 )
			goto st21;
	} else
		goto st21;
	goto tr3;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 45: goto st22;
		case 95: goto st22;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st21;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto tr3;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 32 )
		goto st15;
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto st15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st21;
		} else if ( (*p) >= 65 )
			goto st21;
	} else
		goto st21;
	goto tr3;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 47 )
		goto tr28;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st25;
	goto tr3;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 47 )
		goto tr28;
	goto tr3;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 46 )
		goto st11;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st27;
	goto tr14;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 46 )
		goto st11;
	goto tr14;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 46 )
		goto st9;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st29;
	goto tr14;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	if ( (*p) == 46 )
		goto st9;
	goto tr14;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 46 )
		goto st7;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st31;
	goto tr14;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) == 46 )
		goto st7;
	goto tr14;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 45: goto st32;
		case 95: goto st32;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st33;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st33;
	} else
		goto st33;
	goto tr49;
tr99:
#line 45 "parser.rl"
	{ p_request->tokens.scheme_length = p - ( *pe + p_request->tokens.scheme_start ); }
#line 48 "parser.rl"
	{ p_request->tokens.host_start  = p; }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 639 "parser.c"
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr3;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 32: goto tr15;
		case 47: goto tr7;
		case 58: goto tr9;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr3;
tr7:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
	goto st35;
tr54:
#line 53 "parser.rl"
	{ p_request->tokens.port_length = p - ( *pe + p_request->tokens.port_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 697 "parser.c"
	if ( (*p) == 32 )
		goto tr51;
	if ( 9 <= (*p) && (*p) <= 13 )
		goto tr51;
	goto st35;
tr9:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 52 "parser.rl"
	{ p_request->tokens.port_start  = p; }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 713 "parser.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st37;
	goto tr3;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	switch( (*p) ) {
		case 32: goto tr53;
		case 47: goto tr54;
	}
	if ( (*p) > 13 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st38;
	} else if ( (*p) >= 9 )
		goto tr53;
	goto tr3;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	switch( (*p) ) {
		case 32: goto tr53;
		case 47: goto tr54;
	}
	if ( (*p) > 13 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st39;
	} else if ( (*p) >= 9 )
		goto tr53;
	goto tr3;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	switch( (*p) ) {
		case 32: goto tr53;
		case 47: goto tr54;
	}
	if ( (*p) > 13 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st40;
	} else if ( (*p) >= 9 )
		goto tr53;
	goto tr3;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 32: goto tr53;
		case 47: goto tr54;
	}
	if ( (*p) > 13 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st41;
	} else if ( (*p) >= 9 )
		goto tr53;
	goto tr3;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	switch( (*p) ) {
		case 32: goto tr53;
		case 47: goto tr54;
	}
	if ( 9 <= (*p) && (*p) <= 13 )
		goto tr53;
	goto tr3;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	switch( (*p) ) {
		case 32: goto tr15;
		case 47: goto tr7;
		case 58: goto tr9;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st43;
	goto tr14;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st44;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st73;
	goto tr14;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	switch( (*p) ) {
		case 32: goto tr15;
		case 47: goto tr7;
		case 58: goto tr9;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st45;
	goto tr14;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st46;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st71;
	goto tr14;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 32: goto tr15;
		case 47: goto tr7;
		case 58: goto tr9;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st47;
	goto tr14;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr66;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st69;
	goto tr3;
tr66:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 55 "parser.rl"
	{ p_request->tokens.path_start  = p; }
#line 63 "parser.rl"
	{ p_request->tokens.c_ip_length = p - ( *pe + p_request->tokens.c_ip_start ); }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
#line 931 "parser.c"
	switch( (*p) ) {
		case 32: goto tr51;
		case 45: goto st49;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr51;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st66;
		} else if ( (*p) >= 65 )
			goto st66;
	} else
		goto st66;
	goto st35;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( (*p) == 32 )
		goto tr70;
	if ( 9 <= (*p) && (*p) <= 13 )
		goto tr70;
	goto st35;
tr70:
#line 56 "parser.rl"
	{ p_request->tokens.path_length = p - ( *pe + p_request->tokens.path_start ); }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 965 "parser.c"
	switch( (*p) ) {
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( 36 <= (*p) && (*p) <= 46 )
			goto st16;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto tr71;
	goto tr14;
tr71:
#line 62 "parser.rl"
	{ p_request->tokens.c_ip_start  = p; }
	goto st51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 993 "parser.c"
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st52;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st64;
	goto tr14;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st53;
	goto tr14;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st54;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st62;
	goto tr14;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st55;
	goto tr14;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st56;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st60;
	goto tr14;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st57;
	goto tr14;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 47: goto tr28;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st58;
	goto tr3;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 47: goto tr28;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 46 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st59;
	goto tr3;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 47: goto tr28;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 36 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr3;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st56;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st61;
	goto tr14;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st56;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr14;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st54;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st63;
	goto tr14;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st54;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr14;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st52;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st65;
	goto tr14;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	switch( (*p) ) {
		case 32: goto st17;
		case 33: goto st16;
		case 46: goto st52;
		case 59: goto st16;
		case 61: goto st16;
		case 95: goto st16;
		case 126: goto st16;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 13 ) {
			if ( 36 <= (*p) && (*p) <= 45 )
				goto st16;
		} else if ( (*p) >= 9 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st16;
		} else if ( (*p) >= 65 )
			goto st16;
	} else
		goto st16;
	goto tr14;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 32: goto tr70;
		case 45: goto st67;
		case 46: goto st68;
		case 95: goto st67;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr70;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st66;
		} else if ( (*p) >= 65 )
			goto st66;
	} else
		goto st66;
	goto st35;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 32: goto tr51;
		case 45: goto st67;
		case 95: goto st67;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr51;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st66;
		} else if ( (*p) >= 65 )
			goto st66;
	} else
		goto st66;
	goto st35;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) == 32 )
		goto tr70;
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr70;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st66;
		} else if ( (*p) >= 65 )
			goto st66;
	} else
		goto st66;
	goto st35;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr66;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st70;
	goto tr3;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr66;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr3;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st46;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st72;
	goto tr14;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st46;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr14;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st44;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st74;
	goto tr14;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st44;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr14;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st42;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st76;
	goto tr14;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st42;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr14;
tr2:
#line 44 "parser.rl"
	{ p_request->tokens.scheme_start  = p; }
#line 48 "parser.rl"
	{ p_request->tokens.host_start  = p; }
	goto st77;
tr13:
#line 39 "parser.rl"
	{
		debug( 1, LOC, "Channel ID found in redirector input.  Set 'url_rewrite_concurrency' to '0' in squid.\n" );
		{p++; cs = 77; goto _out;}
	}
#line 44 "parser.rl"
	{ p_request->tokens.scheme_start  = p; }
#line 48 "parser.rl"
	{ p_request->tokens.host_start  = p; }
	goto st77;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
#line 1678 "parser.c"
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st78;
		} else if ( (*p) >= 65 )
			goto st78;
	} else
		goto st33;
	goto tr92;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr9;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st79;
		} else if ( (*p) >= 65 )
			goto st79;
	} else
		goto st33;
	goto tr92;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr95;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st83;
		} else if ( (*p) >= 65 )
			goto st83;
	} else
		goto st33;
	goto tr92;
tr95:
#line 49 "parser.rl"
	{ p_request->tokens.host_length = p - ( *pe + p_request->tokens.host_start ); }
#line 52 "parser.rl"
	{ p_request->tokens.port_start  = p; }
	goto st80;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
#line 1757 "parser.c"
	if ( (*p) == 47 )
		goto st81;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st37;
	goto tr92;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( (*p) == 47 )
		goto st82;
	goto tr92;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr99;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr99;
	} else
		goto tr99;
	goto tr49;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr95;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st84;
		} else if ( (*p) >= 65 )
			goto st84;
	} else
		goto st33;
	goto tr92;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	switch( (*p) ) {
		case 32: goto tr15;
		case 45: goto st32;
		case 46: goto st34;
		case 47: goto tr7;
		case 58: goto tr95;
		case 95: goto st32;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr15;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 65 )
			goto st33;
	} else
		goto st33;
	goto tr92;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 2: 
	case 12: 
	case 13: 
	case 14: 
	case 15: 
	case 16: 
	case 19: 
	case 21: 
	case 22: 
	case 23: 
	case 24: 
	case 25: 
	case 33: 
	case 34: 
	case 35: 
	case 36: 
	case 37: 
	case 38: 
	case 39: 
	case 40: 
	case 41: 
	case 47: 
	case 48: 
	case 49: 
	case 57: 
	case 58: 
	case 59: 
	case 66: 
	case 67: 
	case 68: 
	case 69: 
	case 70: 
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 77: 
	case 78: 
	case 79: 
	case 80: 
	case 81: 
	case 83: 
	case 84: 
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 32: 
	case 82: 
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 17: 
#line 60 "parser.rl"
	{ debug( 3, LOC, "Unable to parse method.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 4: 
	case 5: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
	case 26: 
	case 27: 
	case 28: 
	case 29: 
	case 30: 
	case 31: 
	case 42: 
	case 43: 
	case 44: 
	case 45: 
	case 46: 
	case 50: 
	case 51: 
	case 52: 
	case 53: 
	case 54: 
	case 55: 
	case 56: 
	case 60: 
	case 61: 
	case 62: 
	case 63: 
	case 64: 
	case 65: 
	case 71: 
	case 72: 
	case 73: 
	case 74: 
	case 75: 
	case 76: 
#line 64 "parser.rl"
	{ debug( 3, LOC, "Unable to parse the client IP address.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 1: 
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
	case 3: 
#line 46 "parser.rl"
	{ debug( 3, LOC, "Unable to parse scheme.\n" ); }
#line 50 "parser.rl"
	{ debug( 3, LOC, "Unable to parse hostname.\n" ); }
#line 64 "parser.rl"
	{ debug( 3, LOC, "Unable to parse the client IP address.\n" ); }
#line 37 "parser.rl"
	{ valid = 0; }
	break;
#line 2043 "parser.c"
	}
	}

	_out: {}
	}

#line 190 "parser.rl"

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

	p_request->tokens.scheme_start = NULL;
	p_request->tokens.host_start   = NULL;
	p_request->tokens.port_start   = NULL;
	p_request->tokens.path_start   = NULL;
	p_request->tokens.meth_start   = NULL;
	p_request->tokens.c_ip_start   = NULL;

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

