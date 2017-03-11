/*******************************************************************************
** 文件名: 		STM32F4_SPI_Driver.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		SPI接口驱动
** 相关文件:					
** 修改日志：	 
*******************************************************************************/ 
#ifndef __STM32F4_SPI_DRIVER_H
#define __STM32F4_SPI_DRIVER_H
#include "os.h"

extern OS_MUTEX 	SPI1_mutex;
/*******************************************************************************
* 函数名称： void STM32F4_SPI1_Init(void)
* 函数说明： SPI1 GPIO以及SPI外设初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void STM32F4_SPI1_Init(void);

/*******************************************************************************
* 函数名称： unsigned char STM32F4_SPI1_Read_Write(unsigned char data)
* 函数说明： 读写一个字节
* 输入参数： 写入字节
* 返回参数： 读出字节
*******************************************************************************/
unsigned char STM32F4_SPI1_Read_Write(unsigned char data);

/*******************************************************************************
* 函数名称:	char STM32F4_SPI1_DMA_Send(const unsigned char *buffer,unsigned int buffer_size)
* 函数说明：	SPI采用DMA进行数据发送
* 输入参数： 发送缓冲区大小	 buffer_size  ；发送缓冲区指针 buffer
* 返回参数： 返回0表示成功， 非0表示失败
*******************************************************************************/
char STM32F4_SPI1_DMA_Send(const unsigned char *buffer,unsigned int buffer_size);

/*******************************************************************************
* 函数名称:	char STM32F4_SPI1_DMA_Receive(const unsigned char *buffer,unsigned int buffer_size)
* 函数说明：	SPI采用DMA进行数据接收
* 输入参数： 发送缓冲区大小	 buffer_size  ；发送缓冲区指针 buffer
* 返回参数： 返回0表示成功， 非0表示失败
*******************************************************************************/
char STM32F4_SPI1_DMA_Receive(const unsigned char *buffer,unsigned int buffer_size);


#endif

