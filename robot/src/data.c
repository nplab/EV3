#include "data.h"

wrtcr_rc data_channel_setup(){
  unsigned int stun_urls_length;
  char **stun_urls;

  conf_get_string_array("stun_urls", &stun_urls, &stun_urls_length);

  printf("Len: %d\n", stun_urls_length);
  printf("%s\n", stun_urls[0]);

  return WRTCR_SUCCESS;
}
