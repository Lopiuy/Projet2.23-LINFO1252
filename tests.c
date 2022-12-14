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
    fprintf(stderr,"argv : %s\n",argv[1]);
    int fd = open(argv[1] , O_RDONLY);
    //fprintf(stderr,"%s\n","A");
    if (fd == -1) {
        perror("open(tar_file)");
        return -1;
    }

    char buf[512];
    read(fd,buf,512);
    fprintf(stderr,"buf in main : %s\n",buf);

    tar_header_t* a_header = (tar_header_t*) buf;
    printf("name in main : %s\n", a_header->name);


    int ret = check_archive(fd);
    //fprintf(stderr,"%s\n","B");

    printf("check_archive returned %d\n", ret);

    return 0;
}