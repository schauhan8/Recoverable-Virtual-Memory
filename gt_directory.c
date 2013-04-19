#include "gt_directory.h"

/*
DIRECTORY HELPER FUNCTIONS
*/

rvm_t gt_directory_insert(char *directory_name_passed)
{

  //Initialize all elements
  global_dir_id++;
  char *temp_char = malloc(strlen(directory_name_passed));
  temp_char = directory_name_passed;

  //Initialize the directory structure
  struct gt_directory *dir_node;
  dir_node = malloc(sizeof(struct gt_directory));
  dir_node->dir_name = temp_char;
  dir_node->dir_next = NULL;
  dir_node->dir_id = global_dir_id;
  dir_node->dir_segment_head = NULL;

  if(global_directory_head == NULL)
    global_directory_head = dir_node;
  else
  {	
    dir_node->dir_next = global_directory_head;
    global_directory_head = dir_node;
  }

  return global_dir_id;	
}

int gt_directory_lookup(char *dir_name)
{
  struct gt_directory *temp;
  temp = global_directory_head;
	
  //If the global directory list is empty return 0
  if(temp == NULL)
    return 0;

  //You have to make sure that global_directory points to the head of the list
  while(temp != NULL)
  {
    if(strcmp(dir_name, temp->dir_name) == 0)
    {
      if(DEBUG)
      	printf("%s is found\n", dir_name);
      return 1;						
    }	
    temp = temp->dir_next;
  }
  return 0;
}

int gt_directory_mkdir(char * dir_name)
{
  /* 
  Returns 1 if successful
  Returns 0 if not successful
  */

  char buf[1000];
  char *mkcmd = "mkdir -p";
  char *dname = dir_name;

  snprintf(buf, sizeof(buf), "%s %s", mkcmd, dname);
  int x = system(buf);

  if(x == 256)
    return 0;
  else
    return 1;
}

int gt_directory_check_fs(char *dir_name)
{
  DIR *temp_dir = opendir(dir_name);
  if(temp_dir != NULL)
  {
    closedir(temp_dir);
    return 1;
  }
  else return 0;	
}


struct gt_directory* gt_directory_rvm(rvm_t id)
{
  struct gt_directory *temp;
  temp = global_directory_head;
	
  //If the global directory list is empty return 0
  if(temp == NULL)
    return NULL;

  //You have to make sure that global_directory points to the head of the list

  while(temp != NULL)
  {
    if(id == temp->dir_id)
    {
      if(DEBUG)
        printf("\n %d directory lookup is %s\n", id, temp->dir_name);
      return temp;						
    }	
    temp = temp->dir_next;
   }
   return NULL;
}
