/* THIS CODE IS MY OWN WORK, IT IS WRITTEN WITHOUT CONSULTING A TUTOR OR CODE WRITTEN BY OTHER STUDENT -NANDAR SOE*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <ar.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <utime.h>
#include <dirent.h> 
#include <stdio.h> 
#include <libgen.h>


#ifdef __APPLE__
#ifndef st_mtime
#define st_mtime st_mtimespec.tv_sec
#endif
#endif

#define BUFFER 1024
#define AR_HEADER_SIZE 60

typedef struct ar_hdr Header;
void print_synopsis(void);
void append(int fd, int argc, char **argv);
void extract(int fd, char **argv);
void printt(int fd, char **argv);
void appendAll(int fd, char **argv);

int main(int argc, char * argv[]) {
    if(argc < 3){
        print_synopsis();
        exit(-1);
    }
    
    //key = qxtA
    char key = argv[1][0];
    //name of archive
    char* archiveName = argv[2];
    //single file found
    char* file = argv[3];
    
    //to check if archive exists
    int fd;
    //F_OK = file exists
    if(access(archiveName, F_OK) != -1){
        //Opens file with append or R/W
        fd = open(archiveName, O_APPEND | O_RDWR, 0666);
	
    }
    
    //doesn't exist
    else{
        if(key == 'q' || key =='A') {
            //create a file with permission 666
            fd = open(archiveName, O_RDWR | O_CREAT, 0666);
            write(fd, ARMAG, 8);
        }
        else{
            ///command x && t
            printf("File doesn't exist");
            exit(-1);
        }
    }
 
    int x;
    switch (key) {
        case 'q': append(fd, argc, argv);
            break;
        case 'x': extract(fd, argv);
            break;
        case 't': printt(fd, argv);
            break;
        case 'A': appendAll(fd, argv);
            break;
            
        default:
            print_synopsis();
            break;
    }
    
    //close file descriptor so it can be reused. 
    close(fd);
    exit(0);
    
}

void print_synopsis(){
    printf("Synopsis:\n");
    printf("q: quickly append named file to archive\n");
    printf("x: extract named files\n");
    printf("t: print a concise table of contents of the archive\n");
    printf("A: quickly append all ordinary files in the current directory that have been modified within the last two hours(except the archive itself).\n");
    printf("example: ./myar Hello q hello.c");
}


void append(int fd, int argc, char **argv){
    
    //printf("%d", argc);
    int numofFiles= argc - 3 ;
    //printf("%d", numofFiles);
    int currentfile = 3;
    while(numofFiles>0){
        //Append the fstat info into archive
        struct ar_hdr *file_header = malloc(sizeof(struct ar_hdr));
        struct stat *file_stat = malloc(sizeof(struct stat));
        char* filename = argv[currentfile++];
        
        int temp_fd = open(filename, O_RDONLY);

        char realname[strlen(filename)+1];
        strcpy(realname, filename);
        strcat(realname, "/");
        
        printf("%s", realname);
        
        if(temp_fd == -1){
            printf("file not found");
            close(fd);
            exit(-1);
        }

        fstat(temp_fd, file_stat);

        snprintf(file_header->ar_name, 16, "%-16s", realname);
        snprintf(file_header->ar_date, 12, "%-12d", (int)file_stat->st_mtime);
        snprintf(file_header->ar_uid, 6, "%-6d", (int)file_stat->st_uid);
        snprintf(file_header->ar_gid, 6, "%-6d", (int)file_stat->st_gid);
        snprintf(file_header->ar_mode, 8, "%-8o", file_stat->st_mode);
        snprintf(file_header->ar_size, 10, "%-10d", (int)file_stat->st_size);
        snprintf(file_header->ar_fmag, SARMAG, "%-2s", ARFMAG);

    //    printf("file: %s", filename);
    //    printf("mtime: %-12d", (int)file_stat->st_mtime);
    //    printf("uid: %-6d", (int)file_stat->st_uid);
    //    printf("gid: %-6d", (int)file_stat->st_gid);
    //    printf("mode: %-8o", file_stat->st_mode);
    //    printf("size: %-10d", (int)file_stat->st_size);

        //Write the header information into the file
        write(fd, file_header, AR_HEADER_SIZE);

        //Content of the file
        char tempbuf[(int)file_stat->st_blksize];

        int k = atoi(file_header->ar_size);
        while(k>0){
            if(k>BUFFER){
                read(temp_fd, tempbuf, BUFFER);
                write(fd, tempbuf, BUFFER);
                k = k - BUFFER; 
            }
            else{
                read(temp_fd, tempbuf, k);
                write(fd, tempbuf, k);
                k = 0;
            }
        }

        //offset
        int j=file_stat->st_size;
        if((j%2)==1){
            write(fd,"\n",1);
        }
        close(temp_fd);
        numofFiles--;
    }
    close(fd);
    exit(0);
    
}

void extract(int fd, char **argv){

    struct ar_hdr * fileHeader = malloc(sizeof(struct ar_hdr));
    struct stat *file_stat = malloc(sizeof(struct stat));
    int found = 0;
    int filesize;
    char* archiveName = argv[2];
    char* file = argv[3];
    char* name;
    time_t time;
    
    printf("File:%s", file);
    lseek(fd, SARMAG, SEEK_SET);
    fstat(fd, file_stat);
    filesize = file_stat->st_size;

    while(read(fd, fileHeader, AR_HEADER_SIZE)!=0){
        int j;
        for(j=16; j>=0; j--){
            if(fileHeader->ar_name[j] == '/'){
                fileHeader->ar_name[j] = '\0';
            }
        }
        name = fileHeader->ar_name;
        filesize= (int)strtol(fileHeader->ar_size,NULL,10);
        
        if(strcmp(name,file)!=0){
            if(filesize%2 ==1){
                filesize++;
            }
            lseek(fd, filesize, SEEK_CUR);
        }
        else{
            time = strtol(fileHeader->ar_date, NULL,10);
            found = 1;
            printf(" found\n");
            break;
        }

        
    }
    
    if(found ==0){
        printf(" doesn't exist\n");
    }
    
    //read content
    int a;
    int createdFile = open(name, O_RDWR | O_CREAT, 0666); 
    char contentbuffer[file_stat->st_size];
    int readcount = atoi(fileHeader->ar_size);
    int i;
    while(i<readcount){
        a = read(fd, contentbuffer, 1);
        write(createdFile, contentbuffer, a);
        i++;
    }
    
    //modify time
    struct utimbuf t_buffer;
    t_buffer.actime = time;
    t_buffer.modtime= time;
    utime(file, &t_buffer);
    
   // printf("done t");
    
}

void printt(int fd, char **argv){

    struct ar_hdr * fileHeader = malloc(sizeof(struct ar_hdr));
    struct stat *file_stat = malloc(sizeof(struct stat));
    int found = 0;
    int filesize;
    char* archiveName = argv[2];
    char* file = argv[3];
    char* name;
    
    //printf("%s%s", archiveName, file);
    lseek(fd, SARMAG, SEEK_SET);
    fstat(fd, file_stat);
    filesize = file_stat->st_size;

    while(read(fd, fileHeader, AR_HEADER_SIZE)!=0){
        int j;
        for(j=16; j>=0; j--){
            if(fileHeader->ar_name[j] == '/'){
            	printf("%s\n", fileHeader->ar_name);
            }
        }
        
        filesize= (int)strtol(fileHeader->ar_size,NULL,10);
        name = fileHeader->ar_name;

        if (filesize%2==1) {
            filesize++;
        }
        lseek(fd, filesize, SEEK_CUR);
        
    }
    printf("done t");
    
}

void appendAll(int fd, char **argv){
    
    DIR *d;
    struct dirent *dir;
    char* newargv[10000];
    newargv[0] = argv[0]; //./myar
    newargv[1] = "q";
    newargv[2] = argv[2]; ///The name of the new archive file
    int newargc = 2;

    char myarfile[9];
    
    d = opendir(".");
    if (d) {
	while ((dir = readdir(d)) != NULL) {
            if((dir->d_type == DT_REG) && (strcmp(dir->d_name,argv[2])!=0)){
                char* filename = dir->d_name;
                struct stat * file_stat = malloc(sizeof(struct stat));
                stat(filename, file_stat);
//                printf("mtime: %-12d", (int)file_stat->st_mtime);
                time_t timer;
                int seconds;
                time(&timer);
                seconds = difftime(timer,(int)file_stat->st_mtime);
//                printf("%", seconds);
                if(seconds < 36000){
                    newargv[++newargc] = filename;
//                    printf("%s\n",filename);
                }

            }
        }
        append(fd, newargc+1, newargv);
	closedir(d);
    }
    
}
