
/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include <stdio.h>
#include <string.h> //字串分割
#include <stdlib.h> //atoi

/* Private function prototypes -----------------------------------------------------------------------------*/
void RTC_Configuration(void);
u32 Time_Regulate(void);
void Time_Display(u32 wTimeVar);
u32 ScanUnsignedDecimal(void);

char read_Buffer[256];
void USART_Tx(const u8 TxBuffer, u32 length);
#define delay_count 10000000
/* Private variables ---------------------------------------------------------------------------------------*/
vu32 gwTimeDisplay = 0;
u32 TimeReg = 0;
bool USART_flag = FALSE;
vu32 count = 0;

void USART_Configuration(){
  USART_InitTypeDef USART_InitStructure;

  /* Config AFIO mode as USART1_Rx and USART1_Tx function.                                                  */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_USART_UART);		//USART1_Tx
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_USART_UART);		//USART1_Rx

  /* USART0 configuration ----------------------------------------------------------------------------------*/
  USART_InitStructure.USART_BaudRate = 115200;                      //設定鮑率為    115200
  USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;       //設定字長      8Bits
  USART_InitStructure.USART_StopBits = USART_STOPBITS_1;            //設定停止位    1bit
  USART_InitStructure.USART_Parity = USART_PARITY_NO;               //設定校驗位   no
  USART_InitStructure.USART_Mode = USART_MODE_NORMAL;               //設定模式     正常
  USART_Init(HT_USART0, &USART_InitStructure);                      //照上述設定值進行初始化
  /* Seting Rx FIFO Level                                                                                   */
  USART_IntConfig(HT_USART0, USART_INT_RXDR, ENABLE);
  USART_IntConfig(HT_USART0, USART_INT_TOUT, ENABLE);
  USART_RXTLConfig(HT_USART0, USART_RXTL_01);
  USART_TxCmd(HT_USART0, ENABLE);                                   //Tx Enable
  USART_RxCmd(HT_USART0, ENABLE);                                   //Rx Enable
}


void RTC_Configuration(void)
{
  /* Reset Backup Domain                                                                                    */
  PWRCU_DeInit();

  /* Configure Low Speed External clock (LSE)                                                               */
  RTC_LSESMConfig(RTC_LSESM_FAST);
  RTC_LSECmd(ENABLE);
  while (CKCU_GetClockReadyStatus(CKCU_FLAG_LSERDY) == RESET);

  /* Configure the RTC                                                                                      */
  RTC_ClockSourceConfig(RTC_SRC_LSE);
  RTC_IntConfig(RTC_INT_CSEC, ENABLE);
  RTC_SetPrescaler(RTC_RPRE_32768);

  /* Restart counter when match event occurred                                                              */
  RTC_CMPCLRCmd(ENABLE);
}
void split(char *src,const char *separator,char **dest,int *num) {
	/*
		src 源字串的首地址(buf的地址) 
		separator 指定的分割字元
		dest 接收子字串的陣列
		num 分割後子字串的個數
	*/
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //如果傳入的地址為空或長度為0，直接終止 
        return;
     if (separator == NULL || strlen(separator) == 0) //如未指定分割的字串，直接終止 
        return;
     pNext = (char *)strtok(src,separator); //必須使用(char *)進行強制型別轉換(雖然不寫有的編譯器中不會出現指標錯誤)
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator);  //必須使用(char *)進行強制型別轉換
    }  
    *num = count;
} 	
//char *delim = ":";
//char * pch;
char *revbuf[8] = {0};

u32 Time_Regulate(void)
{
  int Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;
	int num = 0;
	
  printf("\r\n==============Time Settings=====================================");
	printf("\r\nread_Buffer: %s",read_Buffer);
	split(read_Buffer,":",revbuf,&num);

	Tmp_HH = atoi(revbuf[0]);	
	Tmp_MM = atoi(revbuf[1]);
	Tmp_SS = atoi(revbuf[2]);

  return((Tmp_HH * 3600 + Tmp_MM * 60 + Tmp_SS));
}
void USART_Tx(const u8 TxBuffer, u32 length)
{
  int i;

  USART_SendData(HT_USART1, TxBuffer);

  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
  for(i=0;i<delay_count;i++);
}
void Time_Display(u32 wTimeVar)
{
  u32 THH = 0, TMM = 0, TSS = 0;

  /* Compute  hours                                                                                         */
  THH = wTimeVar/3600;
  /* Compute minutes                                                                                        */
  TMM = (wTimeVar % 3600)/60;
  /* Compute seconds                                                                                        */
  TSS = (wTimeVar % 3600)% 60;

	USART_Tx(THH, sizeof(THH));
  printf("\r\nTime: %02u:%02u:%02u\r", (unsigned int)THH, (unsigned int)TMM, (unsigned int)TSS);
}

u32 ScanUnsignedDecimal(void)
{
  u8 bChar = 0;
  u32 wNbr = 0;
  while (1)
  {
    bChar = getchar();
    if ((bChar >= '0') && (bChar <= '9'))
    {
      wNbr = (wNbr * 10) + (bChar - '0');
    }
    else
    {
      break;
    }

  }
  return wNbr;
}
void CKCU_Configuration(){
  /* Enable peripheral clock of AFIO, USART0                                                                */
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.BKP         = 1;
  CKCUClock.Bit.AFIO        = 1;
  CKCUClock.Bit.PA          = 1;
  CKCUClock.Bit.PC          = 1;
	CKCUClock.Bit.USART0      = 1;
  CKCUClock.Bit.USART1      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
int main(void)
{                                      
  CKCU_Configuration();
  #if (!LIBCFG_NO_PWRCU_TEST_REG)
    if (PWRCU_CheckReadyAccessed() != PWRCU_OK)
    {
      while (1);
    }
  #endif
	USART_Configuration();
  RETARGET_Configuration();           /* Retarget Related configuration                                     */
	/* Configure USARTx interrupt                                                                             */
  NVIC_EnableIRQ(USART0_IRQn);
  /* Enable NVIC RTC interrupt                                                                              */
  NVIC_EnableIRQ(RTC_IRQn);
  
  /* RTC Configuration                                                                                      */
  RTC_Configuration();
  USART_Configuration();
  printf("\r\n RTC configured....");
	printf("\r\n Input Format:");
	printf("\r\n XX:XX:XX");

  /* Display current time in infinite loop                                                                  */
  printf("\r\n");

  while (1)
  {
		if (USART_flag==TRUE)
		{
			TimeReg = Time_Regulate();
			USART_flag=FALSE;
			gwTimeDisplay = 1;
			// Enable RTC   
			RTC_Cmd(ENABLE);
		}
		if (gwTimeDisplay == 1)
		{
			Time_Display(RTC_GetCounter() + TimeReg);
		}
  }
}


#if (HT32_LIB_DEBUG == 1)

void assert_error(u8* filename, u32 uline)
{
  while (1)
  {
  }
}
#endif

