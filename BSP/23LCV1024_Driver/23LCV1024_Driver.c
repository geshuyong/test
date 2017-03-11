/******************************************************************************************************
** 文件名: 		23LCV1024.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永 
** 生成日期: 	2014年12月6日20:43:12
** 功能:		RAM存储芯片的初始化读写操作
** 相关文件:		无
** 修改日志：	2014年12月6日20:43:27   创建文档
******************************************************************************************************/
#include "stm32f4xx.h"
#include "../BSP/STM32F4_SPI_Driver/STM32F4_SPI_Driver.h"
#include "23LCV1024_Driver.h" 



#define		READ	0x03			//从所选地址开始从存储器阵列读数据
#define		WRITE	0x02			//从所选地址开始向存储器阵列写数据
#define		RDSR	0x05			//读STATUS 寄存器
#define		WRSR	0x01			//写STATUS 寄存器


/***************************************************************************
 函数原型：unsigned char RAM_SPI_ReadStatusReg(void)
 入口参数：无
 出口参数：RAM芯片状态
 函数功能：获取RAM芯片状态
 ***************************************************************************/
unsigned char RAM_SPI_ReadStatusReg(void)
{
	unsigned char RAM_Status = 0;
	RAM_SPI_CS_LOW;
	RAM_SPI_Read_Write_Byte(RDSR);  					//读状态寄存器命令
	RAM_Status = RAM_SPI_Read_Write_Byte(Dummy_Byte);  	//获取状态寄存器的值
	RAM_SPI_CS_HIGH;
	return RAM_Status;
}

/***************************************************************************
 函数原型：void RAM_Read_Data(unsigned int addr, unsigned char * Buffer, unsigned int size)
 入口参数：addr 地址  ； Buffer数据存放缓存区指针 ； size 读取数据字节数
 出口参数：无
 函数功能：读取一块数据
 ***************************************************************************/
void RAM_Read_Data(unsigned int addr, unsigned char * Buffer, unsigned int size)
{	
#if RAM_SPI_DMA	
	unsigned int j;	
#endif	
//	RAM_SPI_CS_LOW;		                			//片选使能
//	RAM_SPI_Read_Write_Byte(WRSR);			 
//	RAM_SPI_Read_Write_Byte(0X40);
//	RAM_SPI_CS_HIGH;		            			//片选失能
	RAM_SPI_CS_LOW;		                			//片选使能
	RAM_SPI_Read_Write_Byte(READ);		
	RAM_SPI_Read_Write_Byte((addr&0x00ff0000)>>16);	
	RAM_SPI_Read_Write_Byte((addr&0x0000ff00)>>8);	
	RAM_SPI_Read_Write_Byte(addr&0x000000ff);	
#if RAM_SPI_DMA		
	for(j=0;j<size;j++)
	{
		Buffer[j] = RAM_SPI_Read_Write_Byte(Dummy_Byte);		
	}
#else
	RAM_SPI_DMA_Recive( Buffer, size);	
#endif
	RAM_SPI_CS_HIGH;		            			//片选失能		
}

/***************************************************************************
 函数原型：void RAM_Write_Data(unsigned int addr, unsigned char * Buffer,unsigned int size)
 入口参数：addr 地址  ； Buffer 数据存放缓存区指针 ；size 写数据字节数
 出口参数：无
 函数功能：写入一块数据
 ***************************************************************************/
void RAM_Write_Data(unsigned int addr, unsigned char * Buffer,unsigned int size)
{	
#if RAM_SPI_DMA	
	unsigned int j;	
#endif
//	RAM_SPI_CS_LOW;		                			//片选使能
//	RAM_SPI_Read_Write_Byte(WRSR);			 
//	RAM_SPI_Read_Write_Byte(0X40);
//	RAM_SPI_CS_HIGH;		            			//片选失能
	RAM_SPI_CS_LOW;		                			//片选使能
	RAM_SPI_Read_Write_Byte(WRITE);		
	RAM_SPI_Read_Write_Byte((addr&0x00ff0000)>>16);	
	RAM_SPI_Read_Write_Byte((addr&0x0000ff00)>>8);	
	RAM_SPI_Read_Write_Byte(addr&0x000000ff);	
#if RAM_SPI_DMA		
	for(j=0;j<size;j++)			 
	{
		RAM_SPI_Read_Write_Byte(Buffer[j]);					
	}
#else	
	RAM_SPI_DMA_Send( Buffer, size);
#endif	
	RAM_SPI_CS_HIGH;		            			//片选失能	
	
}

