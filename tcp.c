#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

#include<sys/stat.h>
#include<sys/types.h>


#ifndef BUFF_SZ
#define BUFF_SZ 4096
#endif


int file_copy(char* source, const char* destination){
  struct stat stat_buff;
  mode_t permission;
  ino_t inode;

  int dest_fd, source_fd, n;
  void *buff;
  char *newpath, *temp;

  //Read buffer memory
  if((buff = malloc(BUFF_SZ)) == NULL)
    return 1;

  newpath = NULL;


  //Check Permission
  if(stat(source, &stat_buff) == 0){
    permission = stat_buff.st_mode;
    inode = stat_buff.st_ino;

  }
  else
    //Could not reach
    return 1; 

  //Check if Directory
  if (S_ISDIR(stat_buff.st_mode))
    return 3;

  if((source_fd = open(source, O_RDONLY)) < 0)
    return 1;

  
  //Destination file
  if(stat(destination, &stat_buff) < 0 && errno != ENOENT){
    return 2;

  }

  /*
  * Check for directory and if directory appand source
  * filename to be added to destination
  */

  if(S_ISDIR(stat_buff.st_mode)){
    if((temp = strrchr(source, '/')) == NULL)
      temp = source;

    if(destination[strlen(destination) - 1] != '/'){
      if((newpath = malloc(strlen(temp) + strlen(destination) + 1)) == NULL){
	return 2;

      }
      strncat(newpath, destination, strlen(destination));
      strncat(newpath, "/", 1);

    }
    else{
      if((newpath = malloc(strlen(temp) + strlen(destination))) == NULL)
	return 2;

      strncat(newpath, destination, strlen(destination));

    }
    
    strncat(newpath, temp, strlen(temp));

  }

  //Check for copyong file to itself
  if(newpath != NULL){
    if(stat(newpath, &stat_buff) < 0 && errno != ENOENT){
      return 2;

    }

  }
  else{
    if(stat(destination, &stat_buff) < 0 && errno != ENOENT){
      return 2;

    }

    if(stat(destination, &stat_buff) == 0){
      if(stat_buff.st_ino == inode)
	return 4;
    
    }

  }

  //Check for new path or existing
  if(newpath != NULL)
    dest_fd = open(newpath, O_CREAT | O_WRONLY | O_TRUNC, permission);
  else
    dest_fd = open(destination, O_CREAT | O_WRONLY | O_TRUNC, permission);

  if(dest_fd < 0){
    close(source_fd);
    return 2;

  }

  while((n = read(source_fd, buff, BUFF_SZ)) > 0){
    if(write(dest_fd, buff, n) != n){
      close(source_fd);
      close(dest_fd);
      return 2;

    }

  }

  //Check for read error
  if(n < 0){
    close(source_fd);
    close(dest_fd);
    return 1;

  }

  if(close(source_fd) < 0)
    return 1;
  if(close(dest_fd) < 0)
    return 2;


  free(buff);

  //Check new path
  if(newpath != NULL)
    free(newpath);

  return 0;

}


int main(int, char *[]);

static void usage(void){
  (void)fprintf(stderr, "Usage : tcp [source] [destination]\n");
  exit(EXIT_FAILURE);

}


int
main(int argc, char *argv[]){
  int i, err;
  
  if(argc != 3){
    usage();
  
  }
  
  i = optind;
  if((err = file_copy(argv[i], argv[i+1])) > 0){
    switch(err){
    case 1:
      perror(argv[i]);
      break;

    case 2:
      perror(argv[i+1]);
      break;

    case 3:
      fprintf(stderr, "%s : Source cannot be a directory\n", argv[i]);
      break;

    case 4:
      fprintf(stderr, "Cannot copy file to itself");
      break;

    default:
      perror("Error!!");
      break;

    }
    return EXIT_FAILURE;

  }

  return EXIT_SUCCESS;

}
