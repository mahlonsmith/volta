
-- examine the request.
-- this emits to stderr!
--
for k,v in pairs( request ) do print( string.format("request.%-6s --> %s", k, v) ) end

-- all variables need to be declared using local scoping!
--
local redir = 302

-- temporary redirect to a different site, every other second.
-- why would you want to do this?  you probably wouldn't.  just illustrating
-- how easy it is to do custom stuff.
--
if os.time() % 2 == 0 then
	return string.format( "%d:http://example.com%s", redir, request.path )
end

