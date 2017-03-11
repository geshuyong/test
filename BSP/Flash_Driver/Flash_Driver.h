/******************************************************************************************************
** 文件名: 		Flash_Driver.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 作者: 		盖书永 
** 生成日期: 	2013-11-25 19:22:55
** 功能:		W25Q存储芯片的初始化读写操作
** 相关文件:	无
** 修改日志：	2013-11-25 19:23:03   创建文档
******************************************************************************************************/
#ifndef ___Flash_Driver_H
#define ___Flash_Driver_H

#define FLASH_PAGE_SIZE					256
#define FLASH_SECTOR_SIZE				4096
//#define FLASH_SECTOR_COUNT			4096	   //W25Q128
#define FLASH_SECTOR_COUNT				2048	   //W25Q64
#define FLASH_BLOCK_SIZE				65535
#define FLASH_PAGES_PER_SECTOR			FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE


#define Flash_WriteEnable				0x06 
#define Flash_WriteDisable				0x04 
#define Flash_ReadStatusReg				0x05 
#define Flash_WriteStatusReg			0x01 
#define Flash_ReadData					0x0B 
#define Flash_PageProgram				0x02 
#define Flash_SectorErase				0x20
#define Flash_Block_Erase_32			0x52
#define Flash_Block_Erase_64			0xD8
#define Flash_ChipErase					0xC7
#define Flash_JedecDeviceID				0x9F 

#define WIP_Flag                		0x01  						//Write In Progress (WIP) flag

#define Dummy_Byte              		0xFF
                


#define	Winbond_W25Q16		    		0xEF4015
#define	Winbond_W25Q32		    		0xEF4016
#define	Winbond_W25Q64		    		0xEF4017
#define	Winbond_W25Q128		    		0xEF4018


//------------------------------FLASH1接口----------------------------------

/**********************************************************************************************************
 函数原型：void Flash1_SPI_SectorErase(unsigned int SectorAddr)
 入口参数：扇区地址
 出口参数：无
 函数功能：擦除指定扇区
 **********************************************************************************************************/
extern void Flash1_SPI_SectorErase(unsigned int SectorAddr);
/**********************************************************************************************************
 函数原型：void Flash1_SPI_ChipErase(void)
 入口参数：芯片擦除
 出口参数：无
 函数功能：芯片擦除
 **********************************************************************************************************/
extern void Flash1_SPI_ChipErase(void);

/**********************************************************************************************************
 函数原型：unsigned int Flash1_SPI_ReadJedecID(void) 
 入口参数：无
 出口参数：32位的ID，0-7位为容量ID,8-15位为器件ID,16-23制造ID
 函数功能：读取JedecID
 **********************************************************************************************************/
extern unsigned int Flash1_SPI_ReadJedecID(void);

/**********************************************************************************************************
 函数原型：char Flash1_Read_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功， 非0表示失败
 函数功能：读取一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash1_Read_Sector(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash1_Write_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功， 非0表示失败
 函数功能：写入一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash1_Write_Sector(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash1_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：读取数据缓存区地址  ； 扇区地址 ；读取扇区数量
 出口参数：返回0表示成功， 非0表示失败
 函数功能：读取多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash1_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks);

/**********************************************************************************************************
 函数原型：char Flash1_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：数据的缓存区  ； 扇区地址  ；写入扇区数量
 出口参数：返回0表示成功， 非0表示失败
 函数功能：写入多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash1_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks);


//------------------------------FLASH2接口----------------------------------

#define Flash2_Offset_Flag		(*((unsigned int*)(0x0807FFF0+12)))	//FLASH2分区标志
#define Flash2_Partition_1		1									//FLASH2分区1
#define Flash2_Partition_2		2									//FLASH2分区2

/**********************************************************************************************************
 函数原型：void Flash2_SPI_SectorErase(unsigned int SectorAddr)
 入口参数：扇区地址
 出口参数：无
 函数功能：擦除指定扇区
 **********************************************************************************************************/
extern void Flash2_SPI_SectorErase(unsigned int SectorAddr);
/**********************************************************************************************************
 函数原型：void Flash2_SPI_ChipErase(void)
 入口参数：芯片擦除
 出口参数：无
 函数功能：芯片擦除
 **********************************************************************************************************/
extern void Flash2_SPI_ChipErase(void);

/**********************************************************************************************************
 函数原型：unsigned int Flash2_SPI_ReadJedecID(void) 
 入口参数：无
 出口参数：32位的ID，0-7位为容量ID,8-15位为器件ID,16-23制造ID
 函数功能：读取JedecID
 **********************************************************************************************************/
extern unsigned int Flash2_SPI_ReadJedecID(void);

/**********************************************************************************************************
 函数原型：char Flash2_Read_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功， 非0表示失败
 函数功能：读取一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_Read_Sector(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash2_Write_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功， 非0表示失败
 函数功能：写入一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_Write_Sector(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash2_Read_Sector2(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：读取一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_Read_Sector2(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash2_Write_Sector2(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功， 非0表示失败
 函数功能：写入一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_Write_Sector2(unsigned int nSector, unsigned char * pBuffer);

/**********************************************************************************************************
 函数原型：char Flash2_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：读取数据缓存区地址  ； 扇区地址 ；读取扇区数量
 出口参数：返回0表示成功， 非0表示失败
 函数功能：读取多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks);

/**********************************************************************************************************
 函数原型：char Flash2_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：数据的缓存区  ； 扇区地址  ；写入扇区数量
 出口参数：返回0表示成功， 非0表示失败
 函数功能：写入多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
extern char Flash2_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks);


#endif



