#include "MFRC522.h"
#include "MFRC522_conf.h"
u8 serNum[5];
u8 writeDate[16] ={'T', 'e', 'n', 'g', ' ', 'B', 'o', 0, 0, 0, 0, 0, 0, 0, 0,0};
u8 sectorKeyA[16][16] = {	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
u8 sectorNewKeyA[16][16] = {	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};

	u8 str[MAX_LEN];
//extern void Delay(u32 nTime);
	
u8 Get_Card_Number(void){
	u8 status;
	status = MFRC522_Request(PICC_REQIDL, str);
	status = MFRC522_Anticoll(str);
	memcpy(serNum, str, 5);
	if (status == MI_OK)	return 0;
	else 									return 1;
}

u16 SPI_SendByte(u8 byte)
{
  /* Loop while Tx buffer register is empty                                                                 */
  while (!SPI_GetFlagStatus(MFRC522_SPI, SPI_FLAG_TXBE));

  /* Send byte through the SPIx peripheral                                                                  */
  SPI_SendData(MFRC522_SPI, byte);

  /* Loop while Rx is not empty                                                                             */
  while (!SPI_GetFlagStatus(MFRC522_SPI, SPI_FLAG_RXBNE));

  /* Return the byte read from the SPI                                                                      */
  return SPI_ReceiveData(MFRC522_SPI);
}
void Write_MFRC522(u8 addr, u8 val)
{
	GPIO_WriteOutBits(MFRC522_SEL_ID, MFRC522_SEL_PIN, RESET);
	//Delay(1);
	SPI_SendByte((addr<<1)&0x7E);
	SPI_SendByte(val);
	GPIO_WriteOutBits(MFRC522_SEL_ID, MFRC522_SEL_PIN, SET);
}
u8 Read_MFRC522(u8 addr)
{
	u8 val;
	//地址格式：1XXXXXX0
	GPIO_WriteOutBits(MFRC522_SEL_ID, MFRC522_SEL_PIN, RESET);
	//Delay(1);
	SPI_SendByte(((addr<<1)&0x7E) | 0x80);
	val = SPI_SendByte(0x00);
	GPIO_WriteOutBits(MFRC522_SEL_ID, MFRC522_SEL_PIN, SET);
	return val;
}
void SetBitMask(u8 reg, u8 mask)
{
	u8 tmp;
	tmp = Read_MFRC522(reg);
	Write_MFRC522(reg, tmp | mask); // set bit mask
}
void ClearBitMask(u8 reg, u8 mask)
{
	u8 tmp;
	tmp = Read_MFRC522(reg);
	Write_MFRC522(reg, tmp & (~mask)); // clear bit mask
} 

void AntennaOn(void)
{
	u8 temp;
	temp = Read_MFRC522(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}
void AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}
void MFRC522_Reset(void)
{
	Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

void MFRC522_HW_Reset(FlagStatus NewState){
	GPIO_WriteOutBits(MFRC522_Reset_ID, MFRC522_Reset_PIN, NewState);
}
void MFRC522_Init(void)
{
	MFRC522_HW_Reset(RESET);
	MFRC522_HW_Reset(SET);
	MFRC522_Reset();
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	Write_MFRC522(TModeReg, 0x8D); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	Write_MFRC522(TPrescalerReg, 0x3E); //TModeReg[3..0] + TPrescalerReg
	Write_MFRC522(TReloadRegL, 30);
	Write_MFRC522(TReloadRegH, 0);
	Write_MFRC522(TxAutoReg, 0x40); //100%ASK
	Write_MFRC522(ModeReg, 0x3D); //CRC 初始值 0x6363 ???
	AntennaOn(); //打?天?
}
u8 MFRC522_Request(u8 reqMode, u8 *TagType)
{
	u8 status;
	u32 backBits = 0; //接收到的?据位?
	Write_MFRC522(BitFramingReg, 0x07); //TxLastBists = BitFramingReg[2..0] ???
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	if ((status != MI_OK) || (backBits != 0x10))
	{
		status = MI_ERR;
	}

	return status;
}
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u32 *backLen)
{
	u8 status = MI_ERR;
	u8 irqEn = 0x00;
	u8 waitIRq = 0x00;
	u8 lastBits;
	u8 n;
	u32 i;
	u8 rlt;
	switch (command)
	{
	case PCD_AUTHENT: //??卡密
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
	case PCD_TRANSCEIVE: //?送 FIFO 中?据
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
	default:
		break;
	}
	Write_MFRC522(CommIEnReg, irqEn|0x80); //允?中??求
	ClearBitMask(CommIrqReg, 0x80); //清除所有中??求位
	SetBitMask(FIFOLevelReg, 0x80); //FlushBuffer=1, FIFO 初始化

	Write_MFRC522(CommandReg, PCD_IDLE); //NO action;取消?前命令 ???
	//向 FIFO 中?入?据
	for (i=0; i<sendLen; i++)
	{
		Write_MFRC522(FIFODataReg, sendData[i]);
	}
	//?行命令
	Write_MFRC522(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	{
		SetBitMask(BitFramingReg, 0x80); //StartSend=1,transmission of data starts
	} 
	//等待接收?据完成
	i = 2000; //i 根据???率?整，操作 M1 卡最大等待?? 25ms ???
	do
	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = Read_MFRC522(CommIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitIRq));
	ClearBitMask(BitFramingReg, 0x80); //StartSend=0
	if (i != 0)
	{
		rlt = Read_MFRC522(ErrorReg);
		if(!(rlt & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR; //??
			}
			if (command == PCD_TRANSCEIVE)
			{
				n = Read_MFRC522(FIFOLevelReg);
				lastBits = Read_MFRC522(ControlReg) & 0x07;
				if (lastBits)
				{
					*backLen = (n-1)*8 + lastBits;
				}
				else
				{
					*backLen = n*8;
				}
				if (n == 0)
				{
					n = 1;
				}
				if (n > MAX_LEN)
				{
					n = MAX_LEN;
				}
				//?取 FIFO 中接收到的?据
				for (i=0; i<n; i++)
				{
					backData[i] = Read_MFRC522(FIFODataReg);
				}
			}
		}
		else
		{
			status = MI_ERR;
		}
	}
	//SetBitMask(ControlReg,0x80); //timer stops
	//Write_MFRC522(CommandReg, PCD_IDLE);
	return status;
}
u8 MFRC522_Anticoll(u8 *serNum)
{
	u8 status;
	u8 i;
	u8 serNumCheck=0;
	u32 unLen;

	Write_MFRC522(BitFramingReg, 0x00); //TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	if (status == MI_OK)
	{
		//校?卡序列?
		for (i=0; i<4; i++)
		{
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			status = MI_ERR;
		}
	}
	//SetBitMask(CollReg, 0x80); //ValuesAfterColl=1
	return status;
}
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData)
{
	u8 i, n;
	ClearBitMask(DivIrqReg, 0x04); //CRCIrq = 0
	SetBitMask(FIFOLevelReg, 0x80); //清 FIFO 指?
	//Write_MFRC522(CommandReg, PCD_IDLE);
	//向 FIFO 中?入?据
	for (i=0; i<len; i++)
	{
		Write_MFRC522(FIFODataReg, *(pIndata+i)); 
	}
	Write_MFRC522(CommandReg, PCD_CALCCRC);
	//等待 CRC ?算完成
	i = 0xFF;
	do
	{
		n = Read_MFRC522(DivIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x04)); //CRCIrq = 1
	//?取 CRC ?算?果
	pOutData[0] = Read_MFRC522(CRCResultRegL);
	pOutData[1] = Read_MFRC522(CRCResultRegM);
}
u8 MFRC522_SelectTag(u8 *serNum)
{
	u8 i;
	u8 status;
	u8 size;
	u32 recvBits;
	u8 buffer[9];
	//ClearBitMask(Status2Reg, 0x08); //MFCrypto1On=0
	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i=0; i<5; i++)
	{
		buffer[i+2] = *(serNum+i);
	}
	CalulateCRC(buffer, 7, &buffer[7]); //??
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}
	else
	{ 
		size = 0;
	}
	return size;
}
u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum)
{
	u8 status;
	u32 recvBits;
	u8 i;
	u8 buff[12];
	//??指令+?地址＋扇?密?＋卡序列?
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i=0; i<6; i++)
	{
		buff[i+2] = *(Sectorkey+i);
	}
	for (i=0; i<4; i++)
	{
		buff[i+8] = *(serNum+i);
	}
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}
u8 MFRC522_Read(u8 blockAddr, u8 *recvData)
{
	u8 status;
	u32 unLen;
	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	CalulateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}
u8 MFRC522_Write(u8 blockAddr, u8 *writeData)
{
	u8 status;
	u32 recvBits;
	u8 i;
	u8 buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	CalulateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i=0; i<16; i++) //向 FIFO ? 16Byte ?据
		{
			buff[i] = *(writeData+i);
		}
		CalulateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}
void MFRC522_Halt(void)
{
	u8 status;
	u32 unLen;
	u8 buff[4];
	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}