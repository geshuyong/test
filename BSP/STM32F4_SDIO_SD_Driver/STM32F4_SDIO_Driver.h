/*******************************************************************************
** 文件名: 		STM32F4_SDIO_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		SDIO接口驱动
** 相关文件:					
** 修改日志：	 
*******************************************************************************/ 
#ifndef __STM32F4_SDIO_Driver_H
#define __STM32F4_SDIO_Driver_H

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
//SDIO Intialization Frequency (400KHz max)
#define SDIO_INIT_CLK_DIV                ((uint8_t)0x76)
//SDIO Data Transfer Frequency (25MHz max) 
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x00) 


#define SD_SDIO_DMA_STREAM3	          3
//#define SD_SDIO_DMA_STREAM6           6

#ifdef SD_SDIO_DMA_STREAM3
 #define SD_SDIO_DMA_STREAM            DMA2_Stream3
 #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
 #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
 #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
 #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
 #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF3 
 #define SD_SDIO_DMA_IRQn              DMA2_Stream3_IRQn
 #define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream3_IRQHandler 
#elif defined SD_SDIO_DMA_STREAM6
 #define SD_SDIO_DMA_STREAM            DMA2_Stream6
 #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
 #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
 #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
 #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
 #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF6 
 #define SD_SDIO_DMA_IRQn              DMA2_Stream6_IRQn
 #define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream6_IRQHandler
#endif /* SD_SDIO_DMA_STREAM3 */

/*******************************************************************************
* 函数名称： void STM32F4_SDIO_Init(void)
* 函数说明： SDIO GPIO以及SDIO外设初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void STM32F4_SDIO_Init(void);

/*******************************************************************************
* 函数名称： void STM32F4_SDIO_DMA_Send(unsigned int  *Buffer, unsigned int Size)
* 函数说明： SDIO DMA进行发送
* 输入参数： 发送缓冲区地址；发送字节数（在这里无意义，有硬件流控决定）
* 返回参数： 无
*******************************************************************************/
void STM32F4_SDIO_DMA_Send(unsigned int  *Buffer, unsigned int Size);

/*******************************************************************************
* 函数名称： void STM32F4_SDIO_DMA_Receive(unsigned int  *Buffer, unsigned int Size)
* 函数说明： SDIO DMA进行接收
* 输入参数： 接收缓冲区地址；发送字节数（在这里无意义，有硬件流控决定）
* 返回参数： 无
*******************************************************************************/
void STM32F4_SDIO_DMA_Receive(unsigned int  *Buffer, unsigned int Size);

#endif


