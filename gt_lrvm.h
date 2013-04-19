#ifndef GT_LRVM_H
#define GT_LRVM_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "general_includes.h"
#include "rvm.h"

#define MAX 100000
#define DEBUG 0

//API Calls
rvm_t rvm_init(const char *directory);

void * rvm_map(rvm_t rvm, const char *segname, int size_to_create);
void rvm_unmap(rvm_t rvm, void *segbase);
void rvm_destroy(rvm_t rvm, const char *segname);

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);

void rvm_commit_trans(trans_t tid);
void rvm_abort_trans(trans_t tid);
void rvm_truncate_log(rvm_t rvm);

#endif
