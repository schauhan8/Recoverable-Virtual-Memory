#include "rvm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_STRING1 "hello, world"
#define TEST_STRING2 "bleg!"
#define OFFSET2 1000

int main(int argc, char **argv)
{
     rvm_t rvm;
     char *seg;
     char *segs[1];
     trans_t trans;
     
     rvm = rvm_init("rvm_segments");
     printf("Init\n");
     
     rvm_destroy(rvm, "testseg");
     printf("Destroy passed\n");

     segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
     seg = segs[0];

     printf("Map passed. seg[0] contains something\n");

     /* write some data and commit it */
     trans = rvm_begin_trans(rvm, 1, (void**) segs);
     printf("Begin transaction\n");

     rvm_about_to_modify(trans, seg, 0, 100);
     printf("About to modify offset 0\n");
     sprintf(seg, TEST_STRING1);
     
     rvm_about_to_modify(trans, seg, OFFSET2, 100);
     sprintf(seg+OFFSET2, TEST_STRING1);
     printf("About to modify offset 2\n");
     
     printf("Before commit\n");     
     rvm_commit_trans(trans);
     printf("After commit\n");

     /* start writing some different data, but abort */
     trans = rvm_begin_trans(rvm, 1, (void**) segs);
     rvm_about_to_modify(trans, seg, 0, 100);
     sprintf(seg, TEST_STRING2);
     printf("After ABM1t\n");

     
     rvm_about_to_modify(trans, seg, OFFSET2, 100);
     sprintf(seg+OFFSET2, TEST_STRING2);
     printf("After ABM2t\n");

     rvm_abort_trans(trans);
     printf("After abort\n");
     /* test that the data was restored */
     if(strcmp(seg+OFFSET2, TEST_STRING1)) {
	  printf("ERROR: second hello is incorrect (%s)\n",
		 seg+OFFSET2);
	  exit(2);
     }

     if(strcmp(seg, TEST_STRING1)) {
	  printf("ERROR: first hello is incorrect (%s)\n",
		 seg);
	  exit(2);
     }

     rvm_unmap(rvm, seg);

     printf("OK\n");
     exit(0);
}

