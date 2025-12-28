#include "postgres.h"
#include "optimizer/planner.h"
#include "commands/explain.h"
#include "executor/spi.h"

#if PG_VERSION_NUM >= 160000
#include "nodes/queryjumble.h"
#elif PG_VERSION_NUM >= 140000
#include "utils/queryjumble.h"
#endif
#include "utils/builtins.h"
#include "access/hash.h"
#include "../pgsp_jumble.h"


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(test_hash_operations);
PG_FUNCTION_INFO_V1(test_jumble_operations);
PG_FUNCTION_INFO_V1(test_jumble_plan_tree);

#if PG_VERSION_NUM >= 180000
static const char *PLAN_TREE_STRING_1 = "{SEQSCAN :scan.plan.disabled_nodes 0 :scan.plan.startup_cost 0 :scan.plan.total_cost 14 :scan.plan.plan_rows 2 :scan.plan.plan_width 222 :scan.plan.parallel_aware false :scan.plan.parallel_safe true :scan.plan.async_capable false :scan.plan.plan_node_id 0 :scan.plan.targetlist ({TARGETENTRY :expr {VAR :varno 1 :varattno 2 :vartype 1043 :vartypmod 104 :varcollid 100 :varnullingrels (b) :varlevelsup 0 :varreturningtype 0 :varnosyn 1 :varattnosyn 2 :location -1} :resno 1 :resname region_name :ressortgroupref 0 :resorigtbl 39962 :resorigcol 2 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varreturningtype 0 :varnosyn 1 :varattnosyn 1 :location -1} :resno 2 :resname region_id :ressortgroupref 0 :resorigtbl 39962 :resorigcol 1 :resjunk false}) :scan.plan.qual ({OPEXPR :opno 98 :opfuncid 67 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 100 :args ({RELABELTYPE :arg {VAR :varno 1 :varattno 2 :vartype 1043 :vartypmod 104 :varcollid 100 :varnullingrels (b) :varlevelsup 0 :varreturningtype 0 :varnosyn 1 :varattnosyn 2 :location -1} :resulttype 25 :resulttypmod -1 :resultcollid 100 :relabelformat 2 :location -1} {CONST :consttype 25 :consttypmod -1 :constcollid 100 :constlen -1 :constbyval false :constisnull false :location -1 :constvalue 12 [ 48 0 0 0 82 101 103 105 111 110 95 49 ]}) :location -1}) :scan.plan.lefttree <> :scan.plan.righttree <> :scan.plan.initPlan <> :scan.plan.extParam (b) :scan.plan.allParam (b) :scan.scanrelid 1} :partPruneInfos <> :rtable ({RANGETBLENTRY :alias <> :eref {ALIAS :aliasname regions :colnames (\"region_id\" \"region_name\")} :rtekind 0 :relid 39962 :inh false :relkind r :rellockmode 1 :perminfoindex 1 :tablesample <> :lateral false :inFromCl true :securityQuals <>}) :unprunableRelids (b 1) :permInfos ({RTEPERMISSIONINFO :relid 39962 :inh true :requiredPerms 2 :checkAsUser 0 :selectedCols (b 8 9) :insertedCols (b) :updatedCols (b)}) :resultRelations <> :appendRelations <> :subplans <> :rewindPlanIDs (b) :rowMarks <> :relationOids (o 39962) :invalItems <> :paramExecTypes <> :utilityStmt <> :stmt_location -1 :stmt_len -1}";
#elif PG_VERSION_NUM >= 160000
static const char *PLAN_TREE_STRING_1 = "{NESTLOOP :join.plan.startup_cost 1004.93875 :join.plan.total_cost 230385.06479166666 :join.plan.plan_rows 8843570 :join.plan.plan_width 116 :join.plan.parallel_aware false :join.plan.parallel_safe false :join.plan.async_capable false :join.plan.plan_node_id 0 :join.plan.targetlist ({TARGETENTRY :expr {VAR :varno -1 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 1 :location -1} :resno 1 :resname tid :ressortgroupref 0 :resorigtbl 16489 :resorigcol 1 :resjunk false} {TARGETENTRY :expr {VAR :varno -1 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} :resno 2 :resname bid :ressortgroupref 0 :resorigtbl 16489 :resorigcol 2 :resjunk false} {TARGETENTRY :expr {VAR :varno -1 :varattno 3 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location -1} :resno 3 :resname aid :ressortgroupref 0 :resorigtbl 16489 :resorigcol 3 :resjunk false} {TARGETENTRY :expr {VAR :varno -1 :varattno 4 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 4 :location -1} :resno 4 :resname delta :ressortgroupref 0 :resorigtbl 16489 :resorigcol 4 :resjunk false} {TARGETENTRY :expr {VAR :varno -1 :varattno 5 :vartype 1114 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 5 :location -1} :resno 5 :resname mtime :ressortgroupref 0 :resorigtbl 16489 :resorigcol 5 :resjunk false} {TARGETENTRY :expr {VAR :varno -1 :varattno 6 :vartype 1042 :vartypmod 26 :varcollid 100 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 6 :location -1} :resno 6 :resname filler :ressortgroupref 0 :resorigtbl 16489 :resorigcol 6 :resjunk false}) :join.plan.qual <> :join.plan.lefttree {GATHER :plan.startup_cost 1000 :plan.total_cost 119805.22604166667 :plan.plan_rows 104042 :plan.plan_width 4 :plan.parallel_aware false :plan.parallel_safe false :plan.async_capable false :plan.plan_node_id 1 :plan.targetlist ({TARGETENTRY :expr {VAR :varno -2 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 2 :varattnosyn 2 :location -1} :resno 1 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false}) :plan.qual <> :plan.lefttree {SEQSCAN :scan.plan.startup_cost 0 :scan.plan.total_cost 108401.02604166667 :scan.plan.plan_rows 43351 :scan.plan.plan_width 4 :scan.plan.parallel_aware true :scan.plan.parallel_safe true :scan.plan.async_capable false :scan.plan.plan_node_id 2 :scan.plan.targetlist ({TARGETENTRY :expr {VAR :varno 2 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 2 :varattnosyn 2 :location -1} :resno 1 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false}) :scan.plan.qual ({OPEXPR :opno 96 :opfuncid 65 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 0 :args ({VAR :varno 2 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 2 :varattnosyn 2 :location -1} {CONST :consttype 23 :consttypmod -1 :constcollid 0 :constlen 4 :constbyval true :constisnull false :location -1 :constvalue 4 [ 1 0 0 0 0 0 0 0 ]}) :location -1}) :scan.plan.lefttree <> :scan.plan.righttree <> :scan.plan.initPlan <> :scan.plan.extParam (b 0) :scan.plan.allParam (b 0) :scan.scanrelid 2} :plan.righttree <> :plan.initPlan <> :plan.extParam (b) :plan.allParam (b) :num_workers 2 :rescan_param 0 :single_copy false :invisible false :initParam (b)} :join.plan.righttree {MATERIAL :plan.startup_cost 4.938750000000001 :plan.total_cost 35.42625 :plan.plan_rows 85 :plan.plan_width 116 :plan.parallel_aware false :plan.parallel_safe true :plan.async_capable false :plan.plan_node_id 3 :plan.targetlist ({TARGETENTRY :expr {VAR :varno -2 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 1 :location -1} :resno 1 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno -2 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} :resno 2 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno -2 :varattno 3 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location -1} :resno 3 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno -2 :varattno 4 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 4 :location -1} :resno 4 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno -2 :varattno 5 :vartype 1114 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 5 :location -1} :resno 5 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno -2 :varattno 6 :vartype 1042 :vartypmod 26 :varcollid 100 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 6 :location -1} :resno 6 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false}) :plan.qual <> :plan.lefttree {BITMAPHEAPSCAN :scan.plan.startup_cost 4.938750000000001 :scan.plan.total_cost 35.00125 :scan.plan.plan_rows 85 :scan.plan.plan_width 116 :scan.plan.parallel_aware false :scan.plan.parallel_safe true :scan.plan.async_capable false :scan.plan.plan_node_id 4 :scan.plan.targetlist ({TARGETENTRY :expr {VAR :varno 1 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 1 :location -1} :resno 1 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} :resno 2 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 3 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location -1} :resno 3 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 4 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 4 :location -1} :resno 4 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 5 :vartype 1114 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 5 :location -1} :resno 5 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false} {TARGETENTRY :expr {VAR :varno 1 :varattno 6 :vartype 1042 :vartypmod 26 :varcollid 100 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 6 :location -1} :resno 6 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false}) :scan.plan.qual <> :scan.plan.lefttree {BITMAPINDEXSCAN :scan.plan.startup_cost 0 :scan.plan.total_cost 4.9175 :scan.plan.plan_rows 85 :scan.plan.plan_width 0 :scan.plan.parallel_aware false :scan.plan.parallel_safe true :scan.plan.async_capable false :scan.plan.plan_node_id 5 :scan.plan.targetlist <> :scan.plan.qual <> :scan.plan.lefttree <> :scan.plan.righttree <> :scan.plan.initPlan <> :scan.plan.extParam (b) :scan.plan.allParam (b) :scan.scanrelid 1 :indexid 196878 :isshared false :indexqual ({OPEXPR :opno 96 :opfuncid 65 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 0 :args ({VAR :varno -3 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} {CONST :consttype 23 :consttypmod -1 :constcollid 0 :constlen 4 :constbyval true :constisnull false :location -1 :constvalue 4 [ 1 0 0 0 0 0 0 0 ]}) :location -1}) :indexqualorig ({OPEXPR :opno 96 :opfuncid 65 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 0 :args ({VAR :varno 1 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} {CONST :consttype 23 :consttypmod -1 :constcollid 0 :constlen 4 :constbyval true :constisnull false :location -1 :constvalue 4 [ 1 0 0 0 0 0 0 0 ]}) :location -1})} :scan.plan.righttree <> :scan.plan.initPlan <> :scan.plan.extParam (b) :scan.plan.allParam (b) :scan.scanrelid 1 :bitmapqualorig ({OPEXPR :opno 96 :opfuncid 65 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 0 :args ({VAR :varno 1 :varattno 2 :vartype 23 :vartypmod -1 :varcollid 0 :varnullingrels (b) :varlevelsup 0 :varnosyn 1 :varattnosyn 2 :location -1} {CONST :consttype 23 :consttypmod -1 :constcollid 0 :constlen 4 :constbyval true :constisnull false :location -1 :constvalue 4 [ 1 0 0 0 0 0 0 0 ]}) :location -1})} :plan.righttree <> :plan.initPlan <> :plan.extParam (b) :plan.allParam (b)} :join.plan.initPlan <> :join.plan.extParam (b) :join.plan.allParam (b) :join.jointype 0 :join.inner_unique false :join.joinqual <> :nestParams <>}";
#else
static const char *PLAN_TREE_STRING_1 = "{INDEXONLYSCAN :startup_cost 0.28 :total_cost 8.29 :plan_rows 1 :plan_width 11 :parallel_aware false :parallel_safe true :async_capable false :plan_node_id 0 :targetlist ({TARGETENTRY :expr {VAR :varno -3 :varattno 1 :vartype 1043 :vartypmod 104 :varcollid 100 :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location 7} :resno 1 :resname last_name :ressortgroupref 0 :resorigtbl 17453 :resorigcol 3 :resjunk false}) :qual <> :lefttree <> :righttree <> :initPlan <> :extParam (b) :allParam (b) :scanrelid 1 :indexid 17460 :indexqual ({OPEXPR :opno 98 :opfuncid 67 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 100 :args ({VAR :varno -3 :varattno 1 :vartype 1043 :vartypmod 104 :varcollid 100 :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location 38} {CONST :consttype 25 :consttypmod -1 :constcollid 100 :constlen -1 :constbyval false :constisnull false :location 48 :constvalue 13 [ 52 0 0 0 99 117 115 116 111 109 101 114 49 ]}) :location 47}) :recheckqual ({OPEXPR :opno 98 :opfuncid 67 :opresulttype 16 :opretset false :opcollid 0 :inputcollid 100 :args ({RELABELTYPE :arg {VAR :varno -3 :varattno 1 :vartype 1043 :vartypmod 104 :varcollid 100 :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location 38} :resulttype 25 :resulttypmod -1 :resultcollid 100 :relabelformat 2 :location -1} {CONST :consttype 25 :consttypmod -1 :constcollid 100 :constlen -1 :constbyval false :constisnull false :location 48 :constvalue 13 [ 52 0 0 0 99 117 115 116 111 109 101 114 49 ]}) :location 47}) :indexorderby <> :indextlist ({TARGETENTRY :expr {VAR :varno 1 :varattno 3 :vartype 1043 :vartypmod 104 :varcollid 100 :varlevelsup 0 :varnosyn 1 :varattnosyn 3 :location -1} :resno 1 :resname <> :ressortgroupref 0 :resorigtbl 0 :resorigcol 0 :resjunk false}) :indexorderdir 1}";
#endif

#define JUMBLE_SIZE				1024	/* serialization buffer size */

#define APP_JUMB(item) \
	AppendJumble(jstate, (const unsigned char *) &(item), sizeof(item))

static bool test_different_index_ids(StringInfo buf);
static bool test_same_index_different_directions(StringInfo buf);
static bool test_nested_loop(StringInfo buf);
static bool test_hash_join(StringInfo buf);
static bool test_merge_join(StringInfo buf);
static bool test_subquery_scans(StringInfo buf);
static bool test_foreign_scans(StringInfo buf);
static bool test_custom_scans(StringInfo buf);
static bool test_seq_scans(StringInfo buf);
static bool test_append_plans(StringInfo buf);
static bool test_lefttree(StringInfo buf);
static bool test_righttree(StringInfo buf);
static bool test_modify_table(StringInfo buf);
static bool test_gather_plans(StringInfo buf);
static bool test_gather_merge_plans(StringInfo buf);
static bool test_index_only_scans(StringInfo buf);
static bool test_bitmap_index_scans(StringInfo buf);
static bool test_sort_plans(StringInfo buf);
static bool test_incremental_sort_plans(StringInfo buf);
static bool test_group_plans(StringInfo buf);
static bool test_agg_plans(StringInfo buf);
static bool test_window_agg_plans(StringInfo buf);
static bool test_setop_plans(StringInfo buf);
static bool test_nodes(StringInfo buf);
static bool test_merge_append_plans(StringInfo buf);
static bool test_cte_rte(StringInfo buf);
static bool test_range_table(StringInfo buf);
static bool test_bitmap_heap_scans(StringInfo buf);
static bool test_tid_scans(StringInfo buf);
static bool test_tid_range_scans(StringInfo buf);
static bool test_function_scans(StringInfo buf);
static bool test_sample_scans(StringInfo buf);
static bool test_table_func_scans(StringInfo buf);
static bool test_values_scans(StringInfo buf);
static bool test_cte_scans(StringInfo buf);
static bool test_work_table_scans(StringInfo buf);
static bool test_named_tuplestore_scans(StringInfo buf);
static bool test_unique_plans(StringInfo buf);
static bool test_memoize_plans(StringInfo buf);

Datum
test_jumble_operations(PG_FUNCTION_ARGS)
{
    StringInfoData buf;
    SeqScan *scan;
    uint64 hash1, hash2;
	JumbleState *jstate = pgsp_init_jumble_state();

    initStringInfo(&buf);

    /* Create a simple plan node */
    scan = makeNode(SeqScan);
#if PG_VERSION_NUM >= 150000
    scan->scan.scanrelid = 1;
    APP_JUMB(scan->scan.scanrelid);
#else
    scan->scanrelid = 1;
    APP_JUMB(scan->scanrelid);
#endif

    /* Test: Jumble the same node twice, should get same hash */
    
    hash1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    /* Reset and do it again */
    jstate->jumble_len = 0;

#if PG_VERSION_NUM >= 150000
    APP_JUMB(scan->scan.scanrelid);
#else
    APP_JUMB(scan->scanrelid);
#endif
    
    hash2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(&buf, "Test: Deterministic hashing - ");
    if (hash1 == hash2)
        appendStringInfo(&buf, "PASS (hash: " UINT64_FORMAT ")\n", hash1);
    else
        appendStringInfo(&buf, "FAIL (hash1: " UINT64_FORMAT ", hash2: " UINT64_FORMAT ")\n", 
                        hash1, hash2);

    pfree(jstate->jumble);
    pfree(jstate);
    pfree(scan);

    PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}

/*
 * Test plan ids generations.
 * 
 * plan_string: The string output from nodeToString()
 */
Datum
test_jumble_plan_tree(PG_FUNCTION_ARGS)
{
    StringInfoData buf;
    bool success = true;

    initStringInfo(&buf);

    if (!test_modify_table(&buf))
        success = false;
    if (!test_lefttree(&buf))
        success = false;
    if (!test_righttree(&buf))
        success = false;
    if (!test_different_index_ids(&buf))
        success = false;
    if (!test_same_index_different_directions(&buf))
        success = false;
    if (!test_nested_loop(&buf))
        success = false;
    if (!test_hash_join(&buf))
        success = false;
    if (!test_merge_join(&buf))
        success = false;
    if (!test_foreign_scans(&buf))
        success = false;
    if (!test_subquery_scans(&buf))
        success = false;
    if (!test_custom_scans(&buf))
        success = false;
    if (!test_gather_plans(&buf))
        success = false;
    if (!test_gather_merge_plans(&buf))
        success = false;
    if (!test_index_only_scans(&buf))
        success = false;
    if (!test_bitmap_index_scans(&buf))
        success = false;
    if (!test_sort_plans(&buf))
        success = false;
    if (!test_incremental_sort_plans(&buf))
        success = false;
    if (!test_group_plans(&buf))
        success = false;
    if (!test_agg_plans(&buf))
        success = false;
    if (!test_window_agg_plans(&buf))
        success = false;
    if (!test_setop_plans(&buf))
        success = false;
    if (!test_seq_scans(&buf))
        success = false;
    if (!test_append_plans(&buf))
        success = false;
    if (!test_merge_append_plans(&buf))
        success = false;
    if (!test_nodes(&buf))
        success = false;
    if (!test_cte_rte(&buf))
        success = false;
    if (!test_range_table(&buf))
        success = false;
    if (!test_bitmap_heap_scans(&buf))
        success = false;
    if (!test_tid_scans(&buf))
        success = false;
    if (!test_tid_range_scans(&buf))
        success = false;
    if (!test_function_scans(&buf))
        success = false;
    if (!test_sample_scans(&buf))
        success = false;
    if (!test_table_func_scans(&buf))
        success = false;
    if (!test_values_scans(&buf))
        success = false;
    if (!test_cte_scans(&buf))
        success = false;
    if (!test_work_table_scans(&buf))
        success = false;
    if (!test_named_tuplestore_scans(&buf))
        success = false;
    if (!test_unique_plans(&buf))
        success = false;
    if (!test_memoize_plans(&buf))
        success = false;

    if (success)
        appendStringInfoString(&buf, "\nAll tests PASSED\n");
    else
        appendStringInfoString(&buf, "\nSome tests FAILED\n");

    PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}

static bool test_bitmap_heap_scans(StringInfo buf)
{
    BitmapHeapScan *bhs1, *bhs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    bhs1 = makeNode(BitmapHeapScan);
    bhs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) bhs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    bhs2 = makeNode(BitmapHeapScan);
    bhs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) bhs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: BitmapHeapScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(bhs1);
    pfree(bhs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_tid_scans(StringInfo buf)
{
    TidScan *ts1, *ts2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    ts1 = makeNode(TidScan);
    ts1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) ts1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    ts2 = makeNode(TidScan);
    ts2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) ts2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: TidScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(ts1);
    pfree(ts2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_tid_range_scans(StringInfo buf)
{
    TidRangeScan *trs1, *trs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    trs1 = makeNode(TidRangeScan);
    trs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) trs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    trs2 = makeNode(TidRangeScan);
    trs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) trs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: TidRangeScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(trs1);
    pfree(trs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_function_scans(StringInfo buf)
{
    FunctionScan *fs1, *fs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    fs1 = makeNode(FunctionScan);
    fs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) fs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    fs2 = makeNode(FunctionScan);
    fs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) fs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: FunctionScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(fs1);
    pfree(fs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_sample_scans(StringInfo buf)
{
    SampleScan *ss1, *ss2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    ss1 = makeNode(SampleScan);
    ss1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) ss1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    ss2 = makeNode(SampleScan);
    ss2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) ss2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: SampleScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(ss1);
    pfree(ss2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_table_func_scans(StringInfo buf)
{
    TableFuncScan *tfs1, *tfs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    tfs1 = makeNode(TableFuncScan);
    tfs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) tfs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    tfs2 = makeNode(TableFuncScan);
    tfs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) tfs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: TableFuncScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(tfs1);
    pfree(tfs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_values_scans(StringInfo buf)
{
    ValuesScan *vs1, *vs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    vs1 = makeNode(ValuesScan);
    vs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) vs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    vs2 = makeNode(ValuesScan);
    vs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) vs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: ValuesScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(vs1);
    pfree(vs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_cte_scans(StringInfo buf)
{
    CteScan *cs1, *cs2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    cs1 = makeNode(CteScan);
    cs1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) cs1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    cs2 = makeNode(CteScan);
    cs2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) cs2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: CteScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(cs1);
    pfree(cs2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_work_table_scans(StringInfo buf)
{
    WorkTableScan *wts1, *wts2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    wts1 = makeNode(WorkTableScan);
    wts1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) wts1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    wts2 = makeNode(WorkTableScan);
    wts2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) wts2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: WorkTableScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(wts1);
    pfree(wts2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_named_tuplestore_scans(StringInfo buf)
{
    NamedTuplestoreScan *nts1, *nts2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    nts1 = makeNode(NamedTuplestoreScan);
    nts1->scan.scanrelid = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) nts1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    nts2 = makeNode(NamedTuplestoreScan);
    nts2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) nts2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: NamedTuplestoreScan with different scanrelid - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(nts1);
    pfree(nts2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_unique_plans(StringInfo buf)
{
    Unique *uniq1, *uniq2;
    JumbleState *jstate;
    uint64 planId1, planId2;
    AttrNumber *uniqColIdx1, *uniqColIdx2;

    jstate = pgsp_init_jumble_state();

    uniq1 = makeNode(Unique);
    uniq1->numCols = 1;
    uniqColIdx1 = palloc(sizeof(AttrNumber) * 1);
    uniqColIdx1[0] = 1;
    uniq1->uniqColIdx = uniqColIdx1;

    pgsp_jumble_plan_tree(jstate, (Plan *) uniq1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    uniq2 = makeNode(Unique);
    uniq2->numCols = 1;
    uniqColIdx2 = palloc(sizeof(AttrNumber) * 1);
    uniqColIdx2[0] = 2;
    uniq2->uniqColIdx = uniqColIdx2;

    pgsp_jumble_plan_tree(jstate, (Plan *) uniq2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Unique with different uniqColIdx - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(uniq1->uniqColIdx);
    pfree(uniq1);
    pfree(uniq2->uniqColIdx);
    pfree(uniq2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_memoize_plans(StringInfo buf)
{
    Memoize *memo1, *memo2;
    JumbleState *jstate;
    uint64 planId1, planId2;
    Oid *hashOperators1, *hashOperators2;

    jstate = pgsp_init_jumble_state();

    memo1 = makeNode(Memoize);
    memo1->numKeys = 1;
    hashOperators1 = palloc(sizeof(Oid) * 1);
    hashOperators1[0] = 96;
    memo1->hashOperators = hashOperators1;

    pgsp_jumble_plan_tree(jstate, (Plan *) memo1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    memo2 = makeNode(Memoize);
    memo2->numKeys = 1;
    hashOperators2 = palloc(sizeof(Oid) * 1);
    hashOperators2[0] = 97;
    memo2->hashOperators = hashOperators2;

    pgsp_jumble_plan_tree(jstate, (Plan *) memo2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Memoize with different hashOperators - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(memo1->hashOperators);
    pfree(memo1);
    pfree(memo2->hashOperators);
    pfree(memo2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_window_agg_plans(StringInfo buf)
{
    WindowAgg *wa1, *wa2, *wa3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: WindowAgg with winref 1 */
    wa1 = makeNode(WindowAgg);
    wa1->winref = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) wa1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: WindowAgg with different winref */
    wa2 = makeNode(WindowAgg);
    wa2->winref = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) wa2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: WindowAgg with different winref - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: WindowAgg with same winref as wa1 */
    wa3 = makeNode(WindowAgg);
    wa3->winref = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) wa3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: WindowAgg with same winref - ");
    if (planId1 == planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId1: " UINT64_FORMAT ", planId3: " UINT64_FORMAT ")\n", planId1, planId3);

    pfree(wa1);
    pfree(wa2);
    pfree(wa3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 == planId3);
}


static bool test_merge_append_plans(StringInfo buf)
{
    MergeAppend *ma1, *ma2;
    SeqScan *scan1, *scan2, *scan3;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    /* Test 1: MergeAppend with two child plans, 1 sort col */
    ma1 = makeNode(MergeAppend);
    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);
#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 1;
    scan2->scan.scanrelid = 2;
#else
    scan1->scanrelid = 1;
    scan2->scanrelid = 2;
#endif
    ma1->mergeplans = list_make2(scan1, scan2);

    pgsp_jumble_plan_tree(jstate, (Plan *) ma1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    /* Test 2: MergeAppend with three child plans (different from ma1) */
    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    ma2 = makeNode(MergeAppend);
    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);
    scan3 = makeNode(SeqScan);
#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 1;
    scan2->scan.scanrelid = 2;
    scan3->scan.scanrelid = 3;
#else
    scan1->scanrelid = 1;
    scan2->scanrelid = 2;
    scan3->scanrelid = 3;
#endif
    ma2->mergeplans = list_make3(scan1, scan2, scan3);

    pgsp_jumble_plan_tree(jstate, (Plan *) ma2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: MergeAppend with different number of children - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    return (planId1 != planId2);
}
static bool test_setop_plans(StringInfo buf)
{
    SetOp *setop1, *setop2, *setop3, *setop4;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3, planId4;
    AttrNumber *dupColIdx1, *dupColIdx2, *dupColIdx3;
    Oid *dupOperators1, *dupOperators2, *dupOperators3;
    Oid *dupCollations1, *dupCollations2, *dupCollations3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: SetOp with INTERSECT command */
    setop1 = makeNode(SetOp);
    setop1->cmd = SETOPCMD_INTERSECT;
    setop1->strategy = SETOP_SORTED;
    setop1->numCols = 1;
    dupColIdx1 = palloc(sizeof(AttrNumber) * 1);
    dupColIdx1[0] = 1;
    dupOperators1 = palloc(sizeof(Oid) * 1);
    dupOperators1[0] = 96;
    dupCollations1 = palloc(sizeof(Oid) * 1);
    dupCollations1[0] = 0;
#if PG_VERSION_NUM >= 180000
    setop1->cmpColIdx = dupColIdx1;
    setop1->cmpOperators = dupOperators1;
    setop1->cmpCollations = dupCollations1;
#else
    setop1->dupColIdx = dupColIdx1;
    setop1->dupOperators = dupOperators1;
    setop1->dupCollations = dupCollations1;
#endif
    pgsp_jumble_plan_tree(jstate, (Plan *) setop1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: SetOp with different command (EXCEPT) */
    setop2 = makeNode(SetOp);
    setop2->cmd = SETOPCMD_EXCEPT;
    setop2->strategy = SETOP_SORTED;
    setop2->numCols = 1;
    dupColIdx2 = palloc(sizeof(AttrNumber) * 1);
    dupColIdx2[0] = 1;
    dupOperators2 = palloc(sizeof(Oid) * 1);
    dupOperators2[0] = 96;
    dupCollations2 = palloc(sizeof(Oid) * 1);
    dupCollations2[0] = 0;

#if PG_VERSION_NUM >= 180000
    setop2->cmpColIdx = dupColIdx2;
    setop2->cmpOperators = dupOperators2;
    setop2->cmpCollations = dupCollations2;
#else
    setop2->dupColIdx = dupColIdx2;
    setop2->dupOperators = dupOperators2;
    setop2->dupCollations = dupCollations2;
#endif

    pgsp_jumble_plan_tree(jstate, (Plan *) setop2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: SetOp with different commands - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: SetOp with different strategy */
    setop3 = makeNode(SetOp);
    setop3->cmd = SETOPCMD_INTERSECT;
    setop3->strategy = SETOP_HASHED;
    setop3->numCols = 1;
    dupColIdx3 = palloc(sizeof(AttrNumber) * 1);
    dupColIdx3[0] = 1;
    dupOperators3 = palloc(sizeof(Oid) * 1);
    dupOperators3[0] = 96;
    dupCollations3 = palloc(sizeof(Oid) * 1);
    dupCollations3[0] = 0;
#if PG_VERSION_NUM >= 180000
    setop3->cmpColIdx = dupColIdx3;
    setop3->cmpOperators = dupOperators3;
    setop3->cmpCollations = dupCollations3;
#else
    setop3->dupColIdx = dupColIdx3;
    setop3->dupOperators = dupOperators3;
    setop3->dupCollations = dupCollations3;
#endif

    pgsp_jumble_plan_tree(jstate, (Plan *) setop3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: SetOp with different strategy - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 4: SetOp with different numCols */
    setop4 = makeNode(SetOp);
    setop4->cmd = SETOPCMD_INTERSECT;
    setop4->strategy = SETOP_SORTED;
    setop4->numCols = 2;
#if PG_VERSION_NUM >= 180000
    setop4->cmpColIdx = palloc(sizeof(AttrNumber) * 2);
    setop4->cmpColIdx[0] = 1;
    setop4->cmpColIdx[1] = 2;
    setop4->cmpOperators = palloc(sizeof(Oid) * 2);
    setop4->cmpOperators[0] = 96;
    setop4->cmpOperators[1] = 96;
    setop4->cmpCollations = palloc(sizeof(Oid) * 2);
    setop4->cmpCollations[0] = 0;
    setop4->cmpCollations[1] = 0;
#else
    setop4->dupColIdx = palloc(sizeof(AttrNumber) * 2
    setop4->dupColIdx[0] = 1;
    setop4->dupColIdx[1] = 2;
    setop4->dupOperators = palloc(sizeof(Oid) * 2);
    setop4->dupOperators[0] = 96;
    setop4->dupOperators[1] = 96;
    setop4->dupCollations = palloc(sizeof(Oid) * 2);
    setop4->dupCollations[0] = 0;
    setop4->dupCollations[1] = 0;
#endif
    pgsp_jumble_plan_tree(jstate, (Plan *) setop4);
    planId4 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: SetOp with different numCols - ");
    if (planId1 != planId4)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    pfree(setop1);
    pfree(setop2);
    pfree(setop3);
    pfree(setop4);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3 && planId1 != planId4);
}
static bool
test_different_index_ids(StringInfo buf)
{
    IndexScan *iscan1, *iscan2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    iscan1 = makeNode(IndexScan);
    iscan1->scan.scanrelid = 1;
    iscan1->indexid = 100;
    iscan1->indexorderdir = ForwardScanDirection;

    iscan2 = makeNode(IndexScan);
    iscan2->scan.scanrelid = 1;
    iscan2->indexid = 200;
    iscan2->indexorderdir = ForwardScanDirection;

    pgsp_jumble_plan_tree(jstate, (Plan *) iscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) iscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Different index IDs scans- ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);
    
    pfree(iscan1);
    pfree(iscan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool
test_same_index_different_directions(StringInfo buf)
{
    IndexScan *iscan1, *iscan2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    iscan1 = makeNode(IndexScan);
    iscan1->scan.scanrelid = 1;
    iscan1->indexid = 100;
    iscan1->indexorderdir = ForwardScanDirection;

    iscan2 = makeNode(IndexScan);
    iscan2->scan.scanrelid = 1;
    iscan2->indexid = 100;
    iscan2->indexorderdir = BackwardScanDirection;

    pgsp_jumble_plan_tree(jstate, (Plan *) iscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) iscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: same index IDs scans with different directions - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(iscan1);
    pfree(iscan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool
test_merge_join(StringInfo buf)
{
    MergeJoin *mjoin1, *mjoin2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    mjoin1 = makeNode(MergeJoin);
    mjoin1->join.joinqual = NULL;
    mjoin1->join.jointype = JOIN_INNER;
    mjoin1->join.inner_unique = false;

    mjoin2 = makeNode(MergeJoin);
    mjoin2->join.joinqual = NULL;
    mjoin2->join.jointype = JOIN_LEFT;
    mjoin2->join.inner_unique = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) mjoin1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) mjoin2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: merge join types - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(mjoin1);
    pfree(mjoin2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_subquery_scans(StringInfo buf)
{
    SubqueryScan *sscan1, *sscan2;
    JumbleState *jstate;
    uint64 planId1, planId2;
    Plan *subplan1, *subplan2;

    jstate = pgsp_init_jumble_state();

    /* Test 1: SubqueryScan with a specific subplan */
    sscan1 = makeNode(SubqueryScan);
    sscan1->scan.scanrelid = 1;
    subplan1 = (Plan *) makeNode(SeqScan);
    subplan1->plan_node_id = 1;
    sscan1->subplan = subplan1;

    pgsp_jumble_plan_tree(jstate, (Plan *) sscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: SubqueryScan with a different subplan */
    sscan2 = makeNode(SubqueryScan);
    sscan2->scan.scanrelid = 1;
    subplan2 = (Plan *) makeNode(SeqScan);
    subplan2->plan_node_id = 2; /* Different node ID in subplan makes it different */
    sscan2->subplan = subplan2;

    pgsp_jumble_plan_tree(jstate, (Plan *) sscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: SubqueryScan with different subplans - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(sscan1);
    pfree(sscan2);
    pfree(subplan1);
    pfree(subplan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool
test_hash_join(StringInfo buf)
{
    HashJoin *hjoin1, *hjoin2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    hjoin1 = makeNode(HashJoin);
    hjoin1->join.joinqual = NULL;
    hjoin1->join.jointype = JOIN_INNER;
    hjoin1->join.inner_unique = false;

    hjoin2 = makeNode(HashJoin);
    hjoin2->join.joinqual = NULL;
    hjoin2->join.jointype = JOIN_LEFT;
    hjoin2->join.inner_unique = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) hjoin1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) hjoin2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: hash join types - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(hjoin1);
    pfree(hjoin2);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool
test_nested_loop(StringInfo buf)
{
    NestLoop *nloop1 , *nloop2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    nloop1 = makeNode(NestLoop);
    nloop1->join.joinqual = NULL;   
    nloop1->join.jointype = JOIN_INNER;
    nloop1->join.inner_unique = false;

    nloop2 = makeNode(NestLoop);
    nloop2->join.joinqual = NULL;   
    nloop2->join.jointype = JOIN_LEFT;
    nloop2->join.inner_unique = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) nloop1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) nloop2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: nested loop join types - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(nloop1);
    pfree(nloop2);  
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_seq_scans(StringInfo buf)
{
    SeqScan *scan1, *scan2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);

#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 1;
    scan2->scan.scanrelid = 2;
#else
    scan1->scanrelid = 1;
    scan2->scanrelid = 2;
#endif
    pgsp_jumble_plan_tree(jstate, (Plan *) scan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) scan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Different seq scan relids - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");        
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);
    
    pfree(scan1);
    pfree(scan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_foreign_scans(StringInfo buf)
{
    ForeignScan *fscan1, *fscan2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    fscan1 = makeNode(ForeignScan);
    fscan1->scan.scanrelid = 1;

    fscan2 = makeNode(ForeignScan);
    fscan2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) fscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) fscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Different foreign scan relids - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);
    
    pfree(fscan1);
    pfree(fscan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_custom_scans(StringInfo buf)
{
    CustomScan *cscan1, *cscan2;
    JumbleState *jstate;
    uint64 planId1, planId2;

    jstate = pgsp_init_jumble_state();

    cscan1 = makeNode(CustomScan);
    cscan1->scan.scanrelid = 1;

    cscan2 = makeNode(CustomScan);
    cscan2->scan.scanrelid = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) cscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();
    pgsp_jumble_plan_tree(jstate, (Plan *) cscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Different custom scan relids - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);
    
    pfree(cscan1);
    pfree(cscan2);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_gather_plans(StringInfo buf)
{
    Gather *gather1, *gather2, *gather3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Gather with 2 workers */
    gather1 = makeNode(Gather);
    gather1->num_workers = 2;
    gather1->single_copy = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) gather1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    gather2 = makeNode(Gather);
    gather2->num_workers = 4;
    gather2->single_copy = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) gather2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Gather with different num_workers - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    gather3 = makeNode(Gather);
    gather3->num_workers = 2;
    gather3->single_copy = true;

    pgsp_jumble_plan_tree(jstate, (Plan *) gather3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Gather with different single_copy flag - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(gather1);
    pfree(gather2);
    pfree(gather3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_gather_merge_plans(StringInfo buf)
{
    GatherMerge *gm1, *gm2, *gm3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: GatherMerge with 2 workers */
    gm1 = makeNode(GatherMerge);
    gm1->num_workers = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) gm1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: GatherMerge with 4 workers */
    gm2 = makeNode(GatherMerge);
    gm2->num_workers = 4;

    pgsp_jumble_plan_tree(jstate, (Plan *) gm2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: GatherMerge with different num_workers - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: GatherMerge with same workers but different numCols */
    gm3 = makeNode(GatherMerge);
    gm3->num_workers = 2;
    gm3->numCols = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) gm3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: GatherMerge with different numCols - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(gm1);
    pfree(gm2);
    pfree(gm3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_index_only_scans(StringInfo buf)
{
    IndexOnlyScan *ioscan1, *ioscan2, *ioscan3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: IndexOnlyScan with index 100 */
    ioscan1 = makeNode(IndexOnlyScan);
    ioscan1->scan.scanrelid = 1;
    ioscan1->indexid = 100;
    ioscan1->indexorderdir = ForwardScanDirection;

    pgsp_jumble_plan_tree(jstate, (Plan *) ioscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: IndexOnlyScan with different index */
    ioscan2 = makeNode(IndexOnlyScan);
    ioscan2->scan.scanrelid = 1;
    ioscan2->indexid = 200;
    ioscan2->indexorderdir = ForwardScanDirection;

    pgsp_jumble_plan_tree(jstate, (Plan *) ioscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: IndexOnlyScan with different index IDs - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: IndexOnlyScan with same index but different direction */
    ioscan3 = makeNode(IndexOnlyScan);
    ioscan3->scan.scanrelid = 1;
    ioscan3->indexid = 100;
    ioscan3->indexorderdir = BackwardScanDirection;

    pgsp_jumble_plan_tree(jstate, (Plan *) ioscan3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: IndexOnlyScan with different scan directions - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(ioscan1);
    pfree(ioscan2);
    pfree(ioscan3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_bitmap_index_scans(StringInfo buf)
{
    BitmapIndexScan *biscan1, *biscan2, *biscan3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    biscan1 = makeNode(BitmapIndexScan);
    biscan1->scan.scanrelid = 1;
    biscan1->indexid = 100;

    pgsp_jumble_plan_tree(jstate, (Plan *) biscan1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    biscan2 = makeNode(BitmapIndexScan);
    biscan2->scan.scanrelid = 1;
    biscan2->indexid = 200;

    pgsp_jumble_plan_tree(jstate, (Plan *) biscan2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: BitmapIndexScan with different index IDs - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    biscan3 = makeNode(BitmapIndexScan);
    biscan3->scan.scanrelid = 2;
    biscan3->indexid = 100;

    pgsp_jumble_plan_tree(jstate, (Plan *) biscan3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: BitmapIndexScan with different scanrelid - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(biscan1);
    pfree(biscan2);
    pfree(biscan3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_append_plans(StringInfo buf)
{
    Append *append1, *append2, *append3;
    SeqScan *scan1, *scan2, *scan3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Append with two child plans */
    append1 = makeNode(Append);
    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);
#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 1;
    scan2->scan.scanrelid = 2;
#else
    scan1->scanrelid = 1;
    scan2->scanrelid = 2;
#endif

    append1->appendplans = list_make2(scan1, scan2);

    pgsp_jumble_plan_tree(jstate, (Plan *) append1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    /* Test 2: Append with three child plans (different from append1) */
    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    append2 = makeNode(Append);
    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);
    scan3 = makeNode(SeqScan);

#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 1;
    scan2->scan.scanrelid = 2;
    scan3->scan.scanrelid = 3;
#else
    scan1->scanrelid = 1;
    scan2->scanrelid = 2;
    scan3->scanrelid = 3;
#endif

    append2->appendplans = list_make3(scan1, scan2, scan3);

    pgsp_jumble_plan_tree(jstate, (Plan *) append2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Append with different number of children - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    /* Test 3: Append with same number of children but different order */
    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    append3 = makeNode(Append);
    scan1 = makeNode(SeqScan);
    scan2 = makeNode(SeqScan);

#if PG_VERSION_NUM >= 150000
    scan1->scan.scanrelid = 2;
    scan2->scan.scanrelid = 1;
#else
    scan1->scanrelid = 2;
    scan2->scanrelid = 1;
#endif

    append3->appendplans = list_make2(scan1, scan2);

    pgsp_jumble_plan_tree(jstate, (Plan *) append3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Append with same children but different order - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    list_free_deep(append1->appendplans);
    pfree(append1);
    list_free_deep(append2->appendplans);
    pfree(append2);
    list_free_deep(append3->appendplans);
    pfree(append3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_modify_table(StringInfo buf)
{
    ModifyTable *mt1, *mt2, *mt3;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: ModifyTable with INSERT operation */
    mt1 = makeNode(ModifyTable);
    mt1->operation = CMD_INSERT;
    mt1->nominalRelation = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) mt1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    mt2 = makeNode(ModifyTable);
    mt2->operation = CMD_UPDATE;
    mt2->nominalRelation = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) mt2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: ModifyTable with different operations - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    mt3 = makeNode(ModifyTable);
    mt3->operation = CMD_INSERT;
    mt3->nominalRelation = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) mt3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: ModifyTable with different result relations - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(mt1);
    pfree(mt2);
    pfree(mt3);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3);
}

static bool test_sort_plans(StringInfo buf)
{
    Sort *sort1, *sort2, *sort3, *sort4;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3, planId4;
    AttrNumber *sortColIdx1, *sortColIdx2, *sortColIdx3;
    Oid *sortOperators1, *sortOperators2, *sortOperators3;
    Oid *collations1, *collations2, *collations3;
    bool *nullsFirst1, *nullsFirst2, *nullsFirst3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Sort with 1 column */
    sort1 = makeNode(Sort);
    sort1->numCols = 1;
    sortColIdx1 = palloc(sizeof(AttrNumber) * 1);
    sortColIdx1[0] = 1;
    sort1->sortColIdx = sortColIdx1;
    sortOperators1 = palloc(sizeof(Oid) * 1);
    sortOperators1[0] = 97; /* int4_lt */
    sort1->sortOperators = sortOperators1;
    collations1 = palloc(sizeof(Oid) * 1);
    collations1[0] = 0;
    sort1->collations = collations1;
    nullsFirst1 = palloc(sizeof(bool) * 1);
    nullsFirst1[0] = false;
    sort1->nullsFirst = nullsFirst1;

    pgsp_jumble_plan_tree(jstate, (Plan *) sort1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: Sort with 2 columns (different from sort1) */
    sort2 = makeNode(Sort);
    sort2->numCols = 2;
    sortColIdx2 = palloc(sizeof(AttrNumber) * 2);
    sortColIdx2[0] = 1;
    sortColIdx2[1] = 2;
    sort2->sortColIdx = sortColIdx2;
    sortOperators2 = palloc(sizeof(Oid) * 2);
    sortOperators2[0] = 97;
    sortOperators2[1] = 97;
    sort2->sortOperators = sortOperators2;
    collations2 = palloc(sizeof(Oid) * 2);
    collations2[0] = 0;
    collations2[1] = 0;
    sort2->collations = collations2;
    nullsFirst2 = palloc(sizeof(bool) * 2);
    nullsFirst2[0] = false;
    nullsFirst2[1] = false;
    sort2->nullsFirst = nullsFirst2;

    pgsp_jumble_plan_tree(jstate, (Plan *) sort2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Sort with different numCols - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: Sort with same numCols but different sort operator */
    sort3 = makeNode(Sort);
    sort3->numCols = 1;
    sortColIdx3 = palloc(sizeof(AttrNumber) * 1);
    sortColIdx3[0] = 1;
    sort3->sortColIdx = sortColIdx3;
    sortOperators3 = palloc(sizeof(Oid) * 1);
    sortOperators3[0] = 521; /* int4_gt */
    sort3->sortOperators = sortOperators3;
    collations3 = palloc(sizeof(Oid) * 1);
    collations3[0] = 0;
    sort3->collations = collations3;
    nullsFirst3 = palloc(sizeof(bool) * 1);
    nullsFirst3[0] = false;
    sort3->nullsFirst = nullsFirst3;

    pgsp_jumble_plan_tree(jstate, (Plan *) sort3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Sort with different sort operator - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 4: Sort with same numCols and operator but different nullsFirst */
    sort4 = makeNode(Sort);
    sort4->numCols = 1;
    sort4->sortColIdx = palloc(sizeof(AttrNumber) * 1);
    sort4->sortColIdx[0] = 1;
    sort4->sortOperators = palloc(sizeof(Oid) * 1);
    sort4->sortOperators[0] = 97;
    sort4->collations = palloc(sizeof(Oid) * 1);
    sort4->collations[0] = 0;
    sort4->nullsFirst = palloc(sizeof(bool) * 1);
    sort4->nullsFirst[0] = true;

    pgsp_jumble_plan_tree(jstate, (Plan *) sort4);
    planId4 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Sort with different nullsFirst - ");
    if (planId1 != planId4)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(sort1->sortColIdx);
    pfree(sort1->sortOperators);
    pfree(sort1->collations);
    pfree(sort1->nullsFirst);
    pfree(sort1);
    
    pfree(sort2->sortColIdx);
    pfree(sort2->sortOperators);
    pfree(sort2->collations);
    pfree(sort2->nullsFirst);
    pfree(sort2);
    
    pfree(sort3->sortColIdx);
    pfree(sort3->sortOperators);
    pfree(sort3->collations);
    pfree(sort3->nullsFirst);
    pfree(sort3);
    
    pfree(sort4->sortColIdx);
    pfree(sort4->sortOperators);
    pfree(sort4->collations);
    pfree(sort4->nullsFirst);
    pfree(sort4);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3 && planId1 != planId4);
}

static bool test_group_plans(StringInfo buf)
{
    Group *grp1, *grp2, *grp3, *grp4;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3, planId4;
    AttrNumber *grpColIdx1, *grpColIdx2, *grpColIdx3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Group with 1 column */
    grp1 = makeNode(Group);
    grp1->numCols = 1;
    grpColIdx1 = palloc(sizeof(AttrNumber) * 1);
    grpColIdx1[0] = 1;
    grp1->grpColIdx = grpColIdx1;

    pgsp_jumble_plan_tree(jstate, (Plan *) grp1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: Group with 2 columns (different from grp1) */
    grp2 = makeNode(Group);
    grp2->numCols = 2;
    grpColIdx2 = palloc(sizeof(AttrNumber) * 2);
    grpColIdx2[0] = 1;
    grpColIdx2[1] = 2;
    grp2->grpColIdx = grpColIdx2;

    pgsp_jumble_plan_tree(jstate, (Plan *) grp2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Group with different numCols - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: Group with same numCols but different column indices */
    grp3 = makeNode(Group);
    grp3->numCols = 1;
    grpColIdx3 = palloc(sizeof(AttrNumber) * 1);
    grpColIdx3[0] = 2;
    grp3->grpColIdx = grpColIdx3;

    pgsp_jumble_plan_tree(jstate, (Plan *) grp3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Group with different column indices - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 4: Group with same columns but different order */
    grp4 = makeNode(Group);
    grp4->numCols = 2;
    grp4->grpColIdx = palloc(sizeof(AttrNumber) * 2);
    grp4->grpColIdx[0] = 2;
    grp4->grpColIdx[1] = 1;

    pgsp_jumble_plan_tree(jstate, (Plan *) grp4);
    planId4 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Group with different column order - ");
    if (planId2 != planId4)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId2);

    pfree(grp1->grpColIdx);
    pfree(grp1);
    
    pfree(grp2->grpColIdx);
    pfree(grp2);
    
    pfree(grp3->grpColIdx);
    pfree(grp3);
    
    pfree(grp4->grpColIdx);
    pfree(grp4);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3 && planId2 != planId4);
}

static bool test_agg_plans(StringInfo buf)
{
    Agg *agg1, *agg2, *agg3, *agg4, *agg5;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3, planId4, planId5;
    AttrNumber *grpColIdx1, *grpColIdx2, *grpColIdx3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Agg with AGG_PLAIN strategy */
    agg1 = makeNode(Agg);
    agg1->aggstrategy = AGG_PLAIN;
    agg1->aggsplit = AGGSPLIT_SIMPLE;
    agg1->numCols = 1;
    grpColIdx1 = palloc(sizeof(AttrNumber) * 1);
    grpColIdx1[0] = 1;
    agg1->grpColIdx = grpColIdx1;

    pgsp_jumble_plan_tree(jstate, (Plan *) agg1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: Agg with different strategy */
    agg2 = makeNode(Agg);
    agg2->aggstrategy = AGG_HASHED;
    agg2->aggsplit = AGGSPLIT_SIMPLE;
    agg2->numCols = 1;
    grpColIdx2 = palloc(sizeof(AttrNumber) * 1);
    grpColIdx2[0] = 1;
    agg2->grpColIdx = grpColIdx2;

    pgsp_jumble_plan_tree(jstate, (Plan *) agg2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Agg with different aggstrategy - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: Agg with different aggsplit */
    agg3 = makeNode(Agg);
    agg3->aggstrategy = AGG_PLAIN;
    agg3->aggsplit = AGGSPLIT_INITIAL_SERIAL;
    agg3->numCols = 1;
    grpColIdx3 = palloc(sizeof(AttrNumber) * 1);
    grpColIdx3[0] = 1;
    agg3->grpColIdx = grpColIdx3;

    pgsp_jumble_plan_tree(jstate, (Plan *) agg3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Agg with different aggsplit - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 4: Agg with different numCols */
    agg4 = makeNode(Agg);
    agg4->aggstrategy = AGG_PLAIN;
    agg4->aggsplit = AGGSPLIT_SIMPLE;
    agg4->numCols = 2;
    agg4->grpColIdx = palloc(sizeof(AttrNumber) * 2);
    agg4->grpColIdx[0] = 1;
    agg4->grpColIdx[1] = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) agg4);
    planId4 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Agg with different numCols - ");
    if (planId1 != planId4)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 5: Agg with different grpColIdx values */
    agg5 = makeNode(Agg);
    agg5->aggstrategy = AGG_PLAIN;
    agg5->aggsplit = AGGSPLIT_SIMPLE;
    agg5->numCols = 1;
    agg5->grpColIdx = palloc(sizeof(AttrNumber) * 1);
    agg5->grpColIdx[0] = 2;

    pgsp_jumble_plan_tree(jstate, (Plan *) agg5);
    planId5 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Agg with different grpColIdx - ");
    if (planId1 != planId5)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(agg1->grpColIdx);
    pfree(agg1);
    
    pfree(agg2->grpColIdx);
    pfree(agg2);
    
    pfree(agg3->grpColIdx);
    pfree(agg3);
    
    pfree(agg4->grpColIdx);
    pfree(agg4);
    
    pfree(agg5->grpColIdx);
    pfree(agg5);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3 && planId1 != planId4 && planId1 != planId5);
}


static bool test_range_table(StringInfo buf)
{
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;
    List *rtable1, *rtable2, *rtable3;
    RangeTblEntry *rte1, *rte2, *rte3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: Range table with one RTE */
    rte1 = makeNode(RangeTblEntry);
    rte1->rtekind = RTE_RELATION;
    rte1->relid = 100;
    rte1->rellockmode = AccessShareLock;
    rtable1 = list_make1(rte1);

    pgsp_jumble_range_table(jstate, rtable1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: Same range table, should produce same hash */
    rte2 = makeNode(RangeTblEntry);
    rte2->rtekind = RTE_RELATION;
    rte2->relid = 100;
    rte2->rellockmode = AccessShareLock;
    rtable2 = list_make1(rte2);

    pgsp_jumble_range_table(jstate, rtable2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Range table deterministic hashing - ");
    if (planId1 == planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId1: " UINT64_FORMAT ", planId2: " UINT64_FORMAT ")\n", planId1, planId2);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: Different range table (different relid) */
    rte3 = makeNode(RangeTblEntry);
    rte3->rtekind = RTE_RELATION;
    rte3->relid = 200; /* Different relid */
    rte3->rellockmode = AccessShareLock;
    rtable3 = list_make1(rte3);

    pgsp_jumble_range_table(jstate, rtable3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Range table different content - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    list_free_deep(rtable1);
    list_free_deep(rtable2);
    list_free_deep(rtable3);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 == planId2 && planId1 != planId3);
}

static bool test_cte_rte(StringInfo buf)
{
    JumbleState *jstate;
    uint64 planId1, planId2, planId3;
    List *rtable1, *rtable2, *rtable3;
    RangeTblEntry *rte1, *rte2, *rte3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: RTE_CTE with specific name */
    rte1 = makeNode(RangeTblEntry);
    rte1->rtekind = RTE_CTE;
    rte1->ctename = "cte1";
    rte1->self_reference = false;
    rtable1 = list_make1(rte1);

    pgsp_jumble_range_table(jstate, rtable1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: RTE_CTE with different name */
    rte2 = makeNode(RangeTblEntry);
    rte2->rtekind = RTE_CTE;
    rte2->ctename = "cte2";
    rte2->self_reference = false;
    rtable2 = list_make1(rte2);

    pgsp_jumble_range_table(jstate, rtable2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: RTE_CTE with different names - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: RTE_CTE with same name */
    rte3 = makeNode(RangeTblEntry);
    rte3->rtekind = RTE_CTE;
    rte3->ctename = "cte1";
    rtable3 = list_make1(rte3);

    pgsp_jumble_range_table(jstate, rtable3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: RTE_CTE with same names - ");
    if (planId1 == planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId1: " UINT64_FORMAT ", planId2: " UINT64_FORMAT ")\n", planId1, planId2);

    list_free_deep(rtable1);
    list_free_deep(rtable2);
    list_free_deep(rtable3);
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 == planId3);
}

static bool test_incremental_sort_plans(StringInfo buf)
{
    IncrementalSort *isort1, *isort2, *isort3, *isort4;
    JumbleState *jstate;
    uint64 planId1, planId2, planId3, planId4;
    AttrNumber *sortColIdx1, *sortColIdx2, *sortColIdx3;
    Oid *sortOperators1, *sortOperators2, *sortOperators3;
    Oid *collations1, *collations2, *collations3;
    bool *nullsFirst1, *nullsFirst2, *nullsFirst3;

    jstate = pgsp_init_jumble_state();

    /* Test 1: IncrementalSort with 2 columns, 1 presorted */
    isort1 = makeNode(IncrementalSort);
    isort1->sort.numCols = 2;
    isort1->nPresortedCols = 1;
    sortColIdx1 = palloc(sizeof(AttrNumber) * 2);
    sortColIdx1[0] = 1;
    sortColIdx1[1] = 2;
    isort1->sort.sortColIdx = sortColIdx1;
    sortOperators1 = palloc(sizeof(Oid) * 2);
    sortOperators1[0] = 97;
    sortOperators1[1] = 97;
    isort1->sort.sortOperators = sortOperators1;
    collations1 = palloc(sizeof(Oid) * 2);
    collations1[0] = 0;
    collations1[1] = 0;
    isort1->sort.collations = collations1;
    nullsFirst1 = palloc(sizeof(bool) * 2);
    nullsFirst1[0] = false;
    nullsFirst1[1] = false;
    isort1->sort.nullsFirst = nullsFirst1;

    pgsp_jumble_plan_tree(jstate, (Plan *) isort1);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 2: IncrementalSort with different nPresortedCols */
    isort2 = makeNode(IncrementalSort);
    isort2->sort.numCols = 2;
    isort2->nPresortedCols = 0;
    sortColIdx2 = palloc(sizeof(AttrNumber) * 2);
    sortColIdx2[0] = 1;
    sortColIdx2[1] = 2;
    isort2->sort.sortColIdx = sortColIdx2;
    sortOperators2 = palloc(sizeof(Oid) * 2);
    sortOperators2[0] = 97;
    sortOperators2[1] = 97;
    isort2->sort.sortOperators = sortOperators2;
    collations2 = palloc(sizeof(Oid) * 2);
    collations2[0] = 0;
    collations2[1] = 0;
    isort2->sort.collations = collations2;
    nullsFirst2 = palloc(sizeof(bool) * 2);
    nullsFirst2[0] = false;
    nullsFirst2[1] = false;
    isort2->sort.nullsFirst = nullsFirst2;

    pgsp_jumble_plan_tree(jstate, (Plan *) isort2);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: IncrementalSort with different nPresortedCols - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 3: IncrementalSort with different numCols */
    isort3 = makeNode(IncrementalSort);
    isort3->sort.numCols = 3;
    isort3->nPresortedCols = 1;
    sortColIdx3 = palloc(sizeof(AttrNumber) * 3);
    sortColIdx3[0] = 1;
    sortColIdx3[1] = 2;
    sortColIdx3[2] = 3;
    isort3->sort.sortColIdx = sortColIdx3;
    sortOperators3 = palloc(sizeof(Oid) * 3);
    sortOperators3[0] = 97;
    sortOperators3[1] = 97;
    sortOperators3[2] = 97;
    isort3->sort.sortOperators = sortOperators3;
    collations3 = palloc(sizeof(Oid) * 3);
    collations3[0] = 0;
    collations3[1] = 0;
    collations3[2] = 0;
    isort3->sort.collations = collations3;
    nullsFirst3 = palloc(sizeof(bool) * 3);
    nullsFirst3[0] = false;
    nullsFirst3[1] = false;
    nullsFirst3[2] = false;
    isort3->sort.nullsFirst = nullsFirst3;

    pgsp_jumble_plan_tree(jstate, (Plan *) isort3);
    planId3 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: IncrementalSort with different numCols - ");
    if (planId1 != planId3)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    /* Test 4: IncrementalSort with different sort operators */
    isort4 = makeNode(IncrementalSort);
    isort4->sort.numCols = 2;
    isort4->nPresortedCols = 1;
    isort4->sort.sortColIdx = palloc(sizeof(AttrNumber) * 2);
    isort4->sort.sortColIdx[0] = 1;
    isort4->sort.sortColIdx[1] = 2;
    isort4->sort.sortOperators = palloc(sizeof(Oid) * 2);
    isort4->sort.sortOperators[0] = 97;
    isort4->sort.sortOperators[1] = 521; /* different operator */
    isort4->sort.collations = palloc(sizeof(Oid) * 2);
    isort4->sort.collations[0] = 0;
    isort4->sort.collations[1] = 0;
    isort4->sort.nullsFirst = palloc(sizeof(bool) * 2);
    isort4->sort.nullsFirst[0] = false;
    isort4->sort.nullsFirst[1] = false;

    pgsp_jumble_plan_tree(jstate, (Plan *) isort4);
    planId4 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: IncrementalSort with different sort operators - ");
    if (planId1 != planId4)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (both planIds: " UINT64_FORMAT ")\n", planId1);

    pfree(isort1->sort.sortColIdx);
    pfree(isort1->sort.sortOperators);
    pfree(isort1->sort.collations);
    pfree(isort1->sort.nullsFirst);
    pfree(isort1);
    
    pfree(isort2->sort.sortColIdx);
    pfree(isort2->sort.sortOperators);
    pfree(isort2->sort.collations);
    pfree(isort2->sort.nullsFirst);
    pfree(isort2);
    
    pfree(isort3->sort.sortColIdx);
    pfree(isort3->sort.sortOperators);
    pfree(isort3->sort.collations);
    pfree(isort3->sort.nullsFirst);
    pfree(isort3);
    
    pfree(isort4->sort.sortColIdx);
    pfree(isort4->sort.sortOperators);
    pfree(isort4->sort.collations);
    pfree(isort4->sort.nullsFirst);
    pfree(isort4);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2 && planId1 != planId3 && planId1 != planId4);
}
static bool test_lefttree(StringInfo buf)
{
    JumbleState *jstate;
    uint64 planId1, planId2;
    Node       *node;
    Plan       *plan;
    
    jstate = pgsp_init_jumble_state();

    node = stringToNode(PLAN_TREE_STRING_1);

    if (!node)
    {
        return false;
    }

    plan = (Plan *) node;

    pgsp_jumble_plan_tree(jstate, plan);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));


    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();

    plan->lefttree = (Plan *) copyObject(plan);
    pgsp_jumble_plan_tree(jstate, plan);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));

    appendStringInfo(buf, "Test: Left tree - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId: " UINT64_FORMAT ")\n", planId1);

    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}

static bool test_righttree(StringInfo buf)
{
    JumbleState *jstate;
    uint64 planId1, planId2;
    Node       *node;
    Plan       *plan;
    
    jstate = pgsp_init_jumble_state();

    node = stringToNode(PLAN_TREE_STRING_1);

    if (!node)
    {
        return false;
    }

    plan = (Plan *) node;

    pgsp_jumble_plan_tree(jstate, plan);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0)); 
    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();  
    plan->righttree = (Plan *) copyObject(plan);
    pgsp_jumble_plan_tree(jstate, plan);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));
    appendStringInfo(buf, "Test: Right tree - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId: " UINT64_FORMAT ")\n", planId1);      
    pfree(jstate->jumble);
    pfree(jstate);
    return (planId1 != planId2);
}


static bool test_nodes(StringInfo buf)
{
    JumbleState *jstate;
    uint64 planId1, planId2;
    Node       *node;
    Plan       *plan;
    ModifyTable *mt;
    jstate = pgsp_init_jumble_state();

    node = stringToNode(PLAN_TREE_STRING_1);

    if (!node)
    {
        return false;
    }

    plan = (Plan *) node;

    pgsp_jumble_plan_tree(jstate, plan);
    planId1 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0)); 
    pfree(jstate->jumble);
    pfree(jstate);
    jstate = pgsp_init_jumble_state();  

    mt = makeNode(ModifyTable);
    mt->operation = CMD_INSERT;
    mt->nominalRelation = 1;
    plan->lefttree = (Plan *) mt;
    pgsp_jumble_plan_tree(jstate, plan);
    planId2 = DatumGetUInt64(hash_any_extended(jstate->jumble, jstate->jumble_len, 0));
    appendStringInfo(buf, "Test: nodes added - ");
    if (planId1 != planId2)
        appendStringInfo(buf, "PASS\n");
    else
        appendStringInfo(buf, "FAIL (planId: " UINT64_FORMAT ")\n", planId1);      
    pfree(jstate->jumble);
    pfree(jstate);

    return (planId1 != planId2);
}
