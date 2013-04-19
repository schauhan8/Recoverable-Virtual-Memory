#include "gt_utils.h"

//HELPERS

/*void gt_chdir(char *directory)
{
  char buf[1000];
  char *chdir = "cd";
  snprintf(buf, sizeof(buf), "%s %s", chdir, directory);
  system(buf);
}*/

void gt_touchFile(char *fileName) {
    char buf1[1000];
    char *touchcmd = "touch";
    snprintf(buf1, sizeof (buf1), "%s %s", touchcmd, fileName);
    system(buf1);
}

int gt_fileLookup(char *seg_name) {
    struct stat buffer;
    if (stat(seg_name, &buffer) == 0) {
        printf("File size: %d\n", (int) buffer.st_size);
        return 1;
    } else
        return 0;
}

int gt_fileSize(char *seg_name, char *dir_name) {
    int size = 0;
    chdir(dir_name);

    FILE *fptr;
    if ((fptr = fopen(seg_name, "r")) != NULL) {
        fseek(fptr, 0, SEEK_END);
        size = ftell(fptr);
        fclose(fptr);
    }
    chdir("..");
    return size;
}

int gt_generalFileCount(char *file_name) {
    FILE *fptr;
    fptr = fopen(file_name, "r");

    int temp;
    int count = 0;

    if (fptr == NULL)
        gt_exit("file not found");
    else {
        while ((temp = fgetc(fptr)) != EOF)
            count++;
    }
    fclose(fptr);
    return count;
}

void gt_exit(char *print_error) {
    printf("Errorz: %s\n", print_error);
    exit(0);
}

//Log File Operations
    
void gt_fileUpdateLog(int offset, int size, char *seg_name, char *data) {
    FILE *fptr;
    //chdir(dir);
    fptr = fopen("LogFile", "a+");

    if (fptr == NULL)
        gt_exit("file can't open");

    if (DEBUG)
        printf("I get here in update\n");

    fprintf(fptr, "%s\n", seg_name);
    fprintf(fptr, "%d\n", offset);
    fprintf(fptr, "%d\n", size);
    fwrite(data, size, 1, fptr);

    if (DEBUG)
        printf("I get here after update\n");

    fclose(fptr);
    //chdir("..");
}

void gt_fileLookupLog(char *fname, char *seg_name,char *dir) {
    FILE *fptr;
    chdir(dir);
    fptr = fopen(fname, "r");
    char string1[MAX];
    /*
    int temp_offset = 0;
    int temp_length = 0;
    char *temp_data;    
    char *temp;
    */
    FILE * backptr = fopen("BackupLog", "w+");

    while (!feof(fptr)) {
        if (fgets(string1, MAX, fptr)) {
            if (strncmp(string1, seg_name, strlen(string1) - 1) == 0) {
                fgets(string1,MAX,fptr);
                string1[strlen(string1)-1] = NULL;
                int offset = atoi(string1);
                fgets(string1,MAX,fptr);
                string1[strlen(string1)-1] = NULL;
                int size = atoi(string1);
                char* tempBuf = malloc(size);
                fread(tempBuf,size,1,fptr);
                
                gt_fileUpdateSeg(seg_name, tempBuf, offset, size);
                /*int file_read_counter = 0;
                while (file_read_counter != 3) {
                    file_read_counter++;
                    if (file_read_counter == 3) {
                        fread(temp_data, temp_length, 1, fptr);
                        temp_data = temp;
                    }
                    else
                        if (fgets(string1, MAX, fptr)) {
                        temp = malloc(sizeof (char) * (strlen(string1) - 1));
                        strncpy(temp, string1, strlen(string1) - 1);
                        if (file_read_counter == 1) {
                            temp_offset = atoi(temp);
                            free(temp);
                            //printf("%d\n", temp_offset);
                        }
                        if (file_read_counter == 2) {
                            temp_length = atoi(temp);
                            free(temp);
                            //printf("%d\n", temp_length);
                        }
                    }
                }*/
                
            } else {
                fwrite(string1, strlen(string1), 1, backptr);
                fgets(string1,MAX,fptr);
                fwrite(string1, strlen(string1), 1, backptr);
                fgets(string1,MAX,fptr);
                fwrite(string1, strlen(string1), 1, backptr);
                string1[strlen(string1)-1] = NULL;
                int size = atoi(string1);
                char* tempBuf = malloc(size);
                fread(tempBuf,size,1,fptr);
                fwrite(tempBuf,size,1,backptr);
            }
        }
    }
    fclose(fptr);
    fclose(backptr);
    system("rm LogFile");
    system("mv BackupLog LogFile");
    chdir("..");
    //  gt_fileDeleteLog(seg_name);
}

void gt_fileDeleteLog(char * seg_name) {
    FILE * fptr;
    fptr = fopen("LogFile", "r");
    char str[MAX];

    FILE * fptr_backup;
    fptr_backup = fopen("BackupLog", "w+");

    while (!feof(fptr)) {
        if (fgets(str, MAX, fptr)) {
            if (strncmp(str, seg_name, strlen(str) - 1) == 0) {
                int file_read_counter = 0;
                while (file_read_counter != 3) {
                    file_read_counter++;
                    fgets(str, MAX, fptr);
                }
            } else {
                fprintf(fptr_backup, "%s", str);
            }
        }
    }

    fclose(fptr);
    fclose(fptr_backup);

    system("rm LogFile");
    system("mv BackupLog LogFile");
}

//Segment Operations

int gt_fileAppendSeg(char *seg_name, int new_len) {
    int length = gt_generalFileCount(seg_name);
    int i = 0;

    FILE *fptr = fopen(seg_name, "a");

    if (fptr == NULL)
        return 0;
    char nullStr[1];
    nullStr[0] = NULL;
    for (i = 0; i < (new_len - length); i++)
        fwrite(nullStr, sizeof (char), 1, fptr);
    fclose(fptr);

    return 1;
}

int gt_fileWriteSeg(char *seg_name, int seg_size) {

    //Check 3: Write NULLS to file
    FILE *fptr;
    int i = 0;
    fptr = fopen(seg_name, "w+");

    if (DEBUG)
        printf("Just wrote %s\n", seg_name);

    if (fptr == NULL)
        return 0;
    else {
        char nullStr[1];
        nullStr[0] = NULL;
        for (i = 0; i < seg_size; i++)
            fwrite(nullStr, sizeof (char), 1, fptr);
        fclose(fptr);
        return 1;
    }
}

int gt_fileUpdateSeg(char *seg_name, char *data, int offset, int size) {
    int fdesc;
    fdesc = open(seg_name, O_RDWR);
    if (fdesc < 0)
        gt_exit("file open error");

    void *file_copy;
    struct stat statbuf;

    /* find size of input file */
    if (fstat(fdesc, &statbuf) < 0)
        gt_exit("fstat error");

    file_copy = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdesc, 0);
    memcpy(file_copy + offset, data, size * sizeof (char));

    munmap(file_copy, statbuf.st_size);
    return 1;
}