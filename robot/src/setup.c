#include <getopt.h>

#include "setup.h"
#include "utils.h"

int setup(int argc, char *argv[]){
  char *def_conf_file_name = "wrtcr_conf.json";
  char *conf_file_name = NULL;

  //parse arguments
  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "c:")) != -1)
    switch (c)
      {
      case 'c':
        conf_file_name = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -c <config-file> requires the config file as an argument.\n");
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        /* return WRTCR_FAILURE; */
      default:
        abort();
      }

  //set defaults
  if( !conf_file_name){
    conf_file_name = def_conf_file_name;
  }

  read_config(conf_file_name);
  
  sigserv_connect();

  return WRTCR_SUCCESS;
}

int teardown(){
  sigserv_disconnect();

  return WRTCR_SUCCESS;
}
