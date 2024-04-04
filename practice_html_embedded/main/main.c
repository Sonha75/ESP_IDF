

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"

#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "wifi_config.h"
#include "server_app.h"
#include "dht11_app.h"



struct dht11_reading dht11_lastdata,dht11_currentdata;
/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

void data_dht11_callback_handler(void){
    char json_string[100];
    sprintf(json_string,"{\"temperature\":\"20.5\",\"humidity\":\"30.6\"}"); //viet chuoi json
    dht11_response(json_string,strlen(json_string));  //gui chuoi json len server



}

void app_main(void)
{
    

    ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

   set_dht11_callback(data_dht11_callback_handler);
    
    wifi_init_sta();
    start_webserver();
    
    while(1){
        dht11_currentdata = DHT11_read();
        if(dht11_currentdata.status == DHT11_OK){
            dht11_lastdata=dht11_currentdata;
        }
    vTaskDelay(1000/portTICK_PERIOD_MS);
   
        }
    
}
