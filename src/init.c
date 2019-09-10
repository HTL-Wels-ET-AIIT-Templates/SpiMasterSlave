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

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

#include "init.h"

// Variables ------------------------------------------------------------------

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
	// TODO: init SPI
  
  RccInit();
  

}

/**
  * @brief  Check if device is configured as Master or Slave
  * @param  None
  * @retval returns 1 if configured as master otherwise 0
  */
int isMaster(void) {
  // TODO: Detect Master or Slave mode
  
}

/**
  * @brief  Enable clock for all peripherals which are used
  * @param  None
  * @retval None
  */
static void RccInit(void)
{
	// TODO: What GPIOs do we need? -> Configure them!
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
}


