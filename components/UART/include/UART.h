#ifndef UART_H
#define UART_H

#define UART_SERIAL_NUM UART_NUM_0
#define SERIAL_TXD_PIN (GPIO_NUM_1)
#define SERIAL_RXD_PIN (GPIO_NUM_3)

#define BUF_SIZE 1024

void init_uart(void);
void SendUART_Message(const char* message);
void uartSerial_receiveTask(void *pvParameters);

#endif