#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int
main(int argc, char* argv[]){
  int bufsize = 1024;
  char* buf = malloc(bufsize);
  char* cwd = getcwd(buf, bufsize);

  DIR *cdir = opendir(cwd);
  struct dirent *entry;
  if(cdir != NULL){

    // No flags
    if(argc < 2){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    // -a, Include dir entries that begin with a dot
    else if (strncmp(argv[1],"-a", 2) == 0){
      while((entry = readdir(cdir))){
        printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    // -i, Prepend each file's inode number
    else if (strncmp(argv[1], "-i", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%llu %s\t", entry->d_ino, entry->d_name);
      }
      closedir(cdir);
    }

    //-m, list files across the page separated by commas
    else if (strncmp(argv[1], "-m", 2) == 0){
      char s[1024];
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          strcat(s, entry->d_name);
          strcat(s, ", \t");
        }
      }
      int len = strlen(s);
      s[len-3] = '\0';
      printf("%s", s);
      closedir(cdir);
    }

    //-1, one file per line
    else if (strncmp(argv[1],"-1", 2) == 0){
      while((entry = readdir(cdir))){
        printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    // -l:
    // one file per line with file mode, number of links, owner name, group name,
    // number of bytes in the file, abbreviated month, day-of-month file was
    // last modified, hour file last modified, minute file last modified, and
    // the pathname
    else if (strncmp(argv[1], "-l", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.'){
          struct stat f_info;
          int i = stat(entry->d_name, &f_info);

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
  return 0;
  }
}
