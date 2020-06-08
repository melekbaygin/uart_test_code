/*
 * uart_test.h
 *
 *  Created on: 13 Þub 2020
 *      Author: Mustafa Abdullah Kuþ
 */

#ifndef UART_TEST_H_
#define UART_TEST_H_

#include <ti/drivers/UART.h>
typedef void        (*OkumaKesmesi)    (UART_Handle, void *buf, size_t count);
typedef enum eBaudRate
{
    Baud_2400=2400,
    Baud_9600=9600,
    Baud_19200=19200,
    Baud_38400=38400,
    Baud_57600=57600,
    Baud_115200=115200,
    Baud_921600=921600
} eBaudRate;
typedef enum eParite
{
    Parite_NONE = 0,
    Parite_EVEN = 1,
    Parite_ODD  = 2,
    Parite_ZERO = 3,
    Parite_ONE  = 4
} eParite;
typedef enum eStopBit
{
    Stop_ONE = 0,
    Stop_TWO = 1
} eStopBit;
typedef enum eKesmeByte
{
    Kesme_1 = 1,
    Kesme_8 = 8
} eKesmeByte;

void UART_Kurulum(eBaudRate BaudRate, OkumaKesmesi callback,eParite Parite, eStopBit StopBit);
void UART_Step();
void readCallback(UART_Handle handle, void *pcData, size_t num);

void UART_Oku(unsigned char* okunanData, int okunanKapasite);
void UART_Yaz(unsigned char* yazýlanData, int yazýlanKapasite);

#endif /* UART_TEST_H_ */
