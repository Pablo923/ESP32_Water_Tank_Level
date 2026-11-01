#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "string.h"
#include "UART.h"
#include "RAK3172.h"


void init_RAK3172(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // Init RAK3172 Communication Port
    uart_driver_install(UART_RAK3172_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_RAK3172_NUM, &uart_config);
    uart_set_pin(UART_RAK3172_NUM, RAK3172_TXD_PIN, RAK3172_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Configure RAK3172 to P2P mode
    RAK3172_initP2P();
}

void RAK3172_sendCommand(const char* command)
{
    uart_write_bytes(UART_RAK3172_NUM, command, strlen(command));
    uart_write_bytes(UART_RAK3172_NUM, "\r\n", 2);
}

void RAK3172_initP2P(void)
{
    char p2p_command[48];

    // Switch to P2P mode. The module restarts if its mode changes.
    RAK3172_sendCommand("AT+NWM=0");
    vTaskDelay(pdMS_TO_TICKS(2000));

    snprintf(p2p_command, sizeof(p2p_command), "AT+P2P=%s", RAK3172_P2P_CONFIG);
    RAK3172_sendCommand(p2p_command);
    vTaskDelay(pdMS_TO_TICKS(200));

    // Ensure the transmitter is not left in receive mode.
    RAK3172_sendCommand("AT+PRECV=0");
    vTaskDelay(pdMS_TO_TICKS(100));
}

void uartRAK3172_receiveTask(void *pvParameters)
{
    static const char *TAG_RAK3172 = "[RAK3172] ";

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    
    while (1) 
    {
        int len = uart_read_bytes(UART_RAK3172_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0) 
        {
            data[len] = 0;  // Null-terminate the received data

            // Print any Response of RAK3172
            SendUART_Message(TAG_RAK3172);
            SendUART_Message((char*)data);
        }   
    }
    free(data);
    vTaskDelay(pdMS_TO_TICKS(10));
}
