/*********************************************************************************************************//**
 * @file    GPIO/InputOutput/main.c
 * @version $Rev:: 1277         $
 * @date    $Date:: 2016-11-22 #$
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

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup GPIO_Examples GPIO
  * @{
  */

/** @addtogroup InputOutput
  * @{
  */


/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  * @note At this stage the microcontroller clock setting should has already been configured.
  *       This can be done through SystemInit() function which is called from startup
  *       file (startup_ht32fxxxx.s) before branching to application main().
  *       To reconfigure the default setting of SystemInit() function, please refer to
  *       system_ht32fxxxx.c file
  * @details The main function works as the following:
  *    - Check if the backup domain is ready for access so as to disable isolation.
  *      Because some GPIO pins are located in the backup domain.
  *    - Enable AFIO peripheral clock
  *    - Configure WEAKUP, KEY1, KEY2 pins as the input function
  *      - Enable GPIO peripheral clock of input pins
  *      - Configure AFIO mode of input pins
  *      - Configure GPIO direction of input pins
  *    - Configure LED1, LED2, LED3 pins as the output function
  *      - Enable GPIO peripheral clock of output pins
  *      - Configure AFIO mode of output pins
  *      - Configure GPIO direction of output pins
  *    - Infinite loop to read data from the input pin and then output to LED
  *      - Read WEAKUP and then output via LED1
  *      - Read KEY1 and then output via LED2
  *      - Read KEY2 and then output via LED3
  *
  ***********************************************************************************************************/

FlagStatus button[16];
//PA0~PA3為掃描線
//PB0~PA3為資料線
//上拉電阻按鈕陣列

void CKCU_Configuration()
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PA = 1;
	CKCUClock.Bit.PB = 1;
	CKCUClock.Bit.PD = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration(void)
{
	/* Configure WEAKUP, KEY1, KEY2 pins as the input function                                                */
  /* 設定映射到腳位上之功能                                                                      */
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_0, AFIO_FUN_GPIO);
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_1, AFIO_FUN_GPIO);
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_2, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_3, AFIO_FUN_GPIO);
  /* 設定腳位I/O方向                                                          */
  GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_0, GPIO_DIR_IN);
  GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_1, GPIO_DIR_IN);
  GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_2, GPIO_DIR_IN);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_3, GPIO_DIR_IN);
  /* 設定腳位是否加上提升/接地電阻     	*/
	
	//上拉電阻按鈕陣列
	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_0, GPIO_PR_UP);
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_1, GPIO_PR_UP);
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_2, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_3, GPIO_PR_UP);
  /* 設定腳位是否開啟Input功能                                                           */
  GPIO_InputConfig(HT_GPIOB, GPIO_PIN_0, ENABLE);
  GPIO_InputConfig(HT_GPIOB, GPIO_PIN_1, ENABLE);
  GPIO_InputConfig(HT_GPIOB, GPIO_PIN_2, ENABLE);
	GPIO_InputConfig(HT_GPIOB, GPIO_PIN_3, ENABLE);
  /* Configure LED1, LED2, LED3 pins as output function                                                     */
  /* 設定映射到腳位上之功能                                                                     */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_0, AFIO_FUN_GPIO);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_1, AFIO_FUN_GPIO);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_GPIO);
  /* 設定腳位I/O方向                                                                */
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_0, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_1, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_2, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_3, GPIO_DIR_OUT);
}
int main(void){
	int i;
	
  CKCU_Configuration();
  GPIO_Configuration();
//宣告可輸出printf內容到電腦端
  RETARGET_Configuration(); 
  while(1){
    GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0,0);//切換掃描
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_2,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3,1);
    button[0] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_0);
		if(button[0] == 0){printf("1");}
    button[1] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_1);
		if(button[1] == 0){printf("2");}
    button[2] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_2);
		if(button[2] == 0){printf("3");}
    button[3] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_3);
		if(button[3] == 0){printf("A");}
    GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0,1);//切換掃描
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1,0);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_2,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3,1);
    button[4] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_0);
		if(button[4] == 0){printf("4");}
    button[5] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_1);
		if(button[5] == 0){printf("5");}
    button[6] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_2);
		if(button[6] == 0){printf("6");}
    button[7] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_3);
		if(button[7] == 0){printf("B");}

    GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0,1);//切換掃描
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_2,0);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3,1);
    button[8] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_0);
		if(button[8] == 0){printf("7");}
    button[9] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_1);
		if(button[9] == 0){printf("8");}
    button[10] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_2);
		if(button[10] == 0){printf("9");}
    button[11] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_3);
		if(button[11] == 0){printf("C");}

    GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0,1);//切換掃描
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_2,1);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_3,0);
    button[12] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_0);
		if(button[12] == 0){printf("*");}
    button[13] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_1);
		if(button[13] == 0){printf("0");}
    button[14] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_2);
		if(button[14] == 0){printf("#");}
    button[15] = GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_3);
		if(button[15] == 0){printf("D");}

		for(i=0;i<2000000;i++);//避免多次輸出

	
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
