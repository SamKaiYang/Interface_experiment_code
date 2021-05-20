/*********************************************************************************************************//**
 * @file    MCTM/ComplementaryOutput/main.c
 * @version $Rev:: 2320         $
 * @date    $Date:: 2017-12-11 #$
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
#include "ht32_board_config.h"

/* Private function prototypes -----------------------------------------------------------------------------*/
void CKCU_Configuration(void);
void GPIO_Configuration(void);

/* Private variables ---------------------------------------------------------------------------------------*/
TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;
TM_OutputInitTypeDef MCTM_OutputInitStructure;
MCTM_CHBRKCTRInitTypeDef MCTM_CHBRKCTRInitStructure;

/* Global functions ----------------------------------------------------------------------------------------*/

void AFIO_Configuration(void){
  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_4 | GPIO_PIN_5| GPIO_PIN_8, AFIO_FUN_MCTM_GPTM);
}

u16 CRR = 255;

//GPTM
void GPTM_Time_Configuration(void){
  
  TM_TimeBaseInitTypeDef GPTM_TimeBaseInitStructure;
  
  GPTM_TimeBaseInitStructure.CounterReload = CRR - 1;

  GPTM_TimeBaseInitStructure.Prescaler = 1 - 1;

  GPTM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;

  GPTM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;

  TM_TimeBaseInit(HT_GPTM0, &GPTM_TimeBaseInitStructure);
}

//GPTM Outpute Configuration
void GPTM_Output_Configuration(void){

TM_OutputInitTypeDef GPTM_OutputInitStructure;

  u16 CH0CCR = CRR * 0.75;
  u16 CH1CCR = CRR * 0.5;
  u16 CH2CCR = CRR * 0.25;

  GPTM_OutputInitStructure.Channel = TM_CH_0;

  GPTM_OutputInitStructure.OutputMode = TM_OM_PWM2;//因三色LED 輸出，需用PWM2

  GPTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;

  GPTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;

  GPTM_OutputInitStructure.Compare = CH0CCR - 1;

  GPTM_OutputInitStructure.AsymmetricCompare = 0;

  TM_OutputInit(HT_GPTM0, &GPTM_OutputInitStructure);

  GPTM_OutputInitStructure.Channel = TM_CH_1;

  GPTM_OutputInitStructure.Compare = CH1CCR - 1;

  GPTM_OutputInitStructure.AsymmetricCompare = 0;

  TM_OutputInit(HT_GPTM0, &GPTM_OutputInitStructure);

  GPTM_OutputInitStructure.Channel = TM_CH_2;

  GPTM_OutputInitStructure.Compare = CH2CCR - 1;

  GPTM_OutputInitStructure.AsymmetricCompare = 0;

  TM_OutputInit(HT_GPTM0, &GPTM_OutputInitStructure);

  TM_Cmd(HT_GPTM0, ENABLE);
}

void GPTM_PWM1_Output(HT_TM_TypeDef* HT_GPTMx, TM_CH_Enum TM_CH_x, u16 Compare){
  TM_OutputInitTypeDef GPTM_OutputInitStructure;
  GPTM_OutputInitStructure.Channel = TM_CH_x;
  GPTM_OutputInitStructure.OutputMode = TM_OM_PWM2;//因三色LED 輸出，需用PWM2
  GPTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;
  GPTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;
  GPTM_OutputInitStructure.Compare = Compare;
  TM_OutputInit(HT_GPTMx, &GPTM_OutputInitStructure);
}

static void _delay(u32 nCount)
{
  vu32 i;
  for (i = 0; i < 10000 * nCount; i++);
}


/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/

int main(void){
  int i;
  CKCU_Configuration();
  AFIO_Configuration();
  GPTM_Time_Configuration();
  GPTM_Output_Configuration();


	while(1){
  //CH0 PWM
		for(i=0;i<=100;i++){
			GPTM_PWM1_Output(HT_GPTM0,TM_CH_0,(i+0)%101);
			GPTM_PWM1_Output(HT_GPTM0,TM_CH_1,(i+33)%101);
			GPTM_PWM1_Output(HT_GPTM0,TM_CH_2,(i+66)%101);
			_delay(10);
		}
		
		//控制三色LED //輸入色碼值
		R = 255;
		G = 255;
		B = 255;
		GPTM_PWM1_Output(HT_GPTM0,TM_CH_0,CRR * R / 255);
		GPTM_PWM1_Output(HT_GPTM0,TM_CH_1,CRR * G / 255);
		GPTM_PWM1_Output(HT_GPTM0,TM_CH_2,CRR * B / 255);
		_delay(10);
	}
}



/*********************************************************************************************************//**
  * @brief  Configure the system clocks.
  * @retval None
  ***********************************************************************************************************/


void CKCU_Configuration(void)
{
/*
//<e0> Enable Peripheral Clock
//  <h> Communication
//    <q5> EBI
//    <q11> I2C0   <q12> I2C1
//    <q23> I2S
//    <q21> SCI0 <q22> SCI1
//    <q13> SPI0   <q14> SPI1
//    <q17> UART0  <q18> UART1
//    <q15> USART0 <q16> USART1
//    <q3>  USB
//  </h>
//  <h> IO
//    <q7> GPIO Port A <q8>  GPIO Port B <q9>  GPIO Port C <q10>  GPIO Port D
//    <q19> AFIO
//    <q20> EXTI
//  </h>
//  <h> System
//    <q32> ADC
//    <q4>  CKREF
//    <q6>  CRC
//    <q31> CMP
//    <q2>  PDMA
//    <q26> PWRCU
//  </h>
//  <h> Timer
//    <q29> BFTM0 <q30> BFTM1
//    <q33> SCTM0 <q34> SCTM1 <q35> SCTM2 <q36> SCTM3
//    <q27> GPTM0 <q28> GPTM1
//    <q24> MCTM0
//    <q26> RTC   <q25> WDT
//  </h>
//</e>
*/
#if 1
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.PDMA       = 0;
  CKCUClock.Bit.USBD       = 0;
  CKCUClock.Bit.CKREF      = 0;
  CKCUClock.Bit.EBI        = 0;
  CKCUClock.Bit.CRC        = 0;
  CKCUClock.Bit.PA         = 0;
  CKCUClock.Bit.PB         = 0;
  CKCUClock.Bit.PC         = 0;
  CKCUClock.Bit.PD         = 0;
  CKCUClock.Bit.I2C0       = 0;
  CKCUClock.Bit.I2C1       = 0;
  CKCUClock.Bit.SPI0       = 0;
  CKCUClock.Bit.SPI1       = 0;
  CKCUClock.Bit.USART0     = 0;
  CKCUClock.Bit.USART1     = 0;
  CKCUClock.Bit.UART0      = 0;
  CKCUClock.Bit.UART1      = 0;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.EXTI       = 0;
  CKCUClock.Bit.SCI0       = 0;
  CKCUClock.Bit.SCI1       = 0;
  CKCUClock.Bit.I2S        = 0;
  CKCUClock.Bit.MCTM0      = 0;
  CKCUClock.Bit.WDT        = 0;
  CKCUClock.Bit.BKP        = 0;
  CKCUClock.Bit.GPTM0      = 1;
  CKCUClock.Bit.GPTM1      = 0;
  CKCUClock.Bit.BFTM0      = 0;
  CKCUClock.Bit.BFTM1      = 0;
  CKCUClock.Bit.CMP        = 0;
  CKCUClock.Bit.ADC        = 0;
  CKCUClock.Bit.SCTM0      = 0;
  CKCUClock.Bit.SCTM1      = 0;
  CKCUClock.Bit.SCTM2      = 0;
  CKCUClock.Bit.SCTM3      = 0;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports.
  * @retval None
  ***********************************************************************************************************/


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
