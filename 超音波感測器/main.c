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
//將CKCU的配置寫成副函式
void CKCU_Configuration(void){
  //宣告時脈閘設定用型態變數，變數名CKCUClock，且其內預設值為0
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  CKCUClock.Bit.AFIO       = 1;//設立AFIO功能旗標
  CKCUClock.Bit.PA       = 1;//設立GPIO A功能旗標
	CKCUClock.Bit.PB       = 1;
	CKCUClock.Bit.PC       = 1;
	CKCUClock.Bit.PD       = 1;
	CKCUClock.Bit.BFTM0       = 1;
  //對設立旗標的功能之時脈閘進行更動
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
  //若不設定則溢位時間約89s，溢位後計時歸零

  //單次溢位時間10s
  int Compare = SystemCoreClock * 10;
  //設定溢位時間
  BFTM_SetCompare(HT_BFTM0,Compare-1);
  //不啟用單次模式
  BFTM_OneShotModeCmd(HT_BFTM0, DISABLE);
  //BFTM開啟
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}

void BFTM_ReStart(void){
  BFTM_OverCnt = 0;
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}
void BFTM_end(void){//最長計時約89s後溢位
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
	RETARGET_Configuration();//printf設定
	BFTM_Configuration();//參考BFTM計時功能

	GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, RESET);
	
	delay(500000);
	printf("start\n");
	

  /* Infinite loop */
  while(1)
  {
		//發送Trigger
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, SET);
		BFTM_ReStart();
		while(BFTM_us()<10);
		GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_0, RESET);
		
		//等待Echo開始
		while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == RESET);
		BFTM_ReStart();
		
		//確認Echo結束
		while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET);
		BFTM_end();
		
		//計算距離
		Distance = BFTM_us() / 58.0;//58 微秒是超音波往返距離 1 cm 障礙物所花的時間
		
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
