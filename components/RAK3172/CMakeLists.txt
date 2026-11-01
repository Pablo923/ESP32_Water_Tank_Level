idf_component_register(SRCS "RAK3172.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver freertos UART WS2812B
                       )
