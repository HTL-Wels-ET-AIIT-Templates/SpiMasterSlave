/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 * SPI initialization has to be done after LCD_Init() as the latter is changing
 * the SPI configuration.
 * Initialization is done in init.c, program logic is done in main.c
 *
 *
   _________________________                        _________________________
  |           ______________|                      |______________           |
  |          |SPI4          |                      |          SPI4|          |
  |          |              |                      |              |          |
  |          |     CLK(PE02)|______________________|(PE02)CLK     |          |
  |          |              |                      |              |          |
  |          |    MISO(PE05)|______________________|(PE05)MISO    |          |
  |          |              |                      |              |          |
  |          |    MOSI(PE06)|______________________|(PE06)MOSI    |          |
  |          |              |                      |              |          |
  |          |______________|                      |______________|          |
  |      __                 |                      |      __                 |
  |     |__|                |                      |     |__|                |
  |     USER                |                      |     USER                |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32F429i______________|                      |_STM32F429i______________|

 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "init.h"

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef spiHandle;
static volatile unsigned int sendDataCnt, recvDataCnt;
static uint8_t columnCntRecv, columnCntSend;

/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonOnClick(void);
static int GetTouchState (int *xCoord, int *yCoord);
static void workMaster(void);
static void workSlave(void);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "Author: SPI Overlord", CENTER_MODE);

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
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

	if (isMaster()) {
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	}

	LCD_SetFont(&Font16);
	LCD_SetPrintPosition(3, 0);
	if (isMaster()) {
		printf("    Master Ready");
	} else {
		printf("    Slave Ready");
	}


	int cnt = 0;
	/* Infinite loop */
	while (1)
	{
		//execute main loop every 100ms
		HAL_Delay(100);

		// display timer
		cnt++;
		LCD_SetFont(&Font20);
		LCD_SetTextColor(LCD_COLOR_BLUE);
		LCD_SetPrintPosition(5, 0);
		printf("   Timer: %.1f", cnt/10.0);

		// test touch interface
		int x, y;
		if (GetTouchState(&x, &y)) {
			LCD_FillCircle(x, y, 5);
		}

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
static void workMaster(void) {
	uint8_t tx = 'm';
	uint8_t rx;

	if (GetUserButtonOnClick()){

		// TODO: implement sending data


		LCD_SetFont(&Font8);
		LCD_SetPrintPosition(17, columnCntSend);
		printf("%02X ", tx);

		columnCntSend = columnCntSend + 2;

		sendDataCnt++;
		LCD_SetPrintPosition(19, 14);
		printf("%4u ", sendDataCnt);

		// Print rx-ed data in one line
		LCD_SetPrintPosition(27, columnCntRecv);
		printf("%02X", rx);

		columnCntRecv = columnCntRecv + 2;
		recvDataCnt++;

		LCD_SetPrintPosition(29, 14);
		printf("%4u ", recvDataCnt);

	}
}

/**
 * Perform work of Slave -> wait for data
 * @param none
 * @return none
 */
static void workSlave(void) {
	uint8_t tx = 's';
	uint8_t rx;


	// TODO: implement receiving data



	// Print rx-ed data
	LCD_SetFont(&Font8);
	LCD_SetPrintPosition(17, columnCntRecv);
	printf("%02X", rx);

	columnCntRecv = columnCntRecv + 2;
	recvDataCnt++;

	LCD_SetPrintPosition(19, 14);
	printf("%4u ", recvDataCnt);


	LCD_SetPrintPosition(27, columnCntSend);
	printf("%02X ", tx);

	columnCntSend = columnCntSend + 2;

	sendDataCnt++;
	LCD_SetPrintPosition(29, 14);
	printf("%4u ", sendDataCnt);
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPI4_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&spiHandle);
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	// TODO ?
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @retval None
  */
 void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	 // TODO ?
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
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


