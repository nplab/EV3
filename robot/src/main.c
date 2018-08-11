#include "main.h"
#include "pthread.h"
#include "config.h"
#include "signaling.h"
#include "aktsens.h"
#include "data.h"
#include "utils.h"


int main(int argc, char *argv[]) {
  char *def_conf_file_name = "wrtcr_conf.json";
  char *conf_file_name = NULL;

  ZF_LOGI("Starting setup procedure");
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
        if (optopt == 'c'){
          ZF_LOGF("Option -c <config-file> requires the config file as an argument.");
        } else if (isprint (optopt)){
          ZF_LOGF("Unknown option `-%c'.", optopt);
        } else {
          ZF_LOGF("Unknown option character `\\x%x'.", optopt);
        }
      default:
        abort();
      }

  //set defaults
  if( !conf_file_name){
    conf_file_name = def_conf_file_name;
  }

  //start setup of components
  read_config(conf_file_name);
  setup_robot();
  sigserv_connect();
  data_channel_setup();

  clean_exit();

  return WRTCR_SUCCESS;
}

void clean_exit(){
  ZF_LOGI("Starting teardown procedure");
  data_channel_shutdown();
  cleanup_meta_devices();
  cleanup_robot();
  sigserv_disconnect();
  delete_config();
  
  exit(0);
}
