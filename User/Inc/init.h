/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_INIT_H
#define __USER_INIT_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void initGpio(void);
void initSpi(void);

int isMaster(void);

#endif /* __USER_INIT_H */
