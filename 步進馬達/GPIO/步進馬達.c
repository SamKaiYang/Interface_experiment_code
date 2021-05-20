/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"


void CKCU_configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PA = 1;
	CKCUClock.Bit.PB = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_configuration(void)
{                                                   
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_0 | AFIO_PIN_1, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_0 | AFIO_PIN_1, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_OUT);
}
void foward(float angle,int Speed) //Speed range 0~100
{
	int i,j;
	float loop_times;
	//circle 1 => 512 Loop
	//circle 1 => 360 degree
	loop_times = angle*512/360;
	Speed = 1000000/Speed;
	for(i=1;i<=loop_times;i++)
	{
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
	}
}
void backward(float angle,int Speed) //Speed range 0~100
{
	int i,j;
	float loop_times;
	//circle 1 => 512 Loop
	//circle 1 => 360 degree
	loop_times = angle*512/360;
	Speed = 1000000/Speed;
	for(i=1;i<=loop_times;i++)
	{
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,0);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,1);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_0,0);
      GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_1,1);
      for(j=0; j<Speed; ++j);
	}
}
int i = 1;
int main(void)
{
	/*CKCU SET                */
  CKCU_configuration();
	GPIO_configuration();
	foward(360,100);
	backward(360,100);
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
