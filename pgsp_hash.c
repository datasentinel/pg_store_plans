#include "postgres.h"
#include "nodes/plannodes.h"
#include "utils/memutils.h" 
#include "pgsp_hash.h"

/* 
	Hash table used to store planIds (for versions < 18)
*/
static HTAB *pgsp_plan_id_cache = NULL;

void
pgsp_init_plan_id_cache(void)
{
    HASHCTL		ctl;

    if (pgsp_plan_id_cache)
        return;

    memset(&ctl, 0, sizeof(ctl));
    ctl.keysize = sizeof(PgspPlanIdCacheKey);
    ctl.entrysize = sizeof(PgspPlanIdCacheEntry);
    ctl.hcxt = TopMemoryContext; 


    pgsp_plan_id_cache = hash_create("pg_stat_plans plan_id_cache",
                                     PGSP_PLAN_CACHE_SIZE,
                                     &ctl,
                                     HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);
    elog(DEBUG3, "pg_store_plans: initialized plan_id_cache, size: %d", PGSP_PLAN_CACHE_SIZE);
}

void
pgsp_clear_plan_id_cache(void)
{
    if (pgsp_plan_id_cache)
    {
        hash_destroy(pgsp_plan_id_cache);
        pgsp_plan_id_cache = NULL;
    }
    
    /* Re-initialize the cache immediately */
    pgsp_init_plan_id_cache();

    elog(DEBUG3, "pg_store_plans: cleared plan_id_cache");
}

void
pgsp_check_and_clear_plan_id_cache(void)
{
	long num_entries;
	long mem_used;
	
    if (!pgsp_plan_id_cache)
		return;
	

        
    mem_used = hash_get_num_entries(pgsp_plan_id_cache) * sizeof(PgspPlanIdCacheEntry);
    elog(DEBUG3, "pg_store_plans: plan_id_cache memory used: %ld bytes", mem_used);
	num_entries = hash_get_num_entries(pgsp_plan_id_cache);
    elog(DEBUG3, "pg_store_plans: plan_id_cache entries: %ld", num_entries);
	
	if (num_entries >= PGSP_PLAN_CACHE_SIZE - 10)
	{
        HASH_SEQ_STATUS hash_seq;
        PgspPlanIdCacheEntry *entry;

        hash_seq_init(&hash_seq, pgsp_plan_id_cache);
        entry = (PgspPlanIdCacheEntry *) hash_seq_search(&hash_seq);
        if (entry)
        {
            PgspPlanIdCacheKey key_copy = entry->key;

            /* Terminate the scan before modifying the table */
            hash_seq_term(&hash_seq);

            /* Remove using the copy */
            hash_search(pgsp_plan_id_cache, &key_copy, HASH_REMOVE, NULL);
        }
        elog(DEBUG3, "pg_store_plans: plan_id_cache exceeded size, removed one entry");
	}
}
void
pgsp_cache_plan_id(uint64 queryid, uint64 planId)
{
    PgspPlanIdCacheEntry *entry;
    bool found;
    PgspPlanIdCacheKey key;
    
    if (!pgsp_plan_id_cache) pgsp_init_plan_id_cache();

	pgsp_check_and_clear_plan_id_cache();

    key.queryid = queryid;
    entry = (PgspPlanIdCacheEntry *) hash_search(pgsp_plan_id_cache, &key, HASH_ENTER, &found);
    entry->planId = planId;

    elog(DEBUG3, "pg_store_plans: cached planId: %lu for queryid: %lu", planId, queryid);
}

uint64
pgsp_get_cached_plan_id(uint64 queryid)
{
    PgspPlanIdCacheEntry *entry;
    PgspPlanIdCacheKey key;

    if (!pgsp_plan_id_cache) return 0;

    key.queryid = queryid;
    entry = (PgspPlanIdCacheEntry *) hash_search(pgsp_plan_id_cache, &key, HASH_FIND, NULL);
    
    elog(DEBUG3, "pg_store_plans: fetched planId: %lu for queryid: %lu", entry ? entry->planId : 0, queryid);
    return entry ? entry->planId : 0;
}

void
pgsp_remove_plan_id(uint64 queryid)
{
    PgspPlanIdCacheKey key;

    if (!pgsp_plan_id_cache)
        return;

    key.queryid = queryid;
    hash_search(pgsp_plan_id_cache, &key, HASH_REMOVE, NULL);
    
    elog(DEBUG3, "pg_store_plans: removed planId for queryid: %lu", queryid);
}

long
pgsp_get_plan_id_cache_size(void)
{
    if (!pgsp_plan_id_cache)
        return 0;
    
    return hash_get_num_entries(pgsp_plan_id_cache);
}

bool
pgsp_is_plan_id_cache_initialized(void)
{
    return (pgsp_plan_id_cache != NULL);
}

