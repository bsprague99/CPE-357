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

#define PATH_LEN 256
#define HEADER_SIZE 512
#define PREFIX_SIZE 155
#define NAME_SIZE 100
#define VTIME_SIZE 18
#define MAGIC_SIZE 6
#define LINK_SIZE 100
#define MODE_SIZE 8
#define MTIME_SIZE 12
#define SIZE_SIZE 12
#define SUM_SIZE 8
#define OCTAL 8
#define ID_SIZE 8
#define UG_SIZE 32
#define DEV_SIZE 8

int STRICT_FLAG = 0;
int VERBOSE_FLAG = 0;

typedef struct tar_header {
   char name[NAME_SIZE];
   char mode[MODE_SIZE];
   char uid[ID_SIZE];
   char gid[ID_SIZE];
   char size[SIZE_SIZE];
   char mtime[MTIME_SIZE];
   char chksum[SUM_SIZE];
   char typeflag;
   char linkname[LINK_SIZE];
   char magic[MAGIC_SIZE];
   char version[2];
   char uname[UG_SIZE];
   char gname[UG_SIZE];
   char devmajor[DEV_SIZE];
   char devminor[DEV_SIZE];
   char prefix[PREFIX_SIZE];
}tar_header;

void tar_traverse(char *file, int fd);
int tar_checksum(tar_header *header);
int tar_strict(tar_header header);
void tar_prefix(char *file, tar_header *header);
void tar_verbose(char *file, char mode, tar_header *header);
void tar_list(char *file, int num_args, char **args_list);
void tar_create(char *file, int fd);
void tar_extract(char *file, int num_args, char **args_list);
