It is a fork from [http://github.com/ossc-db/pg_store_plans](http://github.com/ossc-db/pg_store_plans) with changes:

#### Enhancements
* Improve performance by changing the way plan IDs are computed (computed using the jumble method, similar to the queryid).

#### Fixes
* Fix memory leak when reading plan text from file.
* Add a null check when reading the plan from file.

## Overview

`pg_store_plans` provides a means to store execution plans of statements in PostgreSQL.  
This fork includes specific modifications and fixes maintained by DATASENTINEL.

## Installation

```
tar xvzf pg_store_plans.tar.gz
cd pg_store_plans

make
sudo make install
```

> 
> Add **pg_store_plans** to **shared_preload_libraries** parameter
>

## Configuration

| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `pg_store_plans.track` | enum | top | - Possible values are: `top`, `all`, `none`, `verbose` <br>Similarly to pg_stat_statements, `pg_store_plans.track` controls which statements are counted by the module. <br>Specify `top` to track top-level statements, `all` to also track nested statements (such as statements invoked within functions except for some commands), or `none` to disable statement statistics collection. <br>When all is specified, the commands executed under CREATE EXTENSION and ALTER EXTENSION commands are still ignored. <br>Specify `verbose` to track all commands including ones excluded by all. Only superusers can change this setting. |
| `pg_store_plans.max` | integer | 1000 | Maximum number of plans tracked by the module (i.e., the maximum number of rows in the pg_store_plans view). If more distinct plans than that are observed, information about the least-executed plan is discarded. The default value is 1000. This parameter can only be set at server start |
| `pg_store_plans.max_plan_length` | integer | 5000 | Maximum byte length of plans in the raw (shortened JSON) format to store. The plan text is truncated at the length if it is longer than that value. This parameter can only be set at server start
| `pg_store_plans.plan_storage` | enum | file | - Possible values are: `file`, `shmem`  <br> Specifies how plan texts are stored while server is running. If it is set to `file`, the plan texts are stored in a temporary file as pg_stat_statements does. `shmem` means to store plan texts on-memory. |
| `pg_store_plans.save` | boolean | on | Specifies whether to save plan statistics across server shutdowns. <br>If it is `off` then statistics are not saved at shutdown nor reloaded at server start. This parameter can only be set in the postgresql.conf file or on the server command line.
| `pg_store_plans.log_analyze` | boolean | off | causes `EXPLAIN ANALYZE` output, rather than just `EXPLAIN` output, to be included in plan.
| `pg_store_plans.log_buffers`  | boolean | off | causes `EXPLAIN (ANALYZE, BUFFERS)` output, rather than just `EXPLAIN` output, to be included in plan.
| `pg_store_plans.log_timing` | boolean | true | Disables to record actual timings. The overhead of repeatedly reading the system clock can slow down the query significantly on some systems, so it may be useful to set this parameter to `false` when only actual row counts, and not exact execution times for each execution nodes, are needed. <br>Run time of the entire statement is always measured when pg_store_plans.log_analyze is `true`.
| `pg_store_plans.log_triggers` | boolean | false | This parameter has no effect unless pg_store_plans.log_analyze is turned `on`.<br>Causes trigger execution statistics to be included in recoreded plans. 
| `pg_store_plans.verbose` | boolean | false | causes `EXPLAIN VERBOSE` output, rather than just `EXPLAIN` output, to be included in plan.

