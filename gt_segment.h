#ifndef GT_SEGMENT_H
#define GT_SEGMENT_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "general_includes.h"
#include "rvm.h"

struct gt_segment {
  char *seg_name;
  int seg_size;
  int seg_is_trans;
  void *seg_base; 
  void *seg_log;
  struct gt_segment *seg_next;
  void *seg_backup;
  trans_t seg_trans_id;
};

//Functions for segment
int gt_segment_is_mapped(char *seg_name, rvm_t rvm);
int gt_segment_is_disk(char *seg_name, rvm_t rvm);
void gt_segment_delete(void * seg_base, rvm_t rvm);

#endif
