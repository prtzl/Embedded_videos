#include "PROJECT/project.h"
#include "main.h"

extern UART_HandleTypeDef huart3;

int sendTextUart(char const* str, size_t length)
{
    // max number of character for 115200 baud and 10ms timeout
    size_t const timeout = 10; // ms
    size_t const maxLength = 1000;
    if (length > maxLength)
    {
        return -1;
    }
    if ( HAL_OK != HAL_UART_Transmit(&huart3, (uint8_t*)str, length, timeout))
    {
        return -2;
    }
    return 0;
}