
Volta
=====

What is volta?
--------------

Volta is a high performance, low resource URI rewriter for use with the
Squid caching proxy server (http://www.squid-cache.org/.)  With it, you
can dynamically alter URI requests that pass through Squid based on
various criteria.

It uses a state machine to parse URIs and rules, and a constant database
to store and access those rules.  It can then either perform conditional
rewrites internally, or by evaluating Lua scripts.


Why is it called "volta"?
-------------------------

It's a type of old Italian music written in triple-time.  Quick!


How fast is it?
---------------

On a 2Ghz Xeon 5130, it can process a million squid requests against
10000 rules in less than 8 seconds, using about 800k of ram.  On an
1.8Ghz Intel E4300, it can do it in 3 seconds.

Your mileage may vary, but for most all intents and purposes the answer
is "definitely fast enough."


Configuring squid
-----------------

You must enable url rewriting from within the squid.conf file.

	url_rewrite_program /usr/local/bin/volta

... and that's it.  You may need some additional customization, like where
the volta database is stored on disk:

	url_rewrite_program /usr/local/bin/volta -f /var/db/squid/volta.db

Busy servers:

While Volta is lightweight enough to simply increase the amount of
rewriter children, it also supports Squid's rewrite_concurrency format
if you find that to be more efficient for your environment.  Adjust to
taste.

	url_rewrite_children 5 startup=1 idle=2 concurrency=50


Using volta
-----------

See the INSTALL file for instructions on how to compile volta.

Volta reads its rewrite rules from a local database.  You can create the
rules in a text editor, then convert it to the database like so:

	% volta -c rules.txt

You'll be left with a "volta.db" file in the current directory.  Put it
wherever you please, and use the -f flag to point to it.


Rule file syntax
----------------

Volta's rule syntax is designed to be easy to parse by humans and
machines.  Blank lines are skipped, as is any line that starts with the
'#' character, so you can keep the ascii version of your rules well
documented and in version control.  There is no practical limit on the
number of rules in this database.

When compiling the ruleset into the database format, volta detects
malformed rules and stops if there are any problems, leaving your
original database intact.  You can change the ruleset at any time while
volta is running, and the new rules will take affect within about 10
seconds.  No need to restart squid!

There are two types of rules -- positive matches, and negative matches.
Positive matches cause the rewrite, negative matches intentionally allow
the original request to pass.  Rule order is consistent, top-down, first
match wins.  Fields are separated by any amount of whitespace (spaces or
tabs.)


### Positive matches:

**First field**: *the hostname to match*

	You can use an exact hostname (www.example.com), or the top level
	domain (tld) if you want to match everything under a specific host
	(example.com.)  You can also use a single '*' to match every request,
	though this essentially bypasses a lot of what makes volta quick, it
	is included for completeness.  You may have an unlimited amount of
	rules per hostname.  Hostnames are compared without case sensitivity.

**Second field**: *the path to match*

	This can be an exact match ('/path/to/something.html'), a regular
	expression ('\.(jpg|gif|png)$'), or a single '*' to match for any
	path. Regular expressions are matched without case sensitivity.  There
	is currently no internal support for captures, though you can use
	a Lua rule (see below) for more complex processing.

**Third field**: *the redirect code and url to rewrite to*

	Any pieces of a url that are omitted are automatically replaced
	with the original request's element -- the exception is a hostname,
	which is required.  If you omit a redirect code, the URL rewrite is
	transparent to the client.  You can attach a 301: or 302: prefix to
	cause a permanent or temporary code to be respectively sent, instead.

	If you require more complex processing than what volta provides
	internally, you can also specify a path to a Lua script (prefixed
	with 'lua:'.)  See the 'Lua rules' section of this README for more
	information.


### Negative matches:

**First field**: *the hostname to match*

See above -- all the same rules apply.


**Second field**: *the path to match*

See above -- all the same rules apply.


**Third field**: *the 'negative' marker*

This is simply the '-' character, that signals to volta that this is
a negative matching rule.


You can easily test your rules by running volta on the command line, and
pasting URLs into it.   Boost the debug level (-d4) if you're having any issues.


Examples
--------

Rewrite all requests to Google to the SSL version:

	google.com * 302:https://www.google.com

	This will redirect the request "http://www.google.com/search?q=test" to
	"https://www.google.com/search?q=test".


Transparently alter all uploaded images on imgur to be my face:  :)

	i.imgur.com \.(gif|png|jpg)$ http://www.martini.nu/images/mahlon.jpg


Expand a local, non qualified hostname to a FQDN (useful alongside the
'dns_defnames' squid setting to enforce browser proxy behaviors):

	local-example * local-example.company.com


Cause all blog content except for 2011 posts to permanently redirect to
an archival page:

	martini.nu /blog/2011 -
	martini.nu /blog 301:martini.nu/content-archived.html


Send all requests to reddit/r/WTF/* through a lua script for further processing.

	reddit.com /r/wtf lua:/path/to/a/lua-script.lua


Turn off rewriting for specific network segment or IP address:

	Squid has this ability built in -- see the 'url_rewrite_access' setting.
	Alternatively, do the checks in lua.



Lua Rules
---------

Volta has an embedded Lua interpreter that you can use to perform all
kinds of conditional rewrites.  Read more about the syntax of the Lua
language here: http://www.lua.org/manual/5.1/

### Loading a script

To use a Lua script, prefix the rewrite target of a volta rule with
'lua:'.  The rest of the target is then treated as a path to the script.
(You can find an example in the Examples section of this README.)

You can specify a path to either an ascii file, or Lua bytecode. (If
speed is an absolute premium, I'm seeing around a 25% performance
increase by using Lua bytecode files.)

You can use different scripts for different rules, or use the same
script across any number of separate rules.

There is no need to restart squid when modifying Lua rules.  Changes are
seen immediately.


### Environment

* Global variable declarations are disabled, so scripts can't accidently stomp on each other.  All variables must be declared with the 'local' keyword.
* There is a global table called 'shared' you may use if you want to share data between separate scripts, or remember things in-between rule evaluations.
* The details of the request can be found in a table, appropriately named 'request'.  HTTP scheme, host, path, port, method, client_ip, and domain are all available by default from the request table.
* Calling Lua's print() function emits debug information to stderr.  Use a debug level of 2 or higher to see it.


### Return value

The return value of the script is sent unmodified to squid, which should
be a URL the request is rewritten to, with an optional redirect code
prefix (301 or 302.)

Omitting a return value, or returning 'nil' has the same effect as a negative
rule match -- the original request is allowed through without any rewrite.


An extremely simple Lua rule script can be found in the 'examples'
directory, distributed with volta.

