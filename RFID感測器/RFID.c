/*********************************************************************************************************//**
 * @file    SPI/Flash/main.c
 * @version $Rev:: 164          $
 * @date    $Date:: 2015-12-04 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
// <<< Use Configuration Wizard in Context Menu >>>

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "MFRC522.h"
#include "MFRC522_conf.h"
/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup SPI_Examples SPI
  * @{
  */

/** @addtogroup Flash
  * @{
  */

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
void SPI_Configuration()
{
  SPI_InitTypeDef SPI_InitStructure;

  SPI_InitStructure.SPI_Mode = SPI_MASTER;
  SPI_InitStructure.SPI_FIFO = SPI_FIFO_DISABLE;
  SPI_InitStructure.SPI_DataLength = SPI_DATALENGTH_8;
  SPI_InitStructure.SPI_SELMode = SPI_SEL_SOFTWARE;
  SPI_InitStructure.SPI_SELPolarity = SPI_SELPOLARITY_LOW;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_FIRST;
  SPI_InitStructure.SPI_RxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_TxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_ClockPrescaler = 48;
  SPI_Init(HT_SPI0, &SPI_InitStructure);

  SPI_SELOutputCmd(HT_SPI0, ENABLE);

  SPI_Cmd(HT_SPI0, ENABLE);
}
void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.PB         = 1;
	CKCUClock.Bit.SPI0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration(void)
{                                                                
	AFIO_GPxConfig(GPIO_PB,AFIO_PIN_2, AFIO_FUN_GPIO);//sel
	
	AFIO_GPxConfig(GPIO_PB,AFIO_PIN_1, AFIO_FUN_GPIO);//reset
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_2, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_1, GPIO_DIR_OUT);
	AFIO_GPxConfig(GPIO_PB,AFIO_PIN_3, AFIO_FUN_SPI);//SCK
	AFIO_GPxConfig(GPIO_PB,AFIO_PIN_4, AFIO_FUN_SPI);//MOSI
	AFIO_GPxConfig(GPIO_PB,AFIO_PIN_5, AFIO_FUN_SPI);//MISO
}
void SYSTICK_Configuration(void){
  //HT32F52352 Default: SYSTICK = SystemCoreClock/8 = 48Mhz/8 = 6MHz
  SYSTICK_ClockSourceConfig(SYSTICK_SRC_STCLK);

  //設定Reload值
  // Reload=SYSTICK          => 1s
  // Reload=SYSTICK/1000000  => 1us
  SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000000);

  // Enable SYSTICK Interrupt
  SYSTICK_IntConfig(ENABLE);
}
int main(void)
{
	CKCU_Configuration();
	GPIO_Configuration();//選片pin、硬體重置pin
	SPI_Configuration();
	RETARGET_Configuration();//將printf與序列小幫手連接用
	SYSTICK_Configuration();
	
	MFRC522_Init();
	while (1){
		if (!Get_Card_Number())
		{
			printf("\rThe card's number is : \n");
			printf("%x",serNum[0]);
			printf("%x",serNum[1]);
			printf("%x",serNum[2]);
			printf("%x",serNum[3]);
			printf("%x\n",serNum[4]);
		}
	}
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
