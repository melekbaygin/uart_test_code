/*
 * uart_test.c
 *
 *  Created on: 13 Þub 2020
 *      Author: Packard Bell
 */

#include "project.h"
#include "uart_test.h"

#define uart_buffersize 8

UART_Handle uart;
UART_Params uartParams;
tRingBufObject RingBuf;

Semaphore_Struct uart_semStruct;
Semaphore_Handle uart_semHandle;

Semaphore_Struct init_semStruct;
Semaphore_Handle init_semHandle;

unsigned char pcBuffer[128], uartBuf[uart_buffersize], pcData[],uartData[128],yazýlanData[],okunanData[];

/**
 * uart_kurulum fonksiyonu uart konfigürasyonlarýný yapmak için oluþturulmuþ bir fonksiyondur.
 *
 * @param BaudRate: eBaudrate adýnda enum tanýmlanmýþtýr.
 * Baud_9600 için 9600 deðeri atanýr.
 * Baud_19200 için 19200 deðeri atanýr.
 * Baud_38400 için 38400 deðeri atanýr.
 * Baud_57600 için 57600 deðeri atanýr.
 * Baud_115200 için 115200 deðeri atanýr.
 *
 * @param callback: uart kesme fonksiyonunu bu fonksiyona argüman olarak (void)*f(UART_Handle handle, void *pcData, size_t num) verilmelidir.
 *
 * @param Parite: eParite adýnda enum tanýmlanmýþtýr.
 * Parite_NONE için parite biti olmamasý ayarlanýr.
 * Parite_EVEN için parite modu even olarak ayarlanýr.
 * Parite_ODD için parite modu odd olarak ayarlanýr.
 * @param StopBit: eStopBit adýnda enum tanýmlanmýþtýr.
 *
 * Stop_ONE için stop biti 1 olarak atanýr.
 * Stop_TWO için stop biti 2 olarak atanýr.
 **/
void UART_Kurulum(eBaudRate BaudRate, OkumaKesmesi callback,eParite Parite,eStopBit StopBit)
{
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    UART_Params_init(&uartParams);
    uartParams.writeDataMode  = UART_DATA_BINARY;
    uartParams.readDataMode   = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = BaudRate;
    // uartParams.writeMode = WriteMode;
    if(callback == NULL) {
        uartParams.readMode  = UART_MODE_BLOCKING;
    }
    else {
        uartParams.readMode  = UART_MODE_CALLBACK;
        uartParams.readCallback  = callback;
    }
    uartParams.parityType = Parite;
    uart = UART_open(Board_UART0, &uartParams);

    RingBufInit(&RingBuf,pcBuffer, sizeof(pcBuffer));
    /*UARTIntRegister(UART0_BASE, readCallback);
    IntMasterEnable();
    IntEnable(INT_UART0);*/
    // UARTIntEnable(UART0_BASE, UART_INT_RX);

}
/**
 * uart_step  fonksiyonu uarttangelen verileri ringbuffera yazmak ve ringbuffer'dan uarta yazmak için oluþturulmuþ fonksiyondur.
 * kullanýlacak diziler "unsigned char pcBuffer[128], uartBuf[uart_buffersize], pcData[],uartData[128];" þeklinde tanýmlanýr.
 **/
void UART_Step()
{
    uint32_t RingSize = RingBufUsed(&RingBuf);
    RingBufRead(&RingBuf,pcBuffer, RingSize);
    UART_write(uart, &pcBuffer, RingSize);
}
/**
 * readcallback  fonksiyonu uartýn kesme fonksiyonudur. uart_step'in çalýþmasý için semaphore buradan post edilir.
 * UART_Handle hangi uart objesinden donaným kesmesi bilgisi geleceðinin bilgisini taþýr.
 * pcData dizisi uarttan kesme geldiði anda kanaldaki verilerin yazýldýðý dizidir.
 * num adlý deðiþkende ise gelen verilinin boyutu tutulmaktadýr.
 **/
void readCallback(UART_Handle handle, void *pcData, size_t num)
{
    System_printf("Kesme geldi");
    RingBufWrite(&RingBuf, pcData, num);
    UART_read(handle, pcData, uart_buffersize);
    Semaphore_post(uart_semHandle);
}
/**
 * UART_Oku fonksiyonu verileri ringbuffer'a yazmak için kullanýlan fonksiyondur.
 * okunanData'nýn içine ringbuffer'a yazýlmak istenen veriler yazýlýr.
 * capacity okunanData dizisinin boyutunu belirtmektedir.
 */
void UART_Oku(unsigned char* okunanData, int okunanKapasite)
{
    const int used = RingBufUsed(&RingBuf);
    if(used > okunanKapasite)
    {
        RingBufRead(&RingBuf,okunanData, okunanKapasite);
        return;
    }
    RingBufRead(&RingBuf,okunanData, used);
}
/**
 * UART_Yaz fonksiyonu yazýlanData dizisindeki boyut kapasitesindeki verileri seri porta yazmak için kullanýlan fonksiyondur.
 * yazýlanData'nýn içine seri porta yazýlmak istenen veriler yazýlýr.
 * yazýlanKapasite yazýlanData dizisinin boyutunu belirtmektedir.
 */
void UART_Yaz(unsigned char* yazýlanData, int yazýlanKapasite)
{
    UART_write(uart,yazýlanData,yazýlanKapasite);
}



