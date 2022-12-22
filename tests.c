#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib_tar.h"

/**
 * You are free to use this file to write tests for your implementation
 */

void debug_dump(const uint8_t *bytes, size_t len) {
    for (int i = 0; i < len;) {
        printf("%04x:  ", (int) i);

        for (int j = 0; j < 16 && i + j < len; j++) {
            printf("%02x ", bytes[i + j]);
        }
        printf("\t");
        for (int j = 0; j < 16 && i < len; j++, i++) {
            printf("%c ", bytes[i]);
        }
        printf("\n");
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s tar_file\n", argv[0]);
        return -1;
    }


    int fd = open(argv[1] , O_RDONLY);
    if (fd == -1) {
        perror("open(tar_file)");
        return -1;
    }


    int ret = check_archive(fd);
    int exf = exists(fd,"Folder/first.txt");
    int exs = exists(fd,"Folder/second.txt");
    int ext = exists(fd,"Folder/third.txt");
    int dirf = is_dir(fd,"Folder");
    int dirt = is_dir(fd,"Folder/");
    int dird = is_dir(fd,"Dossier/");
    int filf = is_file(fd,"Folder/second/");
    int filt = is_file(fd,"Folder/second.txt");
    int fild = is_file(fd,"Folder/third.txt");
    int sym = is_symlink(fd,"Folder/symfile.txt");
    int nsym = is_symlink(fd,"Folder/first.txt");



    printf("check_archive returned %d\n", ret);
    printf("first in Folder exist returned %d\n", exf);
    printf("second in Folder exist returned %d\n", exs);
    printf("third in Folder exist returned %d\n", ext);
    printf("Folder is directory returned %d\n", dirf);
    printf("Folder/ is directory returned %d\n", dirt);
    printf("Dossier/ is directory returned %d\n", dird);
    printf("second/ is file returned %d\n", filf);
    printf("second is file returned %d\n", filt);
    printf("third is file returned %d\n", fild);
    printf("symfile.txt is symfile returned %d\n", sym);
    printf("first.txt is symfile returned %d\n", nsym);

    size_t len = 100;
    uint8_t dest[100];
    ssize_t rett = read_file(fd, "Folder/gang/symfile.txt", 0, dest,  &len);
    printf("len : %zu\n",len);
    if(rett >= 0){
        printf("lecture de symfile : \n");
        for (int i = 0; i < len; ++i) {
            printf("%c",dest[i]);
        }
        printf("\n");
        printf("nbr de bytes lu : %zu\n",len);
    }

    printf("read_file returned %zd\n",rett);

    char* d[10];
    for (int i = 0; i < 10; ++i) {
        d[i] = (char*) malloc(100);
    }
    size_t no = 10;
    int ah = list(fd, "Folder/", d, &no);
    for (int i = 0; i < no; ++i) {
        printf("%s\n",d[i]);
    }
    printf("%zu\n",no);
    printf("list returned %d\n", ah);



    return 0;
}