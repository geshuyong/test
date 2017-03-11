/**********************************************************************************************************
** 文件名: 		Run_Led_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		运行指示灯驱动
** 相关文件:					
** 修改日志：	
**********************************************************************************************************/ 
#include "stm32f4xx.h"

/**********************************************************************************************************
* 函数名称： void Run_Led_Init(void)
* 函数说明： 运行指示灯引脚初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void Run_Led_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
}



