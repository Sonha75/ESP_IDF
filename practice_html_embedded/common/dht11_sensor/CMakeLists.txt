set(app_src dht11_app.c)
set(pri_req esp_timer )
idf_component_register(SRCS ${app_src}
                       INCLUDE_DIRS "."
                       REQUIRES ${pri_req} 
                       PRIV_REQUIRES driver)
