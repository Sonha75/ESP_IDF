/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NAMESPACE_NVS "user"
static nvs_handle_t my_handle;

void app_nvs_set_num(char *key, int value)
{
    int err;
    err = nvs_open("NAMESPACE_NVS", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle!\n");
    }
    err = nvs_set_i32(my_handle, key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
}
void app_nvs_get(char *key, int *value)
{
    int err;
    err = nvs_open("NAMESPACE_NVS", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle!\n");
    }
    err = nvs_get_i32(my_handle, key, value);
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("%s = %d" PRIu32 "\n", key, *value);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");
        break;
    default:
        printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
}
void app_nvs_set_str(char *key, char* value)
{
    int err;
    err = nvs_open("NAMESPACE_NVS", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle!\n");
    }
    err = nvs_set_str(my_handle, key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
}
