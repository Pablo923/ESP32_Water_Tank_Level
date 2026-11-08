#ifndef RAK3172_H
#define RAK3172_H

#define UART_RAK3172_NUM UART_NUM_2
#define RAK3172_TXD_PIN (GPIO_NUM_17)
#define RAK3172_RXD_PIN (GPIO_NUM_16)

#define BUF_SIZE 1024

// Keep these LoRa parameters identical on the receiving node.
// Change the frequency to the channel configured on the receiver.
#define RAK3172_P2P_CONFIG "915000000:7:125:0:10:14"

void init_RAK3172(void);
void RAK3172_initP2P(void);
void RAK3172_sendCommand(const char* command);
void uartRAK3172_receiveTask(void *pvParameters);

#endif
