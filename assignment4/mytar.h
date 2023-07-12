#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <math.h>

#define PATH_MAX 256
#define HEADER 512
#define PREFIX 155
#define NAME 100
#define VTIME 18
#define MAGIC 6
#define LINK 100
#define MODE 8
#define MTIME 12
#define SIZE 12
#define SUM 8
#define OCT 8
#define ID_NUM 8
#define UG_PARAM 32
#define DEV_VAL 8

typedef struct ustar {
   char name[NAME];
   char mode[MODE];
   char uid[ID_NUM];
   char gid[ID_NUM];
   char size[SIZE];
   char mtime[MTIME];
   char chksum[SUM];
   char typeflag;
   char linkname[LINK];
   char magic[MAGIC];
   char version[2];
   char uname[UG_PARAM];
   char gname[UG_PARAM];
   char devmajor[DEV_VAL];
   char devminor[DEV_VAL];
   char prefix[PREFIX];
}ustar;

void helper_archive(char *items[], int num_items);
void v_archive(char *file, char mode, ustar *header);
void prefix_check(char *file, ustar *header);
int chksum(ustar *header);
int s_archive(ustar header);
void c_archive(char *file, int fd);
void x_archive(char *tarfile, char **args_list, int num_args);
void t_archive(char *file, char **args_list, int num_args);
void traverse(char *file, int fd);
