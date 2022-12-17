#include <string.h>
#include "lib_tar.h"
#include <stdio.h>


/**
 * Check if end of archive reached
 * @param fd A file descriptor pointing to the start of a file containing a tar archive.
 * @return 1 if end reached, 0 in if not
 */
int checkEnd(int fd){
    char data[1024];
    read(fd,data,1024);

    uint sum = 0;
    for (int i = 0; i < 512; ++i) {
        sum += data[i];
    }
    lseek(fd,-1024,SEEK_CUR);

    return sum == 0;
}


/**
 * Find the path of a file linked to a symlink file
 * @param sympath path of the symlink file
 * @param file name of the linked file
 * @return path of the linked file
 */
char* pathoflink(char* sympath, char* file){
    int lastBack = 0;
    for (int i = 0; i < strlen(sympath); ++i) {
        if(sympath[i] == '/'){
            lastBack = i;
        }
    }
    if(lastBack != 0){lastBack++;}
    char* path = (char*) malloc(lastBack + strlen(file)+1);
    memcpy(path,sympath,lastBack);
    memcpy(path+lastBack,file, strlen(file));
    path[lastBack + strlen(file)+1] = '\0';
    return path;
}


/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    int n = 0;
    while(!end){
        read(tar_fd,buf,512);

        tar_header_t* a_header = (tar_header_t*) buf;

        if(strncmp(a_header->magic,TMAGIC,TMAGLEN-1) != 0){return -1;}
        if(strncmp(a_header->version, TVERSION, TVERSLEN) != 0){return -2;}


        uint checksum = 0;
        for (int i = 0; i < 512; ++i) {
            if(i>=148 && i<156){
                checksum += ' ';
            }else{
                checksum += buf[i];
            }
        }

        if(TAR_INT(a_header->chksum) != checksum){return -3;}


        skip = TAR_INT(a_header->size)/512; //number of full 512 block
        skip += TAR_INT(a_header->size)%512 != 0; //number of not full blocks
        lseek(tar_fd,skip*512,SEEK_CUR);

        end = checkEnd(tar_fd);

        n++;
    }

    return n;
}

/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    while(!end){
        read(tar_fd,buf,512);

        tar_header_t* a_header = (tar_header_t*) buf;

        if(strncmp(a_header->name,path, strlen(a_header->name)) == 0){return 1;}

        skip = TAR_INT(a_header->size)/512; //number of full 512 block
        skip += TAR_INT(a_header->size)%512 != 0; //number of not full blocks
        lseek(tar_fd,skip*512,SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    while(!end) {
        read(tar_fd, buf, 512);

        tar_header_t *a_header = (tar_header_t *) buf;

        if (strncmp(a_header->name, path, strlen(a_header->name)) == 0) {
            if(a_header->typeflag == DIRTYPE){
                return 1;
            }
            return 0;
        }

        skip = TAR_INT(a_header->size) / 512; //number of full 512 block
        skip += TAR_INT(a_header->size) % 512 != 0; //number of not full blocks
        lseek(tar_fd, skip * 512, SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    while(!end) {
        read(tar_fd, buf, 512);

        tar_header_t *a_header = (tar_header_t *) buf;

        if (strncmp(a_header->name, path, strlen(path)) == 0) {
            if(a_header->typeflag == REGTYPE || a_header->typeflag == AREGTYPE){
                return 1;
            }
            return 0;
        }

        skip = TAR_INT(a_header->size) / 512; //number of full 512 block
        skip += TAR_INT(a_header->size) % 512 != 0; //number of not full blocks
        lseek(tar_fd, skip * 512, SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    while(!end) {
        read(tar_fd, buf, 512);

        tar_header_t *a_header = (tar_header_t *) buf;

        if (strncmp(a_header->name, path, strlen(path)) == 0) {
            if(a_header->typeflag == SYMTYPE){ // || LNKTYPE?
                return 1;
            }
            return 0;
        }

        skip = TAR_INT(a_header->size) / 512; //number of full 512 block
        skip += TAR_INT(a_header->size) % 512 != 0; //number of not full blocks
        lseek(tar_fd, skip * 512, SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    return 0;
}


/**
 * Lists the entries at a given path in the archive.
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    int initBack = 0;
    int c = 0;
    while(!end){
        read(tar_fd,buf,512);

        tar_header_t* a_header = (tar_header_t*) buf;

        if(strncmp(a_header->name,path, strlen(path)) == 0){
            if(a_header->typeflag == SYMTYPE){
                return list(tar_fd,pathoflink(path,a_header->linkname),entries,no_entries);
                //pas sur que pathoflink fonctionne pour ce cas
            }

            int lastBack = 0;
            for (int i = 0; i < strlen(a_header->name); ++i) {
                if(a_header->name[i] == '/'){
                    lastBack = i;
                }
            }
            if(c == 0){
                initBack = lastBack;
            }
            if (lastBack > initBack){
                if(a_header->name[lastBack+1] == '\0'){
                    if(c < *no_entries){
                        memcpy(entries[c], a_header->name, strlen(a_header->name));
                        c++;
                    }
                }

            }else{
                if(c < *no_entries){
                    memcpy(entries[c], a_header->name, strlen(a_header->name));
                    c++;
                }
            }
        }

        skip = TAR_INT(a_header->size)/512; //number of full 512 block
        skip += TAR_INT(a_header->size)%512 != 0; //number of not full blocks
        lseek(tar_fd,skip*512,SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    *no_entries = c;
    return 1;
}


/**
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    lseek(tar_fd,0,SEEK_SET);
    char buf[512];
    long skip = 0;
    int end = 0;
    while(!end){
        read(tar_fd,buf,512);

        tar_header_t* a_header = (tar_header_t*) buf;

        if(strncmp(a_header->name,path, strlen(path)) == 0){
            if(a_header->typeflag == DIRTYPE){
                return -1;
            }
            if(a_header->typeflag == SYMTYPE){
                char* newPath = pathoflink(path,a_header->linkname);
                int ret = read_file(tar_fd,newPath,offset,dest,len);
                free(newPath);
                return ret;
            }
            ssize_t sz = TAR_INT(a_header->size);
            if(offset > sz){
                return -2;
            }
            lseek(tar_fd,offset,SEEK_CUR);
            if((sz-offset) < *len){
                *len = sz-offset;
            }
            read(tar_fd,dest,*len);
            return (sz-offset) - *len;
        }

        skip = TAR_INT(a_header->size)/512; //number of full 512 block
        skip += TAR_INT(a_header->size)%512 != 0; //number of not full blocks
        lseek(tar_fd,skip*512,SEEK_CUR);

        end = checkEnd(tar_fd);
    }
    return -1;
}


/*
 * Ds les strlen des strcompare faut p-e changer prcq pas sur que ce soit très réglo qd je fait strlen(a_header->name)
 *
 */