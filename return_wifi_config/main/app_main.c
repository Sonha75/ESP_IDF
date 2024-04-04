#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "app_config.h"
#include "app_nvs.h"
#include <inttypes.h>

static const char *TAG = "Sonoff";

#define KEY_NUM "restart counter"
#define KEY_STR "restart string"

void app_main(void){
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    


    int restart_counter = 0;
    app_nvs_get_num(KEY_NUM,&restart_counter);
    restart_counter++;
    app_nvs_set_num(KEY_NUM,restart_counter);

    char buffer[100];
    sprintf(buffer,"Hello Son %d",restart_counter);
    
    char str[50];
    app_nvs_get_string(KEY_STR,str);
    app_nvs_set_str(KEY_STR,buffer);



    app_config();
    //chac chan la connected wifi


}
