#include "ht32.h"
#include "ht32_board.h"
#ifndef MFRC522_COMMAND
	#define MFRC522_COMMAND
	#define MAX_LEN 16
	//const int chipSelectPin = 10;//如果控制板? UNO,328,168
	//const int NRSTPD = 5;
	//MF522 命令字
	#define PCD_IDLE 0x00 //NO action;取消?前命令
	#define PCD_AUTHENT 0x0E //??密?
	#define PCD_RECEIVE 0x08 //接收?据
	#define PCD_TRANSMIT 0x04 //?送?据
	#define PCD_TRANSCEIVE 0x0C //?送并接收?据
	#define PCD_RESETPHASE 0x0F //复位
	#define PCD_CALCCRC 0x03 //CRC ?算
	//Mifare_One 卡片命令字
	#define PICC_REQIDL 0x26 //?天???未?入休眠??
	#define PICC_REQALL 0x52 //?天???全部卡
	#define PICC_ANTICOLL 0x93 //防?撞
	#define PICC_SElECTTAG 0x93 //?卡
	#define PICC_AUTHENT1A 0x60 //?? A 密?
	#define PICC_AUTHENT1B 0x61 //?? B 密?
	#define PICC_READ 0x30 //??
	#define PICC_WRITE 0xA0 //??
	#define PICC_DECREMENT 0xC0
	#define PICC_INCREMENT 0xC1
	#define PICC_RESTORE 0xC2 //???据到???
	#define PICC_TRANSFER 0xB0 //保存???中?据
	#define PICC_HALT 0x50 //休眠

	//和 MF522 通??返回的??代?
	#define MI_OK 0
	#define MI_NOTAGERR 1
	#define MI_ERR 2
	//------------------MFRC522 寄存器---------------
	//Page 0:Command and Status
	#define Reserved00 0x00
	#define CommandReg 0x01
	#define CommIEnReg 0x02
	#define DivlEnReg 0x03
	#define CommIrqReg 0x04
	#define DivIrqReg 0x05
	#define ErrorReg 0x06
	#define Status1Reg 0x07
	#define Status2Reg 0x08
	#define FIFODataReg 0x09
	#define FIFOLevelReg 0x0A
	#define WaterLevelReg 0x0B
	#define ControlReg 0x0C
	#define BitFramingReg 0x0D
	#define CollReg 0x0E
	#define Reserved01 0x0F
	//Page 1:Command
	#define Reserved10 0x10
	#define ModeReg 0x11
	#define TxModeReg 0x12
	#define RxModeReg 0x13
	#define TxControlReg 0x14
	#define TxAutoReg 0x15
	#define TxSelReg 0x16
	#define RxSelReg 0x17
	#define RxThresholdReg 0x18
	#define DemodReg 0x19
	#define Reserved11 0x1A
	#define Reserved12 0x1B
	#define MifareReg 0x1C
	#define Reserved13 0x1D
	#define Reserved14 0x1E
	#define SerialSpeedReg 0x1F
	//Page 2:CFG
	#define Reserved20 0x20
	#define CRCResultRegM 0x21
	#define CRCResultRegL 0x22
	#define Reserved21 0x23
	#define ModWidthReg 0x24
	#define Reserved22 0x25
	#define RFCfgReg 0x26
	#define GsNReg 0x27
	#define CWGsPReg 0x28
	#define ModGsPReg 0x29
	#define TModeReg 0x2A
	#define TPrescalerReg 0x2B
	#define TReloadRegH 0x2C
	#define TReloadRegL 0x2D
	#define TCounterValueRegH 0x2E
	#define TCounterValueRegL 0x2F
	//Page 3:TestRegister
	#define Reserved30 0x30
	#define TestSel1Reg 0x31
	#define TestSel2Reg 0x32
	#define TestPinEnReg 0x33
	#define TestPinValueReg 0x34
	#define TestBusReg 0x35
	#define AutoTestReg 0x36
	#define VersionReg 0x37
	#define AnalogTestReg 0x38
	#define TestDAC1Reg 0x39
	#define TestDAC2Reg 0x3A
	#define TestADCReg 0x3B
	#define Reserved31 0x3C
	#define Reserved32 0x3D
	#define Reserved33 0x3E
	#define Reserved34 0x3F
#endif
extern u8 serNum[5];
extern u8 writeDate[16];
extern u8 sectorKeyA[16][16];
extern u8 sectorNewKeyA[16][16];

void Write_MFRC522(u8 addr, u8 val);
u8 Read_MFRC522(u8 addr);
void SetBitMask(u8 reg, u8 mask);
void ClearBitMask(u8 reg, u8 mask);
void AntennaOn(void);
void AntennaOff(void);
void MFRC522_Reset(void);
void MFRC522_Init(void);
u8 MFRC522_Request(u8 reqMode, u8 *TagType);
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u32 *backLen);
u8 MFRC522_Anticoll(u8 *serNum);
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData);
u8 MFRC522_SelectTag(u8 *serNum);
u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum);
u8 MFRC522_Read(u8 blockAddr, u8 *recvData);
u8 MFRC522_Write(u8 blockAddr, u8 *writeData);
void MFRC522_Halt(void);

u8 Get_Card_Number(void);