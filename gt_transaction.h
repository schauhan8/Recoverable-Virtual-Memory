#ifndef GT_TRANSACTION_H
#define GT_TRANSACTION_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "general_includes.h"
#include "rvm.h"

struct uncommitted {
  void *uncommitted_seg_backup;
  void *uncommitted_actual;
  int uncommitted_offset;
  int uncommitted_size;
  char *uncommitted_seg_name;
  struct uncommitted *next;
  struct uncommitted *prev;
};

struct gt_transaction {
  trans_t trans_id;
  struct uncommitted *trans_uncommitted_head;
  void **trans_segbases;
  rvm_t trans_rvm;
  int trans_numsegs;
  struct gt_transaction *trans_next;
};

void gt_transaction_insert(struct gt_transaction *);
struct gt_transaction* gt_transaction_lookup(trans_t tid);
struct gt_segment * gt_transaction_searchDIR(rvm_t, void *segbase);
void gt_transaction_delete_backups(struct gt_transaction * trans_temp, void *segbase);
void gt_transaction_deleteTransaction(trans_t trans_temp);

#endif
