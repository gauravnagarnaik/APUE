#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>

#include "net.c"

void
usage(){
  fprintf(stderr,"usage: sws [ −dh] [ −c dir] [ −i address] [ −l file] [ −p port] dir\n");
  err(EXIT_FAILURE, NULL);
}

int
is_dir(const char *dir)
{
  struct stat sb;

  if (dir == NULL) {
    warnx("the provided dir is NULL");
    return 0;
  }
  if (stat(dir, &sb) < 0) {
    warn("cannot stat dir %s", dir);
    return 0;
  }
  if (!S_ISDIR(sb.st_mode)) {
    warnx("path %s you provided is not a directory", dir);
    return 0;
  }

  return 1;
}


int 
main(int argc, char *argv[])
{
  char ch;

    while ((ch = getopt(argc, argv, "−c:dhi:l:p:"))!= -1) {
        switch (ch) {
            case 'c':
                dir = optarg;
                if (!is_dir(dir)) {
                  errx(EXIT_FAILURE, "invalid dir");
                }
                break;
            case 'd':
                flag_d = 1;
                break;
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case 'i':
              flag_i = 1;
                address = optarg;
                break;
            case 'p':
              flag_p = 1;
              port = atoi(optarg);
              if ((port < MIN_PORT) || (port > MAX_PORT)) {
                errx(EXIT_FAILURE, "port must be between %d and %d", MIN_PORT,
                MAX_PORT);
              }
              break;
            default:
              usage();
              exit(EXIT_FAILURE);
        }
    }
    argc -=optind;
    argv +=optind;

    if (argc < 0)
    {
      usage();
    }

    net();
    
    return 0;
}