/*-------------------------------------------------------------------------
 *
 * pgsp_jumble.h: hash table implementation for plan ID caching.
 *
 * Copyright (c) 2026, DATASENTINEL
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#if PG_VERSION_NUM >= 160000
#include "nodes/queryjumble.h"
#elif PG_VERSION_NUM >= 140000
#include "utils/queryjumble.h"
#endif

/* Function declarations */
extern JumbleState * pgsp_init_jumble_state(void);
extern void AppendJumble(JumbleState *jstate, const unsigned char *item, Size size);
extern void pgsp_jumble_plan_tree(JumbleState *jstate, Plan *plan);
extern void pgsp_jumble_range_table(JumbleState *jstate, List *rtable);