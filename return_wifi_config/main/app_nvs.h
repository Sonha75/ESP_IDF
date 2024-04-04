#ifndef _APP_NVS_H
#define _APP_NVS_H

void app_nvs_set_num(char *key, int value);
void app_nvs_get_num(char *key, int *value);
void app_nvs_set_str(char *key, char* value);
void app_nvs_get_string(char *key, char* outvalue);


#endif
