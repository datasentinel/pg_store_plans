SET client_min_messages = WARNING;

-- Load the parent extension first
CREATE EXTENSION pg_store_plans;

-- Load test extension
CREATE EXTENSION test_pg_store_plans_internals;

-- Run jumble tests
SELECT test_jumble_operations();

-- Run jumble plan tree test
SELECT test_jumble_plan_tree();

-- Cleanup
DROP EXTENSION test_pg_store_plans_internals;
DROP EXTENSION pg_store_plans;