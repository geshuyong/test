/******************************************************************************************************
** 文件名: 		Flash2_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 作者: 		盖书永 
** 生成日期: 	2013-11-25 19:22:55
** 功能:		W25Q存储芯片的初始化读写操作
** 相关文件:	无
** 修改日志：	2013-11-25 19:23:03   创建文档
******************************************************************************************************/
#include "stm32f4xx.h"
#include "../BSP/STM32F4_SPI_Driver/STM32F4_SPI_Driver.h"
#include "Flash_Driver.h" 

#define Flash2_SPI_POLL									0

#define Flash2_SPI_CS_LOW    							GPIOD->BSRRL = GPIO_Pin_3; GPIOD->BSRRH = GPIO_Pin_7 	//Flash2 CS引脚的使能,Flash1 CS引脚的禁止
#define Flash2_SPI_CS_HIGH   							GPIOD->BSRRL = GPIO_Pin_7								//Flash2 CS引脚的使能

#define	Flash2_SPI_Read_Write_Byte(data)				STM32F4_SPI1_Read_Write(data)
#define	Flash2_SPI_DMA_Recive(buffer,buffer_size)		STM32F4_SPI1_DMA_Receive(buffer,buffer_size)
#define Flash2_SPI_DMA_Send(buffer,buffer_size)			STM32F4_SPI1_DMA_Send(buffer,buffer_size)

/**********************************************************************************************************
 函数原型：void Flash2_SPI_Write_Mode(char mode)
 入口参数：mode为	1：使能写入；0：禁止写入
 出口参数：无
 函数功能：写使能启用和禁止
 **********************************************************************************************************/
void Flash2_SPI_Write_Mode(char mode)
{
	Flash2_SPI_CS_LOW;
	if(mode)
	{
		Flash2_SPI_Read_Write_Byte(Flash_WriteEnable);  		//发送写使能指令	
	}
	else
	{
		Flash2_SPI_Read_Write_Byte(Flash_WriteDisable);  		//发送写禁能指令	
	}
	Flash2_SPI_CS_HIGH;
}
/**********************************************************************************************************
 函数原型：unsigned char Flash2_SPI_ReadStatusReg(void)
 入口参数：无
 出口参数：FLASH芯片状态
 函数功能：获取FLASH芯片状态
 **********************************************************************************************************/
unsigned char Flash2_SPI_ReadStatusReg(void)
{
	unsigned char FLASH_Status = 0;
	Flash2_SPI_CS_LOW;
	Flash2_SPI_Read_Write_Byte(Flash_ReadStatusReg);  			//读状态寄存器命令
	FLASH_Status = Flash2_SPI_Read_Write_Byte(Dummy_Byte);  	//获取状态寄存器的值
	Flash2_SPI_CS_HIGH;
	return FLASH_Status;
}

/**********************************************************************************************************
 函数原型：void Flash2_SPI_WriteStatusReg(unsigned char status)
 入口参数：无
 出口参数：要写入的状态值
 函数功能：写状态寄存器，主要功能设置写保护功能
 **********************************************************************************************************/
void Flash2_SPI_WriteStatusReg(unsigned char status)
{
	Flash2_SPI_CS_LOW;
	Flash2_SPI_Read_Write_Byte(Flash_WriteStatusReg);  		//发送写状态寄存器命令
	Flash2_SPI_Read_Write_Byte(status);  			  		//发送要写入的状态
	Flash2_SPI_CS_HIGH;
}

/**********************************************************************************************************
 函数原型：void Flash2_SPI_WaitForWriteEnd(void)
 入口参数：无
 出口参数：无
 函数功能：等待FLASH芯片编程、擦除、写状态寄存器周期结束
 **********************************************************************************************************/
void Flash2_SPI_WaitForWriteEnd(void)
{
	unsigned char flag;
	do
	{
		flag = Flash2_SPI_ReadStatusReg();
	}while((flag&WIP_Flag)==0x01);			//等待编程、擦除、写状态寄存器周期结束,等待写结束
}

/**********************************************************************************************************
 函数原型：void Flash2_SPI_SectorErase(unsigned int SectorAddr)
 入口参数：扇区地址
 出口参数：无
 函数功能：擦除指定扇区
 **********************************************************************************************************/
void Flash2_SPI_SectorErase(unsigned int SectorAddr)
{
	OS_ERR		 err;	
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	SectorAddr*=FLASH_SECTOR_SIZE;
	Flash2_SPI_Write_Mode(1);	            			//发送写使能指令
	Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束
	Flash2_SPI_CS_LOW;		                			//片选使能
	Flash2_SPI_Read_Write_Byte(Flash_SectorErase);		//发送扇区擦除指令
	Flash2_SPI_Read_Write_Byte((SectorAddr & 0xFF0000) >> 16);
	Flash2_SPI_Read_Write_Byte((SectorAddr & 0xFF00) >> 8);
	Flash2_SPI_Read_Write_Byte( SectorAddr & 0xFF);
	Flash2_SPI_CS_HIGH;		            				//片选失能
	Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
 函数原型：void Flash2_SPI_ChipErase(void)
 入口参数：芯片擦除
 出口参数：无
 函数功能：芯片擦除
 **********************************************************************************************************/
void Flash2_SPI_ChipErase(void)
{
	OS_ERR		 err;	
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	Flash2_SPI_Write_Mode(1);	            		//发送写使能指令
	Flash2_SPI_WaitForWriteEnd();            		//等待写操作结束
	Flash2_SPI_CS_LOW;		                		//片选使能
	Flash2_SPI_Read_Write_Byte(Flash_ChipErase);	//发送扇区擦除指令
	Flash2_SPI_CS_HIGH;		            			//片选失能
	Flash2_SPI_WaitForWriteEnd();            		//等待写操作结束
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}


/**********************************************************************************************************
 函数原型：unsigned int Flash2_SPI_ReadJedecID(void) 
 入口参数：无
 出口参数：32位的ID，0-7位为容量ID,8-15位为器件ID,16-23制造ID
 函数功能：读取JedecID
 **********************************************************************************************************/
unsigned int Flash2_SPI_ReadJedecID(void)
{
	unsigned int ID = 0, ID1 = 0, ID2 = 0, ID3 = 0;
	OS_ERR		 err;	
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	Flash2_SPI_CS_LOW;
	Flash2_SPI_Read_Write_Byte(Flash_JedecDeviceID);	//获取JedecDeviceID
	ID1 = Flash2_SPI_Read_Write_Byte(Dummy_Byte);		//制造ID
	ID2 = Flash2_SPI_Read_Write_Byte(Dummy_Byte);		//器件ID
	ID3 = Flash2_SPI_Read_Write_Byte(Dummy_Byte);		//容量ID
	Flash2_SPI_CS_HIGH;
	ID = (ID1 << 16) | (ID2 << 8) | ID3;
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
	return ID;
}

/**********************************************************************************************************
 函数原型：char Flash2_Read_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：读取一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_Read_Sector(unsigned int nSector, unsigned char * pBuffer)
{	
#if	Flash2_SPI_POLL	
    unsigned int i;	
#endif
	if(Flash2_Offset_Flag==Flash2_Partition_2)		//计算扇区偏移量，因为FLASH2分为了两个区，用户镜像对的备份，防止升级失败造成无法WEB访问
	{
		nSector += FLASH_SECTOR_COUNT/2;
	}
	nSector *= FLASH_SECTOR_SIZE;  					//扇区号转为地址
	Flash2_SPI_CS_LOW;		                     	//片选使能
	Flash2_SPI_Read_Write_Byte(Flash_ReadData);
	Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFFFF) >> 16));
	Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFF) >> 8));
	Flash2_SPI_Read_Write_Byte(nSector & 0xFF);
	Flash2_SPI_Read_Write_Byte(0xFF);				//等待一个空字节
#if	Flash2_SPI_POLL	
	for(i=0;i<FLASH_SECTOR_SIZE;i++)
	{	
		pBuffer[i] = Flash2_SPI_Read_Write_Byte(0xFF);
	}
#else
	if(Flash2_SPI_DMA_Recive( pBuffer, FLASH_SECTOR_SIZE ) == 1)
	{
		Flash2_SPI_CS_HIGH;			
		return 1;	
	}
#endif		
	Flash2_SPI_CS_HIGH;
	return 0;
}

/**********************************************************************************************************
 函数原型：char Flash2_Write_Sector(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：写入一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_Write_Sector(unsigned int nSector, unsigned char * pBuffer)
{	
#if	Flash2_SPI_POLL									
	unsigned int i;
#endif
	unsigned int j;	

	if(Flash2_Offset_Flag==Flash2_Partition_2)			//计算扇区偏移量，因为FLASH2分为了两个区，用户镜像对的备份，防止升级失败造成无法WEB访问
	{
		nSector += FLASH_SECTOR_COUNT/2;
	}	
	nSector *= FLASH_SECTOR_SIZE;			 			//扇区号转为地址	
	
	Flash2_SPI_Write_Mode(1);	            			//发送写使能指令
	Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束
	Flash2_SPI_CS_LOW;		                			//片选使能
	Flash2_SPI_Read_Write_Byte(Flash_SectorErase);			//发送扇区擦除指令
	Flash2_SPI_Read_Write_Byte((nSector & 0xFF0000) >> 16);
	Flash2_SPI_Read_Write_Byte((nSector & 0xFF00) >> 8);
	Flash2_SPI_Read_Write_Byte( nSector & 0xFF);
	Flash2_SPI_CS_HIGH;		            				//片选失能
	Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束
	for(j=0;j<FLASH_PAGES_PER_SECTOR;j++)				//一个扇区需要几个页
	{
		Flash2_SPI_Write_Mode(1);
		Flash2_SPI_CS_LOW;	
		Flash2_SPI_Read_Write_Byte(Flash_PageProgram);
	    Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFFFF) >> 16));
	    Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFF) >> 8));
	    Flash2_SPI_Read_Write_Byte(nSector & 0xFF);	
#if	Flash2_SPI_POLL			
		for(i=0;i<FLASH_PAGE_SIZE;i++)								
		{
			Flash2_SPI_Read_Write_Byte(pBuffer[i]);
		}	
#else
		if(Flash2_SPI_DMA_Send( pBuffer, FLASH_PAGE_SIZE ) ==1)
		{
			Flash2_SPI_CS_HIGH;			
			return 1;		
		}
#endif							
		pBuffer += FLASH_PAGE_SIZE;
		nSector += FLASH_PAGE_SIZE;
		Flash2_SPI_CS_HIGH;
		Flash2_SPI_WaitForWriteEnd();
	}
	return 0;
	
}

/**********************************************************************************************************
 函数原型：char Flash2_Read_Sector2(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：读取一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_Read_Sector2(unsigned int nSector, unsigned char * pBuffer)
{	
#if	Flash2_SPI_POLL	
    unsigned int i;	
#endif
	if(Flash2_Offset_Flag==Flash2_Partition_1)		//计算扇区偏移量，因为FLASH2分为了两个区，用户镜像对的备份，防止升级失败造成无法WEB访问
	{
		nSector += FLASH_SECTOR_COUNT/2;
	}
	nSector *= FLASH_SECTOR_SIZE;  					//扇区号转为地址
	Flash2_SPI_CS_LOW;		                     	//片选使能
	Flash2_SPI_Read_Write_Byte(Flash_ReadData);
	Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFFFF) >> 16));
	Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFF) >> 8));
	Flash2_SPI_Read_Write_Byte(nSector & 0xFF);
	Flash2_SPI_Read_Write_Byte(0xFF);				//等待一个空字节
#if	Flash2_SPI_POLL	
	for(i=0;i<FLASH_SECTOR_SIZE;i++)
	{	
		pBuffer[i] = Flash2_SPI_Read_Write_Byte(0xFF);
	}
#else
	if(Flash2_SPI_DMA_Recive( pBuffer, FLASH_SECTOR_SIZE ) == 1)
	{
		Flash2_SPI_CS_HIGH;			
		return 1;	
	}
#endif		
	Flash2_SPI_CS_HIGH;
	return 0;
}

/**********************************************************************************************************
 函数原型：char Flash2_Write_Sector2(unsigned int nSector, unsigned char * pBuffer)
 入口参数：nSector扇区地址  ； pBuffer数据存放缓存区指针
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：写入一个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_Write_Sector2(unsigned int nSector, unsigned char * pBuffer)
{	
#if	Flash2_SPI_POLL									
	unsigned int i;
#endif
	unsigned int j;	
	OS_ERR		 err;
	if(Flash2_Offset_Flag==Flash2_Partition_1)				//计算扇区偏移量，因为FLASH2分为了两个区，用户镜像对的备份，防止升级失败造成无法WEB访问
	{
		nSector += FLASH_SECTOR_COUNT/2;
	}

	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	if((nSector%16)==0)
	{
		nSector *= FLASH_SECTOR_SIZE;			 			//扇区号转为地址		
		Flash2_SPI_Write_Mode(1);	            			//发送写使能指令
		Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束
		Flash2_SPI_CS_LOW;		                			//片选使能
		Flash2_SPI_Read_Write_Byte(Flash_Block_Erase_64);	//发送64K擦除指令
		Flash2_SPI_Read_Write_Byte((nSector & 0xFF0000) >> 16);
		Flash2_SPI_Read_Write_Byte((nSector & 0xFF00) >> 8);
		Flash2_SPI_Read_Write_Byte( nSector & 0xFF);
		Flash2_SPI_CS_HIGH;		            				//片选失能
		Flash2_SPI_WaitForWriteEnd();            			//等待写操作结束	
	}
	else
	{
		nSector *= FLASH_SECTOR_SIZE;			 			//扇区号转为地址	
	}

	for(j=0;j<FLASH_PAGES_PER_SECTOR;j++)				//一个扇区需要几个页
	{
		Flash2_SPI_Write_Mode(1);
		Flash2_SPI_CS_LOW;	
		Flash2_SPI_Read_Write_Byte(Flash_PageProgram);
	    Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFFFF) >> 16));
	    Flash2_SPI_Read_Write_Byte(((nSector & 0xFFFF) >> 8));
	    Flash2_SPI_Read_Write_Byte(nSector & 0xFF);	
#if	Flash2_SPI_POLL			
		for(i=0;i<FLASH_PAGE_SIZE;i++)								
		{
			Flash2_SPI_Read_Write_Byte(pBuffer[i]);
		}	
#else
		if(Flash2_SPI_DMA_Send( pBuffer, FLASH_PAGE_SIZE ) ==1)
		{
			Flash2_SPI_CS_HIGH;	
			OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE, &err);
			return 1;		
		}
#endif							
		pBuffer += FLASH_PAGE_SIZE;
		nSector += FLASH_PAGE_SIZE;
		Flash2_SPI_CS_HIGH;
		Flash2_SPI_WaitForWriteEnd();
	}
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
	return 0;
	
}

/**********************************************************************************************************
 函数原型：char Flash2_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：读取数据缓存区地址  ； 扇区地址 ；读取扇区数量
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：读取多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_ReadMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
{
	unsigned int i;
	OS_ERR		 err;	
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	for(i=0;i<NumberOfBlocks;i++)
	{
		if(Flash2_Read_Sector(Addr, buff) == 1) return 1;		
		Addr++;
		buff += FLASH_SECTOR_SIZE;	
	}
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
	return 0;
}

/**********************************************************************************************************
 函数原型：char Flash2_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
 入口参数：数据的缓存区  ； 扇区地址  ；写入扇区数量
 出口参数：返回0表示成功 ，非0表示失败
 函数功能：写入多个扇区，适应于FATFS文件系统
 **********************************************************************************************************/
char Flash2_WriteMultiBlocks(unsigned char *buff, unsigned int Addr, unsigned int NumberOfBlocks)
{
	unsigned int i;
	OS_ERR		 err;	
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	for(i=0;i<NumberOfBlocks;i++)
	{
		if(Flash2_Write_Sector(Addr, buff) == 1) return 1;
		Addr++;
		buff += FLASH_SECTOR_SIZE;	
	}
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
	return 0;
}



