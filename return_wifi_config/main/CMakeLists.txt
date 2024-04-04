idf_component_register(SRCS "app_main.c" "app_config.c" "app_http_server.c" "app_nvs.c"
                    INCLUDE_DIRS ".")

                    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
                    #bo canh bao ve string format