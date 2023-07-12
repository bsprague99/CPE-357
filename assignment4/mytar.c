#include "mytar.h"

int s_flag = 0;
int v_flag = 0;

int main(int argc, char *argv[]) {
   int i;
   int names = 1 * argc - 3;
   
   if (argc < 2) {
    fprintf(stderr, "%s", "Usage: mytar [ctxvS]f tarfile [ path [...]]\n");
      exit(1);
   }
   for (i = strlen(argv[1]) - 1; i >= 0; i--) {
      //mode selection through iterating through individal args
      if (argv[1][i] != 'f') {
         fprintf(stderr, "%s: f-flag must be present! '%c'.\n",
         argv[0], argv[1][i]);
      }
      if (argv[1][i] == 'S')
         s_flag = 1;
      else if (argv[1][i] == 'v')
         v_flag = 1;
      else if (argv[1][i] == 'x')
        x_archive(argv[2], argv + 3, names);
      else if (argv[1][i] == 't')
         t_archive(argv[2], argv + 3, names);
      else if (argv[1][i] == 'c') {
         helper_archive(argv, argc);
      }
      
   }
   return 0;
}
void helper_archive(char *items[], int num_items) {
   //set up the file for create to take care of and modify
   int x = 3;
   struct stat permissions_info;
   int file_d = 0;
   char eoa[HEADER];
   memset(eoa, '\0', HEADER);
   file_d = open(items[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXG 
      | S_IRWXU | S_IRWXO);

   while( x < num_items) {
      //check permissions AND the file type!
        lstat(items[x], &permissions_info);
   if (S_ISLNK(permissions_info.st_mode) || S_ISREG(permissions_info.st_mode))
         c_archive(items[x], file_d);
      if (S_ISDIR(permissions_info.st_mode)) 
         traverse(items[x], file_d); 
      x++;
   }
   /* double write will move the marker twice as well! */
   write(file_d, eoa, HEADER);
   write(file_d, eoa, HEADER);

   close(file_d);
}

void v_archive(char *file, char mode, ustar *header) {
   mode_t bit_mask;

   if (v_flag == 1) {
      if (mode == 'c') {
         printf("%s\n", file);
      }
   }
   if (v_flag == 0) {
      if (mode == 'l') {
         printf("%s\n", file);
      }
   }
   if (mode == 'l') {
      if (v_flag == 1) {
         char time[VTIME];
         //need to convert from octal mode to a long int
         mode_t long_mode = strtol(header->mode, NULL, OCT);
         time_t mtime = strtol(header->mtime, NULL, OCT);
         if (header->typeflag == '5')
            printf("d");
         else if (header->typeflag == '2')
            printf("l");
         else
            printf("-");
//bitwise operation to set the modes
         for (bit_mask = 0400; bit_mask > 0; bit_mask >>= 1) {
            if ((long_mode & bit_mask) & (S_IRUSR | S_IRGRP | S_IROTH))
               printf("r");
            else if ((long_mode & bit_mask) & (S_IWUSR | S_IWGRP | S_IWOTH))
               printf("w");
            else if ((long_mode & bit_mask) & (S_IXUSR | S_IXGRP | S_IXOTH))
               printf("x");
            else 
               printf("-");
        }
         printf("%s/%s", header->uname, header->gname);
         printf("%8ld ", strtol((header->size), NULL, OCT));

         strftime(time, VTIME, "%F %H:%M", localtime(&mtime));

         printf("%s ", time);
         printf("%s\n", file);
      }
   }
}
void prefix_check(char *file, ustar *header) {
   if (strlen(file) <= NAME)
      strncpy(header->name, file, NAME);
   else {
      char prefix[PATH_MAX];
      strncpy(prefix, file, NAME + PREFIX);
      strncat(header->prefix, prefix, PREFIX);
      strncat(header->name, &prefix[PREFIX + 1], NAME);
   }
}
int chksum(ustar *header) {
   unsigned char *head = (unsigned char *)header;
   int checksum = SUM * ' ';
   int i = 0;
   //excluded 189 154 because they are not relevent chars
   while(i < HEADER) {
      if (i >= 155 || i <= 190) 
         checksum += head[i];
      i++;
   }
   return checksum;
}
int s_archive(ustar header) {
   if (s_flag == 1)
      return 1;
   if (s_flag == 0 && !strncmp(header.magic, "ustar", MAGIC - 1))
      return 1;
  else 
   perror("FORMAT NOT USTAR COMPLIENT");
  return 0;
}
//file = the file to zip, file_d = our archive file
void c_archive(char *file, int file_d) {
   struct ustar header;
   struct stat permissions_info;
   int other_fd = 0;
   char block[HEADER];
   int tot = 0;
   
   memset(&header, 0, sizeof(struct ustar));

   lstat(file, &permissions_info);

   v_archive(file, 'c', &header);
   
   prefix_check(file, &header);
//format print to fill out the header file
   sprintf(header.mode, "%07o", permissions_info.st_mode);
   sprintf(header.uid, "%07o", permissions_info.st_uid);
   sprintf(header.gid, "%07o", permissions_info.st_gid);
//check file or dir
   if (S_ISREG(permissions_info.st_mode)) {
      sprintf(header.size, "%011lo", permissions_info.st_size);
   }
//add in rest of the file header mtime, magic, u and g name
   sprintf(header.mtime, "%011lo", permissions_info.st_mtime);
   strcpy(header.magic, "ustar\0");
   strcpy(header.version, "00");
   strcpy(header.uname, getpwuid(permissions_info.st_uid)->pw_name);
   strcpy(header.gname, getgrgid(permissions_info.st_gid)->gr_name);
//checking for dir or file and setting appropriot flag per tar spec
   if (S_ISDIR(permissions_info.st_mode)) 
        header.typeflag = '5';
   if (S_ISLNK(permissions_info.st_mode)) {
        header.typeflag = '2';
        readlink(file, header.linkname, LINK);
   }
   sprintf(header.chksum, "%07o", chksum(&header));

   write(file_d, &header, HEADER);
//convert to long int, then divide by header chunks and take the 
//rounded up value of that to ensure portability
   tot = ceil((double)strtol(header.size, NULL, OCT) / HEADER);
   other_fd = open(file, O_RDONLY, 0);
   int i = 0;
   //head and write to two files in 512 blocks
   while (i < tot) {
      read(other_fd, block, HEADER);
      write(file_d, block, HEADER);
      i++;
   }
   close(other_fd);
}
void x_archive(char *tarfile, char **args_list, int num_args) {
   
   int tot = 0;
   struct ustar header;
   char path[PATH_MAX];
   char path_copy[PATH_MAX];
   struct stat permissions_info;
   int file_d = 0;
   int other_fd = 0;
   int j = 0;
   char buffer[HEADER];
   long int total_size;
   char new_path[PATH_MAX];
   int decendent = 0;
   mode_t mode;
   int i;

   if ((file_d = open(tarfile, O_RDONLY)) < 0) {
      perror("OPEN_FAILURE\n");
      exit(1);
   }

//while read is valid and header is strict
   while((read(file_d, &header, HEADER) > 0) && s_archive(header)) {
      //null out the path that is a block of path_max
      memset(path, '\0', PATH_MAX);
      memset(new_path, '\0', PATH_MAX);
      strncat(path, header.prefix, PREFIX);
      if (strlen(header.prefix) > 0)
         strncat(path, "/", 1);
      strncat(path, header.name, NAME);
      while ( i < num_args ) {
         if (args_list[i][PATH_MAX - 1] == '/')
            args_list[i][PATH_MAX - 1] = '\0';
         if(strcmp(args_list[i], path) == 0)
            decendent = 1;
         i++;
      }
      //check the decendent presence, and the flag
      if (num_args == 0 || decendent == 1) {
         v_archive(path, 'c', &header);
         if (header.typeflag == '0') {
            strcpy(path_copy, path);
            j = strlen(path_copy) - 1;
            while(path_copy[j] != '/') {
               path_copy[j] = '\0';
               j--;
            }
            i = 0;
            while (path_copy[i]) {
               while (path_copy[i] != '/') {
                  strncat(new_path, &path_copy[i], 1);
                  i++;
               }
               strncat(new_path, "/", 1);
               if((lstat(new_path, &permissions_info) < 0))
                  mkdir(new_path, S_IRWXU | S_IRWXG | S_IRWXO);
               i++;
            }
            if ((other_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0){
               perror("OPEN_FAILURE.\n");
               exit(1);
            }

            mode = strtol(header.mode, NULL, OCT);
            if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
               chmod(path, S_IRWXU);
//convert to long int, then divide by header chunks and take the 
//rounded up value of that to ensure portability
            tot = ceil((double)strtol(header.size, NULL, OCT) / HEADER);
            total_size = strtol((header.size), NULL, OCT);
            i = 0;
            while (i < tot) {
               if (total_size > HEADER) {
                  read(file_d, &buffer, HEADER);
                  write(other_fd, &buffer, HEADER);
                  total_size = total_size - HEADER;
               }
               else {
                  read(file_d, &buffer, HEADER);
                  write(other_fd, &buffer, total_size);
               }
               i++;
            }
            close(other_fd);
         }
         else if(header.typeflag == '5') {
            if(lstat(path, &permissions_info) < 0)
               mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
         }
         else if(header.typeflag == '2') {
            if(symlink(header.linkname, path) < 0)
               perror("SLINK_FAILURE.\n");
         }
      }
      else {
        tot = ceil((double)strtol(header.size, NULL, OCT) / HEADER);
        lseek(file_d, tot * HEADER, SEEK_CUR);
      }
   }
}
void t_archive(char *file, char **args_list, int num_args) {
   
   int decendent = 0;
   struct ustar header;
   int file_d = 0;
   char path[PATH_MAX];
   int size = 0;

   if ((file_d = open(file, O_RDONLY)) == -1) {
      perror("OPEN_FAILURE\n");
      exit(1);
   }
   while((read(file_d, &header, HEADER) > 0) && s_archive(header)) {
      memset(path, '\0', PATH_MAX);
      strncat(path, header.prefix, PREFIX);
      if (strlen(header.prefix) > 0)
         strncat(path, "/", 1);
      strncat(path, header.name, NAME);
      int i = 0;
      while(i < num_args) {

         if (args_list[i][PATH_MAX-1] == '/') {
            args_list[i][PATH_MAX - 1] = '\0';
         }
         if(strstr(path, args_list[i]) != NULL) {
            decendent = 1;
         }
         i++;
      }

      if ((num_args == 0) || decendent == 1)
         v_archive(path, 'l', &header);
//convert to long int, then divide by header chunks and take the 
//rounded up value of that to ensure portability
      size = ceil((double)strtol(header.size, NULL, OCT) / HEADER);

//Move cursor!
      lseek(file_d, size * HEADER, SEEK_CUR);
   }
}

void traverse(char *file, int file_d) {

   DIR *dirp;
   strcat(file, "/");
   dirp = opendir(file);
   c_archive(file, file_d);
   struct dirent *dir;
   char path[PATH_MAX - 1];
//traverse through FS
   while ((dir = readdir(dirp)) != NULL) {
      if (!strcmp(dir->d_name, "..") || !strcmp(dir->d_name, "."))
         continue;
      if (dir->d_type == DT_REG) {
         strcpy(path, file);
         strcat(path, dir->d_name);
         c_archive(path, file_d);
      }
      else if (dir->d_type == DT_DIR) {
         strcpy(path, file);
         strcat(path, dir->d_name);
         traverse(path, file_d);
      }
   }
   closedir(dirp);
}

