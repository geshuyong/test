/**********************************************************************************
** 文件名: 		NVIC_Config.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 作者: 		盖书永
** 生成日期: 	2012-09-20
** 功能:		硬件中断优先级分组方式，中断优先级，使能配置
** 相关文件:	无
** 修改日志：	 
**********************************************************************************/
#include "stm32f4xx.h"
#include "../BSP/STM32F4_SDIO_SD_Driver/STM32F4_SDIO_Driver.h"
/*********************************************************************************
* 函数名称:	 void NVIC_Configuration(void)
* 函数说明： 中断优先级分组方式，中断优先级，使能配置
* 输入参数： 无
* 返回参数： 无
**********************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//USART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		   //中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //指定抢占式优先级别 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		   //次优先级 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   //通道中断使能
	NVIC_Init(&NVIC_InitStructure);  						   //初始化中断
//USART2_IRQn
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
//USART3_IRQn
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
//USART6_IRQn
 	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
                      	 		

//TIM2的中断通道
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;      
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	
// DMA1 STREAMx Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel 	= DMA1_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_Init(&NVIC_InitStructure);	

// DMA2 STREAM7 Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel 	= DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_Init(&NVIC_InitStructure);	
	
// DMA2 STREAM7 Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel 	= DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_Init(&NVIC_InitStructure);	
	

	
}

