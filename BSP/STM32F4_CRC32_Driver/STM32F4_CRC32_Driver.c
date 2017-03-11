/**********************************************************************************************************
** 文件名: 		STM32F4_CRC32_DMA_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		CRC32采用DMA模式进行校验
** 相关文件:	
** 修改日志：	 
**********************************************************************************************************/ 
#include "stm32f4xx.h"

/**********************************************************************************************************
* 函数名称： unsigned int STM32F4_CRC32_DMA(unsigned int* Address, unsigned int Size, char flag)
* 函数说明： CRC32采用DMA模式进行校验，当处理数据流类数据时，第一次调用flag需置1，进行CRC32初始化，之后调用置0
* 输入参数： 无符号32位整型缓存区地址 ， 无符号32位整型数， 初始化标志（1：会初始化CRC32，0：不初始化CRC32）
* 返回参数： 返回无符号32位整型CRC32校验结果
**********************************************************************************************************/
unsigned int STM32F4_CRC32_DMA(unsigned int* buf, unsigned int len, char flag)
{
	DMA_InitTypeDef 	DMA_InitStruct;

	if(flag==1)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);		
		CRC_ResetDR();	
	}
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStruct.DMA_PeripheralBaseAddr 		= (unsigned int)buf;
	DMA_InitStruct.DMA_Channel 					= DMA_Channel_0;
	DMA_InitStruct.DMA_DIR 						= DMA_DIR_MemoryToMemory;
	DMA_InitStruct.DMA_Memory0BaseAddr 			= CRC_BASE;
	DMA_InitStruct.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStruct.DMA_FIFOThreshold 			= DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst 				= DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst			= DMA_PeripheralBurst_Single;
	DMA_InitStruct.DMA_BufferSize 				= len;
	DMA_InitStruct.DMA_PeripheralInc 			= DMA_PeripheralInc_Enable;
	DMA_InitStruct.DMA_MemoryInc 				= DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize 		= DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Word;
	DMA_InitStruct.DMA_Mode 					= DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority 				= DMA_Priority_High;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_Cmd(DMA2_Stream0, ENABLE);	
	while(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)==RESET){}
	DMA_DeInit(DMA2_Stream0);	
	return 	CRC->DR;
}


