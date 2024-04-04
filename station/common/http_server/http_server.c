
#include "http_server.h"
#include <esp_log.h>
// #include <sys/param.h>
// #include "esp_netif.h"
// #include<esp_eth.h>
// #include<esp_system.h>
// #include<esp_event.h>
// #include<esp_wifi.h>
#include <esp_http_server.h>



static const char *TAG = "HTTP_SERVER";
 static httpd_handle_t server = NULL;

extern const uint8_t index_html_start[] asm("_binary_appearance_html_start");
extern const uint8_t index_html_end[] asm("_binary_appearance_html_end");

static httpd_req_t *REG;

static get_pointer_callback_t  sensor_callback_handler=NULL;
static post_pointer_callback_t led_callback_handler=NULL;
static post_pointer_callback_t led_slider_callback_handler=NULL;
/* An HTTP GET handler */
static esp_err_t data_get_handler(httpd_req_t *req)
{   
    REG=req;
    sensor_callback_handler();  //nhay vao ham xu li data_sensor_callback_handler 
   
    // const char* resp_str = (const char*) "{\"temperature\":\"20.5\",\"humidity\":\"30\"}"; 

    // httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static const httpd_uri_t get_data1= {
    .uri       = "/getdata",
    .method    = HTTP_GET,
    .handler   = data_get_handler,
    .user_ctx  =NULL
};
void dht11_response(char *buf, int len){
httpd_resp_send(REG, buf, len); //ban chuoi len server
}


static esp_err_t hello_get_handler(httpd_req_t *req)
{
    
    // const char* resp_str = (const char*) "hello world 1010"; //gui lai Hello Son
    // httpd_resp_send(req, resp_str, strlen(resp_str));
     httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start);
   
    return ESP_OK;
}
/* An HTTP POST handler */

static esp_err_t data_led_handler(httpd_req_t *req)
{
    char buff[100];
    int length = req->content_len;
        httpd_req_recv(req,buff,length);

        led_callback_handler(buff,length);
    return ESP_OK;
}
static esp_err_t data_slider_handler(httpd_req_t *req)
{
    char buff[100];
    int length = req->content_len;
        httpd_req_recv(req,buff,length);

        led_slider_callback_handler(buff,length);
    return ESP_OK;
}
static esp_err_t data_post_handler(httpd_req_t *req)
{
    char buf[100];
    int length = req->content_len;

    printf("read data");
        /* Read the data for the request */
        httpd_req_recv(req, buf,length);
                        
            
               
        printf("send back");
        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, length);
        

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", length, buf);
        ESP_LOGI(TAG, "====================================");
    

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
//ban tin/resource
static const httpd_uri_t get_du_lieu_dc_request = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = "Hello Son!"// nma tren kia viet luon hello world 1010 thay cho req->user_ctx=Hello Son roi
};
static const httpd_uri_t post_du_lieu = {   //truy cap trang /post_dât se nhay vao handler
    .uri       = "/post_data",
    .method    = HTTP_POST,
    .handler   = data_post_handler,
    .user_ctx  = NULL
};
static const httpd_uri_t data_led_post = {
    .uri       = "/ledhandler",
    .method    = HTTP_POST,
    .handler   = data_led_handler,
    .user_ctx  = NULL
};
static const httpd_uri_t slider_led_post = {
    .uri       = "/slider",
    .method    = HTTP_POST,
    .handler   = data_slider_handler,
    .user_ctx  = NULL
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } 
       httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
// static esp_err_t ctrl_put_handler(httpd_req_t *req)
// {
//     char buf;
//     int ret;

//     if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
//         if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//             httpd_resp_send_408(req);
//         }
//         return ESP_FAIL;
//     }

void  start_webserver(void)
{
   
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
       
        httpd_register_uri_handler(server, &get_du_lieu_dc_request); //handle resource requested
         httpd_register_uri_handler(server, &post_du_lieu);  //dki thanh ghi post
          httpd_register_uri_handler(server, &get_data1);  
          httpd_register_uri_handler(server, &data_led_post);
          httpd_register_uri_handler(server,&slider_led_post);
          httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
       
    }else{

    ESP_LOGI(TAG, "Error starting server!");
    }
}

void stop_webserver(void)
{
    // Stop the httpd server
     httpd_stop(server);
}
void set_sensor_callback(void *cb){
    sensor_callback_handler=cb;
}
void set_led_callback(void *cb){
    led_callback_handler=cb;
}
void set_slider_callback(void *cb){
    led_slider_callback_handler=cb;
}
   






