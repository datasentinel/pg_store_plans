/*-------------------------------------------------------------------------
 *
 * pgsp_jumble.c: hash table implementation for plan ID caching.
 *
 * Copyright (c) 2026, DATASENTINEL
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"
#include "miscadmin.h"
#include "access/hash.h"
#include "optimizer/planner.h"
#include "access/amapi.h"
#include "access/sdir.h"
#include "access/tableam.h"
#include "access/tsmapi.h"
#include "commands/event_trigger.h"
#include "commands/trigger.h"
#include "executor/tuptable.h"
#include "foreign/fdwapi.h"
#include "nodes/bitmapset.h"
#include "nodes/execnodes.h"
#include "nodes/extensible.h"
#include "nodes/lockoptions.h"
#include "nodes/miscnodes.h"
#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pathnodes.h"
#include "nodes/plannodes.h"
#include "nodes/primnodes.h"
#include "nodes/replnodes.h"
#include "nodes/supportnodes.h"
#include "nodes/value.h"
#include "nodes/print.h"
#include "utils/rel.h"
#if PG_VERSION_NUM >= 160000
#include "nodes/queryjumble.h"
#elif PG_VERSION_NUM >= 140000
#include "utils/queryjumble.h"
#endif

#include "pgsp_jumble.h"

#define JUMBLE_SIZE				1024	/* plan serialization buffer size */

static void pgsp_jumble_node(JumbleState *jstate, Node *node);

/*
 * Initialize a new JumbleState for plan jumbling.
 * Returns an initialized JumbleState.
 */
JumbleState *
pgsp_init_jumble_state(void)
{
    JumbleState *jstate;

    jstate = (JumbleState *) palloc(sizeof(JumbleState));

    /* Set up workspace for plan jumbling */
    jstate->jumble = (unsigned char *) palloc(JUMBLE_SIZE);
    jstate->jumble_len = 0;
    jstate->clocations_buf_size = 32;
    jstate->clocations = (LocationLen *)
        palloc(jstate->clocations_buf_size * sizeof(LocationLen));
    jstate->clocations_count = 0;
    jstate->highest_extern_param_id = 0;

    return jstate;
}

/*
 * AppendJumble - Append a chunk of data to the jumble state
 * 
 * This is a local implementation since the core PostgreSQL function
 * is static and not exported.
 */
void
AppendJumble(JumbleState *jstate, const unsigned char *item, Size size)
{
	unsigned char *jumble = jstate->jumble;
	Size		jumble_len = jstate->jumble_len;

	/*
	 * Whenever the jumble buffer is full, we hash the current contents and
	 * reset the buffer to contain just that hash value, thus relying on the
	 * hash to summarize everything so far.
	 */
	while (size > 0)
	{
		Size		part_size;

		if (jumble_len >= JUMBLE_SIZE)
		{
			uint64		start_hash;

			start_hash = DatumGetUInt64(hash_any_extended(jumble,
														  JUMBLE_SIZE, 0));
			memcpy(jumble, &start_hash, sizeof(start_hash));
			jumble_len = sizeof(start_hash);
		}
		part_size = Min(size, JUMBLE_SIZE - jumble_len);
		memcpy(jumble + jumble_len, item, part_size);
		jumble_len += part_size;
		item += part_size;
		size -= part_size;
	}
	jstate->jumble_len = jumble_len;
}


/*
 * Helper macros to append data to the jumble state
 */
#define APP_JUMB(item) \
	AppendJumble(jstate, (const unsigned char *) &(item), sizeof(item))


void
pgsp_jumble_range_table(JumbleState *jstate, List *rtable)
{
	ListCell   *lc;

	foreach(lc, rtable)
	{
		RangeTblEntry *expr = lfirst_node(RangeTblEntry, lc);

		switch (expr->rtekind)
		{
			case RTE_RELATION:
				APP_JUMB(expr->relid);
				break;
			case RTE_CTE:
				APP_JUMB(expr->ctename);
				break;
			default:
				break;
		}
	}
}

/*
 * Jumble a Node to generate a unique fingerprint.
 * Not all Exprs are handled here; only those relevant to plan structure.
 */
void
pgsp_jumble_node(JumbleState *jstate, Node *node)
{

	if (node == NULL)
	{
		return;
	}
	
	/* Guard against stack overflow due to overly complex expressions */
	check_stack_depth();

	/* Jumble the node type first */
	APP_JUMB(nodeTag(node));

	switch (nodeTag(node))
	{
		case T_List:
			{
				List *expr = (List *) node;
				ListCell *l;

				foreach(l, expr)
				{
					Node *subnode = (Node *) lfirst(l);
					pgsp_jumble_node(jstate, subnode);
				}
			}
			break;
		case T_OpExpr:
			{
			OpExpr *expr = (OpExpr *) node;

			APP_JUMB(expr->opno);
			pgsp_jumble_node(jstate, (Node *) expr->args);
			}
			break;
		case T_Var:
			{
				Var		   *var = (Var *) node;
				APP_JUMB(var->varattno);
				APP_JUMB(var->varlevelsup);
			}
			break;
		case T_Const:
			{
				Const	   *c = (Const *) node;
				APP_JUMB(c->consttype);
			}
			break;
		case T_Param:
			{
				Param	   *p = (Param *) node;

				APP_JUMB(p->paramkind);
				APP_JUMB(p->paramid);
				APP_JUMB(p->paramtype);
			}
			break;
		case T_RelabelType:
			{
				RelabelType *expr = (RelabelType *) node;

				pgsp_jumble_node(jstate, (Node *) expr->arg);
				APP_JUMB(expr->resulttype);
			}
			break;
		case T_TargetEntry:
			{
				TargetEntry *te = (TargetEntry *) node;

				APP_JUMB(te->resno);
				pgsp_jumble_node(jstate, (Node *) te->expr);
			}
			break;
		default:
			elog(DEBUG3, "pg_store_plans: Unhandled node type %d in jumbling node",
				 (int) nodeTag(node));
			break;
		}
}

/*
 * Jumble a Plan tree to generate a unique fingerprint (PlanId).
 * This logic mirrors ExplainNode in explain.c to capture relevant details.
 */
void
pgsp_jumble_plan_tree(JumbleState *jstate, Plan *plan)
{

	ListCell   *lc;

	if (plan == NULL)
	{
		return;
	}
	
	/* Jumble the node type first */
	APP_JUMB(nodeTag(plan));
	// elog(DEBUG3, "pg_store_plans: nodeTag(node) = %d", nodeTag(plan));

	switch (nodeTag(plan))
	{
		case T_ModifyTable:
			{
				ModifyTable *mt = (ModifyTable *) plan;
				APP_JUMB(mt->operation);
				APP_JUMB(mt->nominalRelation);
				APP_JUMB(mt->onConflictAction);
			}
			break;
		case T_NestLoop:
		case T_MergeJoin:
		case T_HashJoin:
			{
				Join *join = (Join *) plan;
				APP_JUMB(join->jointype);
				APP_JUMB(join->inner_unique);
				pgsp_jumble_node(jstate, (Node *) join->joinqual);
			}
			break;
		case T_BitmapHeapScan:
			{
				BitmapHeapScan *bhs = (BitmapHeapScan *) plan;
				APP_JUMB(bhs->scan.scanrelid);
				pgsp_jumble_node(jstate, (Node *) bhs->bitmapqualorig);
			}
			break;
		case T_TidScan:
			{
				TidScan *ts = (TidScan *) plan;
				APP_JUMB(ts->scan.scanrelid);
				pgsp_jumble_node(jstate, (Node *) ts->tidquals);
			}
		break;
		case T_TidRangeScan:
			{
				TidRangeScan *trs = (TidRangeScan *) plan;
				APP_JUMB(trs->scan.scanrelid);
				pgsp_jumble_node(jstate, (Node *) trs->tidrangequals);
			}
		break;
		case T_FunctionScan:
			{
				FunctionScan *fs = (FunctionScan *) plan;
				APP_JUMB(fs->scan.scanrelid);
				pgsp_jumble_node(jstate, (Node *) fs->functions);
			}
			break;
		case T_SeqScan:
		case T_SampleScan:
		case T_SubqueryScan:
		case T_TableFuncScan:
		case T_ValuesScan:
		case T_CteScan:
		case T_WorkTableScan:
		case T_NamedTuplestoreScan:
			{
				Scan *scan = (Scan *) plan;
				APP_JUMB(scan->scanrelid);
				APP_JUMB(scan->plan.plan_node_id);
				pgsp_jumble_node(jstate, (Node *) scan->plan.qual);
			}
			break;

		case T_ForeignScan:
			{
				ForeignScan *fs = (ForeignScan *) plan;
				APP_JUMB(fs->scan.scanrelid);
				APP_JUMB(fs->operation);
			}
			break;

		case T_CustomScan:
			{
				CustomScan *cs = (CustomScan *) plan;
				APP_JUMB(cs->scan.scanrelid);
				pgsp_jumble_node(jstate, (Node *) cs->custom_plans);
				pgsp_jumble_node(jstate, (Node *) cs->custom_exprs);
			}
			break;

		case T_Gather:
			{
				Gather *gather = (Gather *) plan;
				APP_JUMB(gather->num_workers);
				APP_JUMB(gather->rescan_param);
				APP_JUMB(gather->single_copy);
			}
			break;

		case T_GatherMerge:
			{
				GatherMerge *gm = (GatherMerge *) plan;
				APP_JUMB(gm->num_workers);
				APP_JUMB(gm->rescan_param);
				APP_JUMB(gm->numCols);
			}
			break;

		case T_IndexScan:
			{
				IndexScan *is = (IndexScan *) plan;
				APP_JUMB(is->scan.scanrelid);
				APP_JUMB(is->indexid);
				APP_JUMB(is->indexorderdir);
				pgsp_jumble_node(jstate, (Node *) is->indexqual);
				pgsp_jumble_node(jstate, (Node *) is->indexorderby);
				}
			break;

		case T_IndexOnlyScan:
			{
				IndexOnlyScan *ios = (IndexOnlyScan *) plan;
				APP_JUMB(ios->scan.scanrelid);
				APP_JUMB(ios->indexid);
				APP_JUMB(ios->indexorderdir);
				pgsp_jumble_node(jstate, (Node *) ios->recheckqual);
				pgsp_jumble_node(jstate, (Node *) ios->indextlist);
				pgsp_jumble_node(jstate, (Node *) ios->indexqual);
				pgsp_jumble_node(jstate, (Node *) ios->indexorderby);
			}
			break;

		case T_BitmapIndexScan:
			{
				BitmapIndexScan *bis = (BitmapIndexScan *) plan;
				APP_JUMB(bis->scan.scanrelid);
				APP_JUMB(bis->indexid);
				pgsp_jumble_node(jstate, (Node *) bis->indexqual);
				pgsp_jumble_node(jstate, (Node *) bis->indexqualorig);
			}
			break;

		case T_Sort:
			{
				Sort *sort = (Sort *) plan;
				APP_JUMB(sort->numCols);
				for (int i = 0; i < sort->numCols; i++)
				{
					APP_JUMB(sort->sortColIdx[i]);
					APP_JUMB(sort->sortOperators[i]);
					APP_JUMB(sort->collations[i]);
					APP_JUMB(sort->nullsFirst[i]);
				}
			}
			break;
		case T_Unique:
			{
				Unique *uniq = (Unique *) plan;
				APP_JUMB(uniq->numCols);
				for (int i = 0; i < uniq->numCols; i++)
					APP_JUMB(uniq->uniqColIdx[i]);
			}
			break;
		case T_Memoize:
			{
				Memoize *memo = (Memoize *) plan;
				APP_JUMB(memo->numKeys);
				for (int i = 0; i < memo->numKeys; i++)
					APP_JUMB(memo->hashOperators[i]);
			}
			break;
		case T_IncrementalSort:
			{
				IncrementalSort *is = (IncrementalSort *) plan;
				APP_JUMB(is->sort.numCols);
				APP_JUMB(is->nPresortedCols);
				for (int i = 0; i < is->sort.numCols; i++)
				{
					APP_JUMB(is->sort.sortColIdx[i]);
					APP_JUMB(is->sort.sortOperators[i]);
					APP_JUMB(is->sort.collations[i]);
					APP_JUMB(is->sort.nullsFirst[i]);
				}
			}
			break;

		case T_Group:
			{
				Group *grp = (Group *) plan;
				APP_JUMB(grp->numCols);
				for (int i = 0; i < grp->numCols; i++)
					APP_JUMB(grp->grpColIdx[i]);
			}
			break;

		case T_Agg:
			{
				Agg *agg = (Agg *) plan;
				APP_JUMB(agg->aggstrategy);
				APP_JUMB(agg->aggsplit);
				APP_JUMB(agg->numCols);
				for (int i = 0; i < agg->numCols; i++)
					APP_JUMB(agg->grpColIdx[i]);
			}
			break;

		case T_WindowAgg:
			{
				WindowAgg *wa = (WindowAgg *) plan;
				APP_JUMB(wa->winref);
				APP_JUMB(wa->partNumCols);
				APP_JUMB(wa->ordNumCols);
				APP_JUMB(wa->frameOptions);
				APP_JUMB(wa->inRangeAsc);
				APP_JUMB(wa->inRangeNullsFirst);
				for (int i = 0; i < wa->partNumCols; i++)
					APP_JUMB(wa->partColIdx[i]);
			}
			break;
		case T_SetOp:
			{
				SetOp *setop = (SetOp *) plan;
				APP_JUMB(setop->cmd);
				APP_JUMB(setop->strategy);
				APP_JUMB(setop->numCols);
				for (int i = 0; i < setop->numCols; i++)
				    #if PG_VERSION_NUM >= 180000
						APP_JUMB(setop->cmpColIdx[i]);
					#else
						APP_JUMB(setop->dupColIdx[i]);
					#endif
			}
			break;
		default:
			elog(DEBUG3, "pg_store_plans: Unhandled plan node type %d in jumbling",
				 (int) nodeTag(plan));
			break;
	}

	/* Recursively jumble standard children */
	pgsp_jumble_plan_tree(jstate, plan->lefttree);
	pgsp_jumble_plan_tree(jstate, plan->righttree);

	/* Recursively jumble special child lists */
	if (IsA(plan, Append))
	{
		Append *append = (Append *) plan;
		foreach(lc, append->appendplans)
			pgsp_jumble_plan_tree(jstate, lfirst(lc));
	}
	else if (IsA(plan, MergeAppend))
	{
		MergeAppend *ma = (MergeAppend *) plan;
		foreach(lc, ma->mergeplans)
			pgsp_jumble_plan_tree(jstate, lfirst(lc));
	}
	else if (IsA(plan, BitmapAnd))
	{
		BitmapAnd *ba = (BitmapAnd *) plan;
		foreach(lc, ba->bitmapplans)
			pgsp_jumble_plan_tree(jstate, lfirst(lc));
	}
	else if (IsA(plan, BitmapOr))
	{
		BitmapOr *bo = (BitmapOr *) plan;
		foreach(lc, bo->bitmapplans)
			pgsp_jumble_plan_tree(jstate, lfirst(lc));
	}
	else if (IsA(plan, SubqueryScan))
	{
		SubqueryScan *ss = (SubqueryScan *) plan;
		pgsp_jumble_plan_tree(jstate, ss->subplan);
	}
	else if (IsA(plan, CustomScan))
	{
		CustomScan *cs = (CustomScan *) plan;
		foreach(lc, cs->custom_plans)
			pgsp_jumble_plan_tree(jstate, lfirst(lc));
	}
}