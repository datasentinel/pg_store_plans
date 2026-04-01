-- pg_store_plans 2.0 to 2.0.1: add relids column to pg_store_plans()
-- ALTER EXTENSION pg_store_plans UPDATE TO '2.0.1';


DROP VIEW pg_store_plans;
DROP FUNCTION pg_store_plans();

CREATE FUNCTION pg_store_plans(
    OUT userid oid,
    OUT dbid oid,
    OUT queryid int8,
    OUT planid int8,
    OUT plan text,
    OUT calls int8,
    OUT total_time float8,
    OUT min_time float8,
    OUT max_time float8,
    OUT mean_time float8,
    OUT stddev_time float8,
    OUT rows int8,
    OUT shared_blks_hit int8,
    OUT shared_blks_read int8,
    OUT shared_blks_dirtied int8,
    OUT shared_blks_written int8,
    OUT local_blks_hit int8,
    OUT local_blks_read int8,
    OUT local_blks_dirtied int8,
    OUT local_blks_written int8,
    OUT temp_blks_read int8,
    OUT temp_blks_written int8,
    OUT shared_blk_read_time float8,
    OUT shared_blk_write_time float8,
    OUT local_blk_read_time float8,
    OUT local_blk_write_time float8,
    OUT temp_blk_read_time float8,
    OUT temp_blk_write_time float8,
    OUT first_call timestamptz,
    OUT last_call timestamptz,
    OUT relids oid[],
    OUT cmd_type text
)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'pg_store_plans_2_0_1'
LANGUAGE C
VOLATILE PARALLEL SAFE;

CREATE VIEW pg_store_plans AS
  SELECT * FROM pg_store_plans();

GRANT SELECT ON pg_store_plans TO PUBLIC;
