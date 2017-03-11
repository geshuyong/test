/*********************************************************************************************************
** 文件名: 		DS18B20_Driver.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		DS18B20 温度传感器驱动
** 相关文件:					
** 修改日志：	 
*********************************************************************************************************/
#ifndef __DS18B20_Driver_H
#define __DS18B20_Driver_H

extern double 		 DS18B20_Temp[6];
extern unsigned char DS18B20_Status[6];

/*********************************************************************************************************
* 函数名称： void DS18B20_Temp_Collection(void)
* 函数说明： DS18B20 温度采集任务
* 输入参数： 无
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_Temp_Collection(void *p_arg);

#endif  
