#include <unistd.h> /* for chdir() */
#include <stdio.h>  /* for printf() */
#include <errno.h>  /* for errno */
#include <string.h> /* for strerror() */
#include <stdlib.h> /* for malloc() */
#include <fcntl.h>  /* for open() */
#include <dirent.h>

int
main(int argc, char* argv[]){
  int bufsize = 1024;
  char* buf = malloc(bufsize);
  char* cwd = getcwd(buf, bufsize);

  DIR *cdir = opendir(cwd);
  struct dirent *entry;
  if(cdir != NULL){
    while((entry = readdir(cdir))){
      if(entry->d_name[0] != '.')
        printf("%s\t", entry->d_name);
    }

    closedir(cdir);
  }


  if (argc > 2)
    printf("error");
  switch(*argv[1]){
  }




  return 0;
}
