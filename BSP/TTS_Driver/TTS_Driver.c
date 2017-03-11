/**********************************************************************************************************
** 文件名: 		TTS_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2015年1月15日13:52:17
** 功能:		TTS 语音服务驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/
#include "Includes.h"


#define	TTS_RDY			GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) 
#define	TTS_RESET_ON	GPIOE->BSRRH = GPIO_Pin_6  
#define	TTS_RESET_OFF	GPIOE->BSRRL = GPIO_Pin_6
#define	AO_SIM800		GPIOB->BSRRH = GPIO_Pin_9  
#define	AO_OUT			GPIOB->BSRRL = GPIO_Pin_9

char 	TTS_Static;
OS_SEM 	TTS_SEM;		//语音合成信号量

/**********************************************************************************************************
* 函数名称： void TTS_START(void) 
* 函数说明： 在调用语音服务之前，先调用此函数，目的防止多任务使用语音服务，导致冲突，语音服务结束后调用TTS_END()
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_START(void) 
{
	OS_ERR err;
	OSSemPend(&	TTS_SEM,0,OS_OPT_PEND_BLOCKING,0,&err);  		//等待接收信号 才往下执行任务		
}

/**********************************************************************************************************
* 函数名称： void TTS_END(void)
* 函数说明： 语音服务结束后，调用此函数。
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_END(void)
{
	OS_ERR err;	
	OSSemPost(&	TTS_SEM,OS_OPT_POST_1,&err);					//发送信号量，释放信号量
	AO_SIM800;
}

/**********************************************************************************************************
* 函数名称： unsigned char TTS_Reset(void)
* 函数说明： 复位
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_Reset(void)
{
	OS_ERR 	err;		
	TTS_RESET_ON;
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
	TTS_RESET_OFF;
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);	
}

/*******************************************************************************
* 函数名称： void TTS_Init(void)
* 函数说明： TTS初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void TTS_Init(void)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;
	OS_ERR 					err;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;				//TTS100 RESET复位引脚
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;				//语音输出选择引脚
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5;				//TTS100 RDY状态引脚
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_Init(GPIOE, &GPIO_InitStructure);	

	UART_Init(TTS100,115200);									//TTS100串口初始化

	AO_SIM800;
	TTS_Reset();
	OSSemCreate(&TTS_SEM,"TTS_SEM",1,&err);						//创建信号量		
}

/**********************************************************************************************************
* 函数名称： int TTS_Check_Status(void)
* 函数说明： 状态查询
* 输入参数： 无
* 返回参数： 获取成功返回响应状态，获取失败返回0
**********************************************************************************************************/
int TTS_Check_Status(void)
{
	char i;
	for(i=0; i<5; i++)
	{
		UART_SendReceive(TTS100, 1000, "\xFD\x00\x01\x21", 4, &TTS_Static, 1);//发送状态查询指令
//		printf("\r\nStatus:--%02x",TTS_Static);
		if(TTS_Static==INTI_OK||TTS_Static==CMD_OK||TTS_Static==CMD_ERR||TTS_Static==BUSY||TTS_Static==IDLE)
		{		
			return TTS_Static;
		}
		if(i==2) TTS_Reset();
	}
	return 0;
}

/**********************************************************************************************************
* 函数名称： int TTS_Begin_Sleep(void)
* 函数说明： 进入睡眠
* 输入参数： 无
* 返回参数： 睡眠成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Begin_Sleep(void)
{
	char i;
	for(i=0; i<5; i++)
	{
		UART_SendReceive(TTS100, 1000, "\xFD\x00\x01\x88", 4, &TTS_Static, 1);//发送状态查询指令	
		if(TTS_Static==CMD_OK) return 1;
		if(i==2) TTS_Reset();
	}
	return 0;	
}

/**********************************************************************************************************
* 函数名称： int TTS_Begin_Wake(void)
* 函数说明： 唤醒
* 输入参数： 无
* 返回参数： 唤醒成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Begin_Wake(void)
{
	char i;
	for(i=0; i<5; i++)
	{
		UART_SendReceive(TTS100, 1000, "\xFD\x00\x01\xFF", 4, &TTS_Static, 1);	//发送唤醒指令			
		if(TTS_Static==INTI_OK) return 1;
		if(i==2) TTS_Reset();
	}
	return 0;	
}


/**********************************************************************************************************
* 函数名称： int TTS_Speech_Synthesis(char *buf, int len, char Encoding)
* 函数说明： 停止语音合成
* 输入参数： 无
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Stop_Speech_Synthesis(void)
{
	char i;
	for(i=0; i<5; i++)
	{	
		UART_SendReceive(TTS100, 1000, "\xFD\x00\x01\x02",4, &TTS_Static, 1);	//发送停止合成指令
		if(TTS_Static==CMD_OK) return 1;
		if(i==2) TTS_Reset();
	}
	return 0;
}

/**********************************************************************************************************
* 函数名称： int TTS_Speech_Synthesis(char *txt, int len, char Encoding, char outline)
* 函数说明： 语音合成
* 输入参数： 输入文本缓存指针，文本缓存区长度，编码方式（0：GB2312、1：GBK、3：BIG5、3：UNICODE、4:UTF8）,输出线路：0-SIM800,1-AOUT
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Speech_Synthesis(char *txt, int len, char Encoding, char outline)
{
	OS_ERR 			err;			
	unsigned short  size; 
	unsigned char 	*buf;
	char 			i;
	if(Encoding == 4)
	{
		buf = (unsigned char*)ram_malloc(len*2+5);	
	}
	else
	{
		buf = (unsigned char*)ram_malloc(len+5);		
	}

	if(buf == NULL)
	{
		Log_Fatal("ram_malloc failed");		
		return 0;
	}	
	if(Encoding == 4)
	{
		utf8_to_unicode_little((unsigned char*)txt, len, buf+5, &size);
		Encoding = 3;
		len = size;
	}
	else
	{
		memcpy(buf+5,txt,len);
	}
	len += 2;
	buf[0] = 0xFD;
	buf[1] = (unsigned char)(len>>8);
	buf[2] = (unsigned char)(len&0xFF);
	buf[3] = 0x01;
	buf[4] = Encoding;

	for(i=0; i<5; i++)
	{
		TTS_Stop_Speech_Synthesis();
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
		if(outline==1)	AO_OUT;
		else			AO_SIM800;		
		UART_SendReceive(TTS100, 1000, (char*)buf,len+3, &TTS_Static, 1);	//语音合成	
		if(TTS_Static==CMD_OK)
		{
			ram_free(buf);						//释放内存		
			return 1;
		}
		if(i==2) TTS_Reset();
	}
	ram_free(buf);								//释放内存	
	return 0;	
}









