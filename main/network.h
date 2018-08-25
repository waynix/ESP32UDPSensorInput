

#ifndef NETWORK_H
#define NETWORK_H
#ifdef __cplusplus
extern "C" {
#endif


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
   ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
   ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
   ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
   ESP_ERROR_CHECK( esp_wifi_start() );
}
#ifdef __cplusplus
}
#endif
#endif /* NETWORK_H */

