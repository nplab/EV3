#include <getopt.h>

#include "setup.h"

int setup(int argc, char *argv[]){
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
        if (optopt == 'c')
          ZF_LOGF("Option -c <config-file> requires the config file as an argument.");
        else if (isprint (optopt))
          ZF_LOGF("Unknown option `-%c'.", optopt);
        else
          ZF_LOGF("Unknown option character `\\x%x'.", optopt);
        /* return WRTCR_FAILURE; */
      default:
        abort();
      }

  //set defaults
  if( !conf_file_name){
    conf_file_name = def_conf_file_name;
  }

  //start setup of components
  read_config(conf_file_name);
  /* sigserv_connect(); */
  /* data_channel_setup(); */
  setup_robot();
  int8_t speeds[] = {1, -1, 10, -10, 20, -20, 30, -30, 40, -40, 50, -50, 60, -60, 70, -70, 100, -100, 0, 0};
  for(int i=0; i<10; i++){
    drive(&(speeds[i*2]));
    sleep(100);
  }

  return WRTCR_SUCCESS;
}

int teardown(){
  ZF_LOGI("Starting teardown procedure");
  sigserv_disconnect();
  delete_config();

  return WRTCR_SUCCESS;
}
