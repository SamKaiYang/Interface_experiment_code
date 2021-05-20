/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"


void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PA = 1;
	CKCUClock.Bit.PB = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration(void)
{                                                   
  /* 設定映射到腳位上之功能                                                                     */
//  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_0 | AFIO_PIN_1, AFIO_FUN_GPIO);
//	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_0 | AFIO_PIN_1, AFIO_FUN_GPIO);
//  /* 設定腳位I/O方向                                                                */
//  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_OUT);
//	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_OUT);
}
//------main.c------//

//---Delay功能---//
static vu32 DelayTime;
void Delay(u32 nTime)
{
  /* Enable the SysTick Counter */
  SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

  DelayTime = nTime;

  while(DelayTime != 0);

  /* Disable SysTick Counter */
  SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
  /* Clear SysTick Counter */
  SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
}

void TimingDelay(void)
{
  if(DelayTime != 0)
  {
    DelayTime--;
  }
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

//---DHT11---//
char Data[5];

char read_data()
{
  char data = 0;
  char i;

  //需讀取8bit
  for(i=0; i<8; i++)
  {
    //等待位元起始訊號結束
    while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == RESET);

    //等待1/0數據通用時間
    Delay(33);
    //判斷高位超時，超時為1，已回歸低位(下一bit起始訊號)為0
    if(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET)
      data |= (1<<(7-i)); //高位在前，低位在后
    
    //若數數据為1，需等待其回歸低位(下一bit起始訊號)
    while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET);
  }
  return data;
}
void start_read_DHT11()
{
  u8 i;

  //開始訊號
  //延遲要大于18ms，以便 DHT11 能讀到開始訊號
  GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1, RESET);
  Delay(18000);
  GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1, SET);
  Delay(30);
  
  //切換為接收  GPIO output=>input
  //其餘設定請於GPIO_Configuration()內設定
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_1, GPIO_DIR_IN);
	
  //等待確認DHT11響應訊號
  while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET);
  Delay(40);
  
  //等待確認DHT11響應訊號歸位
  while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == RESET);
  Delay(40);

  //確認開始傳送資料
  while(GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_1) == SET);
  for(i=0;i<5;i++){
    //讀取封包
    //數據格式： 8bit濕度整數位+8bit濕度小數位
    //        +8bit溫度整數位+8bit溫度小數位
    //        +8bit校驗和
    Data[i] = read_data();
  }
  
  //接收完畢切換為傳送 GPIO input=>output
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_1, GPIO_DIR_OUT);
  //無訊號時常態高位
  GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1, SET);
}

int main(void){

  CKCU_Configuration();
  SYSTICK_Configuration();
  //AFIO_Configuration();
  //GPIO_Configuration();

  //無訊號時常態高位
  GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_1, SET);

  //啟動前需等待感測器啟動，說明書寫1s
  Delay(2000000); 

  while(1){
      //說明書寫讀取採樣週期1s
      start_read_DHT11();
      Delay(1000000);
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
