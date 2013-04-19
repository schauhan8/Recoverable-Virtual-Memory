#ifndef RVM_H
#define RVM_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "general_includes.h"

extern rvm_t global_dir_id;
extern trans_t global_trans_id;
extern struct gt_directory * global_directory_head;
extern struct gt_transaction * global_transaction_head;

#endif
