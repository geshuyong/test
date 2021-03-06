/*******************************************************************************
** 文件名: 		SysTick_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		系统滴答驱动
** 相关文件:					
** 修改日志：	
*******************************************************************************/ 
#include "stm32f4xx.h"

/*******************************************************************************
* 函数名称： void SysTick_Init(void)
* 函数说明： 系统滴答初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void SysTick_Init(void)
{
    RCC_ClocksTypeDef	RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency/100);
}



