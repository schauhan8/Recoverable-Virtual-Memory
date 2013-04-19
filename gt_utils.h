#ifndef GT_UTILS_H
#define GT_UTILS_H

#include "gt_lrvm.h"
#include "gt_directory.h"
#include "gt_transaction.h"
#include "gt_utils.h"
#include "gt_segment.h"
#include "rvm.h"
#include "general_includes.h"
#include <fcntl.h>
#include <sys/mman.h>

//Generic Helper functions
void gt_exit(char *);
//void gt_chdir(char *);
int gt_fileLookup(char *);
void gt_touchFile(char *);
int gt_generalFileCount(char *file_name);
int gt_fileSize(char *seg_name, char *dir_name);

//Functions to modify Log File entries
void gt_fileDeleteLog(char *seg_name);
void gt_fileUpdateLog(int offset, int size, char *seg_name, char *data);
void gt_fileLookupLog(char *fname, char *seg_name,char *dir);

//Functions to modify Segment File entries
int gt_fileUpdateSeg(char *seg_name, char *data, int offset, int size);
int gt_fileAppendSeg(char *seg_name, int new_len);
int gt_fileWriteSeg(char *seg_name, int seg_size);
#endif
