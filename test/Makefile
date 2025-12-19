MODULE_big = test_pg_store_plans_internals
OBJS = test_pg_store_plans_internals.o ../pgsp_hash.o ../pgsp_jumble.o

EXTENSION = test_pg_store_plans_internals
DATA = test_pg_store_plans_internals--1.0.sql

REGRESS = test_pg_store_plans_internals

# Need to link against parent module's objects
#SHLIB_LINK = -L../ -lpg_store_plans

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# Add parent directory to include path
override CPPFLAGS := -I../ $(CPPFLAGS)