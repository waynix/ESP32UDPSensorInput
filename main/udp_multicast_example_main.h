#ifndef UDP_MULTICAST_EXAMPLE_MAIN
#define UDP_MULTICAST_EXAMPLE_MAIN
#include "esp_event_loop.h"
esp_err_t event_handler(void *ctx, system_event_t *event);
extern EventGroupHandle_t wifi_event_group;


#endif /* UDP_MULTICAST_EXAMPLE_MAIN*/
