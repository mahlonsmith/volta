/* vim: set noet nosta sw=4 ts=4 ft=ragel : */

#include "volta.h"

%%{
	machine redirector;

	action yay {
		printf( "YAH\n" );
	}

	# http://, ftp://, https://, etc
	proto = alpha{3,5} . '://';

	# http://mahlon:password@example.com or http://mahlon@example.com
    #       username              optional password
	creds = ( alnum | [+._\-] )+ . ( ':' . any+ )? . '@';

	main := ' ' @yay;
}%%
%% write data;

/* state machine data */
/*
%%{
	machine redirector;

	action yay {
		printf( "YAH\n" );
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

int
parse( char *p )
{
   	/* initial machine state */
	short int cs = 0;

	/* the client request object */
	/* request c_request; */
	/* request *cp_request = &c_request; */

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
	return( 0 );
}

