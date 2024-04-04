#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "protocol_examples_utils.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "app_http_server.h"
#include <esp_log.h>

static const char *TAG = "HTTP";
static void app_http_server_post_default_handler_func(char *buff, int len);
static void app_http_server_get_default_handler_func(char *url_querry, char *host);
static http_post_handler_func_t http_post_handler_func = app_http_server_post_default_handler_func;
static http_get_handler_func_t http_get_handler_func = app_http_server_get_default_handler_func;
static httpd_req_t *resp;
static char http_post_buff[APP_HTTP_SERVER_POST_MAX_BUFFER];
static httpd_handle_t http_server;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static void app_http_server_post_default_handler_func(char *buff, int len)
{
    ESP_LOGI(TAG, "===========RECEIVED POST DATA================");
    ESP_LOGI(TAG, "%.*s", len, buff);
    ESP_LOGI(TAG, "================================");
}
static void app_http_server_get_default_handler_func(char *url_querry, char *host)
{
    ESP_LOGI(TAG, "=======RECEIVED GET PARAM================");
    ESP_LOGI(TAG, "url_querry=%s host=%s", url_querry, host);
    app_http_server_send_response(HTTP_GET_RESPONE_DEFAULT, sizeof(HTTP_GET_RESPONE_DEFAULT));
    ESP_LOGI(TAG, "================================");
}
void app_http_server_send_response(char *buff, int len)
{
    httpd_resp_send(resp, buff, len);
    if (httpd_req_get_hdr_value_len(resp, "Host") == 0)
    {
        ESP_LOGI(TAG, "Request header lost");
    }
}

esp_err_t http_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}
httpd_uri_t http_get = {
    .uri = APP_HTTP_SERVER_GET_DEFAULT_URI,
    .method = HTTP_GET,
    .handler = http_get_handler,
    .user_ctx = NULL};
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char *buff = http_post_buff;
    ;
    int buff_len = sizeof(http_post_buff);
    int ret, remaining = req->content_len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buff,
                                  MIN(remaining, buff_len))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        remaining -= ret;

        http_post_handler_func(buff, req->content_len);
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t http_post = {
    .uri = APP_HTTP_SERVER_POST_DEFAULT_URI,
    .method = HTTP_POST,
    .handler = http_post_handler,
    .user_ctx = NULL

};
void app_http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&http_server, &config) == ESP_OK)
    {
        // register URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(http_server, &http_get);
        httpd_register_uri_handler(http_server, &http_post);
        return;
    }
    ESP_LOGI(TAG, "Error starting http server");
}
void app_http_server_stop(void)
{
    // Stop the httpd server
    if (!http_server)
    {
        httpd_stop(http_server);
        http_server = NULL;
    }
}
void app_http_server_post_set_callback(void *post_handler_callback)
{
    if (post_handler_callback != NULL)
    {
        http_post_handler_func = (http_post_handler_func_t)post_handler_callback;
    }
}
void app_http_server_get_set_callback(void *get_handler_callback)
{
    if (get_handler_callback != NULL)
    {
        http_get_handler_func = (http_get_handler_func_t)get_handler_callback;
    }
}
