SET client_min_messages = 'error';
CREATE EXTENSION IF NOT EXISTS pg_store_plans;

DROP TABLE IF EXISTS t1;
CREATE TABLE t1 (a int, b text);
INSERT INTO t1 SELECT i, 'value_' || i FROM generate_series(1, 100) i;
ANALYZE t1;

SET enable_seqscan = on;
SET enable_indexscan = off;
SET enable_bitmapscan = off;

-- anonymize=off (default): constants must appear verbatim in stored plan
SET pg_store_plans.anonymize = off;
SELECT pg_store_plans_reset();
\o /dev/null
SELECT * FROM t1 WHERE a = 42;
SELECT * FROM t1 WHERE b = 'value_10';
\o

SELECT regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan
FROM pg_store_plans
WHERE plan LIKE '%Filter%'
ORDER BY plan;

-- anonymize=on: constants must be replaced with ?
SET pg_store_plans.anonymize = on;
SELECT pg_store_plans_reset();
\o /dev/null
SELECT * FROM t1 WHERE a = 42;
SELECT * FROM t1 WHERE b = 'value_10';
\o

SELECT regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan
FROM pg_store_plans
WHERE plan LIKE '%Filter%'
ORDER BY plan;

-- Same query run twice must still produce one entry (same planid)
SELECT count(*) AS plan_count
FROM pg_store_plans
WHERE plan LIKE '%Filter: (a%';

-- Restore
SET pg_store_plans.anonymize = off;
RESET enable_indexscan;
RESET enable_bitmapscan;
SELECT pg_store_plans_reset();
DROP TABLE t1;
