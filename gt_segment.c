#include "gt_segment.h"

/*
Segment helper functions
*/

//seg is mapped i.e exists in memory?
int gt_segment_is_mapped(char *segname, rvm_t rvm)
{
  struct gt_directory *temp_dir = gt_directory_rvm(rvm);	
  if(temp_dir != NULL)	
  {
    struct gt_segment *temp_seg = temp_dir->dir_segment_head;
    while(temp_seg != NULL) 
    {
      if(strcmp(temp_seg->seg_name, segname) == 0)
	    { 
        return 1;
      }
      else 
      	temp_seg = temp_seg->seg_next;
     }

     return 0;
   }
   else
     gt_exit("directory not found\n");
return 0;
}


int gt_segment_is_disk(char *segname, rvm_t rvm)
{
  struct gt_directory *temp;	
  char *curr_dir;

  temp = gt_directory_rvm(rvm);

  if(temp == NULL)
  {
    gt_exit("directory not found\n");
  }
  else
  {
    curr_dir = temp->dir_name;
    chdir(curr_dir);

    if(gt_fileLookup(segname) == 1)
    {	
      //system("cd ..");
      chdir("..");
      return 1;
    }
    else
    {
      //system("cd ..");
      chdir("..");
      return 0;
    }
   }		
return 0;
}

void gt_segment_delete(void * seg_base, rvm_t rvm)
{
   //Find the rvm id in the directory
  struct gt_directory *dir_temp;
  dir_temp = gt_directory_rvm(rvm);

  struct gt_segment * seg_head = dir_temp->dir_segment_head;
  struct gt_segment * temp = seg_head;
  struct gt_segment * prev = seg_head;

  if(temp==NULL)
    return;

  if(temp->seg_next==NULL)
  {
    if(temp->seg_base==seg_base)
      dir_temp->dir_segment_head=NULL;
    return;
  }

  while(temp!=NULL)
  { 
    prev = temp;
    temp=temp->seg_next;
    if(temp!=NULL)
    {
      if(temp->seg_base==seg_base)
      {  
        prev->seg_next=temp->seg_next;
        return;
      }
    }
  } 
}