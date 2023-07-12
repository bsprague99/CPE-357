#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>



int myfind(char *path, char *name){
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    size_t length = strlen(path);
    size_t size;

    if (!(dir = opendir(path))) {
        fprintf(stderr, "path not found: %s: %s\n",
                path, strerror(errno));
        return 0;
    }
    //puts(path);
    while ((ent = readdir(dir)) != NULL) {
        char *path_name = ent->d_name;
        if (ent->d_type == DT_DIR) {
            if (!strcmp(path_name, ".") || !strcmp(path_name, ".."))
                continue;
            if (length + strlen(path_name) + 2 > PATH_MAX) {
                fprintf(stderr, "path too long: %s/%s\n", path, path_name);
                continue;
            }
            if (strstr(path, path_name) == path[0]) {
                path[length] = '/';
                strcpy(path + length + 1, path_name);
                myfind(path, name);
                path[length] = '\0';
            }
        } else {
            if (strstr(path, path_name) == path[0]) {
                printf("%s/%s\n", path, path_name);
            }
        }
    }
    closedir(dir);
    return count;
}

int main(int argc, char *argv[]) {
    int num = 0;
    num = myfind(argv[1], argv[2]);
    return num;

}

