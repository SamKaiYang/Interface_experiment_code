/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.ADC        = 1;
	CKCUClock.Bit.PA         = 1;
  CKCUClock.Bit.PC         = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}

void GPIO_configuration(void)
{
  //LED0
  AFIO_GPxConfig(GPIO_PC,AFIO_PIN_14,AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_14, GPIO_DIR_OUT);
  //LED1
  AFIO_GPxConfig(GPIO_PC,AFIO_PIN_15,AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_15, GPIO_DIR_OUT);
  //LED2
  AFIO_GPxConfig(GPIO_PC,AFIO_PIN_1,AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_1, GPIO_DIR_OUT);
}
void ADC_configuration(void)
{
  /* Config AFIO mode as ADC function                                                                       */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_6, AFIO_FUN_ADC);

  /* Continuous Mode, Length 1, SubLength 1                                                                 */
  ADC_RegularGroupConfig(HT_ADC, CONTINUOUS_MODE, 1, 1);

  /* ADC Channel n, Rank 0, Sampling clock is (1.5 + 0) ADCLK
  Conversion time = (sampling clock + 12.5) / ADCLK = 12.4 uS */
  ADC_RegularChannelConfig(HT_ADC, ADC_CH_6, 0);

  /* Use Software Trigger as ADC trigger source                                                             */
  ADC_RegularTrigConfig(HT_ADC, ADC_TRIG_SOFTWARE);

  ADC_Cmd(HT_ADC, ENABLE);

  /* Software trigger to start continuous mode                                                              */
  ADC_SoftwareStartConvCmd(HT_ADC, ENABLE);
}


u32 ADC_value;
float ADC_ratio;
float Voltage_mV;
float Temperature;

int main(void)
{
  u32 Data = 0;
  u32 Data_10 = 0;
	//宣告可輸出printf內容到電腦端
  RETARGET_Configuration();  
  CKCU_Configuration();
  GPIO_configuration();
  ADC_configuration();
  while (1)
  {
	      //取得順位0的ADC數值
      ADC_value = ADC_GetConversionData( HT_ADC, 0);
      //將12bits(0~4095)的ADC數值轉為電壓比值
      ADC_ratio = ADC_value/4095.0;

      //將電壓比值轉為電壓
      //參考電壓：晶片電壓3.3V=3300mv
      Voltage_mV = ADC_ratio * 3300;

      //...code...//
			printf("\r\nVoltage_mV: %f",Voltage_mV);
  }
}
