#include <stdio.h>

#include "setup.h"

int main(int argc, const char *argv[]) {

  if( setup(argc, argv) != WRTCR_SUCCESS )
    return WRTCR_FAILURE;


  if( teardown() != WRTCR_SUCCESS )
    return WRTCR_FAILURE;

  return WRTCR_SUCCESS;
}
