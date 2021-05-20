/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.ADC        = 1;
  CKCUClock.Bit.PB         = 1;	
	COM1_CLK(CKCUClock)  = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_configuration(void)
{
	AFIO_GPxConfig(COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, AFIO_FUN_USART_UART);
  //LED0
  /* 設定映射到腳位上之功能                                                                     */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_GPIO);
  /* 設定腳位I/O方向                                                                */
  GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_4, GPIO_DIR_OUT);
}
void USART_configuration(void){
	USART_InitTypeDef USART_InitStructure;
	/* USART0 configuration ----------------------------------------------------------------------------------*/
  /* USART0 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - None parity bit
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
  USART_InitStructure.USART_Parity = USART_PARITY_NO;
  USART_InitStructure.USART_Mode = USART_MODE_NORMAL;

  USART_Init(COM1_PORT, &USART_InitStructure);
  USART_TxCmd(COM1_PORT, ENABLE);
  USART_RxCmd(COM1_PORT, ENABLE);
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
void delay(){
  int i;
  for(i=0; i<20000000; ++i);//20000000
}

int main(void)
{
	
  u32 Data = 0;
  u32 Data_2 = 0;
	
  CKCU_Configuration();
	GPIO_configuration();
  ADC_configuration();
	USART_configuration();
  while (1)
  {			
	//讀取ADC數值
    Data = ADC_GetConversionData(HT_ADC,ADC_REGULAR_DATA0);		
	//將得到的數值(0-4095)轉換為(0-1) 
    Data_2 = (Data)*1/4095;
		
    if(Data_2==0)
		{
			printf("Data_AO: %d\n",Data);
			printf("Data_DO: %d\n",Data_2);
			printf("** Close Fire **\n");
			printf("Fire Detected\n");
			printf("LED on\n");
			GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_4, 1);
			delay();
			GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_4, 0);
			delay();
    }
    else if(Data_2==1)
		{
			printf("Data_AO: %d\n",Data);
			printf("Data_DO: %d\n",Data_2);
			printf("No Fire\n");
			delay();
    }
	}
}
