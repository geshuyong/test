/**********************************************************************************************************
** 文件名: 		STM32F4_BKPSRAM_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		备份域4K SARM驱动
** 相关文件:	
** 修改日志：	 
**********************************************************************************************************/ 
#include "stm32f4xx.h"
#include "STM32F4_BKPSRAM_Driver.h"

Boot_Mode 		BootLoader_Mode __attribute__((at(0x40024000)));	//BootLoader引导启动模式设定
unsigned int 	Product_Model	__attribute__((at(0x40024004)));	//产品型号设定
unsigned int 	Production_Time __attribute__((at(0x40024008)));	//生产时间设定

/**********************************************************************************************************
* 函数名称： void STM32F4_BKPSRAM_Init(void)
* 函数说明： 备份域4K SRAM初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_BKPSRAM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 	//启动PWR时钟
	PWR_BackupAccessCmd(ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE); //启动BKPSRAM时钟		
	if(!PWR_GetFlagStatus(PWR_FLAG_BRR))
	{
		PWR_BackupRegulatorCmd(ENABLE);						//启用备份调压器
		while( !PWR_GetFlagStatus(PWR_FLAG_BRR) );			//等待备份调压器就绪
	}
}


