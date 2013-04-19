#ifndef GT_DIRECTORY_H
#define GT_DIRECTORY_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "general_includes.h"
#include "rvm.h"

struct gt_directory {
  rvm_t dir_id;
  char *dir_name;
  struct gt_segment *dir_segment_head;
  struct gt_directory *dir_next;
  char *dir_path;
};

//Functions for directory
rvm_t gt_directory_insert(char *dir_name);
int gt_directory_lookup(char *dir_name);
int gt_directory_mkdir(char *dir_name);
int gt_directory_check_fs(char *dir_name);
struct gt_directory * gt_directory_rvm(rvm_t);

#endif
