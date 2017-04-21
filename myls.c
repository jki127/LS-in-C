#include <unistd.h> /* for chdir() */
#include <stdio.h>  /* for printf() */
#include <errno.h>  /* for errno */
#include <string.h> /* for strerror() */
#include <stdlib.h> /* for malloc() */
#include <fcntl.h>  /* for open() */
#include <dirent.h>
#include <string.h>

int
main(int argc, char* argv[]){
  int bufsize = 1024;
  char* buf = malloc(bufsize);
  char* cwd = getcwd(buf, bufsize);

  DIR *cdir = opendir(cwd);
  struct dirent *entry;
  if(cdir != NULL){

    // No flags
    if(argc == 1){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    // -a, Include dir entries that begin with a dot
    if (strncmp(argv[1],"-a", 2) == 0){
      while((entry = readdir(cdir))){
        printf("%s\t", entry->d_name);
      }
      closedir(cdir);
    }

    // -i, Prepend each file's inode number
    if (strncmp(argv[1], "-i", 2) == 0){
      while((entry = readdir(cdir))){
        if(entry->d_name[0] != '.')
          printf("%llu %s\t", entry->d_ino, entry->d_name);
      }
      closedir(cdir);
    }

    //-m, list files across the page separated by commas
    if (strncmp(argv[1], "-m", 2) == 0){
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

    if (strncmp(argv[1], "-l", 2) == 0){

    }
  }

  return 0;
}
