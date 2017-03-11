/**********************************************************************************************************
** 文件名: 		STM32F4_RTC_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2015年4月11日20:44:54
** 功能:		时间日历的实现
** 相关文件:					
** 修改日志：	修正星期寄存器 
**********************************************************************************************************/ 
#include "Includes.h"

#define	leap_year(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leap_year(a) ? 366 : 365)

unsigned int Start_Time;	//记录系统启动时的时间和日期

/**********************************************************************************************************
* 函数名称： void STM32F4_RTC_Init(void)
* 函数说明： RTC初始化
* 输入参数： 无
* 返回参数： 0：成功，1：失败
**********************************************************************************************************/
int STM32F4_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //启动PWR时钟
	PWR_BackupAccessCmd(ENABLE);                        //允许访问到RTC		
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xA5A5)
	{
		RCC_LSEConfig(RCC_LSE_ON);                          //使能LSE振荡器
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); //等待，直到LSE振荡器就绪
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);             //选择RTC时钟源
		RCC_RTCCLKCmd(ENABLE);                              //使能RTC时钟
		RTC_WaitForSynchro();                               //为RTC建立同步寄存器等待
/************************************************************************************************************
		32768/ (1 + 0x7F) = 32768 /128 = 256Hz,
		256/ (1 + 0xFF) = 1Hz, This is the calender frequecy~
*************************************************************************************************************/   
		RTC_InitStructure.RTC_AsynchPrediv  = 0x7F;         //指定异步预分频器的值不能大于7F
		RTC_InitStructure.RTC_SynchPrediv   = 0xFF;        //指定同步预分频器的值
		RTC_InitStructure.RTC_HourFormat    = RTC_HourFormat_24;//指定小时格式
		if(RTC_Init(&RTC_InitStructure) == ERROR)
		{
			return 1;
		}
		else
		{
			RTC_WriteBackupRegister(RTC_BKP_DR0,0xA5A5);
		}
	}
	Start_Time = Get_Current_Timestamp();	
	return 0;
}

/**********************************************************************************************************
* 函数名称： void STM32F4_WakeUp_Init(unsigned short second)
* 函数说明： 唤醒定时器配置，时间到后触发唤醒，同时PC13 开漏输出，极性输出1
* 输入参数： 唤醒时间周期,单位秒， 最大65536秒
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_WakeUp_Init(unsigned short second)
{		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //启动PWR时钟
	PWR_BackupAccessCmd(ENABLE);                        //允许访问到RTC		
	RTC->WPR = 0xCA;									//关闭RTC寄存器的写保护序列
	RTC->WPR = 0x53;									//关闭RTC寄存器的写保护序列
	RTC->CR &= ~(RTC_CR_OSEL | RTC_CR_POL);				//清楚配置位
	RTC->CR |= RTC_Output_WakeUp | RTC_OutputPolarity_High;	//配置唤醒输出，输出极性为1	
	RTC->TAFCR &= ~(RTC_TAFCR_ALARMOUTTYPE);			//清空唤醒输出类型
	RTC->TAFCR |= RTC_OutputType_OpenDrain; 			//设置唤醒输出为开漏	
	RTC->CR &= ~RTC_CR_WUTE;							//关闭唤醒定时器
	while((RTC->ISR & RTC_ISR_WUTWF) == RESET);			//等待确认
	RTC->CR &= ~RTC_CR_WUCKSEL;							//清空唤醒时钟源寄存器
	RTC->CR |= RTC_WakeUpClock_CK_SPRE_16bits;			//设置时钟源	
	RTC->WUTR = second;									//配置唤醒周期，单位秒
    RTC->CR |= RTC_CR_WUTE;								//使能唤醒定时器	
	RTC->WPR = 0xFF; 									//使能写保护寄存器	
}


/**********************************************************************************************************
* 函数名称： void STM32F4_RTC_ALARM_Init(int hour, int minute, int second)
* 函数说明： RTC 闹钟配置，时间到后触发闹钟，同时PC13 开漏输出，极性输出1
* 输入参数： 小时(0-23)，分(0-59)，秒(0-59) 
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_RTC_ALARM_Init(int hour, int minute, int second)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //启动PWR时钟
	PWR_BackupAccessCmd(ENABLE);                        //允许访问到RTC			
	RTC->WPR = 0xCA;									//关闭RTC寄存器的写保护序列
	RTC->WPR = 0x53;									//关闭RTC寄存器的写保护序列
	RTC->CR &= ~(RTC_CR_OSEL | RTC_CR_POL);				//清楚配置位
	RTC->CR |= RTC_Output_AlarmB | RTC_OutputPolarity_High;	//配置唤醒输出，输出极性为1	
	RTC->TAFCR &= ~(RTC_TAFCR_ALARMOUTTYPE);			//清空唤醒输出类型
	RTC->TAFCR |= RTC_OutputType_OpenDrain; 			//设置唤醒输出为开漏	
	RTC->CR &= ~RTC_CR_ALRBE;							//关闭RTC 闹钟 A
	while((RTC->ISR & RTC_ISR_ALRBWF) == RESET){}		//等待确认
	hour   = ((hour/10)<<4)|(hour%10);
	minute = ((minute/10)<<4)|(minute%10);
	second = ((second/10)<<4)|(second%10);
    RTC->ALRMBR =0X91808000|(hour<<16)|(minute<<8)|second;
    RTC->CR |= RTC_CR_ALRBE;							//使能RTC 闹钟 A	
	RTC->WPR = 0xFF; 									//使能写保护寄存器	
}

/**********************************************************************************************************
* 函数名称:	 void Timedate_Calibration(Time_t *timedate);
* 函数说明： 时间日期校准
* 输入参数： 时间日期结构体指针
* 返回参数： 0：成功，1：时间设置失败，2：日期设置失败
**********************************************************************************************************/
int Timedate_Calibration(Time_t *timedate)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;
	
    RTC_TimeStructure.RTC_Hours     = timedate->hour; 
    RTC_TimeStructure.RTC_Minutes   = timedate->minute; 
    RTC_TimeStructure.RTC_Seconds   = timedate->second; 
    RTC_DateStructure.RTC_Year      = timedate->year - 2000;
    RTC_DateStructure.RTC_Month     = timedate->month;
    RTC_DateStructure.RTC_Date      = timedate->day; 
    RTC_DateStructure.RTC_WeekDay   = timedate->weekday+1; 

    if(RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure)==ERROR )
    {
        return 1;  
    }
    if(RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure)==ERROR)
    {
        return 2;
    }
	return 0;
}

/**********************************************************************************************************
* 函数名称:	 void Get_Current_Timedate(Time_t *timedate)
* 函数说明： 获取当前时间日期
* 输入参数： 时间日期结构体指针
* 返回参数： 无
**********************************************************************************************************/
void Get_Current_Timedate(Time_t *timedate)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	timedate->year		= RTC_DateStructure.RTC_Year+2000;
	timedate->month		= RTC_DateStructure.RTC_Month;
	timedate->day		= RTC_DateStructure.RTC_Date;
	timedate->weekday	= RTC_DateStructure.RTC_WeekDay-1;
	timedate->hour		= RTC_TimeStructure.RTC_Hours;
	timedate->minute	= RTC_TimeStructure.RTC_Minutes;
	timedate->second	= RTC_TimeStructure.RTC_Seconds;
}

/**********************************************************************************************************
* 函数名称:	 void Timedate_Calibration(char *string)
* 函数说明： 时间日期校准
* 输入参数： 时间字符串，格式为 "YYYY-MM-DD HH:MM:SS"
* 返回参数： 无
**********************************************************************************************************/
void TimedateStr_Calibration(char *string)
{
	Time_t time;
	unsigned int year,month,day,hour,minute,second;
	sscanf(string,"%u-%u-%u %u:%u:%u",&year,&month,&day,&hour,&minute,&second);
	time.year	= year;
	time.month	= month;
	time.day	= day;
	time.hour	= hour;
	time.minute	= minute;
	time.second	= second;
	Get_weekday(&time);
	Timedate_Calibration(&time);
	second = TimedateToTimestamp(&time);	
	SN_Writer(second);	//写入序列号
}

/**********************************************************************************************************
* 函数名称:	 int Get_Current_TimedateStr(const char *format, char *string)
* 函数说明： 获取当前时间日期，以字符串形式输出
* 输入参数： 格式化方式（例如："%u-%02u-%02u %02u:%02u:%02u"），输出字符串缓存指针（至少20字节）
* 返回参数： 输出字符串长度
**********************************************************************************************************/
int Get_Current_TimedateStr(const char *format, char *string)
{
	Time_t time;
	Get_Current_Timedate(&time);
	return sprintf(string,format,time.year,time.month,time.day,time.hour,time.minute,time.second);		
}

/*******************************************************************************
* 函数名称:  int Get_weekday(Time_t * system_time)
* 函数说明： 获取星期
* 输入参数： 系统时间指针
* 返回参数： 0-6对应周一到周日
*******************************************************************************/
int Get_weekday(Time_t * system_time)
{
	int month = system_time->month;
	int year  = system_time->year;	
    if(month==1||month==2) {
        month+=12;
        year--;
    }
    system_time->weekday=(system_time->day+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;	
	return system_time->weekday;
}

/**********************************************************************************************************
* 函数名称:	 void TimestampToTimedata(unsigned int Timestamp, Time_t * timedate)
* 函数说明： Unix时间戳转时间日期
* 输入参数： 从1970年开始到现在的总秒数 ， 时间日期结构体指针
* 返回参数： 无
**********************************************************************************************************/
void TimestampToTimedata(unsigned int Timestamp, Time_t * timedate)
{
	unsigned int		i, day, hour;
	const unsigned char	Leap_Month[12] 	 = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	const unsigned char	Normal_Month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	Timestamp += 28800; 									//北京时间再8时区，所以加28800
	day  = Timestamp / 86400;
	hour = Timestamp % 86400;

	timedate->hour 	= hour / 3600;							//获取时
	timedate->minute = (hour % 3600) / 60;					//获取分
	timedate->second = (hour % 3600)	% 60;				//获取秒

	for (i = 1970; day >= days_in_year(i); i++)				//获取年 
	{
		day -= days_in_year(i);
	}
	timedate->year = i;

	if(leap_year(timedate->year)) 					   		//获取月 
	{
		for (i = 1; day >= Leap_Month[i-1]; i++) 
		{
			day -= Leap_Month[i-1];
		}
		timedate->month = i;
	}
	else
	{
		for (i = 1; day >= Normal_Month[i-1]; i++) 
		{
			day -= Normal_Month[i-1];
		}
		timedate->month = i;
	}
	timedate->day = day + 1;							 	//获取日
	Get_weekday(timedate);									//获取星期									
}

/**********************************************************************************************************
* 函数名称:	 unsigned int TimedateToTimestamp(Time_t * timedate)
* 函数说明： 时间日期转时间戳
* 输入参数： 时间日期结构体指针
* 返回参数： 从1970年开始到现在的总秒数
**********************************************************************************************************/
unsigned int TimedateToTimestamp(Time_t * timedate)
{
	unsigned int	second,minute,hour,day,month,year;
	second	= timedate->second;
	minute 	= timedate->minute;
	hour	= timedate->hour;
	day		= timedate->day	;
	month	= timedate->month;
	year	= timedate->year;
	if(0 >= (int)(month -= 2))
	{	
		month += 12;		
		year  -= 1;
	}
	//减28800的原因是由北京时间获取到“格林尼治标准时间”
	return ((((unsigned int)(year/4 - year/100 + year/400 + 367*month/12 + day) + year*365 - 719499)*24 + hour)*60 + minute)*60 + second -28800;
}

/**********************************************************************************************************
* 函数名称:	 unsigned int Get_Current_Timestamp(void)
* 函数说明： 获取当前时间戳，从1970年开始到现在的总秒数
* 输入参数： 无
* 返回参数： 时间戳
**********************************************************************************************************/
unsigned int Get_Current_Timestamp(void)
{	
	unsigned int second,minute,hour,day,month,year;	
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	year	= RTC_DateStructure.RTC_Year+2000;
	month	= RTC_DateStructure.RTC_Month;
	day		= RTC_DateStructure.RTC_Date;
	hour	= RTC_TimeStructure.RTC_Hours;
	minute	= RTC_TimeStructure.RTC_Minutes;
	second	= RTC_TimeStructure.RTC_Seconds;
	if(0 >= (int)(month -= 2))
	{	
		month += 12;		
		year  -= 1;
	}
	//减28800的原因是由北京时间获取到“格林尼治标准时间”
	return ((((unsigned int)(year/4 - year/100 + year/400 + 367*month/12 + day) + year*365 - 719499)*24 + hour)*60 + minute)*60 + second -28800;
}


