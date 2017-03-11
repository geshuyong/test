/*********************************************************************************************************
** 文件名: 		DS18B20_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		DS18B20 温度传感器驱动
** 相关文件:					
** 修改日志：	 
*********************************************************************************************************/
#include "Includes.h"

unsigned int  us_base;
double 		  DS18B20_Temp[6];
unsigned char DS18B20_Status[6]={1,1,1,1,1,1};
/*********************************************************************************************************
* 函数名称： void Delay_Init(void)
* 函数说明： 延时初始化
* 输入参数： 无
* 返回参数： 无
*********************************************************************************************************/
void Delay_Init(void)
{  
    RCC_ClocksTypeDef  rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
	us_base = rcc_clocks.HCLK_Frequency/1000000;
}

/*********************************************************************************************************
* 函数名称： void Delay_us(unsigned int us)
* 函数说明： 延时X微秒
* 输入参数： 微妙数
* 返回参数： 无
*********************************************************************************************************/
void Delay_us(unsigned int us)
{  
	unsigned int reload, ticks, tstart, tnow, tcnt=0;
	ticks = us*us_base; 						//需要的节拍数	 
	reload = SysTick->LOAD;						//LOAD的值
	
	tstart = SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow!=tstart)
		{	    
			if(tnow<tstart)tcnt+=tstart-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+tstart;	    
			tstart=tnow;
			if(tcnt>=ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}  
	};	 	
}


/*********************************************************************************************************
* 函数名称： void DS18B20_Init(void)
* 函数说明： DS18B20 初始化
* 输入参数： 无
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
}

/*********************************************************************************************************
* 函数名称： void DS18B20_GPIO_High(unsigned int ch)
* 函数说明： DS18B20 数据引脚高电平输出
* 输入参数： 通道编号
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_GPIO_High(unsigned int ch)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin[ch];	
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_25MHz;    
	GPIO_Init((GPIO_TypeDef*)GPIOx[ch], &GPIO_InitStructure);
	((GPIO_TypeDef*)GPIOx[ch])->BSRRL= GPIO_Pin[ch];			
}

/*********************************************************************************************************
* 函数名称： void DS18B20_GPIO_Low(unsigned int ch)
* 函数说明： DS18B20 数据引脚低电平输出
* 输入参数： 通道编号
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_GPIO_Low(unsigned int ch)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin[ch];	
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_25MHz;    
	GPIO_Init((GPIO_TypeDef*)GPIOx[ch], &GPIO_InitStructure);
	((GPIO_TypeDef*)GPIOx[ch])->BSRRH= GPIO_Pin[ch];
}

/*********************************************************************************************************
* 函数名称： int DS18B20_GPIO_IN(unsigned int ch)
* 函数说明： DS18B20 数据引脚输入检测
* 输入参数： 通道编号
* 返回参数： 电平状态
*********************************************************************************************************/
int DS18B20_GPIO_IN(unsigned int ch)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin[ch];					
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;  
	GPIO_Init((GPIO_TypeDef*)GPIOx[ch], &GPIO_InitStructure);

	return GPIO_ReadInputDataBit((GPIO_TypeDef*)GPIOx[ch],GPIO_Pin[ch]);
}

/*********************************************************************************************************		 
* 函数名称： unsigned char DS18B20_CRC(unsigned char *RP_ByteData,unsigned char size)
* 函数说明： 利用CRC验证前8个数据（包括CRC验证码总共9个数据）
* 输入参数： 要验证的数据地址指针
* 返回参数： 验证结果  
*********************************************************************************************************/
const unsigned char  T_CrcTable[256]={
   0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
 157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
  35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
 190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
  70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
 219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
 101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
 248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
 140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
  17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
 175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
  50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
 202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
  87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
 233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
 116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53};


unsigned char DS18B20_CRC(unsigned char *RP_ByteData,unsigned char size)
{
	unsigned char i,x,CRC_Data;
    CRC_Data=0;
    for(i=0;i<size;i++) 
    {  
       x = CRC_Data ^ (*RP_ByteData);
       CRC_Data = T_CrcTable[x];
       RP_ByteData++;
    }
    return CRC_Data;
 } 

/*********************************************************************************************************
* 函数名称： int DS18B20_Reset(unsigned int ch)
* 函数说明： 复位DS18B20,并检测设备是否存在
* 输入参数： 通道编号
* 返回参数： 无
*********************************************************************************************************/			
int DS18B20_Reset(unsigned int ch)
{  
    OS_ERR   err;	
	unsigned char  i,j,k;
	for(i=0;i<10;i++)
    {
		OSSchedLock(&err);			//阻止ucos调度，防止打断us延时	
        DS18B20_GPIO_Low(ch);		//送出低电平复位信号
        Delay_us(600);              //延时至少480us
        DS18B20_GPIO_High(ch);   	//强退为高
		DS18B20_GPIO_IN(ch);  		//释放数据线
        Delay_us(100);              //等待至少60us			  
        j = DS18B20_GPIO_IN(ch);  	//检测存在脉冲
        Delay_us(600);              //等待设备释放数据线
        k = DS18B20_GPIO_IN(ch);  	//检测存在脉冲	
		OSSchedUnlock(&err);		//开启ucos调度 	
	   	if(j==0 && k==1) return 1;
    }
	return 0;						//传感器故障或不存在
}

/*********************************************************************************************************
* 函数名称： unsigned char DS18B20_Read_Byte((unsigned int ch)
* 函数说明： 从DS18B20读1字节数据
* 输入参数： 无
* 返回参数： 返回一字节数据
*********************************************************************************************************/
unsigned char DS18B20_Read_Byte(unsigned int ch)
{
    unsigned char i,dat = 0;

    for (i=0; i<8; i++)             //8位计数器
    {
        dat >>= 1;
        DS18B20_GPIO_Low(ch);  		//开始时间片
        Delay_us(2);                //延时等待
        DS18B20_GPIO_High(ch);   	//强退为高
		DS18B20_GPIO_IN(ch);  		//释放数据线
        Delay_us(20);               //接收延时
        if (DS18B20_GPIO_IN(ch)) 	//读取数据
		dat |= 0x80;        
        Delay_us(50);               //等待时间片结束
    }
    return dat;
}

/*********************************************************************************************************
* 函数名称： void DS18B20_Write_Byte(unsigned int ch,unsigned char dat)
* 函数说明： 向DS18B20写1字节数据
* 输入参数： 通道编号，dat
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_Write_Byte(unsigned int ch,unsigned char dat)
{
	unsigned char i;
    for (i=0; i<8; i++)             //8位计数器
    { 
		if(dat&0x01)
		{
			DS18B20_GPIO_Low(ch);  	//开始时间片
			Delay_us(2);       		//延时等待
			DS18B20_GPIO_High(ch);	//开始时间片
			Delay_us(65);			//等待时间片结束
		}
		else
		{
			DS18B20_GPIO_Low(ch);  	//开始时间片
			Delay_us(65);       	//延时等待
			DS18B20_GPIO_High(ch);	//开始时间片
			Delay_us(2);			//等待时间片结束
		}
		dat=dat>>1;
    }
	DS18B20_GPIO_IN(ch);  			//释放数据线	
}

/*********************************************************************************************************
* 函数名称： void DS18B20_Temp_Collection(void)
* 函数说明： DS18B20 温度采集任务
* 输入参数： 无
* 返回参数： 无
*********************************************************************************************************/
void DS18B20_Temp_Collection(void *p_arg)
{
	OS_ERR   err;
	unsigned char buf[9];
	unsigned char i,ch;
	short  Value;
	Delay_Init();
	DS18B20_Init();
	while(1)
	{
		for(ch=0; ch<6; ch++)
		{
			if(FUN_MODE[ch]==5)
			{
				if(DS18B20_Reset(ch))                          	//复位和存在脉冲检测
				{
					OSSchedLock(&err);							//阻止ucos调度，防止打断us延时			
					DS18B20_Write_Byte(ch, 0xCC);               //跳过ROM命令
					DS18B20_Write_Byte(ch, 0x44);               //开始转换命令  
					OSSchedUnlock(&err);						//开启ucos调度 				
					while(!DS18B20_GPIO_IN(ch))
					{
						OSTimeDly(100,OS_OPT_TIME_DLY,&err);
					}
					OSSchedLock(&err);							//阻止ucos调度，防止打断us延时				
					DS18B20_Reset(ch);                   		//设备复位
					DS18B20_Write_Byte(ch, 0xCC);     			//跳过ROM命令
					DS18B20_Write_Byte(ch, 0xBE);              	//读暂存存储器命令
					for(i=0;i<9;i++)
					{
						buf[i]= DS18B20_Read_Byte(ch);
					}	
					OSSchedUnlock(&err);						//开启ucos调度 				
					if(DS18B20_CRC(buf,8)==buf[8])
					{	 
						Value = buf[1]*256+buf[0];
						DS18B20_Temp[ch] = Value*0.0625;
						DS18B20_Status[ch] = 0;					
					}
				}
				else
				{
					DS18B20_Status[ch] = 1;
				}
			}
			else
			{
				DS18B20_Status[ch] = 2;
			}
		}
		OSTimeDly(100,OS_OPT_TIME_DLY,&err);
	}
}





