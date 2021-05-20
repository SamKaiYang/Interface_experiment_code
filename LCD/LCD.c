/*********************************************************************************************************//**
 * @file    I2C/10_bit_mode/main.c
 * @version $Rev:: 242          $
 * @date    $Date:: 2016-02-20 #$
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
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup I2C_Examples I2C
  * @{
  */

/** @addtogroup I2C_10_bit_mode
  * @{
  */
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define I2C_1062_E 0x04  // Enable bit
#define I2C_1062_RW 0x02  // Read/Write bit
#define I2C_1062_RS 0x01  // Register select bit

#define LCD_I2C_CH 		HT_I2C1
#define LCD_MAX_ROWS	2


u8 _backlightval;
u8 _displaycontrol;
u8 _displayfunction;

#define I2C_MASTER_ADDRESS     0x60
#define I2C_SLAVE_ADDRESS      0x3F   //PCF8574A
//#define I2C_SLAVE_ADDRESS      0x27 //PCF8574
#define ClockSpeed             200000
void Delay_us(int cnt)
{
	int i;
	for(i=0;i<cnt;i++);
}
void I2C_Write(HT_I2C_TypeDef* I2Cx, u16 slave_address, u8* buffer, u8 BufferSize){
	u8 Tx_Index = 0;
	
  /* Send I2C START & I2C slave address for write                                                           */
  I2C_TargetAddressConfig(I2Cx, slave_address, I2C_MASTER_WRITE);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(I2Cx, I2C_MASTER_SEND_START));
	
  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(I2Cx, I2C_MASTER_TRANSMITTER_MODE));
	
  /* Send data                                                                                              */
  while (Tx_Index < BufferSize)
  {
    /* Check on Master Transmitter TXDE condition                                                           */
    while (!I2C_CheckStatus(I2Cx, I2C_MASTER_TX_EMPTY));
    /* Master Send I2C data                                                                                 */
    I2C_SendData(I2Cx, buffer[Tx_Index ++]);
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(I2Cx);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(I2Cx, I2C_REGISTER_SR)&0x80000);
}
void I2C_Read(HT_I2C_TypeDef* I2Cx, u16 slave_address, u8* buffer, u8 BufferSize){
	u8 Rx_Index = 0;
	
	/* Send I2C START & I2C slave address for read                                                            */
  I2C_TargetAddressConfig(I2Cx, slave_address, I2C_MASTER_READ);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(I2Cx, I2C_MASTER_SEND_START));

  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(I2Cx, I2C_MASTER_RECEIVER_MODE));

  I2C_AckCmd(I2Cx, ENABLE);
  /* Send data                                                                                              */
  while (Rx_Index < BufferSize)
  {

    /* Check on Slave Receiver RXDNE condition                                                              */
    while (!I2C_CheckStatus(I2Cx, I2C_MASTER_RX_NOT_EMPTY));
    /* Store received data on I2C1                                                                          */
    buffer[Rx_Index ++] = I2C_ReceiveData(I2Cx);
    if (Rx_Index == (BufferSize-1))
    {
      I2C_AckCmd(I2Cx, DISABLE);
    }
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(I2Cx);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(I2Cx, I2C_REGISTER_SR)&0x80000);
}
void LCD_I2C_1602_4bit_Write(u8 data){
	data |= _backlightval;
	
	I2C_Write(LCD_I2C_CH,I2C_SLAVE_ADDRESS,&data,1);

	data |= I2C_1062_E;
	I2C_Write(LCD_I2C_CH,I2C_SLAVE_ADDRESS,&data,1);
	Delay_us(1000);
	
	data &= ~I2C_1062_E;
	I2C_Write(LCD_I2C_CH,I2C_SLAVE_ADDRESS,&data,1);
	Delay_us(50000);
}

void LCD_command(u8 command){
	u8 high_4b = command & 0xF0;
	u8 low_4b = (command<<4) & 0xF0;
	
	LCD_I2C_1602_4bit_Write(high_4b);
	LCD_I2C_1602_4bit_Write(low_4b);
}

void LCD_ini(void){
	_backlightval=LCD_BACKLIGHT;

	_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	Delay_us(200000);

	LCD_I2C_1602_4bit_Write(0x30);
	Delay_us(500000);
	LCD_I2C_1602_4bit_Write(0x30);
	Delay_us(200000);
	LCD_I2C_1602_4bit_Write(0x30);
	Delay_us(200000);
	//printf("33");
	LCD_I2C_1602_4bit_Write(LCD_FUNCTIONSET | LCD_4BITMODE);
	
	LCD_command(LCD_FUNCTIONSET | _displayfunction); 
	
	_displaycontrol = LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
	
	LCD_command(LCD_CLEARDISPLAY);
	Delay_us(200000);
	
	LCD_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
	
	LCD_command(LCD_RETURNHOME);
	Delay_us(200000);
	
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_Backlight(u8 enable){
	u8 data = 0;
	if(enable)	_backlightval=LCD_BACKLIGHT;
	else				_backlightval=LCD_NOBACKLIGHT;
	data = _backlightval;
	I2C_Write(LCD_I2C_CH,I2C_SLAVE_ADDRESS,&data,1);
}

//也需設定CKCU、AFIO
void I2C_Configuration(/*HT_I2C_TypeDef* I2Cx*/){
  /* I2C Master configuration                                                                               */
  I2C_InitTypeDef  I2C_InitStructure;

  I2C_InitStructure.I2C_GeneralCall = DISABLE;
  I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;//I2C_ADDRESSING_7BIT 
  I2C_InitStructure.I2C_Acknowledge = DISABLE;
  I2C_InitStructure.I2C_OwnAddress = I2C_MASTER_ADDRESS;
  I2C_InitStructure.I2C_Speed = ClockSpeed;
  I2C_Init(HT_I2C1, &I2C_InitStructure);

	
	I2C_InitStructure.I2C_Acknowledge = DISABLE;
  I2C_InitStructure.I2C_OwnAddress = I2C_SLAVE_ADDRESS;
  //I2C_InitStructure.I2C_Speed = ClockSpeed;
  I2C_Init(HT_I2C0, &I2C_InitStructure);
  /* Enable I2C                                                                                             */
  I2C_Cmd(HT_I2C0 , ENABLE);
	I2C_Cmd(HT_I2C1 , ENABLE);
}

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
void LCD_setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > LCD_MAX_ROWS) {
		row = LCD_MAX_ROWS-1;    // we count rows starting w/0
	}
	LCD_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_Write(u8 Data){
	u8 high_4b = (Data & 0xF0) | I2C_1062_RS;
	u8 low_4b = ((Data<<4) & 0xF0) | I2C_1062_RS;
	
	LCD_I2C_1602_4bit_Write(high_4b);
	LCD_I2C_1602_4bit_Write(low_4b);
}
void CKCU_Configuration(void){
  //宣告時脈閘設定用型態變數，變數名CKCUClock，且其內預設值為0
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  CKCUClock.Bit.AFIO       = 1;//設立AFIO功能旗標
  CKCUClock.Bit.PA       = 1;//設立GPIO A功能旗標
	//CKCUClock.Bit.PB       = 1;
	CKCUClock.Bit.PC       = 1;
	//CKCUClock.Bit.PD       = 1;
	CKCUClock.Bit.I2C0       = 1;
	CKCUClock.Bit.I2C1       = 1;
  //對設立旗標的功能之時脈閘進行更動
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void GPIO_Configuration(){
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_0 | 
                          AFIO_PIN_1 , AFIO_FUN_I2C);
	//GPIO_PIN_0:I2C1_SCL
	//GPIO_PIN_1:I2C1_SDK
}
int main(void)
{
	u8 i,n;
	u8 rlt;
	CKCU_Configuration();
	GPIO_Configuration();
	I2C_Configuration();
	RETARGET_Configuration();
	LCD_ini();
  while (1){
		LCD_setCursor(0,0);//設定游標座標
		//for(i=0;i<4;i++,n++){
			LCD_Write('a');//設定顯示字符
			LCD_Write('b');
			LCD_Write('c');
			LCD_Write('d');
		  LCD_Write('A');
			LCD_Write('B');
			LCD_Write('C');
			LCD_Write('D');
		//}
		LCD_setCursor(0,1);//設定游標座標
		for(i=0;i<10;i++){
			LCD_Write('0'+i);//設定顯示字符
		}		
		Delay_us(10000000);
	}

}

/*********************************************************************************************************//**
  * @brief  Compare two buffers.
  * @param  Buffer1, Buffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval None
  ***********************************************************************************************************/
//TestResult CmpBuffer(u8* Buffer1, u8* Buffer2, u32 BufferLength)
//{
//  while (BufferLength--)
//  {
//    if (*Buffer1 != *Buffer2)
//    {
//      return Fail;
//    }

//    Buffer1++;
//    Buffer2++;
//  }

//  return Pass;
//}

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
