-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION test_pg_store_plans" to load this file. \quit

CREATE FUNCTION test_hash_operations()
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION test_jumble_operations()
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION test_jumble_plan_tree() 
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;