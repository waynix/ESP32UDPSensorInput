

#include "network.h"


#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <time.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
inlcude "udp_multicast_example_main.h"


#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD


static void initialise_wifi(void)
{
   tcpip_adapter_init();
   wifi_event_group = xEventGroupCreate();
   ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
   ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
   wifi_config_t wifi_config = {
      .sta = {
         .ssid = EXAMPLE_WIFI_SSID,
         .password = EXAMPLE_WIFI_PASS,
      },
   };
   ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
   ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
   ESP_ERROR_CHECK( esp_wifi_start() );
}
