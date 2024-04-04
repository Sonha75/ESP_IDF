#include "app_config.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_smartconfig.h"
#include "app_http_server.h"
prototype_type_t provision_type = PROVISION_ACCESSPOINT;
static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int CONFIG_DONE_BIT = BIT2;
static const char *TAG = "CONFIG_WIFI";

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) // khi wifi_start event xay ra
    {
        esp_wifi_connect();
        printf("Wifi connected 1");
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();

        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    { // 2 events cua access point
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG, "Scan done");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(TAG, "Found channel");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        // uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
        {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        esp_wifi_connect();
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}
bool is_config(void)
{

    bool provisioned = false;
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_STA, &wifi_config); // get config của station

    if (wifi_config.sta.ssid[0] != 0x00) // true nếu ssid available
    {
        provisioned = true;
    }
    return provisioned;
}

char ssid[30] = { 0 };
char password[65] = { 0 };
void http_post_data_callback(char *buff, int len)
{
    printf("%s\n", buff);
    char *pt = strtok(buff, "/");
    printf("ssid: %s\n", pt);
    strcpy(ssid, pt);
    pt = strtok(NULL, "/");
    printf("password: %s\n", pt);
    strcpy(password, pt);
    xEventGroupSetBits(s_wifi_event_group, CONFIG_DONE_BIT);
}
void ap_start(void)
{
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "SonHaESP",
            .ssid_len = strlen((char*)"SonHaESP"),
            .channel = 1,
            .password = "ESP32sonha",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,

        },
    };
    if (wifi_config.ap.password[0] == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
void app_config(void) // check wifi config
{
    // dki event co the xay ra
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    s_wifi_event_group = xEventGroupCreate();

    bool is_provisioned = is_config(); // check wifi config

    if (!is_provisioned) // if wifi is not config
    {

        if (provision_type == PROVISION_SMARTCONFIG)
        {
            ESP_ERROR_CHECK(esp_wifi_start());
            ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
            smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

            xEventGroupWaitBits(s_wifi_event_group, ESPTOUCH_DONE_BIT, false, true, portMAX_DELAY);
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
        }
        else if (provision_type == PROVISION_ACCESSPOINT)
        {

            ap_start(); //config wifi ap
            app_http_server_start();
           
            app_http_server_post_set_callback(http_post_data_callback);  //printf config wifi ap
            xEventGroupWaitBits(s_wifi_event_group, CONFIG_DONE_BIT, false, true, portMAX_DELAY);
            // convert to station mode and connect router
            app_http_server_stop();
            wifi_config_t wificonfig;
            bzero(&wificonfig, sizeof(wifi_config_t));
            memcpy(wificonfig.sta.ssid, ssid, strlen(ssid));
            memcpy(wificonfig.sta.password, password, strlen(password));

            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); //If mode is WIFI_MODE_STA, it stops station and frees station control block
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wificonfig));
            ESP_ERROR_CHECK(esp_wifi_start()); 
            ESP_LOGI(TAG, "wifi connected 2");
            
        }
    }
    else
    {
        ESP_ERROR_CHECK(esp_wifi_start());
        printf("\nstarted");
    }
    // xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    // ESP_LOGI(TAG, "wifi connected 3");
      
}
