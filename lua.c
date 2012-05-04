/* vim: set noet nosta sw=4 ts=4 ft=c : */
/*
Copyright (c) 2011-2012, Mahlon E. Smith <mahlon@martini.nu>
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
#include "lua.h"

/*
 * Emit a lua error if a variable is declared without the use of 'local'.
 * Since we only start up one lua interpreter, we want to ensure the
 * global namespace isn't polluted by random scripts.
 *
 */
int
luaV_globalindex( lua_State *lua )
{
	return luaL_error( lua, "attempt to set global var '%s' (use local!)", lua_tostring(lua, -2) );
}


/*
 * Override the regular lua print() function with one that formats the string
 * in the same fashion as the rest of volta output -- and goes to stderr instead
 * of stdout, which would just confuse squid.
 *
 */
int
luaV_print( lua_State *lua )
{
	lua_Debug info;
	int i = 0;

	/* get the file and line number print() was called from. */
	lua_getstack( lua, 1, &info );
	lua_getinfo( lua, "Sl", &info );

	/* output each argument */
	for( i = 1; i <= lua_gettop( lua ); i++ )
		debug( 2, info.short_src, info.currentline, "%s\n", lua_tostring(lua, i) );

	return( 1 );
}


/*
 * Create the initial lua interpreter, and configure the default environment.
 *
 */
lua_State *
luaV_setup( void )
{
	lua_State *lua = luaL_newstate();
	luaL_openlibs( lua ); /* include lua standard libraries */

	/* Predeclare the request table. */
	lua_pushstring( lua, "request" );
	lua_createtable( lua, 0, 7 );
	lua_settable( lua, LUA_GLOBALSINDEX );

	/* Predeclare a table for shared data */
	lua_pushstring( lua, "shared" );
	lua_newtable( lua );
	lua_settable( lua, LUA_GLOBALSINDEX );

	/* replace the lua print() function with one that calls debug() instead */
	lua_register( lua, "print", luaV_print );

	/* Restrict additional globals. */
    lua_createtable( lua, 0, 1);
    lua_pushcfunction( lua, luaV_globalindex );
    lua_setfield( lua, -2, "__newindex");
    lua_pushboolean( lua, 0 );
    lua_setfield( lua, -2, "__metatable");
    lua_setmetatable( lua, LUA_GLOBALSINDEX );

	lua_settop( lua, 0 );  /* wipe the stack */
	return( lua );
}


/*
 * Convert the request struct into a lua table, and inject it into the interpreter.
 *
 */
void
luaV_setup_request( parsed *request )
{
	lua_getfield( v.lua, LUA_GLOBALSINDEX, "request" );
	lua_pushstring( v.lua, request->scheme );
	lua_setfield( v.lua, 1, "scheme" );
	lua_pushstring( v.lua, request->host );
	lua_setfield( v.lua, 1, "host" );
	lua_pushstring( v.lua, request->path );
	lua_setfield( v.lua, 1, "path" );
	lua_pushstring( v.lua, request->port );
	lua_setfield( v.lua, 1, "port" );
	lua_pushstring( v.lua, request->method );
	lua_setfield( v.lua, 1, "method" );
	lua_pushstring( v.lua, request->client_ip );
	lua_setfield( v.lua, 1, "client_ip" );
	lua_pushstring( v.lua, request->tld );
	lua_setfield( v.lua, 1, "domain" );

	return;
}


/*
 * Given a request struct and a path to a lua script (or bytecode),
 * execute the script within the global lua interpreter, and return
 * a pointer to the string it generated (or NULL).
 *
 */
char *
luaV_run( parsed *request, char *path )
{
	int lua_err = 0;

	/* provide access to the request struct */
	luaV_setup_request( request );

	/* suck in the lua chunk(s) */
	debug( 4, LOC, "Loading Lua code from '%s'\n", path );
	lua_err = luaL_loadfile( v.lua, path );
	if ( lua_err ) {
		debug( 2, LOC, "Unable to run lua rule: %s\n", lua_tostring(v.lua, -1) );
		lua_settop( v.lua, 0 );
		return( NULL );
	}

	/* execute the lua, expecting one value to be returned. */
	lua_err = lua_pcall( v.lua, 0, 1, 0 );
	if ( lua_err ) {
		debug( 2, LOC, "Unable to run lua rule: %s\n", lua_tostring(v.lua, -1) );
		lua_settop( v.lua, 0 );
		return( NULL );
	}

	/* get the last element in the stack, which should be the script's return value. */
	char *rewrite = (char *)lua_tostring( v.lua, -1 );

	debug( 5, LOC, "Lua is currently consuming %dKB of memory\n", lua_gc(v.lua, LUA_GCCOUNT, 0) );
	lua_settop( v.lua, 0 ); /* reset the stack. */

	return( rewrite );
}

