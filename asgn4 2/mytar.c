#include "mytar.h"

int main(int argc, char *argv[]) {
   if (argc < 2) {
      fprintf(stderr, "%s\n", "Usage: mytar [ctxvS]f tarfile [ path [...]]");
      exit(1);
   }
   for (int i = strlen(argv[1]) - 1; i >= 0; i--) {
      if (argv[1][i] == 'S')
         STRICT_FLAG = 1;
      else if (argv[1][i] == 'v')
         VERBOSE_FLAG = 1;
      else if (argv[1][i] == 'x')
         tar_extract(argv[2], argc - 3, argv + 3);
      else if (argv[1][i] == 't')
         tar_list(argv[2], argc - 3, argv + 3);
      else if (argv[1][i] == 'c') {
         struct stat st;
         int fd = 0;
         char eoa_marker[HEADER_SIZE];
         memset(eoa_marker, '\0', HEADER_SIZE);
         fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU 
            | S_IRWXG | S_IRWXO);

         for (int i = 3; i < argc; i++) {
            if (lstat(argv[i], &st) < 0)
               perror("Lstat: failed.\n");
            if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode))
               tar_create(argv[i], fd);
            if (S_ISDIR(st.st_mode)) 
               tar_traverse(argv[i], fd); 
         }
         write(fd, eoa_marker, HEADER_SIZE);
         write(fd, eoa_marker, HEADER_SIZE);
         close(fd);
      }
      else if (argv[1][i] != 'f') {
         fprintf(stderr, "%s\n", "Usage: mytar [ctxvS]f tarfile [ path [...]]");
         exit(1);
      }
   }
   return 0;
}
void tar_verbose(char *file, char mode, tar_header *header) {
   if((VERBOSE_FLAG == 1 && mode == 'c')||(VERBOSE_FLAG == 0 && mode == 'l')) 
      printf("%s\n", file);
   else if (VERBOSE_FLAG == 1 && mode == 'l') {
      char time[VTIME_SIZE];
      mode_t long_mode = strtol(header->mode, NULL, OCTAL);
      time_t mtime = strtol(header->mtime, NULL, OCTAL);
      if (header->typeflag == '5')
         printf("d");
      else if (header->typeflag == '2')
         printf("l");
      else
         printf("-");
     for (mode_t bit_mask = 0400; bit_mask > 0; bit_mask >>= 1) {
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
      printf("%8ld ", strtol((header->size), NULL, OCTAL));
      strftime(time, VTIME_SIZE, "%F %H:%M", localtime(&mtime));
      printf("%s ", time);
      printf("%s\n", file);
   }
}
void tar_prefix(char *file, tar_header *header) {
   if (strlen(file) <= NAME_SIZE)
      strncpy(header->name, file, NAME_SIZE);
   else {
      char prefix[PATH_LEN];
      strncpy(prefix, file, NAME_SIZE + PREFIX_SIZE);
      strncat(header->prefix, prefix, PREFIX_SIZE);
      strncat(header->name, &prefix[PREFIX_SIZE + 1], NAME_SIZE);
   }
}
int tar_checksum(tar_header *header) {
   unsigned char *head = (unsigned char *)header;
   int checksum = SUM_SIZE * ' ';
   for (int i = 0; i < HEADER_SIZE; i++) {
      if (i <= 148 || i >= 155) 
         checksum += head[i];
   }
   return checksum;
}
int tar_strict(tar_header header) {
  if(STRICT_FLAG == 1 && strcmp(header.version,"00") == 0
   && header.magic[MAGIC_SIZE - 1] == '\0') 
      return 1;
  else if (STRICT_FLAG == 0 && 
   strncmp(header.magic, "ustar", MAGIC_SIZE - 1) == 0)
      return 1;
  else 
   perror("Bad header. Time to give up!");
  return 0;
}
//file = the file to zip, fd = our archive file
void tar_create(char *file, int fd) {
   struct tar_header header;
   struct stat st;
   int fd2 = 0;
   char block[HEADER_SIZE];
   int size = 0;
   
   memset(&header, 0, sizeof(struct tar_header));

   if (lstat(file, &st) < 0) {
      perror("Lstat: failed.\n");
      return;
   }
   tar_verbose(file, 'c', &header);
   
   tar_prefix(file, &header);

   sprintf(header.mode, "%07o", st.st_mode);
   sprintf(header.uid, "%07o", st.st_uid);
   sprintf(header.gid, "%07o", st.st_gid);
   if (S_ISREG(st.st_mode))
      sprintf(header.size, "%011lo", st.st_size);
   sprintf(header.mtime, "%011lo", st.st_mtime);
   strcpy(header.magic, "ustar\0");
   strcpy(header.version, "00");
   strcpy(header.uname, getpwuid(st.st_uid)->pw_name);
   strcpy(header.gname, getgrgid(st.st_gid)->gr_name);
   if (S_ISDIR(st.st_mode)) 
        header.typeflag = '5';
   else if (S_ISLNK(st.st_mode)) {
        header.typeflag = '2';
        readlink(file, header.linkname, LINK_SIZE);
   }
   sprintf(header.chksum, "%07o", tar_checksum(&header));

   write(fd, &header, HEADER_SIZE);
   size = ceil((double)strtol(header.size, NULL, OCTAL) / HEADER_SIZE);
   fd2 = open(file, O_RDONLY, 0);
   for (int i = 0; i < size; i++) {
      read(fd2, block, HEADER_SIZE);
      write(fd, block, HEADER_SIZE);
   }
   close(fd2);
}
void tar_list(char *file, int num_args, char **args_list) {
   int fd = 0;
   char path[PATH_LEN];
   int size = 0;
   int child = 0;
   struct tar_header header;

   if ((fd = open(file, O_RDONLY)) == -1) {
      perror("Open: error.\n");
      exit(1);
   }
   while((read(fd, &header, HEADER_SIZE) > 0) && tar_strict(header)) {
      memset(path, '\0', PATH_LEN);
      strncat(path, header.prefix, PREFIX_SIZE);
      if (strlen(header.prefix) > 0)
         strncat(path, "/", 1);
      strncat(path, header.name, NAME_SIZE);
      for (int i = 0; i < num_args; i++) {
         if (args_list[i][PATH_LEN-1] == '/')
            args_list[i][PATH_LEN - 1] = '\0';
         if(strcmp(args_list[i], path) == 0)
            child = 1;
      }
      if ((num_args == 0) || child == 1)
         tar_verbose(path, 'l', &header);
      size = ceil((double)strtol(header.size, NULL, OCTAL) / HEADER_SIZE);
      lseek(fd, size * HEADER_SIZE, SEEK_CUR);
   }
}

void tar_traverse(char *file, int fd) {
   DIR *d;
   struct dirent *dir;
   char path[PATH_LEN - 1];  
   strcat(file, "/");
   tar_create(file, fd);
   d = opendir(file);
   while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || 
         strcmp(dir->d_name, "..") == 0)
         continue;
      if (dir->d_type == DT_REG) {
         strcpy(path, file);
         strcat(path, dir->d_name);
         tar_create(path, fd);
      }
      else if (dir->d_type == DT_DIR) {
         strcpy(path, file);
         strcat(path, dir->d_name);
         tar_traverse(path, fd);
      }
   }
   closedir(d);
}
void tar_extract(char *tarfile, int num_args, char **args_list) {
   int fd = 0;
   int fd2 = 0;
   int path_len = 0;
   int size = 0;
   struct tar_header header;
   char path[PATH_LEN];
   char path_copy[PATH_LEN];
   struct stat st;
   char buffer[HEADER_SIZE];
   long int total_size;
   char new_path[PATH_LEN];
   int child = 0;
   mode_t mode;

   if ((fd = open(tarfile, O_RDONLY)) < 0) {
      perror("Open: failed.\n");
      exit(1);
   }
   while((read(fd, &header, HEADER_SIZE) > 0) && tar_strict(header)) {
      memset(path, '\0', PATH_LEN);
      memset(new_path, '\0', PATH_LEN);
      strncat(path, header.prefix, PREFIX_SIZE);
      if (strlen(header.prefix) > 0)
         strncat(path, "/", 1);
      strncat(path, header.name, NAME_SIZE);
      for (int i = 0; i < num_args; i++) {
         if (args_list[i][PATH_LEN - 1] == '/')
            args_list[i][PATH_LEN - 1] = '\0';
         if(strcmp(args_list[i], path) == 0)
            child = 1;
      }
      if (num_args == 0 || child == 1) {
         tar_verbose(path, 'c', &header);
         if (header.typeflag == '0') {
            strcpy(path_copy, path);
            
            for(path_len = strlen(path_copy) - 1; 
path_copy[path_len] != '/'; path_len--)
               path_copy[path_len] = '\0';
            for (int i = 0; path_copy[i]; i++) {
               for (; path_copy[i] != '/'; i++)
                  strncat(new_path, &path_copy[i], 1);
               strncat(new_path, "/", 1);
               if((lstat(new_path, &st) < 0))
                  mkdir(new_path, S_IRWXU | S_IRWXG | S_IRWXO);
            }
            if ((fd2 = open(path, O_WRONLY | O_CREAT | O_TRUNC, 
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0){
               perror("Open: failed.\n");
               exit(1);
            }

            mode = strtol(header.mode, NULL, OCTAL);
            if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
               chmod(path, S_IRWXU);

            size = ceil((double)strtol(header.size, NULL, OCTAL) / HEADER_SIZE);
            total_size = strtol((header.size), NULL, OCTAL);
            for (int i = 0; i < size; i++) {
               if (total_size > HEADER_SIZE) {
                  read(fd, &buffer, HEADER_SIZE);
                  write(fd2, &buffer, HEADER_SIZE);
                  total_size = total_size - HEADER_SIZE;
               }
               else {
                  read(fd, &buffer, HEADER_SIZE);
                  write(fd2, &buffer, total_size);
               }
            }
            close(fd2);
         }
         else if(header.typeflag == '5') {
            if(lstat(path, &st) < 0)
               mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
         }
         else if(header.typeflag == '2') {
            if(symlink(header.linkname, path) < 0)
               perror("Symlink: failed.\n");
         }
      }
      else {
        size = ceil((double)strtol(header.size, NULL, OCTAL) / HEADER_SIZE);
        lseek(fd, size * HEADER_SIZE, SEEK_CUR);
      }
   }
}
