/*********************************************************************************************************//**
 * @file    BFTM/RepetitiveToggle/main.c
 * @version $Rev:: 2191         $
 * @date    $Date:: 2017-11-28 #$
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

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup BFTM_Examples BFTM
  * @{
  */

/** @addtogroup RepetitiveToggle
  * @{
  */


/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
//???CKCU????????????????????????
void CKCU_Configuration(void){
  //????????????????????????????????????????????????CKCUClock????????????????????????0
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  CKCUClock.Bit.AFIO       = 1;//??????AFIO????????????
  CKCUClock.Bit.PA       = 1;//??????GPIO A????????????
	CKCUClock.Bit.PB       = 1;
	CKCUClock.Bit.PC       = 1;
	CKCUClock.Bit.PD       = 1;
	CKCUClock.Bit.BFTM0       = 1;
  //????????????????????????????????????????????????
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration()
{														 
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_1, GPIO_DIR_IN);
	GPIO_PullResistorConfig(HT_GPIOA,GPIO_PIN_1,GPIO_PR_UP);
	GPIO_InputConfig(HT_GPIOA, GPIO_PIN_1,ENABLE);
	
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_0, GPIO_DIR_OUT);
}
vu32 BFTM_OverCnt;
void BFTM_Configuration(void){
  //??????????????????????????????89s????????????????????????

  //??????????????????10s
  int Compare = SystemCoreClock * 10;
  //??????????????????
  BFTM_SetCompare(HT_BFTM0,Compare-1);
  //?????????????????????
  BFTM_OneShotModeCmd(HT_BFTM0, DISABLE);
  //BFTM??????
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}

void BFTM_ReStart(void){
  BFTM_OverCnt = 0;
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}
void BFTM_end(void){//???????????????89s?????????
  BFTM_EnaCmd(HT_BFTM0, DISABLE);
}
float BFTM_us(void){
  // Reload=SYSTICK          => 1s
  // Reload=SYSTICK/1000     => 1ms
  // Reload=SYSTICK/1000000  => 1us
  float count = BFTM_GetCounter(HT_BFTM0);
  return count/(SystemCoreClock/1000000) + BFTM_OverCnt*1000000*10;;
}
void delay(int cnt)
{
	int i;
	for(i=0;i<cnt;i++);
}
///---------------
int main(void){
	float Distance;
	
	CKCU_Configuration();
	GPIO_Configuration();
	RETARGET_Configuration();//printf??????
	BFTM_Configuration();//??????BFTM????????????

	GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, RESET);
	
	delay(500000);
	printf("start\n");
	

  /* Infinite loop */
  while(1)
  {
		//??????Trigger
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, SET);
		BFTM_ReStart();
		while(BFTM_us()<10);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, RESET);
		
		//??????Echo??????
		while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == RESET);
		BFTM_ReStart();
		
		//??????Echo??????
		while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET);
		BFTM_end();
		
		//????????????
		Distance = BFTM_us() / 58.0;//58 ?????????????????????????????? 1 cm ????????????????????????
		
		printf("\r\nTime: %6d,Distance: %3.2f\n",(int)BFTM_us(),Distance);
		delay(100000);
  }
//...code...//
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
