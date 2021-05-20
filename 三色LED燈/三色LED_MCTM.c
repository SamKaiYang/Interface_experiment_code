/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

/* Private variables ---------------------------------------------------------------------------------------*/
TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;
TM_OutputInitTypeDef MCTM_OutputInitStructure;
MCTM_CHBRKCTRInitTypeDef MCTM_CHBRKCTRInitStructure;

void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.MCTM0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}


void GPIO_Configuration(void)
{
	AFIO_GPxConfig(GPIO_PC, AFIO_PIN_1, AFIO_FUN_MCTM_GPTM);//C1  MT0
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_2, AFIO_FUN_MCTM_GPTM);//B2  MT2
	AFIO_GPxConfig(GPIO_PC, AFIO_PIN_14, AFIO_FUN_MCTM_GPTM);//C14 MT3

  /* Configure MCTM Break pin                                                                               */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);
}

void MCTM_Configuration(void)
{
 /* MCTM Time Base configuration                                                                            */
  MCTM_TimeBaseInitStructure.CounterReload = HTCFG_MCTM_RELOAD - 1;
  MCTM_TimeBaseInitStructure.Prescaler = 0;
  MCTM_TimeBaseInitStructure.RepetitionCounter = 0;
  MCTM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;
  MCTM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_MCTM0, &MCTM_TimeBaseInitStructure);

  /* MCTM Channel 0 output configuration                                                                    */
  MCTM_OutputInitStructure.Channel = TM_CH_0;
  MCTM_OutputInitStructure.OutputMode = TM_OM_PWM2;  //因三色LED 輸出，需用PWM2
  MCTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.ControlN = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;//TM_CHP_NONINVERTED
  MCTM_OutputInitStructure.PolarityN = TM_CHP_NONINVERTED;
  MCTM_OutputInitStructure.IdleState = MCTM_OIS_LOW;
  MCTM_OutputInitStructure.IdleStateN = MCTM_OIS_HIGH;
	
  MCTM_OutputInitStructure.Channel = TM_CH_0;
  MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * 0/255;
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);

  MCTM_OutputInitStructure.Channel = TM_CH_2;
  MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * 0/255;
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);

	
	MCTM_OutputInitStructure.Channel = TM_CH_3;
  MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * 0/255;
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
	
	
  /* MCTM Off State, lock, Break, Automatic Output enable, dead time configuration                          */
  MCTM_CHBRKCTRInitStructure.OSSRState = MCTM_OSSR_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.OSSIState = MCTM_OSSI_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.LockLevel = MCTM_LOCK_LEVEL_2;
  MCTM_CHBRKCTRInitStructure.Break0 = MCTM_BREAK_DISABLE;  ///!!!!!!!!記得要關掉
  MCTM_CHBRKCTRInitStructure.Break0Polarity = MCTM_BREAK_POLARITY_LOW;
  MCTM_CHBRKCTRInitStructure.AutomaticOutput = MCTM_CHAOE_ENABLE;
  MCTM_CHBRKCTRInitStructure.DeadTime = HTCFG_MCTM_DEAD_TIME;
  MCTM_CHBRKCTRInitStructure.BreakFilter = 0;
  MCTM_CHBRKCTRConfig(HT_MCTM0, &MCTM_CHBRKCTRInitStructure);

  /* MCTM counter enable                                                                                    */
  TM_Cmd(HT_MCTM0, ENABLE);

  /* MCTM Channel Main Output enable                                                                        */
  MCTM_CHMOECmd(HT_MCTM0, ENABLE);
}
/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  int i,cnt,R,G,B;
  CKCU_Configuration();               /* System Related configuration                                       */
  GPIO_Configuration();               /* GPIO Related configuration                                         */
  MCTM_Configuration();
	
  while (1){
    for(i=0;i<255;i++){
      MCTM_OutputInitStructure.Channel = TM_CH_0;
      MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * i / 255;
      TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
			MCTM_OutputInitStructure.Channel = TM_CH_2;
      MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * i / 255;
      TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
			MCTM_OutputInitStructure.Channel = TM_CH_3;
      MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * i / 255;
      TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
      for(cnt=0;cnt<100000;cnt++);
    }
		  for(i=0;i<255;i++){
      MCTM_OutputInitStructure.Channel = TM_CH_2;
      MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * i / 255;
      TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
      for(cnt=0;cnt<100000;cnt++);
    }
			for(i=0;i<255;i++){
      MCTM_OutputInitStructure.Channel = TM_CH_3;
      MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * i / 255;
      TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
      for(cnt=0;cnt<100000;cnt++);
    }
	
	//控制三色LED //輸入色碼值
	R = 255;
	G = 255;
	B = 255;
	MCTM_OutputInitStructure.Channel = TM_CH_0;
    MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * R / 255;
    TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
	MCTM_OutputInitStructure.Channel = TM_CH_2;
    MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * G / 255;
    TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
	MCTM_OutputInitStructure.Channel = TM_CH_3;
    MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * B / 255;
    TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
    for(cnt=0;cnt<100000;cnt++);
  }
}
