//
// SPI Master
//
// Hardware: STM32F429I-Discovery  Board
// IDE:	MDK 4.72a
//
// V1.0
// Datum: 2016-03-13 scmi
//
// Ein Datenwort kann aus einem oder zwei Byte bestehem
//
// SPI_Init muss nach LCD_Init stehen, da LCD_Init laut Test die
// Schnittstelleneinstellungen speziell die Baudrateneinstellungen
// ueberschreibt
// Der Bustakt laeuft (SCK) nur dann, wenn ein Datentransfer durchgefuehrt wird
// d.h. der Master muss die Funktion SPISendData ausfuehren

// Includes ------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

#include "init.h"

// Defines -----------------------------------------------------------------
#define RECV_OK               -1
#define E_RECV_TIMEOUT       -10
#define E_SEND_TIMEOUT       -11

// SPI-Mode
typedef enum {
  SPI_MODE_0 = 0,  // CPOL=0, CPHA=0
  SPI_MODE_1,      // CPOL=0, CPHA=1
  SPI_MODE_2,      // CPOL=1, CPHA=0
  SPI_MODE_3       // CPOL=1, CPHA=1
}SPI_Mode_t;

// Variables ------------------------------------------------------------------
volatile uint32_t SysTickCnt;
static uint16_t sendData, recvData;
static uint32_t sendDataCnt, recvDataCnt;
static uint8_t columnCntRecv, columnCntSend;
// ToDo: Notwendige Variablen hier anlegen
// ...


// Function Declarations ------------------------------------------------------
static void workMaster(void);
static void workSlave(void);
static int8_t SPISendData(uint16_t sendData, uint16_t* recvData);
static int GetUserButtonOnClick(void);
static void delay(uint32_t nCount);


// Functions ------------------------------------------------------------------
void SysTick_Handler(void) {	// wird durch den System-Timer alle 10 ms aufgerufen
  SysTickCnt++;
}


int main(void) {

  RCC_ClocksTypeDef Clocks;

  // ToDo: Notwendige Variablen hier anlegen
  // ...

  SysTickCnt = 0;

  RCC_GetClocksFreq(&Clocks);

  SysTick_Config( Clocks.HCLK_Frequency/100 - 1 );	// 100 Hz ( T=10ms)

  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  LCD_Clear(LCD_COLOR_BLUE);

  LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE); // TextColor,BackColor
  LCD_SetFont(&Font16x24);
  LCD_SetPrintPosition(1,0);
  printf("   HTL-WELS    ");
  LCD_SetPrintPosition(2,0);
  if(isMaster()) {
    printf("  SPI MASTER");
  } else {
    printf("   SPI SLAVE");
  }

  LCD_SetFont(&Font8x8);
  LCD_SetPrintPosition(39,0);
  LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLUE); // TextColor,BackColor
  printf("  Author: SPI Overlord  ");

  LCD_SetFont(&Font8x8);
  LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLUE);
  if(isMaster()) {
    LCD_SetPrintPosition(15,0);
    printf(" SPI4: Send");
    LCD_SetPrintPosition(19,0);
    printf("Send Data Cnt:");
    LCD_SetPrintPosition(19, 14);
  } else {
    LCD_SetPrintPosition(15,0);
    printf(" SPI4: Recv");
    LCD_SetPrintPosition(19,0);
    printf("Recv Data Cnt:");
    LCD_SetPrintPosition(19, 14);
  }    
  printf("%4u ", 0);

  if(isMaster()) {
    LCD_SetPrintPosition(25,0);
    printf(" SPI4: Recv");
    LCD_SetPrintPosition(29,0);
    printf("Recv Data Cnt:");
    LCD_SetPrintPosition(29, 14);
  } else {
    LCD_SetPrintPosition(25,0);
    printf(" SPI4: Send");
    LCD_SetPrintPosition(29,0);
    printf("Send Data Cnt:");
    LCD_SetPrintPosition(29, 14);
  }    
  printf("%4u ", 0);

  LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE);
  
  sendData = 0;
  recvData = 0;
  sendDataCnt = 0;
  recvDataCnt = 0;
  columnCntRecv = 0;
  columnCntSend = 0;

  // ToDo: Initialisierung der GPIOs fuer SCK, MOSI, MISO
  // Auswahl von Port und Pin fuer SPI4 siehe STM32F4xx-Datenblatt
  //
  initGpio();

  // ToDo: Initialisierung vom SPI4
  // Beachte dazu das Beispiel und fuehre sinnvolle Kommentare ein
  initSpi();

  // Warten bis Sendebuffer leer (Sicherheitsabfrage)
  while(!SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE));
  // TODO: Set NSS to default state

  while (1) {

    if (isMaster()) {
      workMaster();
    } else {
      workSlave();
    }
  }
}

/**
* Perform work of Master -> send data when user button is pressed
* @param none
* @return none
*/
void workMaster(void) {
  int8_t retVal;

  if (GetUserButtonOnClick()){

    retVal = SPISendData(sendData, &recvData);

    // Ausgabe vom gesendeten Zeichen in einer Zeile
    if(columnCntSend >= 24) {		// Zeile voll?
      LCD_ClearLine(LCD_LINE_17);
      columnCntSend = 0;
    }

    LCD_SetPrintPosition(17, columnCntSend);
    printf("%02X ", sendData);

    columnCntSend = columnCntSend + 2;
    sendData = ++sendData & 0xff;

    sendDataCnt++;
    LCD_SetPrintPosition(19, 14);
    printf("%4u ", sendDataCnt);

    if (retVal == RECV_OK) {

      // Ausgabe vom empfangenen Zeichen in einer Zeile
      if(columnCntRecv >= 24) {		// Zeile voll?
        LCD_ClearLine(LCD_LINE_27);
        columnCntRecv = 0;
      }

      LCD_SetPrintPosition(27, columnCntRecv);
      printf("%02X", recvData);

      columnCntRecv = columnCntRecv + 2;
      recvDataCnt++;

      LCD_SetPrintPosition(29, 14);
      printf("%4u ", recvDataCnt);
    }
  }
}

/**
* Perform work of Slave -> wait for data
* @param none
* @return none
*/
void workSlave(void) {
  // Slave im Polling Modus
//  SPI_NSSInternalSoftwareConfig(SPI4, SPI_NSSInternalSoft_Reset);
  if(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == SET) {
    // Daten empfangen
    recvData = SPI_I2S_ReceiveData(SPI4);	// Empfangsdaten Slave lesen
    SPI_I2S_ClearFlag(SPI4, SPI_I2S_FLAG_RXNE);
//    SPI_NSSInternalSoftwareConfig(SPI4, SPI_NSSInternalSoft_Set);


//    LCD_SetPrintPosition(13,0);
//    printf(" SR: %X", SPI4->SR);
//    while(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE) != SET);
    sendData = 0x11; // ToDo: Kennung einfügen
//    SPI_I2S_SendData(SPI4, sendData); 	//Senden Slave
    
    // Ausgabe der empfangenen Daten
    if(columnCntRecv >= 24) {		// Zeile voll? 
      LCD_ClearLine(LCD_LINE_17);
      columnCntRecv = 0;
    }	

    LCD_SetPrintPosition(17, columnCntRecv);
    printf("%02X", recvData);

    columnCntRecv = columnCntRecv + 2;
    recvDataCnt++;

    LCD_SetPrintPosition(19, 14);
    printf("%4u ", recvDataCnt);

    // Ausgabe der gesendeten Zeichen
    if(columnCntSend >= 24) {		// Zeile voll? 
      LCD_ClearLine(LCD_LINE_27);
      columnCntSend = 0;
    }	

    LCD_SetPrintPosition(27, columnCntSend);
    printf("%02X ", sendData);

    columnCntSend = columnCntSend + 2;
    
    sendDataCnt++;
    LCD_SetPrintPosition(29, 14);
    printf("%4u ", sendDataCnt);
  }
}


/**
* Send and receive a data word via SPI
* @param sendData data word to send
* @param recvData data word which has been received
* @return <return_description>
* @details RECV_OK on success, otherwise E_SEND_TIMEOUT or E_RECV_TIMEOUT
*/
static int8_t SPISendData(uint16_t sendData, uint16_t* recvData) {

  uint32_t startTickCnt, currTickCnt;

  *recvData = 0;

  startTickCnt = SysTickCnt;
  currTickCnt = SysTickCnt;
  while (SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE) == RESET
            && (currTickCnt < (startTickCnt + 100) )) {
      currTickCnt = SysTickCnt;
  }

  if (currTickCnt >= (startTickCnt + 100) ) {
    SPI_I2S_ClearFlag(SPI4, SPI_I2S_FLAG_RXNE);
    return E_SEND_TIMEOUT;
  }
  
  // TODO: Activate NSS and send data (SPI_I2S_SendData() )
  

  startTickCnt = SysTickCnt;
  currTickCnt = SysTickCnt;
  while (SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == RESET
            && (currTickCnt < (startTickCnt + 100) )) {
      currTickCnt = SysTickCnt;
  }
  delay(100);
  
  // TODO: deactivate NSS again

  
  
  
  if (currTickCnt >= (startTickCnt + 100) ) {
    return E_RECV_TIMEOUT;
  }

  // TODO: Get received data from rx buffer (SPI_I2S_ReceiveData() )

  
  
  return RECV_OK;
}


/**
* Check if User Button has been pressed
* @param none
* @return 1 if rising edge has been detected
*/
static int GetUserButtonOnClick(void) {
  static uint8_t oldState = 0;
  uint8_t newState;
  uint8_t userButtonOnClick;

  userButtonOnClick = 0;

  newState = (GPIOA->IDR & 0x0001);

  if ( (newState == 1) && (oldState == 0)) {
    userButtonOnClick = 1;
  }

  oldState = newState;

  return userButtonOnClick;

}

/**
* Produces delay with busy waiting
* @param nCount number of loops to perform in busy wait
* @return none
*/
static void delay(uint32_t nCount)
{
  uint32_t index = 0; 
  for(index = nCount; index != 0; index--)
  {
  }
}

