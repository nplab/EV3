#include "setup.h"

int setup(const int argc, const char *argv[]){
  char def_conf_file_name = "wrtcr_conf.json"
  char *conf_file_name = NULL;

  //replace this with getopt!!!
  if( argc == 1){ //if there are no arguments, go to defaults
    conf_file_name = &def_conf_file_name;
  }else{ //parse command line arguments for config file
    
  }

  read_config(conf_file_name);
  
  sigserv_connect();

  return WRTCR_SUCCESS;
}

int teardown(){
  sigserv_disconnect();

  return WRTCR_SUCCESS;
}
