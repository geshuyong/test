/**********************************************************************************************************
** 文件名: 		STM32F4_SDIO_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		SDIO接口驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/ 
#include "Includes.h"

/**********************************************************************************************************
* 函数名称:	 static void NVIC_Configuration(void)
* 函数说明： 中断优先级分组方式，中断优先级，使能配置
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

/**********************************************************************************************************
* 函数名称： void STM32F4_SDIO_Init(void)
* 函数说明： SDIO GPIO以及SDIO外设初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_SDIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOC and GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8,  GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9,  GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2,  GPIO_AF_SDIO);

	/* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PD.02 CMD line */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure PC.12 pin: CLK pin */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable the SDIO APB2 Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

	/* Enable the DMA2 Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	NVIC_Configuration();
}

/**********************************************************************************************************
* 函数名称： void STM32F4_SDIO_DMA_Send(unsigned int  *Buffer, unsigned int Size)
* 函数说明： SDIO DMA进行发送
* 输入参数： 发送缓冲区地址；发送字节数（在这里无意义，有硬件流控决定）
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_SDIO_DMA_Send(unsigned int  *Buffer, unsigned int Size)
{
	DMA_InitTypeDef 	SDDMA_InitStructure;

	/* DMA2 Stream3  or Stream6 Config */
	DMA_DeInit(SD_SDIO_DMA_STREAM);

	SDDMA_InitStructure.DMA_Channel 			= DMA_Channel_4;
	SDDMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)&SDIO->FIFO;;
	SDDMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)Buffer;
	SDDMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral;
	SDDMA_InitStructure.DMA_BufferSize 			= 0;
	SDDMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
	SDDMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
	SDDMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Word;
	SDDMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Word;
	SDDMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;
	SDDMA_InitStructure.DMA_Priority 			= DMA_Priority_VeryHigh;
	SDDMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Enable;
	SDDMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	SDDMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_INC4;
	SDDMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_INC4;
	DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
//	DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);//由硬件流控控制发送字节数

	/* DMA2 Stream3  or Stream6 enable */
	DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
    
}

/**********************************************************************************************************
* 函数名称： void STM32F4_SDIO_DMA_Receive(unsigned int  *Buffer, unsigned int Size)
* 函数说明： SDIO DMA进行接收
* 输入参数： 接收缓冲区地址；发送字节数（在这里无意义，有硬件流控决定）
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_SDIO_DMA_Receive(unsigned int  *Buffer, unsigned int Size)
{
	DMA_InitTypeDef 	SDDMA_InitStructure;

	/* DMA2 Stream3 or Stream6 Config */
	DMA_DeInit(SD_SDIO_DMA_STREAM);

	SDDMA_InitStructure.DMA_Channel 			= DMA_Channel_4;
	SDDMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)&SDIO->FIFO;;
	SDDMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)Buffer;
	SDDMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory;
	SDDMA_InitStructure.DMA_BufferSize 			= 0;
	SDDMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
	SDDMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
	SDDMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Word;
	SDDMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Word;
	SDDMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;
	SDDMA_InitStructure.DMA_Priority 			= DMA_Priority_VeryHigh;
	SDDMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Enable;
	SDDMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	SDDMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_INC4;
	SDDMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_INC4;
	DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
//	DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

	/* DMA2 Stream3 or Stream6 enable */
	DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
}

/**********************************************************************************************************
 函数原型：void SDIO_IRQHandler(void)
 入口参数：无
 出口参数：无
 函数功能：SDIO传输完成中断 
**********************************************************************************************************/
void SDIO_IRQHandler(void)
{
	OSIntEnter();	
	SD_ProcessIRQSrc();
	OSIntExit();	
}
