#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H
void  start_webserver(void);
void  stop_webserver(void);

typedef void (*post_pointer_callback_t) (char *buffer, int length);
typedef void (*get_pointer_callback_t)(void);

void set_sensor_callback(void *cb);
void set_led_callback(void *cb);
void LED_button_callback(int pin);
void dht11_response(char *buf, int len);
void set_slider_callback(void *cb);
#endif

