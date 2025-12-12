#include "postgres.h"
#include "nodes/plannodes.h"
#include "utils/hsearch.h"

/* Key structure for the plan ID cache */
typedef struct PgspPlanIdCacheKey
{
    uint64 queryid;
} PgspPlanIdCacheKey;

/* 
    Entry structure for the plan ID cache 
*/
typedef struct PgspPlanIdCacheEntry
{
    PgspPlanIdCacheKey key;
    uint64		planId;
} PgspPlanIdCacheEntry;

/* 
    size of the plan ID cache hash table 
*/
#define PGSP_PLAN_CACHE_SIZE 1024

/* Function declarations */
extern void pgsp_init_plan_id_cache(void);
extern void pgsp_clear_plan_id_cache(void);
extern void pgsp_check_and_clear_plan_id_cache(void);
extern void pgsp_cache_plan_id(uint64 queryid, uint64 planId);
extern uint64 pgsp_get_cached_plan_id(uint64 queryid);
extern void pgsp_remove_plan_id(uint64 queryid);
extern bool pgsp_is_plan_id_cache_initialized(void);
extern long pgsp_get_plan_id_cache_size(void);
