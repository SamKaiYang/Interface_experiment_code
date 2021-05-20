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
#include <inttypes.h>
#include <time.h>

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
void CKCU_configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PA = 1;
	CKCUClock.Bit.BFTM0      = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_configuration(void)
{
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_0, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_0, GPIO_DIR_IN);
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_0, GPIO_PR_DISABLE);
  GPIO_InputConfig(HT_GPIOA, GPIO_PIN_0, ENABLE);  
}

void BFTM_Configuration_0(float Time_s, ControlStatus able){
  u32 Compare = SystemCoreClock * Time_s;
  BFTM_SetCompare(HT_BFTM0,Compare);
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_OneShotModeCmd(HT_BFTM0, DISABLE);
  BFTM_IntConfig(HT_BFTM0, ENABLE);
  BFTM_EnaCmd(HT_BFTM0, able);
}


void NVIC_Configuration(void){  
	NVIC_EnableIRQ(BFTM0_IRQn);
}

uint8_t count = 0;
uint32_t data;
vu32 flag_state;
uint32_t receive_data(void)
{
	uint32_t code=0;
	int i = 0;
	
	while (!GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_0));   
	while (GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_0)); 
	
	for (i=0; i<32; i++)
	{
		flag_state = FALSE;
		count=0;
		while (!GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_0)); 
		while (GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_0))  
		 {			 
			 count++;
			 BFTM_Configuration_0(0.0001, ENABLE);
			 while(!flag_state);
			 BFTM_Configuration_0(1, DISABLE);
			 flag_state = FALSE;			 
		 }

		 if (count > 12) // if the space is more than 1.2 ms
		 {
			code |= (1UL << (31-i));   // write 1
		 }
		 else{ 
			 code &= ~(1UL << (31-i));  // write 0
		 }
	}
	return code;
}

void convert_code (uint32_t code)
{
		switch (code)
		{
			case (0xFFA25D):
				printf("CH-\r\n");
				break;
			
			case (0xFF629D):
				printf("CH\r\n");
				break;			

			case (0xFFE21D):
				printf("CH+\r\n");
				break;

			case (0xFF22DD):
				printf("PREV\r\n");
				break;

			case (0xFF02FD):
				printf("NEXT\r\n");
				break;

			case (0xFFC23D):
				printf("PLAY/PAUSE\r\n");
				break;

			case (0xFFE01F):
				printf("-\r\n");
				break;

			case (0xFFA857):
				printf("+\r\n");
				break;

			case (0xFF906F):
				printf("EQ\r\n");
				break;

			case (0XFF6897):
				printf("0\r\n");
				break;			
						
			case (0xFF9867):
				printf("100+\r\n");
				break;

			case (0xFFB04F):
				printf("200+\r\n");
				break;

			case (0xFF30CF):
				printf("1\r\n");
				break;		
			
			case (0xFF18E7):
				printf("2\r\n");
				break;	

			case (0xFF7A85):
				printf("3\r\n");
				break;	

			case (0xFF10EF):
				printf("4\r\n");
				break;

			case (0xFF38C7):
				printf("5\r\n");
				break;

			case (0xFF5AA5):
				printf("6\r\n");
				break;

			case (0xFF42BD):
				printf("7\r\n");
				break;

			case (0xFF4AB5):
				printf("8\r\n");
				break;

			case (0xFF52AD):
				printf("9\r\n");
				break;			
			
			default :
				break;
		}
}

int main(void)
{
	NVIC_Configuration();
  CKCU_configuration();
  GPIO_configuration();
	BFTM_Configuration_0(1, DISABLE);
	RETARGET_Configuration();
	
	while(1)
	{
		flag_state = FALSE;
		while (GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_0));
		data = receive_data();		

		BFTM_Configuration_0(0.2, ENABLE);
		while(!flag_state);
		BFTM_Configuration_0(1, DISABLE);		
		
		convert_code(data);
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
