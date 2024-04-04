#ifndef _BLE_APP_H
#define _BLE_APP_H

typedef void(*ble_recv_handler_t) (uint8_t *data, uint16_t len);
void app_ble_start(void);
void app_ble_stop(void);
void app_ble_send_data(uint8_t *data, uint16_t len);
void app_ble_set_recv_callback(void *cb);


#endif