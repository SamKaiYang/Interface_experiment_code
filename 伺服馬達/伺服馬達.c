/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;
TM_OutputInitTypeDef MCTM_OutputInitStructure;
MCTM_CHBRKCTRInitTypeDef MCTM_CHBRKCTRInitStructure;

void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.PC         = 1;
	CKCUClock.Bit.MCTM0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration(void)
{
  /* Configure MCTM Channel 0 output pin                                                                    */
  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_1, AFIO_FUN_MCTM_GPTM);
	
//	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);
}

int Period; 

//將MCTM計數器的配置寫成副函式
void MCTM_Time_Configuration(float Frequency){
  int DIV;
  TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;

  //設每(SystemCoreClock / 0x10000)個Clock一個週期
  //即當除頻值為0x10000時，週期為 1 秒=>頻率為 1 Hz
  //即當除頻值為0x10000/n時，週期為 1/n 秒=>頻率為 n Hz
  Period = (SystemCoreClock / 0x10000);
  MCTM_TimeBaseInitStructure.CounterReload = Period - 1;

  //除頻器直設為(0x10000/Frequency)
  DIV = (0x10000/Frequency);
  MCTM_TimeBaseInitStructure.Prescaler = DIV - 1;

  MCTM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;
  MCTM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_UPDATE;
  TM_TimeBaseInit(HT_MCTM0, &MCTM_TimeBaseInitStructure);
}
//將MCTM輸出的配置寫成副函式
void MCTM_Output_Configuration(float Percent){
  TM_OutputInitTypeDef MCTM_OutputInitStructure;
  u16 CH0CCR = Period * (Percent/100);

  //標記設定哪一個通道
  MCTM_OutputInitStructure.Channel = TM_CH_0;
  //設定輸出模式
  MCTM_OutputInitStructure.OutputMode = TM_OM_PWM1;
  //是否開啟該通道輸出控制
  MCTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;
  //輸出是否需要反相
  MCTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;
  //設定 [比較值(正常模式)] / [上數比較值(非對稱模式)]
  MCTM_OutputInitStructure.Compare = CH0CCR ;
  //設定下數比較值(非對稱模式)
  MCTM_OutputInitStructure.AsymmetricCompare = 0;
  //依上述參數進行設定
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
}
void delay(int cnt)
{
	int i;
	for(i=0;i<cnt;i++);
}
float AngletoDuty(float angle)
{
	int PWM_FREQ = 50;
	float duty_cycle;
	//當頻率為PWM_FREQ且角度為x時的工作週期(%)
	duty_cycle = (0.05 * PWM_FREQ) + (0.19 * PWM_FREQ * angle / 180);
	return duty_cycle;
	
}

int main(void){
  float i;
  float start_angle;
  float end_angle;
  CKCU_Configuration();
  GPIO_Configuration();
	 
  MCTM_Time_Configuration(50);//轉動頻率50Hz
  MCTM_Output_Configuration(0);

  TM_Cmd(HT_MCTM0, ENABLE);

  MCTM_CHMOECmd(HT_MCTM0, ENABLE);
	
	while (1){
//	MCTM_Output_Configuration(AngletoDuty(0));
//	for(cnt=0;cnt<10000000;cnt++);
//		
//	MCTM_Output_Configuration(AngletoDuty(180));
//	for(cnt=0;cnt<10000000;cnt++);
//	
//	MCTM_Output_Configuration(AngletoDuty(0));
//	for(cnt=0;cnt<10000000;cnt++);
//		
//	MCTM_Output_Configuration(AngletoDuty(90));
//	for(cnt=0;cnt<10000000;cnt++);
//		
//	MCTM_Output_Configuration(AngletoDuty(0));
//	for(cnt=0;cnt<10000000;cnt++);
		
		
		start_angle = 0;
		end_angle = 181;
		for( i = start_angle; i < end_angle; i += 15 )
		{
				MCTM_Output_Configuration(AngletoDuty(i));
				delay(500000);
		
		}
		delay(10000000);
		start_angle = 180;
		end_angle = -1;
		for( i = start_angle; i > end_angle; i -= 15)
		{
				MCTM_Output_Configuration(AngletoDuty(i));
				delay(500000);
		}
		delay(10000000);
		
	}
}
