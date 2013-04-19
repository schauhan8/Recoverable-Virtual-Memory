#include "gt_lrvm.h"

rvm_t global_dir_id;
trans_t global_trans_id;

struct gt_directory *global_directory_head;
struct gt_transaction *global_transaction_head;

/*Todos:
Mak Todos:
1. Replace fprintf with fwrite for data segment
2. Replace fgets with fread for data segment
3. Remove the transaction from the list of transactions

Major todos:
1. Test unmap, destroy and truncate

Check in begin transaction/about to modify that the uncommitted_actual is allotted properly.

Urgent:
  Ask about this.
  Fix write_to_seg: replace "" with NULLs

Super urgent:
  Return values of map!!
*/

rvm_t rvm_init(const char *cdirectory)
{
  char * directory = strdup(cdirectory);//(char *) malloc(strlen(cdirectory)+1);
  //strcpy(directory, cdirectory);
  if(directory == NULL)
    gt_exit("empty directory name");

  rvm_t ret_val;

  if(gt_directory_check_fs(directory))
  {
    if(gt_directory_lookup(directory))
      gt_exit("directory already exists");
    else
      ret_val = gt_directory_insert(directory);
  }
  else
  {
    gt_directory_mkdir(directory);
    ret_val = gt_directory_insert(directory);
    if(chdir(directory)==-1) exit(0);
    gt_touchFile("LogFile");
    chdir("..");
  }
  return ret_val;
}


//rvm_map
void * rvm_map(rvm_t rvm, const char *csegname, int size_to_create)
{
  //Points

  struct gt_directory *temp;
  char *curr_dir;

  void *ret_pointer;
  ret_pointer = NULL;

  char * segname = strdup(csegname);//(char *) malloc(sizeof(csegname));
  //strcpy(segname, csegname);
 
  //Instantiate segment structures
  struct gt_segment *insertable_seg = (struct gt_segment *) malloc(sizeof(struct gt_segment));

  //Check 1 : Resolved
  insertable_seg->seg_name = segname;
  insertable_seg->seg_size = size_to_create;
  insertable_seg->seg_base =  malloc(size_to_create);
  /*
    for (i = 0; i < size_to_create; i++)
    { 
      memcpy(&insertable_seg->seg_base[i], "", 1);
    }
  */
  insertable_seg->seg_next = NULL;
  insertable_seg->seg_backup = NULL;
  insertable_seg->seg_is_trans = 0;
  insertable_seg->seg_trans_id = 0;

  //Not in disk
  //Case III
  //The segment is mapped but is not in the disk

  if(gt_segment_is_mapped(segname, rvm))
  {
    gt_exit("segment mapped already");
  }

  if(gt_segment_is_disk(segname, rvm)==1)
  {

    //CURR_DIR IS IMPORTANT
    temp = gt_directory_rvm(rvm);
    
    if (temp == NULL)
      gt_exit("directory not found");
    else 
    {
      
      curr_dir = temp->dir_name;
      if(curr_dir==NULL)
        gt_exit("NULL DIRECTORY");
      else
        printf("%s is valid\n", curr_dir);
    }

    if(chdir(curr_dir)==-1) exit(0);

    int file_size = gt_fileSize(segname, curr_dir);

    if(file_size < size_to_create)
      if(gt_fileAppendSeg(segname, size_to_create) == 0)
        gt_exit("segment append failed");
		
    //This function updates the segment file and the log file both
    gt_fileLookupLog("LogFile", segname,curr_dir);

    struct gt_directory *temp = gt_directory_rvm(rvm);
    //Check if rvm directory exists

    void *myBuf = (void *) malloc((size_t)file_size);
    chdir(curr_dir);
    FILE *f = fopen( segname, "r" );
    fread(myBuf, 1 ,(size_t) file_size, f);
    fclose(f);

    //Does this even work?
    insertable_seg->seg_base = myBuf;

    if(temp->dir_segment_head == NULL)
      temp->dir_segment_head = insertable_seg;
    else
    {
      insertable_seg->seg_next = temp->dir_segment_head;
      temp->dir_segment_head = insertable_seg;
    }

    chdir("..");
    ret_pointer = insertable_seg->seg_base;
    return ret_pointer;
  }
  else
  {
    int temp_ret;


    struct gt_directory *temp = gt_directory_rvm(rvm);
    if(temp==NULL)
      gt_exit("Not initialized\n");

    //Insert this segment into the directory's segment list here
    if(temp->dir_segment_head == NULL)
      temp->dir_segment_head = insertable_seg;
    else
    {
      insertable_seg->seg_next = temp->dir_segment_head;
      temp->dir_segment_head = insertable_seg;
    }

    //Go into this directory now.
    if(chdir(temp->dir_name)==-1) printf("ERROR!");

    temp_ret = gt_fileWriteSeg(segname, size_to_create);
    if(temp_ret == 0)
      gt_exit("segment file could not be updated");

    chdir("..");
    ret_pointer = insertable_seg->seg_base;
    return insertable_seg->seg_base;
  }
}

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases)
{

  global_trans_id++;

  struct gt_transaction *trans_temp;
  trans_temp = global_transaction_head;

  int i = 0;
  int j = 0;

  //First check if any of the segbases match
  if(trans_temp != NULL)
    for(i = 0; i < numsegs; i++)
    {
      while(trans_temp != NULL)
      {
        for(j = 0; j < trans_temp->trans_numsegs; j++)
          if(segbases[i] == trans_temp->trans_segbases[j])
          {
            printf("segments modified by other transaction\n");
            return (trans_t)-1;
          }
        trans_temp = trans_temp->trans_next;
      }
      trans_temp = global_transaction_head;
    }

  struct gt_transaction *trans_insertable;
  trans_insertable = (struct gt_transaction*) malloc(sizeof(struct gt_transaction));
  trans_insertable->trans_rvm = rvm;
  trans_insertable->trans_id = global_trans_id;

  trans_insertable->trans_segbases = (void **) malloc(sizeof(void *) * numsegs);
  trans_insertable->trans_numsegs = numsegs;
  for(i = 0; i < numsegs; i++)
    trans_insertable->trans_segbases[i] = segbases[i];

  trans_insertable->trans_next = NULL;
  trans_insertable->trans_uncommitted_head = NULL;
  gt_transaction_insert(trans_insertable);

  return (trans_t)global_trans_id;
}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size)
{
  int i = 0; 
  int flag = 0;

  struct gt_transaction *trans_temp;
  trans_temp = gt_transaction_lookup(tid);

  if(trans_temp == NULL)
    gt_exit("transaction not found");


  for(i = 0; i < trans_temp->trans_numsegs; i++)
  {
    if(trans_temp->trans_segbases[i] == segbase)
    {
      flag = 1;
      break;
    }
  }

  if(flag == 0)
    gt_exit("segment not belonging to this transaction");

  struct gt_segment * temp_seg_uniq = gt_transaction_searchDIR(trans_temp->trans_rvm, segbase);
  char *seg_name = temp_seg_uniq->seg_name;


  struct uncommitted *uncommitted_insertable = (struct uncommitted *) malloc(sizeof(struct uncommitted));
  uncommitted_insertable->uncommitted_offset = offset;
  uncommitted_insertable->uncommitted_size = size;
  uncommitted_insertable->next = NULL;
  uncommitted_insertable->prev = NULL;
  uncommitted_insertable->uncommitted_seg_backup = malloc(size);
  uncommitted_insertable->uncommitted_seg_name = strdup(seg_name);
  uncommitted_insertable->uncommitted_actual = segbase;
  memcpy(uncommitted_insertable->uncommitted_seg_backup, segbase+offset, size);

  if(trans_temp->trans_uncommitted_head == NULL)
    trans_temp->trans_uncommitted_head = uncommitted_insertable;
  else
  {
    struct uncommitted *uncommitted_temp = trans_temp->trans_uncommitted_head;

    while(uncommitted_temp->next != NULL)
      uncommitted_temp = uncommitted_temp->next;

   /* uncommitted_temp->prev = uncommitted_insertable;
    uncommitted_insertable->next = uncommitted_temp;*/
    uncommitted_temp->next = uncommitted_insertable;
    uncommitted_insertable->prev = uncommitted_temp;
  }
}

void rvm_commit_trans(trans_t tid)
{
  struct gt_transaction *trans_temp;
  trans_temp = gt_transaction_lookup(tid);

  if(trans_temp == NULL)
    gt_exit("transaction not found");

  //Code changes into directory to write log file into. For this I also need directory name
  struct gt_directory *dir_temp;
  dir_temp = gt_directory_rvm(trans_temp->trans_rvm);
  char *cd = dir_temp->dir_name;

  chdir(cd);

  struct uncommitted *uncommitted_temp = trans_temp->trans_uncommitted_head;

  while(uncommitted_temp != NULL)
  {
    int offset = uncommitted_temp->uncommitted_offset;
    int size = uncommitted_temp->uncommitted_size;
    
    char *seg_name = uncommitted_temp->uncommitted_seg_name;
    char *data = malloc(size);

    memcpy(data, uncommitted_temp->uncommitted_actual + offset, size);
    gt_fileUpdateLog(offset, size, seg_name, data);
    uncommitted_temp->uncommitted_seg_backup=NULL;
    uncommitted_temp=uncommitted_temp->next;
  }

  //Free transaction space
  gt_transaction_deleteTransaction(tid);
  chdir("..");
}

void rvm_abort_trans(trans_t tid)
{
  struct gt_transaction *trans_temp;
  int i = 0;

  trans_temp = gt_transaction_lookup(tid);

  if(trans_temp == NULL)
    gt_exit("transaction not found");
  
  struct uncommitted* currentModRecord = trans_temp->trans_uncommitted_head;
  while(currentModRecord){
      void* currSegBase = currentModRecord->uncommitted_actual;
      void* backup = currentModRecord->uncommitted_seg_backup;
      memcpy(currSegBase+currentModRecord->uncommitted_offset,backup,currentModRecord->uncommitted_size);
      currentModRecord = currentModRecord->next;
  }
  
  trans_temp->trans_uncommitted_head = NULL;
  gt_transaction_deleteTransaction(tid);
}

void rvm_unmap(rvm_t rvm, void *segbase){

  if(segbase==NULL)
    gt_exit("Not found");

  //Search for it in all transactions.
  //Remove the corresponding uncommitted records
  struct gt_transaction * trans_temp;
  trans_temp  = global_transaction_head;

  if(trans_temp)
  { 
    while(trans_temp!=NULL)
    {
      gt_transaction_delete_backups( trans_temp, segbase);
      trans_temp = trans_temp->trans_next;
    }
  }

  //Delete it from memory that is from the segment list inside the directory node.
  gt_segment_delete(segbase, rvm);
}

void rvm_destroy(rvm_t rvm, const char *csegname)
{

  char * segname = strdup(csegname);
  if(gt_segment_is_mapped(segname, rvm))
  {
    printf("\nSegment already mapped. Cannot be destroyed");
    //Check 6: Maybe should exit
    return;  
  }

  struct gt_directory *dir_temp;
  dir_temp = gt_directory_rvm(rvm);

  if(dir_temp)
  {

    chdir(dir_temp->dir_name);    
    
    //Change log
    gt_fileLookup(segname);

    chdir("..");

    chdir(dir_temp->dir_name);    
    if(gt_fileLookup(segname))
      if(remove(segname)!=0)
      {
        printf("\nError deleting segment file\n");
      }
    chdir("..");
  }
}

void rvm_truncate_log(rvm_t rvm){
  //Check if the directory is present or not.
  //If not return error
  struct gt_directory *dir_temp;
  dir_temp = gt_directory_rvm(rvm);
  if(dir_temp==NULL)
    gt_exit("Directory does not exist. Truncate not possible\n");

  chdir(dir_temp->dir_name);    

  struct  gt_segment * seg_temp;
  seg_temp = dir_temp->dir_segment_head;
  while(seg_temp!=NULL)
  {
    gt_fileLookupLog("LogFile", seg_temp->seg_name,dir_temp);
    seg_temp = seg_temp->seg_next;
  }
  chdir("..");
}
