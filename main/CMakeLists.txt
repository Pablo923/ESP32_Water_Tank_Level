idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES wifi mqtt_connection tm1637 ultrasonic RAK3172 UART WS2812B)