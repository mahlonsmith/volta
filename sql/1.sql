--- vim: set noet nosta sw=4 ts=4 ft=sql:

BEGIN;

DROP TABLE IF EXISTS rules;
CREATE TABLE rules (
	redir TINYINT NOT NULL DEFAULT 0 CHECK( redir IN (0,1,2) )
);

COMMIT;
