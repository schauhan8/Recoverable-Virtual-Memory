#include "gt_transaction.h"

/*Todo:
Fix const issue:
seg_found_name

*/
//Add insert into transaction head here.
void gt_transaction_insert(struct gt_transaction *trans_insertable)
{
  if(global_transaction_head == NULL)
    global_transaction_head = trans_insertable;
  else
  {
    trans_insertable->trans_next = global_transaction_head;
    global_transaction_head = trans_insertable;
  }
}

//Returns the transaction from the global transaction directory
struct gt_transaction* gt_transaction_lookup(trans_t tid)
{
  struct gt_transaction *temp;
  temp = global_transaction_head;
  if(global_transaction_head == NULL)
     return NULL;
  else
  {
     while(temp != NULL)
     {
       if(temp->trans_id == tid)
         return temp;
       temp = temp->trans_next;
     }
  }
  return NULL;
}

struct gt_segment * gt_transaction_searchDIR(rvm_t rvm, void *segbase)
{
  struct gt_directory *dir_temp_lookup = gt_directory_rvm(rvm);
  struct gt_segment *seg_temp_lookup = dir_temp_lookup->dir_segment_head;
  struct gt_segment *seg_found_segment;



  int flag = 0;
  if(seg_temp_lookup == NULL)
    perror("no available segment");

  while(seg_temp_lookup != NULL)
  {
    if(segbase == seg_temp_lookup->seg_base)
    {
    //  printf("Voila!!!\n");
      seg_found_segment = seg_temp_lookup;
      flag = 1;
    }
    seg_temp_lookup = seg_temp_lookup->seg_next;
  }

  if(flag = 0)
    perror("segment not found");

/*
  if(DEBUG)
  if (flag==1)
  {
    printf("HI!! In search dir now. the troublesome function. value of %s\n", seg_found_segment->seg_name);
  }
*/

  if(seg_found_segment==NULL)
    gt_exit("segment not found  ");

//  printf("HI!! In search dir now. the troublesome function. value of %s\n", seg_found_segment->seg_name);

  return seg_found_segment;
}

void gt_transaction_delete_backups(struct gt_transaction * trans_temp, void *segbase)
{
  if(trans_temp==NULL)
    return;

  if(trans_temp->trans_uncommitted_head==NULL)
    return;

  struct uncommitted * uncommitted_cur = trans_temp->trans_uncommitted_head;
  struct uncommitted * uncommitted_prev = trans_temp->trans_uncommitted_head;

  if(uncommitted_cur->uncommitted_actual==segbase)
    trans_temp->trans_uncommitted_head=NULL;

  while(uncommitted_cur)
  {
   uncommitted_prev = uncommitted_cur;
   uncommitted_cur=uncommitted_cur->next; 

   if(uncommitted_cur)
   {  
     if(uncommitted_cur->uncommitted_actual==segbase)
     {
       uncommitted_prev->next = uncommitted_cur->next;
       return;
     } 
   }
  }
}

void gt_transaction_deleteTransaction(trans_t temp_id)
{
    if(global_transaction_head->trans_id == temp_id)
    {
      global_transaction_head = NULL;
      return;
    }
      
    struct gt_transaction * trans_cur = global_transaction_head;
    struct gt_transaction * trans_prev = global_transaction_head;

    while(trans_cur)
    {
      trans_prev = trans_cur;
      trans_cur=trans_cur->trans_next; 

       if(trans_cur)
       {  
         if(trans_cur->trans_id==temp_id)
         {
           trans_prev->trans_next = trans_cur->trans_next;
           return;
         } 
       }
    }
}