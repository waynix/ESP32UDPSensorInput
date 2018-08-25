/* UDP MultiCast Send/Receive Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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
#include "sha3.h"
#include <string.h>
#include "timefunctions.h"
#include "network.h"
/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define UDP_PORT CONFIG_EXAMPLE_PORT
#define MULTICAST_LOOPBACK CONFIG_EXAMPLE_LOOPBACK

#define MULTICAST_TTL CONFIG_EXAMPLE_MULTICAST_TTL


#define LISTEN_DEFAULT_IF CONFIG_EXAMPLE_LISTEN_DEFAULT_IF


/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   we use two - one for IPv4 "got ip", and
   one for IPv6 "got ip". */
const int IPV4_GOTIP_BIT = BIT0;
const int IPV6_GOTIP_BIT = BIT1;
const int HASHLEN = 32;  
static const char *TAG = "udp test";

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
    default:
        break;
    }
    return ESP_OK;
}



static void udp_example_task(void *pvParameters)
{
	uint32_t bits = 0;
   #ifdef CONFIG_EXAMPLE_IPV4
	        bits |= IPV4_GOTIP_BIT;
#endif
#ifdef CONFIG_EXAMPLE_IPV6
		        bits |= IPV6_GOTIP_BIT;
#endif
   init_time();
   while (1) 
   {
      ESP_LOGI(TAG, "Waiting for AP connection...");
      xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
      ESP_LOGI(TAG, "Connected to AP");

	
	
      static int fd;
      static struct sockaddr_in serveraddr;
      char ipaddr[] ="46.38.251.37";
      uint16_t port = 1234;
      if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
         return;
      }

      uint32_t ip_addr_bytes;
      inet_aton(ipaddr, &ip_addr_bytes);

      memset( &serveraddr, 0, sizeof(serveraddr) );
      serveraddr.sin_family = AF_INET;
      serveraddr.sin_port = htons( port );
      serveraddr.sin_addr.s_addr = ip_addr_bytes;
      while(1)	{
         uint8_t buff[1024];
         memcpy(buff,"01234567890123456789012345678901",HASHLEN);
         size_t index=HASHLEN;
         uint32_t mytime = htonl(time(0));
         //uint64_t time = 0x1234567890abcdef;
         size_t timesize = sizeof(mytime);
         memcpy(buff + index, &mytime,timesize);
         index += timesize;
         uint8_t hash_result[32]; 
         puts("\r\ndata:");
         for(size_t i = 0; i < index; i++)
         {
            //putchar(buff[i]);
         printf("%x", buff[i]);
         }  
         
         //And now calculate the checksum
         sha3_256(hash_result,HASHLEN,buff,index);
         //copy the chrecksum to the message
         memcpy(buff,hash_result,HASHLEN);
         printf("index: %d \r\n chksum:", index);
         for(size_t i = 0; i < index; i++)
         {
            //putchar(buff[i]);
         printf("%x", buff[i]);
         }  
         //Send the data to the server
         size_t i = sendto(fd, buff, index, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));	  
         printf("gesendet: %d\n", i );
         //ESP_LOGI(TAG, "sample");
         vTaskDelay(1000 / portTICK_PERIOD_MS);


      }
   }
}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xTaskCreate(&udp_example_task, "mcast_task", 4096, NULL, 5, NULL);
}
