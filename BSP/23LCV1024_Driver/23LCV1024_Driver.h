/******************************************************************************************************
** 文件名: 		23LCV1024.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永 
** 生成日期: 	2014年12月6日20:43:12
** 功能:		RAM存储芯片的初始化读写操作
** 相关文件:		无
** 修改日志：	2014年12月6日20:43:27   创建文档
******************************************************************************************************/
#ifndef ___23LCV1024_H
#define ___23LCV1024_H


#define RAM_SPI_DMA									1
#define Dummy_Byte              					0xFF


#define RAM_SPI_CS_LOW    							GPIOD->BSRRH = GPIO_Pin_7	//RAM CS引脚的使能
#define RAM_SPI_CS_HIGH   							GPIOD->BSRRL = GPIO_Pin_7	//RAM CS引脚的使能


#define	RAM_SPI_Read_Write_Byte(data)				STM32F4_SPI1_Read_Write(data)
#define	RAM_SPI_DMA_Recive(buffer,buffer_size)		STM32F4_SPI1_DMA_Receive(buffer,buffer_size)
#define RAM_SPI_DMA_Send(buffer,buffer_size)		STM32F4_SPI1_DMA_Send(buffer,buffer_size)


/***************************************************************************
 函数原型：unsigned char RAM_SPI_ReadStatusReg(void)
 入口参数：无
 出口参数：RAM芯片状态
 函数功能：获取RAM芯片状态
 ***************************************************************************/
unsigned char RAM_SPI_ReadStatusReg(void);

/***************************************************************************
 函数原型：void RAM_Read_Data(unsigned int addr, unsigned char * Buffer, unsigned int size)
 入口参数：addr 地址  ； Buffer数据存放缓存区指针 ； size 读取数据字节数
 出口参数：无
 函数功能：读取一块数据
 ***************************************************************************/
void RAM_Read_Data(unsigned int addr, unsigned char * Buffer, unsigned int size);

/***************************************************************************
 函数原型：void RAM_Write_Data(unsigned int addr, unsigned char * Buffer,unsigned int size)
 入口参数：addr 地址  ； Buffer 数据存放缓存区指针 ；size 写数据字节数
 出口参数：无
 函数功能：写入一块数据
 ***************************************************************************/
void RAM_Write_Data(unsigned int addr, unsigned char * Buffer,unsigned int size);

#endif

