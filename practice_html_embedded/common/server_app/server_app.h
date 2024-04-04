#ifndef _SERVER_APP_H
#define _SERVER_APP_H
#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

typedef void (*get_pointer_callback_t)(void);

void set_dht11_callback(void *cb);
void dht11_response(char *buf, int len);
void start_webserver(void);
void stop_webserver(void);
#endif