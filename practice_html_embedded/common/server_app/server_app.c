#include "server_app.h"
// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
#include <esp_log.h>
// #include <nvs_flash.h>
 #include <sys/param.h>
// #include "esp_netif.h"
// #include "protocol_examples_common.h"
//  #include "protocol_examples_utils.h"
// #include "esp_tls_crypto.h"
#include <esp_http_server.h>
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "esp_tls.h"
static httpd_handle_t server = NULL;
static const char *TAG = "practice_server";

extern const uint8_t index_html_start[] asm("_binary_dht11_html_start");
extern const uint8_t index_html_end[] asm("_binary_dht11_html_end");

static get_pointer_callback_t  dht11_handler=NULL;
static httpd_req_t *REG;
/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start);
return ESP_OK;
 
}
static esp_err_t dht11_pointer_handler(httpd_req_t *req){
    REG=req;
    dht11_handler();
    return ESP_OK;
}
/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}


static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};
static const httpd_uri_t dht11 = {
    .uri       = "/getdata",
    .method    = HTTP_GET,
    .handler   = dht11_pointer_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

void dht11_response(char *buf, int len){
httpd_resp_send(REG, buf, len); //ban chuoi len server
}



void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &dht11);
       
       
    }

    //ESP_LOGI(TAG, "Error starting server!");
    
}

void stop_webserver(void)
{
    // Stop the httpd server
     httpd_stop(server);
}
void set_dht11_callback(void *cb){
    dht11_handler=cb;
}










