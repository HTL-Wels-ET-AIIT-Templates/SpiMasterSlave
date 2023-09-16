/**
   @file init.c
   @brief Initialize peripherals
   @author Bernhard Breinbauer
 */

// Includes ------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "main.h"
#include "init.h"

// Variables ------------------------------------------------------------------
/* SPI handler declaration */
extern SPI_HandleTypeDef spiHandle;

// Function Declarations ------------------------------------------------------
static void RccInit(void);

// Functions ------------------------------------------------------------------

/**
 * Initialize the required GPIO Pins
 * @param None
 * @retval None
 */
void initGpio(void) {
	RccInit();
	// TODO: init GPIO

}

/**
 * initialize the SPI peripheral
 * @param None
 * @retval None
 */
void initSpi(void){
	RccInit();
	// TODO: init SPI


}

/**
 * @brief  Check if device is configured as Master or Slave
 * @param  None
 * @retval returns 1 if configured as master otherwise 0
 */
int isMaster(void) {
	// TODO: Detect Master or Slave mode
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)) {
		return 1;
	}
	return 0;

}

/**
 * @brief  Enable clock for all peripherals which are used
 * @param  None
 * @retval None
 */
static void RccInit(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_SPI4_CLK_ENABLE();
}


