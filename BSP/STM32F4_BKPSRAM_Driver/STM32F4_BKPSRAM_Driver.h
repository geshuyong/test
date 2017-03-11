/**********************************************************************************************************
** 文件名: 		STM32F4_BKPSRAM_Driver.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		备份域4K SARM驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/ 
#ifndef  __STM32F4_BKPSRAM_Driver_H__
#define  __STM32F4_BKPSRAM_Driver_H__

typedef enum {
	Normal_Start						= 0x00000000,				//正常运行
	Upgrade_EN							= 0x5A5AA5A5,				//程序更新使能
	Clear_All_Data_EN					= 0x5A5AA5A6,				//清除所有数据使能
	Upgrade_And_Clear_All_Data_EN		= 0x5A5AA5A7,				//程序更新和清除所有数据使能
	ETH_Upgrade_EN						= 0x5A5AA5A8,				//以太网升级使能
	SN_Writer_EN						= 0x5A5AA5A9				//序列号写入使能	
} Boot_Mode;														//BootLoader引导启动模式


extern Boot_Mode 	BootLoader_Mode __attribute__((at(0x40024000)));	//BootLoader引导启动模式设定
extern unsigned int Product_Model 	__attribute__((at(0x40024004)));	//产品型号设定
extern unsigned int Production_Time __attribute__((at(0x40024008)));	//生产时间设定

/**************************************4K备份 SRAM使用分配表**********************************************
名称				起始地址									大小（单位字节）
BootLoader_Mode		0x40024000									4
Product_Model		0x40024004									4
Production_Time		0x40024008									4


**********************************************************************************************************/



/**********************************************************************************************************
* 函数名称： void STM32F4_BKPSRAM_Init(void)
* 函数说明： 备份域4K SRAM初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_BKPSRAM_Init(void);

#endif


