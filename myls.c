/*
 * LS project
 * ----------
 * Jayson Isaac
 * CSCI 493
 */


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct fileinfo{
  char name[100];
  off_t size;
  time_t mtime;
};

int
fsize_compare(const void *a, const void *b){
  struct fileinfo *x = (struct fileinfo *) a;
  struct fileinfo *y = (struct fileinfo *) b;
  return y->size - x->size;
};

int
fmtime_compare(const void *a, const void *b){
  struct fileinfo *x = (struct fileinfo *) a;
  struct fileinfo *y = (struct fileinfo *) b;
  return y->mtime - x->mtime;
};

void
print_total_blocks(struct dirent *entry, DIR *cdir){
  int sum = 0;
  while((entry = readdir(cdir))){
    if(entry->d_name[0] != '.'){
      struct stat f_info;
      stat(entry->d_name, &f_info);

      sum += f_info.st_blocks;
    }
  }

  printf("total %d\n", sum);

  rewinddir(cdir);
}

int
main(int argc, char* argv[]){
  int bufsize = 1024;
  char* buf = malloc(bufsize);
  char* cwd = getcwd(buf, bufsize);

  DIR *cdir = opendir(cwd);
  struct dirent *entry;
  if(cdir != NULL){

    /*
     *  NO FLAGS
     *  Doesn't display files/directory names starting with dots
     */
    if(argc < 2){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    /*
     *  -a
     *  Include files/directory names that begin with a dot
     *
     */
    else if (strncmp(argv[1],"-a", 2) == 0){
      while((entry = readdir(cdir))){
        printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    /*
     *  -i
     *  Prepend each file's inode number
     */
    else if (strncmp(argv[1], "-i", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%llu %s\t", entry->d_ino, entry->d_name);
      }
      closedir(cdir);
    }

    /*
     *  -m
     *  List files across the page separated by commas
     */
    else if (strncmp(argv[1], "-m", 2) == 0){
      char s[1024];
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          strcat(s, entry->d_name);
          strcat(s, ", ");
        }
      }
      int len = strlen(s);
      s[len-2] = '\0';
      printf("%s", s);
      closedir(cdir);
    }

    /*
     *  -1
     *  one file per line
     */
    else if (strncmp(argv[1],"-1", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          printf("%s\n", entry->d_name);
        }
      }
      closedir(cdir);
    }

    /*
     *  -l
     *  one file per line with file mode, number of links, owner name, group name,
     *  number of bytes in the file, abbreviated month, day-of-month file was
     *  last modified, hour file last modified, minute file last modified, and
     *  the pathname
     */
    else if (strncmp(argv[1], "-l", 2) == 0){
      print_total_blocks(entry, cdir);
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          // Permissions
          printf( (S_ISDIR(f_info.st_mode)) ? "d" : "-");
          printf( (f_info.st_mode & S_IRUSR) ? "r" : "-");
          printf( (f_info.st_mode & S_IWUSR) ? "w" : "-");
          printf( (f_info.st_mode & S_IXUSR) ? "x" : "-");
          printf( (f_info.st_mode & S_IRGRP) ? "r" : "-");
          printf( (f_info.st_mode & S_IWGRP) ? "w" : "-");
          printf( (f_info.st_mode & S_IXGRP) ? "x" : "-");
          printf( (f_info.st_mode & S_IROTH) ? "r" : "-");
          printf( (f_info.st_mode & S_IWOTH) ? "w" : "-");
          printf( (f_info.st_mode & S_IXOTH) ? "x" : "-");

          // number of hard links
          printf("  %d", f_info.st_nlink);

          // user name
          struct passwd *pws;
          pws = getpwuid(f_info.st_uid);
          printf(" %s", pws->pw_name);

          // group name
          struct group *grp;
          grp = getgrgid(f_info.st_gid);
          printf("  %s", grp->gr_name);

          // file size in bytes
          printf("  %llu", f_info.st_size);

          // last modification date
          struct tm *modif_time;
          char time_buf[20];
          modif_time = localtime(&f_info.st_mtime);
          strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", modif_time);
          printf(" %s", time_buf);

          // file name
          printf(" %s\n", entry->d_name);
        }
      }
      closedir(cdir);
    }

    // 6. -S, sort files by size
    else if (strncmp(argv[1], "-S", 2) == 0){
      int filecount = 0;
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          filecount++;
        }
      }
      rewinddir(cdir);

      int i = 0;
      struct fileinfo files[filecount]; //array size should be calculated
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          struct fileinfo file;
          strcpy(file.name, entry->d_name);
          file.size = f_info.st_size;
          files[i] = file;

          i++;
        }
      }

      qsort(files, sizeof(files) / sizeof(*files), sizeof(*files), fsize_compare);

      for(int k = 0; k < (sizeof(files)/sizeof(*files)); k++){
        printf("%s\t", files[k].name);
      }
      closedir(cdir);
    }

    //7. -o, the long format without group id
    else if (strncmp(argv[1], "-o", 2) == 0){
      print_total_blocks(entry, cdir);
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          // Permissions
          printf( (S_ISDIR(f_info.st_mode)) ? "d" : "-");
          printf( (f_info.st_mode & S_IRUSR) ? "r" : "-");
          printf( (f_info.st_mode & S_IWUSR) ? "w" : "-");
          printf( (f_info.st_mode & S_IXUSR) ? "x" : "-");
          printf( (f_info.st_mode & S_IRGRP) ? "r" : "-");
          printf( (f_info.st_mode & S_IWGRP) ? "w" : "-");
          printf( (f_info.st_mode & S_IXGRP) ? "x" : "-");
          printf( (f_info.st_mode & S_IROTH) ? "r" : "-");
          printf( (f_info.st_mode & S_IWOTH) ? "w" : "-");
          printf( (f_info.st_mode & S_IXOTH) ? "x" : "-");

          // number of hard links
          printf("  %d", f_info.st_nlink);

          // user name
          struct passwd *pws;
          pws = getpwuid(f_info.st_uid);
          printf(" %s", pws->pw_name);

          // file size in bytes
          printf("  %llu", f_info.st_size);

          // last modification date
          struct tm *modif_time;
          char time_buf[20];
          modif_time = localtime(&f_info.st_mtime);
          strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", modif_time);
          printf(" %s", time_buf);

          // file name
          printf(" %s\n", entry->d_name);
        }
      }
      closedir(cdir);
    }

    //8. -t Sort by last modification time (Most recently updated first)
    else if (strncmp(argv[1], "-t", 2) == 0){
      int filecount = 0;
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          filecount++;
        }
      }
      rewinddir(cdir);

      int i = 0;
      struct fileinfo files[filecount]; //array size should be calculated
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          struct fileinfo file;
          strcpy(file.name, entry->d_name);
          file.mtime = f_info.st_mtime;
          files[i] = file;

          i++;
        }
      }

      qsort(files, sizeof(files) / sizeof(*files), sizeof(*files), fmtime_compare);

      for(int k = 0; k < (sizeof(files)/sizeof(*files)); k++){
        printf("%s\t", files[k].name);
      }
      closedir(cdir);
    }

    //9. -s, display number of file systems block used by each file
    else if (strncmp(argv[1], "-s", 2) == 0){
      print_total_blocks(entry, cdir);
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          printf("%llu %s  ", f_info.st_blocks, entry->d_name);
        }
      }
    }

    //10. -p, add a slash after each filename if that file is a directory
    else if (strncmp(argv[1], "-p", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          stat(entry->d_name, &f_info);

          if(S_ISDIR(f_info.st_mode)){
            printf("%s/\t", entry->d_name);
          }
          else {
            printf("%s\t", entry->d_name);
          }
        }
      }
    }
    printf("\n");
    return 0;
  }
}
