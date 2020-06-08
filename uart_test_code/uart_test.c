/*
 * uart_test.c
 *
 *  Created on: 13 �ub 2020
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

unsigned char pcBuffer[128], uartBuf[uart_buffersize], pcData[],uartData[128],yaz�lanData[],okunanData[];

/**
 * uart_kurulum fonksiyonu uart konfig�rasyonlar�n� yapmak i�in olu�turulmu� bir fonksiyondur.
 *
 * @param BaudRate: eBaudrate ad�nda enum tan�mlanm��t�r.
 * Baud_9600 i�in 9600 de�eri atan�r.
 * Baud_19200 i�in 19200 de�eri atan�r.
 * Baud_38400 i�in 38400 de�eri atan�r.
 * Baud_57600 i�in 57600 de�eri atan�r.
 * Baud_115200 i�in 115200 de�eri atan�r.
 *
 * @param callback: uart kesme fonksiyonunu bu fonksiyona arg�man olarak (void)*f(UART_Handle handle, void *pcData, size_t num) verilmelidir.
 *
 * @param Parite: eParite ad�nda enum tan�mlanm��t�r.
 * Parite_NONE i�in parite biti olmamas� ayarlan�r.
 * Parite_EVEN i�in parite modu even olarak ayarlan�r.
 * Parite_ODD i�in parite modu odd olarak ayarlan�r.
 * @param StopBit: eStopBit ad�nda enum tan�mlanm��t�r.
 *
 * Stop_ONE i�in stop biti 1 olarak atan�r.
 * Stop_TWO i�in stop biti 2 olarak atan�r.
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
 * uart_step  fonksiyonu uarttangelen verileri ringbuffera yazmak ve ringbuffer'dan uarta yazmak i�in olu�turulmu� fonksiyondur.
 * kullan�lacak diziler "unsigned char pcBuffer[128], uartBuf[uart_buffersize], pcData[],uartData[128];" �eklinde tan�mlan�r.
 **/
void UART_Step()
{
    uint32_t RingSize = RingBufUsed(&RingBuf);
    RingBufRead(&RingBuf,pcBuffer, RingSize);
    UART_write(uart, &pcBuffer, RingSize);
}
/**
 * readcallback  fonksiyonu uart�n kesme fonksiyonudur. uart_step'in �al��mas� i�in semaphore buradan post edilir.
 * UART_Handle hangi uart objesinden donan�m kesmesi bilgisi gelece�inin bilgisini ta��r.
 * pcData dizisi uarttan kesme geldi�i anda kanaldaki verilerin yaz�ld��� dizidir.
 * num adl� de�i�kende ise gelen verilinin boyutu tutulmaktad�r.
 **/
void readCallback(UART_Handle handle, void *pcData, size_t num)
{
    System_printf("Kesme geldi");
    RingBufWrite(&RingBuf, pcData, num);
    UART_read(handle, pcData, uart_buffersize);
    Semaphore_post(uart_semHandle);
}
/**
 * UART_Oku fonksiyonu verileri ringbuffer'a yazmak i�in kullan�lan fonksiyondur.
 * okunanData'n�n i�ine ringbuffer'a yaz�lmak istenen veriler yaz�l�r.
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
 * UART_Yaz fonksiyonu yaz�lanData dizisindeki boyut kapasitesindeki verileri seri porta yazmak i�in kullan�lan fonksiyondur.
 * yaz�lanData'n�n i�ine seri porta yaz�lmak istenen veriler yaz�l�r.
 * yaz�lanKapasite yaz�lanData dizisinin boyutunu belirtmektedir.
 */
void UART_Yaz(unsigned char* yaz�lanData, int yaz�lanKapasite)
{
    UART_write(uart,yaz�lanData,yaz�lanKapasite);
}



