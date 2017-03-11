/*****************************************************************************************************
** 文件名: 		AT_Server.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2015年1月15日13:52:17
** 功能:		AT服务驱动
** 相关文件:
** 修改日志：
*****************************************************************************************************/
#include "Includes.h"

#define		SIM800_STATUS			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) 

#define		SIM800_PWRKEY_ON		GPIOA->BSRRH = GPIO_Pin_3  
#define		SIM800_PWRKEY_OFF		GPIOA->BSRRL = GPIO_Pin_3 

#define		AT_R_BUF_SIZE			1500

AT_StateInfo_Def 	AT_StateInfo;				//设备状态信息
static char 		AT_R_BUF[AT_R_BUF_SIZE];	//数据缓存区
static char 		AT_T_BUF[340];				//数据缓存区
static char 		AT_CMD[64];					//数据缓存区

const char 			*CARD_STATUS[3] = {"Unknown", "Exist", "Not exist"};
const char 			*NET_REG[3] 	= {"Unknown", "Registered", "Unregistered"};
const char 			*OPERATOR[3] 	= {"Unknown", "China Mobile", "China Unicom"};
const char 			*SIGNAL_Q[4] 	= {"Unknown", "Normal", "Weak", "No signal"};

/*****************************************************************************************************
* 函数名称： void SIM800_GPIO_Init(void)
* 函数说明： SIM800初始化
* 输入参数： 无
* 返回参数： 无
*****************************************************************************************************/
void SIM800_GPIO_Init(void)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3;				//SIM800 PWRKEY开机引脚  
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;     
    GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0;				//SIM800 STATUS状态引脚
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_DOWN;			//下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	UART_Init(SIM800,115200);									//SIM800初始化

	SIM800_PWRKEY_OFF;
}

/**********************************************************************************************************
* 函数名称： void SIM800_ON(void)
* 函数说明： SIM800开机
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void SIM800_ON(void)
{
	OS_ERR 	err;
	int 	time;
	Log_Info("SIM800 ON");	
	while(1)
	{
		time = 20;
		SIM800_PWRKEY_ON;
		OSTimeDly(1500, OS_OPT_TIME_DLY, &err);
		SIM800_PWRKEY_OFF;
		OSTimeDly(3500, OS_OPT_TIME_DLY, &err);	
		while(!SIM800_STATUS && time)
		{
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			time--;
		}
		if(SIM800_STATUS && time) break;
	}
	Log_Info("SIM800 Start running...");
}

/**********************************************************************************************************
* 函数名称： void SIM800_OFF(void)
* 函数说明： SIM800关机
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void SIM800_OFF(void)
{
	OS_ERR 	err;
	int 	time;
	while(SIM800_STATUS)
	{
		time = 15;
		SIM800_PWRKEY_ON;
		OSTimeDly(1500, OS_OPT_TIME_DLY, &err);
		SIM800_PWRKEY_OFF;
		while(SIM800_STATUS && time)
		{
			OSTimeDly(200, OS_OPT_TIME_DLY, &err);
			time--;
		}
		time = 15;
	}
	Log_Info("SIM800 shutdown.");
}

/********************************************************************************;**************************
* 函数名称： void Baud_Sync(void)
* 函数说明： 波特率同步
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void Baud_Sync(void)
{
	OS_ERR err;
	int    times = 0;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT\r", 3, AT_R_BUF, AT_R_BUF_SIZE);
		if(strstr(AT_R_BUF, "OK"))	break;			   	//判断是否接收到OK
		times++;
		if(times >= 10)
		{
			Log_Info("Baud synchronization fails, begin to restart.");
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			SIM800_ON();
			times = 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： void AT_Module_Info(void)
* 函数说明： AT设备信息打印
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void AT_Module_Info(void)
{
	sprintf(AT_R_BUF, "\r\n----------------------------------------\r\nError Status:\t\t%s\r\nSim Card Status:\t%s\r\nNetwork Registration:\t%s\r\nNetwork Operators:\t%s\r\nSignal Quality:\t\t%s\r\nRSSI:\t\t\t%d\r\nBER:\t\t\t%d\r\nSMS Center:\t\t%s\r\n----------------------------------------\r\n",
		AT_StateInfo.status ? "Fault" : "Normal",
		CARD_STATUS[AT_StateInfo.card],
		NET_REG[AT_StateInfo.net_reg],
		OPERATOR[AT_StateInfo.Operator],
		SIGNAL_Q[AT_StateInfo.signal_quality],
		AT_StateInfo.rssi,
		AT_StateInfo.ber,
		AT_StateInfo.sms_center_num
	);
	Log_Info(AT_R_BUF);
}

/**********************************************************************************************************
* 函数名称： int AT_Parameter_Setting(const char *param,const char *ret)
* 函数说明： AT参数设定
* 输入参数： 设定参数 , 返回参数
* 返回参数： 返回设定结果，返回1成功，其他失败
**********************************************************************************************************/
int AT_Parameter_Setting(const char *param, const char *ret)
{
	OS_ERR err;
	int    times = 0;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, (char*)param, strlen(param), AT_R_BUF, AT_R_BUF_SIZE);
		if(strstr(AT_R_BUF, ret))
		{
			return 1;
		}
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			Log_Info("AT_Parameter_Setting fails.");
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_SIM_Card_Status(void)
* 函数说明： 获取SIM卡状态
* 输入参数： 无
* 返回参数： 返回0表示获取失败，1表示SIM已经检测到，2表示SIM卡未检测到
**********************************************************************************************************/
int AT_Get_SIM_Card_Status(void)
{
	OS_ERR 	err;
	int 	times = 0;
	char 	*buf;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CSMINS?\r", 11, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			buf = AT_R_BUF;
			while(*buf != ',' && *buf != '\0') buf++;
			if(*buf == ',')
			{
				if(*(++buf) == '1')
				{
					AT_StateInfo.card = 1;
					return 1;
				}
				else AT_StateInfo.card = 2;
			}
		}
		else AT_StateInfo.card = 0;
		times++;
		if(times >= 5)
		{
			if(AT_StateInfo.card == 0)
			{
				AT_StateInfo.status++;
				Log_Info("AT_Get_SIM_Card_Status fails.");
			}
			else if(AT_StateInfo.card == 2) Log_Info("SIM card does not exist.");
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_SIM_Card_Status(void)
* 函数说明： 获取新号质量
* 输入参数： 无
* 返回参数： 返回0表示获取失败，1表示信号强度正常，2信号强度太弱，无法进行通讯，3无信号
**********************************************************************************************************/
int AT_Get_Signal_Quality(void)
{
	int 	times = 0, rssi, ber;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CSQ\r", 7, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			sscanf(AT_R_BUF, "%*[^:]: %d,%d", &rssi, &ber);
			AT_StateInfo.rssi = rssi;
			AT_StateInfo.ber  = ber;
			if(rssi >= 0 && rssi <= 7)
			{
				AT_StateInfo.signal_quality = 2;
				return	2;
			}
			else if(rssi == 99 || ber == 99 ) 	AT_StateInfo.signal_quality = 3;
			else
			{
				AT_StateInfo.signal_quality = 1;
				return	1;
			}
			OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
		}
		else AT_StateInfo.signal_quality = 0;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.signal_quality == 0) 		Log_Info("AT_Get_Signal_Quality fails.");
			else if(AT_StateInfo.signal_quality == 3) 	Log_Info("No signal.");
			return AT_StateInfo.signal_quality;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_SIM_Card_Status(void)
* 函数说明： 获取网络注册状态
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示网络已经注册，2网络未注册
**********************************************************************************************************/
int AT_Get_Network_Reg_Status(void)
{
	int 	times = 0;
	char 	*buf;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CREG?\r", 9, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			buf = AT_R_BUF;
			while(*buf != ',' && *buf != '\0') buf++;
			if(*buf == ',')
			{
				buf++;
				if(*buf == '1' || *buf == '5')
				{
					AT_StateInfo.net_reg = 1;
					return 1;
				}
				else
				{
					AT_StateInfo.net_reg = 2;
					OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
				}
			}
			else AT_StateInfo.net_reg = 0;
		}
		else AT_StateInfo.net_reg = 0;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.net_reg == 0) 		Log_Info("AT_Get_Network_Reg_Status fails.");
			else if(AT_StateInfo.net_reg == 2)	Log_Info("Network Unregistered.");
			return AT_StateInfo.net_reg;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_SIM_Card_Status(void)
* 函数说明： 获取网路运行商
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示网络运行商为中国移动，2表示网络运行商为中国联通
**********************************************************************************************************/
int AT_Get_Network_Operator(void)
{
	int 	times = 0;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+COPS?\r", 9, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			if(strstr(AT_R_BUF, "CHINA MOBILE") != NULL)
			{
				AT_StateInfo.Operator = 1;
				if(sys_cfg.apn[0] == '\0' || strncmp(sys_cfg.apn, "UNINET", 6) == 0)
				{
					strcpy(sys_cfg.apn, "CMNET");
				}
				return 1;
			}
			else if(strstr(AT_R_BUF, "CHN-UNICOM") != NULL)
			{
				AT_StateInfo.Operator = 2;
				if(sys_cfg.apn[0] == '\0' || strncmp(sys_cfg.apn, "CMNET", 5) == 0)
				{
					strcpy(sys_cfg.apn, "UNINET");				
				}				
				return 2;
			}
			else AT_StateInfo.Operator = 0;
		}
		else AT_StateInfo.Operator = 0;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.Operator == 0) 	 Log_Info("AT_Get_Network_Operator fails.");
			return AT_StateInfo.Operator;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_SMS_Center_Number(void)
* 函数说明： 获取短信中心号码
* 输入参数： 无
* 返回参数： 返回0表示获取信息失败，1表示短信中心号码获取成功
**********************************************************************************************************/
int AT_Get_SMS_Center_Number(void)
{
	int 	times = 0;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CSCA?\r", 9, AT_R_BUF, AT_R_BUF_SIZE);	//获取短信中心号码指令
		if(AT_R_BUF[0])
		{
			AT_StateInfo.sms_center_num[0] = '\0';
			sscanf(AT_R_BUF, "%*[^\"]\"+%[^\"]", AT_StateInfo.sms_center_num);
			return 1;
		}
		else AT_StateInfo.sms_center_num[0] = '\0';
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.sms_center_num[0] == '\0') Log_Info("AT_Get_SMS_Center_Number fails.");
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_GPRS_Reg_Status(void)
* 函数说明： 获取GPRS网络注册状态
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示网络已经注册，2网络未注册
**********************************************************************************************************/
int AT_Get_GPRS_Reg_Status(void)
{
	int 	times = 0;
	char 	*buf;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CGREG?\r", 10, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			buf = AT_R_BUF;
			while(*buf != ',' && *buf != '\0') buf++;
			if(*buf == ',')
			{
				buf++;
				if(*buf == '1' || *buf == '5')
				{
					AT_StateInfo.gprs_reg = 1;
					return 1;
				}
				else
				{
					AT_StateInfo.gprs_reg = 2;
					OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
				}
			}
			else AT_StateInfo.gprs_reg = 0;
		}
		else AT_StateInfo.gprs_reg = 0;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.gprs_reg == 0) 		Log_Info("AT_Get_GPRS_Reg_Status fails.");
			else if(AT_StateInfo.gprs_reg == 2)	Log_Info("GPRS Network Unregistered.");
			return AT_StateInfo.gprs_reg;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Get_GPRS_Server_Attach_Status(void)
* 函数说明： 获取GPRS服务附着状态
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示GPRS网络已附着，2表示GPRS网络已分离
**********************************************************************************************************/
int AT_Get_GPRS_Server_Attach_Status(void)
{
	int 	times = 0;
	char 	*buf;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CGATT?\r", 10, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			buf = AT_R_BUF;
			while(*buf != ':' && *buf != '\0') buf++;
			if(*buf == ':')
			{
				buf++;
				buf++;
				if(*buf == '1')
				{
					AT_StateInfo.attach = 1;
					return 1;
				}
				else
				{
					AT_StateInfo.attach = 2;
					OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
				}
			}
			else AT_StateInfo.attach = 0;
		}
		else AT_StateInfo.attach = 0;
		times++;
		if(times >= 5)
		{
//			AT_StateInfo.status++;
			if(AT_StateInfo.attach == 0) 		Log_Info("AT_Get_GPRS_Server_Attach_Status fails.");
			else if(AT_StateInfo.attach == 2)	Log_Info("GPRS Server is detacged.");
			return AT_StateInfo.attach;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Set_APN(void)
* 函数说明： 设定APN接入点
* 输入参数： 无
* 返回参数： 返回0 APN设置失败，1表示APN设置成功
**********************************************************************************************************/
int AT_Set_APN(void)
{
	int 	times = 0, len;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		len = sprintf(AT_T_BUF, "AT+CSTT=\"%s\",\"%s\",\"%s\"\r", sys_cfg.apn, sys_cfg.apn_user, sys_cfg.apn_pwd);
		UART_SendReceive(SIM800, 1000, AT_T_BUF, len, AT_R_BUF, AT_R_BUF_SIZE);
		if(strstr(AT_R_BUF, "OK")) return 1;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			Log_Info("AT_Set_APN fails.");
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Bring_Up_GPRS_Connect(void)
* 函数说明： 激活GPRS 连接
* 输入参数： 无
* 返回参数： 返回0表示激活失败，1表示激活成功
**********************************************************************************************************/
int AT_Bring_Up_GPRS_Connect(void)
{
	UART_SendReceive(SIM800, 60000, "AT+CIICR\r", 9, AT_R_BUF, AT_R_BUF_SIZE);
	if(strstr(AT_R_BUF, "OK")) return 1;
	else return 0;
}

/**********************************************************************************************************
* 函数名称： int AT_Get_Local_IP_Address(void)
* 函数说明： 获取本地IP地址
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示IP地址获取成功
**********************************************************************************************************/
int AT_Get_Local_IP_Address(void)
{
	int 	times = 0, j, i;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CIFSR\r", 9, AT_R_BUF, AT_R_BUF_SIZE);
		if(AT_R_BUF[0])
		{
			AT_StateInfo.local_ip[0] = '\0';
			for(j = 0; (AT_R_BUF[j] > '9' || AT_R_BUF[j] < '0') && AT_R_BUF[j] != 0; j++) {}
			for(i = 0; (AT_R_BUF[j] <= '9' && AT_R_BUF[j] >= '0') || AT_R_BUF[j] == '.'; j++, i++)
			{
				AT_StateInfo.local_ip[i] = AT_R_BUF[j];
			}
			AT_StateInfo.local_ip[i] = 0;
			return 1;
		}
		else AT_StateInfo.local_ip[0] = '\0';
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			if(AT_StateInfo.local_ip[0] == '\0') 	 Log_Info("AT_Get_Local_IP_Address fails.");
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_TCP_Connection(void)
* 函数说明： 建立TCP链接
* 输入参数： 无
* 返回参数： 返回0表示获取状态失败，1表示TCP开始执行链接
**********************************************************************************************************/
int AT_TCP_Connection(void)
{
	int 	times = 0, len;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		len = sprintf(AT_T_BUF, "AT+CIPSTART=\"TCP\",\"%s\",%u\r", sys_cfg.server_ip, sys_cfg.server_port);
		UART_SendReceive(SIM800, 1000, AT_T_BUF, len, AT_R_BUF, AT_R_BUF_SIZE);
		if(strstr(AT_R_BUF, "OK"))return 1;
		if(strstr(AT_R_BUF, "ERROR"))return 0;
		times++;
		if(times >= 5)
		{
			AT_StateInfo.status++;
			return 0;
		}
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Init(void)
* 函数说明： AT模块初始化
* 输入参数： 无
* 返回参数： 返回0成功，1失败
**********************************************************************************************************/
int AT_Module_Init(void)
{
	OS_ERR 	err;
	int i=0;
	AT_StateInfo.gsm_equipment = 0;							//设备不存在
	SIM800_GPIO_Init();										//接口初始化
	SIM800_ON();											//开机
	Baud_Sync();											//波特率同步
	AT_StateInfo.gsm_equipment = 1;							//设备存在	
	for(i=0; i<20; i++)
	{
		AT_StateInfo.status = 0;							//状态清零
		AT_Parameter_Setting("AT&F0\r", "OK");				//恢复出厂值
		AT_Parameter_Setting("AT+IPR=115200\r", "OK");		//恢复出厂值
		AT_Parameter_Setting("ATE0\r",  "OK");				//关闭回显
		AT_Get_SIM_Card_Status();							//获取SIM卡是否存在
		AT_Get_Signal_Quality();							//获取信号质量
		AT_Get_Network_Reg_Status();						//获取网络运营商
		AT_Get_Network_Operator();							//获取网络运营商
		AT_Get_GPRS_Server_Attach_Status();					//获取GPRS网络附着状态
		AT_Parameter_Setting("AT+CNMI=2,1,0,0,0\r", "OK");	//短消息收到后缓存
		AT_Get_SMS_Center_Number();							//获取短信中心号码
		AT_Parameter_Setting("AT+CMGDA=6\r", "OK");			//删除所有短消息PDU模式下
		AT_Module_Info();									//打印设备信息
		if(AT_StateInfo.status == 0) return 0;
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	}
	return 1;
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Status_Check(void)
* 函数说明： AT模块状态检查
* 输入参数： 无
* 返回参数： 返回0成功，1失败
**********************************************************************************************************/
int AT_Module_Status_Check(void)
{
	int i = 0;
	for(i=0; i<10; i++)
	{
		AT_StateInfo.status = 0;							//状态清零
		if(AT_Parameter_Setting("ATE0\r",  "OK"))			//关闭回显
		AT_Get_SIM_Card_Status();							//获取SIM卡是否存在
		AT_Get_Signal_Quality();							//获取信号质量
		AT_Get_Network_Reg_Status();						//获取网络注册状态
		AT_Get_GPRS_Server_Attach_Status();					//获取GPRS网络附着状态
		if(AT_StateInfo.status == 0) return 0;
	}
	return 1;
}

#if 0
//--------------------------------------------TCP服务普通模式------------------------------------------------------

/**********************************************************************************************************
* 函数名称： void GPRS_TCP_Connect(void)
* 函数说明： 建立GPRS TCP 连接
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
int GPRS_TCP_Connect(void)
{
	int 	times = 0;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CIPSTATUS\r", 13, AT_R_BUF, AT_R_BUF_SIZE);					//获取连接状态
		if(AT_R_BUF[0])
		{
			if(strstr(AT_R_BUF, "IP INITIAL"))					//初始化
			{
				AT_Set_APN();
			}
			else if(strstr(AT_R_BUF, "IP START"))				//启动任务
			{
				AT_Bring_Up_GPRS_Connect();						//激活场景
			}
			else if(strstr(AT_R_BUF, "IP CONFIG"))				//配置场景
			{
				OSTimeDly(500, OS_OPT_TIME_DLY, &err);
			}
			else if(strstr(AT_R_BUF, "IP GPRSACT"))				//接收场景配置
			{
				AT_Get_Local_IP_Address();						//获取本地IP
			}
			else if(strstr(AT_R_BUF, "IP STATUS"))				//获得本地IP
			{
				AT_TCP_Connection();							//发送建立链接命令
			}
			else if(strstr(AT_R_BUF, "TCP CONNECTING"))			//正在建立链接
			{
				OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
			}
			else if(strstr(AT_R_BUF, "CONNECT OK"))				//链接建立成功
			{
				return 1;
			}
			else if(strstr(AT_R_BUF, "TCP CLOSING"))				//TCP连接正在关闭
			{
				OSTimeDly(500, OS_OPT_TIME_DLY, &err);
			}
			else if(strstr(AT_R_BUF, "TCP CLOSED"))				 //TCP连接断开
			{
				AT_TCP_Connection();							 //发送建立链接命令
			}
			else if(strstr(AT_R_BUF, "PDP DEACT"))				 //场景释放
			{
				AT_Parameter_Setting("AT+CIPSHUT\r", "SHUT OK");//关闭移动场景
			}
		}
		else
		{
			times++;
			if(times >= 10)AT_StateInfo.status++;
		}
		if(AT_StateInfo.status) return 0;
	}
}

/**********************************************************************************************************
* 函数名称： int GPRS_TCP_Send_Data(unsigned char *data, unsigned int size)
* 函数说明： GPRS TCP 数据发送
* 输入参数： 发送缓存区指针，而且发送缓存区必须大于size+1，因为此函数会添加一字节数据0x1A（发送指令）
* 返回参数： 返回1，发送成功，返回0，发送失败
**********************************************************************************************************/
int GPRS_TCP_Send_Data(unsigned char *data, unsigned int size)
{
	int i = 0, len, j;
	unsigned char buf;
	while(1)
	{
		UART_SendReceive(SIM800, 1000, "AT+CIPSEND?\r", 12, AT_R_BUF, AT_R_BUF_SIZE);//获取发送数据包的的大小
		for(j = 0; (AT_R_BUF[j] > '9' || AT_R_BUF[j] < '0') && AT_R_BUF[j] != 0; j++) {}
		if(AT_R_BUF[j] <= '9' && AT_R_BUF[j] >= '0')
		{
			len = atoi((char *)(AT_R_BUF + j));
		}
		else return 0;
		UART_SendReceive(SIM800, 1000, "AT+CIPSEND\r", 11, AT_R_BUF, AT_R_BUF_SIZE);	//发送数据包命令
		if(strstr(AT_R_BUF, ">") == NULL) return 0;
		if(size <= len)
		{
			buf = data[i + size];									//保存替换字符
			data[i + size] = 0x1A;
			UART_SendReceive(SIM800, 5000, (const char *)(data + i), size + 1, AT_R_BUF, AT_R_BUF_SIZE);
			data[i + size] = buf ;								//恢复替换字符
			if(strstr(AT_R_BUF, "SEND OK")) return 1;
			else	return 0;
		}
		else
		{
			buf = data[i + len];									//保存替换字符
			data[i + len] = 0x1A;
			UART_SendReceive(SIM800, 5000, (data + i), len + 1, AT_R_BUF, AT_R_BUF_SIZE);
			data[i + len] = buf ;									//恢复替换字符
			if(strstr(AT_R_BUF, "SEND OK") == NULL) return 0;
			i    = i + len;
			size = size - len;
		}
	}
}


#else
//--------------------------------------------TCP服务透明传输------------------------------------------------------

/**********************************************************************************************************
* 函数名称： void GPRS_TCP_Connect(void)
* 函数说明： 建立GPRS TCP 连接
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
int GPRS_TCP_Connect(void)
{
	int 	times = 0;
	OS_ERR 	err;
	while(1)
	{
		OSTimeDly(50, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CIPSTATUS\r", 13, AT_R_BUF, AT_R_BUF_SIZE);//获取连接状态
		if(AT_R_BUF[0])
		{
			if(strstr(AT_R_BUF, "IP INITIAL"))					//初始化
			{
				AT_Parameter_Setting("AT+CIPMODE=1\r", "OK");	//启用透明传输
				AT_Set_APN();
			}
			else if(strstr(AT_R_BUF, "IP START"))				//启动任务
			{
				AT_Bring_Up_GPRS_Connect();						//激活场景
			}
			else if(strstr(AT_R_BUF, "IP CONFIG"))				//配置场景
			{
				OSTimeDly(500, OS_OPT_TIME_DLY, &err);
			}
			else if(strstr(AT_R_BUF, "IP GPRSACT"))				//接收场景配置
			{
				AT_Get_Local_IP_Address();						//获取本地IP
			}
			else if(strstr(AT_R_BUF, "IP STATUS"))				//获得本地IP
			{
				if(AT_TCP_Connection())							//发送建立链接命令
				{
					UART_Receive(SIM800, 5000, AT_R_BUF, AT_R_BUF_SIZE);
					if(strstr(AT_R_BUF, "CONNECT"))				//链接成功
					{
						Log_Info("CONNECT OK.");
						return 1;
					}
				}
			}
			else if(strstr(AT_R_BUF, "TCP CLOSING"))			//TCP连接正在关闭
			{
				OSTimeDly(500, OS_OPT_TIME_DLY, &err);
			}
			else if(strstr(AT_R_BUF, "TCP CLOSED"))				 //TCP连接断开
			{
				AT_TCP_Connection();							 //发送建立链接命令
			}
			else if(strstr(AT_R_BUF, "PDP DEACT"))				 //场景释放
			{
				AT_Parameter_Setting("AT+CIPSHUT\r", "SHUT OK");//关闭移动场景
			}
			else												 //场景释放
			{
				AT_Parameter_Setting("AT+CIPSHUT\r", "SHUT OK");//关闭移动场景
			}
		}
		else
		{
			OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
			UART_Send(SIM800, 2000, "+++", 3);					//获取连接状态
			OSTimeDly(500, OS_OPT_TIME_DLY, &err);
			times++;
			if(times >= 10)AT_StateInfo.status++;
		}
		if(AT_StateInfo.status) return 0;
	}
}


/**********************************************************************************************************
* 函数名称： int GPRS_TCP_Send_Data(unsigned char *data, unsigned int size)
* 函数说明： GPRS TCP 数据发送
* 输入参数： 发送缓存区指针, 发送数据大小
* 返回参数： 返回1，发送成功，返回0，发送失败
**********************************************************************************************************/
int GPRS_TCP_Send_Data(unsigned char *data, unsigned int size)
{
	int i = 0;
	OS_ERR 	err;
	while(1)
	{
		if(size <= 3072)
		{
			UART_Send(SIM800, 5000, (char *)(data + i), size);
			return 1;
		}
		else
		{
			UART_Send(SIM800, 5000, (char *)(data + i), 3072);
			i    = i + 3072;
			size = size - 3072;
		}
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}


#endif

/**********************************************************************************************************
* 函数名称： int GPRS_TCP_Close(void)
* 函数说明： GPRS TCP 关闭连接
* 输入参数： 无
* 返回参数： 返回1,关闭成功； 返回0，关闭失败
**********************************************************************************************************/
int GPRS_TCP_Close(void)
{
	OS_ERR 	err;
	int 	times = 0;
	while(1)
	{
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
		UART_Send(SIM800, 2000, "+++", 3);							//获取连接状态
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 1000, "AT+CIPSTATUS\r", 13, AT_R_BUF, AT_R_BUF_SIZE);	//获取连接状态
		if(strstr(AT_R_BUF, "CONNECT OK"))							//链接建立成功
		{
			UART_Send(SIM800, 2000, "AT+CIPCLOSE=1\r", 14);			//发送关闭连接指令
		}
		else if(strstr(AT_R_BUF, "TCP CLOSING"))	;				//TCP连接正在关闭
		else if(strstr(AT_R_BUF, "TCP CLOSED"))						//TCP连接断开
		{
			return 1;
		}
		else
		{
			times++;
			if(times >= 2) return 0;
		}
	}
}

//------------------------------------------------短信服务---------------------------------------------------

/**********************************************************************************************************
* 函数名称： void HEX_to_Hexstr(unsigned char *hex, unsigned short hexsize, char *hexstr, unsigned short *hexstrsize)
* 函数说明： 字节数据转换为可打印字符串  	如：  {0xC8,0x32,0x9B,0xFD,0x0E,0x01}-->"C8329BFD0E01"
* 输入参数： hex 要转换数据的指针，hexsize 要转换数据的字节长度；hexstr 转换结果字符串指针 ，hexstrsize转换结果字符串长度
* 返回参数： 无
**********************************************************************************************************/
void HEX_to_Hexstr(unsigned char *hex, unsigned short hexsize, char *hexstr, unsigned short *hexstrsize)
{
	const  char 	Char_table[] =	"0123456789ABCDEF"; 	//0x0-0xF的字符查找表
	unsigned short  i;
	for(i = 0; i < hexsize; i++)
	{
		*hexstr++ = Char_table[*hex >> 4];   					//输出高4位
		*hexstr++ = Char_table[*hex & 0x0F]; 					//输出低4位
		hex++;
	}
	*hexstr = '\0';          								//输出字符串加个结束符
	if(hexstrsize != NULL)
	{
		*hexstrsize = hexsize * 2;   							//返回目标字符串长度
	}
}

/**********************************************************************************************************
* 函数名称： void Hexstr_to_HEX(char *hexstr, unsigned short hexstrsize, unsigned char *hex, unsigned short *hexsize )
* 函数说明： HEX字符串，转化为HEX  	如：  "C8329BFD0E01"-->{0xC8,0x32,0x9B,0xFD,0x0E,0x01}
* 输入参数： hexstr 转换结果字符串指针 ，hexstrsize转换结果字符串长度 ，hex 要转换数据的指针，hexsize 要转换数据的字节长度；
* 返回参数： 无
**********************************************************************************************************/
void Hexstr_to_HEX(char *hexstr, unsigned short hexstrsize, unsigned char *hex, unsigned short *hexsize )
{
	unsigned short i;
	unsigned char  temp1, temp2;
	for(i = 0; i < hexstrsize && *hexstr != 0; i = i + 2)
	{
		if(*hexstr <= 57)				//数字0---9
		{
			temp1 = *hexstr - 48;
		}
		else						//字母A---F
		{
			temp1 = *hexstr - 55;
		}
		hexstr++;
		if(*hexstr <= 57)				//数字0---9
		{
			temp2 = *hexstr - 48;
		}
		else						//字母A---F
		{
			temp2 = *hexstr - 55;
		}
		hexstr++;
		*hex = temp1 * 16 + temp2;
		hex++;
	}
	if(hexsize != NULL)
	{
		*hexsize = i / 2;   			//返回目标字符串长度
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Send_One_SMS(char* num, unsigned char pdu_type, char *sms, unsigned char sms_len)
* 函数说明： AT设备单条短信发送
* 输入参数： 短信号码， 长短信（0x51）和短短信(0x11)类型，unicode编码短信内容， 短信长度
* 返回参数： 返回1发送成功，返回0发送失败
**********************************************************************************************************/
int AT_Module_Send_One_SMS(char *num, unsigned char pdu_type, char *sms, unsigned char sms_len)
{
	unsigned short 	size    = 0, len;
	unsigned char 	num_len = 0;
	char			*index = AT_T_BUF;
	if(num[0] == '\0')
	{
		Log_Info("Mobile phone number is empty");
		return 0;
	}
	if(num[0] == '8' && num[1] == '6') num += 2;
	num_len = strlen(num);	//获取手机号码长度
	//----------SCA 短信中心号码处理------
	//---SCA以00写入，应用默认的服务中心地址，该服务中心地址用AT+CSCA指令写入
	*index++ = '0';
	*index++ = '0';
	//----------PDU TYPE------------------
	HEX_to_Hexstr(&pdu_type, 1, index, NULL);
	index += 2;
	//----------MR------------------------
	*index++ = '0';
	*index++ = '0';
	//----------DA 手机号码处理-----------
	if(num_len < 11)		//少于11位为本地号码
	{
		char i;
		HEX_to_Hexstr(&num_len, 1, index, NULL);
		index += 2;
		*index++ = '8';
		*index++ = '1';
		for(i = 0; i < num_len; i += 2)
		{
			*index++ = num[i + 1];
			*index++ = num[i];
		}
		if(num_len & 1)
		{
			*(index - 2) = 'F';
		}
	}
	else 			  	//11位为国际号码
	{
		char i;
		num_len += 2;
		HEX_to_Hexstr(&num_len, 1, index, NULL);
		index += 2;
		*index++ = '9';
		*index++ = '1';
		*index++ = '6';
		*index++ = '8';
		num_len -= 2;
		for(i = 0; i < num_len; i += 2)
		{
			*index++ = num[i + 1];
			*index++ = num[i];
		}
		if(num_len & 1)
		{
			*(index - 2) = 'F';
		}
	}
	//----------PID:协议标识--------------
	*index++ = '0';
	*index++ = '0';
	//----------DCS：数据编码方案---------
	*index++ = '0';
	*index++ = '8';
	//----------VP：信息有效期------------
	*index++ = '9';
	*index++ = '0';
	//----------UDL：用户数据长度---------
	HEX_to_Hexstr(&sms_len, 1, index, NULL);
	index += 2;
	//----------UD：用户数据--------------
	HEX_to_Hexstr((unsigned char *)sms, sms_len, index, &size);
	*(index + size)	= 0x1a;				//添加发送短信命令
	*(index + size + 1)	= '\0';			//添加字符串结束符
	//----------END-----------------------
	if(num_len < 11) 	size  = 11 + sms_len;
	else				size  = 15 + sms_len;
	for(num_len = 0; num_len < 2; num_len++)						//如果发送失败，尝试发送2次
	{
		AT_Parameter_Setting("AT+CMGF=0\r", "OK");					//设置消息格式为PDU模式
		len = sprintf(AT_CMD, "AT+CMGS=%u\r", size);
		UART_SendReceive(SIM800, 1000, AT_CMD, len, AT_R_BUF, AT_R_BUF_SIZE);//发送短信命令
		if(strstr(AT_R_BUF, "\x3E\x20"))
		{
			UART_SendReceive(SIM800, 30000, AT_T_BUF, strlen(AT_T_BUF), AT_R_BUF, AT_R_BUF_SIZE);	//发送短信内容
			if(strstr(AT_R_BUF, "OK"))
			{
				Log_Info("短信发送成功！");
				return 1;											//发送成功，退出此函数
			}
			UART_Send(SIM800, 2000, "\x1B", 1);						//取消发送短信,防止进入数据写入模式，各种指令失效，使之进入命令模式
			Log_Info("短信发送失败！");
		}
	}
	return 0;														//发送失败
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Send_More_SMS(char* num, char *sms, unsigned int sms_len)
* 函数说明： 多条短信发送
* 输入参数： 短信号码， unicode编码短信内容， 短信长度
* 返回参数： 返回1发送成功，返回0发送失败
**********************************************************************************************************/
int AT_Module_Send_More_SMS(char *num, char *sms, unsigned int sms_len)
{
	unsigned int	size, index , i = 0, j = 0, last_len;
	char *buf;
	static unsigned char ID = 0;
	if(sms_len <= 140)
	{
		if(AT_Module_Send_One_SMS(num, 0x11, sms, sms_len)) return 1;
		else return 0;
	}
	else
	{
		buf = (char *)ram_malloc(256);	//开辟内存空间
		if(buf == NULL)
		{
			Log_Fatal("ram_malloc failed");
			return 0;
		}
		ID++;
		if(ID == 0XFF) ID = 0;
		//------------分析短信分几条发送----------
		size 	 = sms_len / 134;
		last_len = sms_len % 134;
		if(last_len) size++;
		//-------------------END-------------------
		//---------------拆分短信进行发送----------
		for(index = 1; index <= size; index++)
		{
			buf[0] = 0x05;				//协议长度
			buf[1] = 0x00;				//表示拆分短信
			buf[2] = 0x03;				//拆分数据的长度
			buf[3] = ID;				//唯一标识
			buf[4] = size;				//共被拆分size条短信
			buf[5] = index;				//序号，这是其中的第n条短信
			last_len = sms_len - j;
			if(last_len >= 134)
			{
				for(i = 6; i < 140; i++, j++)
				{
					buf[i] = sms[j];
				}
				if(!AT_Module_Send_One_SMS(num, 0x51, buf, 140))
				{
					ram_free(buf);		//释放内存空间
					return 0;
				}
			}
			else
			{
				for(i = 6; i < last_len + 6; i++, j++)
				{
					buf[i] = sms[j];
				}
				if(AT_Module_Send_One_SMS(num, 0x51, buf, last_len + 6))
				{
					ram_free(buf);		//释放内存空间
					return 1;
				}
				else
				{
					ram_free(buf);		//释放内存空间
					return 0;				
				}					
			}
		}
		ram_free(buf);					//释放内存空间
	}
	return 0;
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Query_Read_SMS(char *num, char *sms, int *sms_len)
* 函数说明： AT设备查询短信如果收到短信解析短信内容。
* 输入参数： 短信号码， 短信内容空间， 短信字节数
* 返回参数： 返回1发送成功，返回0发送失败
**********************************************************************************************************/
int AT_Module_Query_Read_SMS(char *num, char *sms, int *sms_len)
{
	unsigned char	len, i, pdu_type, dcs;
	uint16_e		size;
//	unsigned char	scts[10];								//年月日时分秒  时区
	char			*buf;
	UART_SendReceive(SIM800, 1000, "AT+CMGL=0\r", 10, AT_R_BUF, AT_R_BUF_SIZE);	//pdu_type!都区所有接收未读的短信
	buf = strstr(AT_R_BUF, "+CMGL:");
	if(buf)
	{
		buf = strstr(buf, "\r\n") + 2;
		Hexstr_to_HEX(buf, 2, &len, NULL);		 			//分析SCA
		buf = buf + len * 2 + 2;
		Hexstr_to_HEX(buf, 2, &pdu_type, NULL);		 		//分析PDU Type
		buf = buf + 2;
		Hexstr_to_HEX(buf, 2, &len, NULL);		 			//分析OA
		buf = buf + 4;
		if(len % 2) len += 1;
		if(buf[0] == '6' && buf[1] == '8')
		{
			buf += 2;
			len -= 2;
		}
		for(i = 0; i < len; i += 2)
		{
			num[i]   = *(buf + 1 + i);
			num[i+1] = *(buf + i);
		}
		if(num[len - 1] == 'F')	num[len - 1] = '\0';
		else num[len] = '\0';
		buf = buf + len;
		if(buf[0] != '0' || buf[1] != '0')
		{
			Log_Info("SMS encoding error!");
			return 0;
		}
		buf = buf + 2;
		Hexstr_to_HEX(buf, 2, &dcs, NULL);		 		//分析DCS
		buf = buf + 2;
//		for(i=0; i<7; i++)								//分析时间戳SCTS，11600311039323? - 时间戳：11/06/30 11:30:39 32(+32时区)
//		{
//			scts[i] = (buf[i*2]-0x30) + (buf[i*2+1]-0x30)*10;
//		}
		buf = buf + 14;
		Hexstr_to_HEX(buf, 2, &len, NULL);		 		//分析UDL
		if(pdu_type & 0x40)								//长短信分析
		{
			buf = buf + 14;
			len-= 6;
			Hexstr_to_HEX(buf, len * 2, (unsigned char *)buf, NULL);
		}
		else												//短短信分析
		{
			buf = buf + 2;
			Hexstr_to_HEX(buf, len * 2, (unsigned char *)buf, NULL);
		}
		if((dcs & 0x0C) == 0x08)							//unicode编码分析
		{
			unicode_to_utf8((uint8_e *)buf, len, (uint8_e *)sms, &size);
			*sms_len  = size;
			sms[size] = '\0';
		}
		else if((dcs & 0x0C) == 0x00)						//7bit编码分析
		{
			unsigned char k, n, m1, m2, j;
			for(i = 0, j = 0; j < len; i++)
			{
				k = i / 7;
				n = i % 7;
				if(n == 0)
				{
					sms[i + k]   = buf[i] & 0x7f;
					j++;
				}
				else
				{
					m1 = 0xff >> (n + 1);
					m2 = ~(0xff >> (n));
					sms[i + k]   = ((buf[i] & m1) << n) | ((buf[i - 1] & m2) >> (8 - n));
					j++;
				}
				if(n == 6)
				{
					sms[i + k + 1] = ((buf[i] & 0xfe) >> 1);
					j++;
				}
			}
			sms[j]   = '\0';
			*sms_len = j;
		}
		else if((dcs & 0x0C) == 0x04)				//8bit编码分析
		{
			Log_Info("PDU 8bit Encoding temporarily not supported.");
			AT_Parameter_Setting("AT+CMGDA=6\r", "OK");	//删除所有短消息PDU模式下
			return 0;
		}
		AT_Parameter_Setting("AT+CMGDA=6\r", "OK");	//删除所有短消息PDU模式下
		return 1;
	}
	else return 0;
}


//------------------------------------------------电话服务---------------------------------------------------

/**********************************************************************************************************
* 函数名称： int AT_Module_Call(char* Number, char dtmf_enable)
* 函数说明： 拨打电话
* 输入参数： 电话号码, DTMF使能：0-禁止，1-启用
* 返回参数： 返回1电话接通，返回0电话无法接通
**********************************************************************************************************/
int AT_Module_Call(char *Number, char dtmf_enable)
{
	int	 len;
	if(Number[0]=='\0')
	{
		Log_Debug("Mobile phone number is empty");
		return 0;
	}	
	if(Number[0]=='8'&&Number[1]=='6') Number +=2;	
	if(dtmf_enable == 1)
	{
		AT_Parameter_Setting("AT+DDET=1\r", "OK");		//使能DTMF检测功能
	}
	else
	{
		AT_Parameter_Setting("AT+DDET=0\r", "OK");		//禁止DTMF检测功能
	}
	AT_Parameter_Setting("AT+COLP=1\r", "OK");			//设置被叫号码显示
	len = sprintf(AT_CMD, "ATD%s;\r", Number);
	UART_SendReceive(SIM800, 70000, AT_CMD, len, AT_R_BUF, AT_R_BUF_SIZE);	//发送拨号命令
	if(strstr(AT_R_BUF, "OK"))
	{
		Log_Info("Telephone connection.");
		return 1;										//电话接通，退出此函数
	}
	else
	{
		Log_Info("The phone can't get through.");
		return 0;										//电话无法接通，退出此函数
	}
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Query_Answer_Phone(void)
* 函数说明： 查询是否有电话呼叫并接听电话
* 输入参数： 无
* 返回参数： 返回1已经接通，返回0无电话呼叫
**********************************************************************************************************/
int AT_Module_Query_Answer_Phone(void)
{
	UART_SendReceive(SIM800, 1000, "AT+CPAS\r", 8, AT_R_BUF, AT_R_BUF_SIZE);	//查询是否有电话呼叫
	if(strstr(AT_R_BUF, "+CPAS: 3"))
	{
		if(AT_Parameter_Setting("ATA\r", "OK")) return 1;
	}
	return 0;
}

/**********************************************************************************************************
* 函数名称： void AT_Module_Hang_Up_Phone(void)
* 函数说明： 挂断电话
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void AT_Module_Hang_Up_Phone(void)
{
	AT_Parameter_Setting("ATH\r", "OK");
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Get_Call_State(void)
* 函数说明： 获取通话状态
* 输入参数： 无
* 返回参数： 返回0空闲状态，1振铃状态，2通话状态，3未知
**********************************************************************************************************/
int AT_Module_Get_Call_State(void)
{
	UART_SendReceive(SIM800, 1000, "AT+CPAS\r", 8, AT_R_BUF, AT_R_BUF_SIZE);	//查询是否有电话呼叫
	if(strstr(AT_R_BUF, "+CPAS: 0")) 	 return 0;		//空闲状态
	else if(strstr(AT_R_BUF, "+CPAS: 3")) return 1;		//振铃状态
	else if(strstr(AT_R_BUF, "+CPAS: 4")) return 2;		//通话中
	else return 3;										//未知
}

/**********************************************************************************************************
* 函数名称： char AT_Module_DTMF_Decoder(void)
* 函数说明： 查询并DTMF解码
* 输入参数： 无
* 返回参数： 成功返回按键值，失败返回0
**********************************************************************************************************/
char AT_Module_DTMF_Decoder(void)
{
	char *index;
	if(UART_Receive(SIM800, 3000, AT_R_BUF, AT_R_BUF_SIZE)) //接收返回状态，等待延时2秒
	{
		index = strstr(AT_R_BUF, "+DTMF:");
		if(index) return *(index + 7);
	}
	return 0;
}

//------------------------------------------------FTP服务---------------------------------------------------

/**********************************************************************************************************
* 函数名称： char *get_error_reason(int code)
* 函数说明： 根据状态码获取状态原因短语
* 输入参数： 状态码
* 返回参数： 返回原因短语
**********************************************************************************************************/
char *get_error_reason(int code)
{
	static const struct
	{
		int     code;
		char   *str;
	} msg[] =
	{
		{ 61, "Net error" },
		{ 62, "DNS error" },
		{ 63, "Connect error" },
		{ 64, "Timeout" },
		{ 65, "Server error" },
		{ 66, "Operation not allow" },
		{ 70, "Replay error" },
		{ 71, "User error" },
		{ 72, "Password error" },
		{ 73, "Type error" },
		{ 74, "Rest error" },
		{ 75, "Passive error" },
		{ 76, "Active error" },
		{ 77, "Operate error" },
		{ 78, "Upload error" },
		{ 79, "Download error" }
	};
	int i;
	for (i = 0; i < sizeof(msg) / sizeof(*msg); ++i)
	{
		if (msg[i].code == code)
			return msg[i].str;
	}
	return "Unknown";
}


/**********************************************************************************************************
* 函数名称： int AT_Module_Bearer_Config(void)
* 函数说明： 承载配置
* 输入参数： 无
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int AT_Module_Bearer_Config(void)
{
	OS_ERR 	err;
	int 	i, j;
	char 	*index;
	for(i = 0; i < 5; i++)
	{
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
		UART_SendReceive(SIM800, 2000, "AT+SAPBR=2,1\r", 13, AT_R_BUF, AT_R_BUF_SIZE);	//承载状态查询命令
		index = strstr(AT_R_BUF, "OK");
		if(index)
		{
			index = strstr(AT_R_BUF, "+SAPBR: 1,");
			index += 10;
			j = atoi(index);
			switch(j)
			{
				case 0:
//					printf("\r\nAT+SAPBR=2,1  status:Connecting");
					break;
				case 1:
//					printf("\r\nAT+SAPBR=2,1  status:Already connected");
					return 1;
				case 2:
//					printf("\r\nAT+SAPBR=2,1  status:Disconnecting");
					break;
				case 3:
//					printf("\r\nAT+SAPBR=2,1  status:Has been disconnected");
					AT_Parameter_Setting("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r", "OK");
					sprintf(AT_CMD, "AT+SAPBR=3,1,\"APN\",\"%s\"\r", sys_cfg.apn);
					AT_Parameter_Setting(AT_CMD, "OK");
					sprintf(AT_CMD, "AT+SAPBR=3,1,\"USER\",\"%s\"\r", sys_cfg.apn_user);
					AT_Parameter_Setting(AT_CMD, "OK");
					sprintf(AT_CMD, "AT+SAPBR=3,1,\"PWD\",\"%s\"\r", sys_cfg.apn_pwd);
					AT_Parameter_Setting(AT_CMD, "OK");
					UART_SendReceive(SIM800, 10000, "AT+SAPBR=1,1\r", 13, AT_R_BUF, AT_R_BUF_SIZE);							//开启承载
					if(!strstr(AT_R_BUF, "OK"))
					{
						Log_Info("Bearer open failed,error:%s", AT_R_BUF);
					}
					break;
				default:
					Log_Info("AT+SAPBR=2,1  status:%s", AT_R_BUF);
					break;
			}
		}
	}
	AT_Parameter_Setting("AT+SAPBR=0,1\r", "OK");								//关闭承载
	Log_Info("Bearing failure.");
	return 0;																	//承载打开失败，退出此函数
}

/**********************************************************************************************************
* 函数名称： int AT_Module_FTP_Update(char *filepath, char *filename, char *verdate)
* 函数说明： FTP升级
* 输入参数： 升级FTP文件路径，升级文件名，升级文件版本日期
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int AT_Module_FTP_Update(char *filepath, char *filename, char *verdate)
{
	unsigned int  i, j, len = 0, size = 0, nSector = 0, crc = 1;
	char	*buf, *index;
	OS_ERR err;
	if(!AT_Module_Bearer_Config()) return 0;
	AT_Parameter_Setting("AT+FTPCID=1\r", "OK");							//设置FTP承载文件标识
	AT_Parameter_Setting("AT+FTPPORT=21\r", "OK");							//设置FTP服务器端口
	AT_Parameter_Setting("AT+FTPSERV=\"cloudiot.cc\"\r", "OK");			//设置FTP服务器地址
	AT_Parameter_Setting("AT+FTPUN=\"anonymous\"\r", "OK");					//设置FTP用户名
	AT_Parameter_Setting("AT+FTPPW=\"******\"\r", "OK");					//设置FTP口令
	snprintf(AT_T_BUF, 50,"AT+FTPGETPATH=\"/ROM_IMG/%s/\"\r", filepath);	//设置下载路径
	AT_Parameter_Setting(AT_T_BUF, "OK");
	AT_Parameter_Setting("AT+FTPGETNAME=\"Version.txt\"\r", "OK");			//获取当前版本
	if(!AT_Parameter_Setting("AT+FTPGET=1\r", "OK")) return 0;				//打开FTP会话
	UART_Receive(SIM800, 60000, AT_R_BUF, AT_R_BUF_SIZE);					//接收返回状态，等待延时30秒
	if(strstr(AT_R_BUF, "+FTPGET:1,1"))
	{
		while(1)
		{
			UART_SendReceive(SIM800, 1000, "AT+FTPGET=2,1460\r", 17, AT_R_BUF, AT_R_BUF_SIZE);//开始下载文件
			if(strstr(AT_R_BUF, "Date"))
			{
				//printf("\r\n%s",AT_R_BUF);
				if(strstr(AT_R_BUF, verdate))	return 0;
				else break;
			}
			if(strstr(AT_R_BUF, "+FTPGET:1,0"))  return 0;
			UART_SendReceive(SIM800, 1000, "AT+FTPSTATE\r", 12, AT_R_BUF, AT_R_BUF_SIZE);//查询FTP状态
			if(strstr(AT_R_BUF, "+FTPSTATE: 0")) return 0;
			OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
		}
	}
	else
	{
		index = strstr(AT_R_BUF, "+FTPGET:1,");
		if(index)
		{
			i = atoi(index + 10);
			Log_Info("FTPGET ERROR:%s", get_error_reason(i));
		}
		else
		{
			Log_Info("FTPGET ERROR:Unknown,%s", AT_R_BUF);
		}
		return 0;
	}
	Log_Info("Upgrade image found new version, start the download.");
	while(1)
	{
		UART_SendReceive(SIM800, 1000, "AT+FTPGET=2,1460\r", 17, AT_R_BUF, AT_R_BUF_SIZE);	//开始下载文件
		if(strstr(AT_R_BUF, "+FTPGET:1,0"))  break;
		UART_SendReceive(SIM800, 1000, "AT+FTPSTATE\r", 12, AT_R_BUF, AT_R_BUF_SIZE);		//查询FTP状态
		if(strstr(AT_R_BUF, "+FTPSTATE: 0")) break;
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
	snprintf(AT_CMD, 50, "AT+FTPGETNAME=\"%s\"\r", filename);			//设置下载文件名
	AT_Parameter_Setting(AT_CMD, "OK");
	AT_Parameter_Setting("AT+FTPSIZE\r", "OK");							//获取文件大小
	UART_Receive(SIM800, 30000, AT_R_BUF, AT_R_BUF_SIZE);				//接收返回状态，等待延时30秒
	buf = strstr(AT_R_BUF, "+FTPSIZE:1,0,");
	if(buf)
	{
		size = atoi(buf + 13);
//		printf("\r\nsize=%d",size);
		if(!AT_Parameter_Setting("AT+FTPGET=1\r", "OK")) return 0;		//打开FTP会话
		UART_Receive(SIM800, 60000, AT_R_BUF, AT_R_BUF_SIZE);			//接收返回状态，等待延时30秒
		if(strstr(AT_R_BUF, "+FTPGET:1,1"))
		{
			buf = (char *)ram_malloc(6000);
			if(buf)
			{
				i = 0;
				Log_Info("Start download image files.");
				while(1)
				{
					OSTimeDly(50, OS_OPT_TIME_DLY, &err);
					j = UART_SendReceive(SIM800, 1000, "AT+FTPGET=2,1460\r", 17, AT_R_BUF, AT_R_BUF_SIZE);	//开始下载文件
					if(!(AT_R_BUF[j - 4] == 'O' && AT_R_BUF[j - 3] == 'K' && AT_R_BUF[j - 2] == 0x0D && AT_R_BUF[j - 1] == 0x0A))
					{
						Log_Info("Receive data error.");
					}
					index = strstr(AT_R_BUF, "+FTPGET:");
					if(index)
					{
						index += 8;
						if(index[0] == '2')
						{
							index += 2;
							j = atoi(index);
							if(j)
							{
								while(!(index[0] == 0x0D && index[1] == 0x0A)) index++;
								index += 2;
								memcpy(buf + i, index, j);
								len += j;
								i   += j;
								if(i >= 4096)
								{
									Flash2_Write_Sector2(nSector, (unsigned char *)buf);
									nSector++;
									j     = i - 4096;
									index = buf + 4096;
									for(i = 0; i < j; i++)
									{
										buf[i] = index[i];
									}
								}
							}
						}
						else if(index[0] == '1')
						{
							index += 2;
							crc = atoi(index);
							if(crc)
							{
								Log_Info("AT+FTPGET=2,1460 ERROR:%s", get_error_reason(crc));
								break;
							}
						}
						if(len >= size || crc == 0)
						{
							if(i > 0)
							{
								Flash2_Write_Sector2(nSector, (unsigned char *)buf);
								nSector++;
							}
							Log_Info("Image file download is complete, the size of %d bytes", len);
							for(i = 0; i < nSector - 1; i++)
							{
								Flash2_Read_Sector(i, (unsigned char *)buf);
								if(i == 0)
								{
									crc = STM32F4_CRC32_DMA((unsigned int *)buf, 1024, 1);
								}
								else
								{
									crc = STM32F4_CRC32_DMA((unsigned int *)buf, 1024, 0);
								}
							}
							if((len - i * 4096 - 4) > 0)
							{
								Flash2_Read_Sector(i, (unsigned char *)buf);
								crc = STM32F4_CRC32_DMA((unsigned int *)buf, (len - i * 4096) / 4 - 1, 0);
								i = ((unsigned int *)buf)[(len - i * 4096) / 4 - 1];
							}
							else
							{
								Flash2_Read_Sector(i, (unsigned char *)buf);
								i = ((unsigned int *)buf)[0];
							}
//							printf("\r\nCRC-32:\t\t%X\r\n",crc);
//							printf("File_CRC-32:\t%X\r\n",i);
							if(crc == i)
							{
								BootLoader_Mode = Upgrade_EN;			//置升级标志位
								ram_free(buf);
								Log_Info("Image file verification is successful, begin to restart the system.");
								NVIC_SystemReset();	   					//复位
								return 1;
							}
							else
							{
								Log_Info("Image file checksum error.");
								break;
							}
						}
					}
					else
					{
						Log_Info("FTPGET ERROR:Unknown,%s", AT_R_BUF);
						break;
					}
				}
				ram_free(buf);
			}
			else
			{
				Log_Fatal("ram_malloc failed");
			}
		}
		else
		{
			index = strstr(AT_R_BUF, "+FTPGET:1,");
			if(index)
			{
				i = atoi(index + 10);
				Log_Info("FTPGET ERROR:%s", get_error_reason(i));
			}
			else
			{
				Log_Info("FTPGET ERROR:Unknown,%s", AT_R_BUF);
			}
		}
	}
	else
	{
		index = strstr(AT_R_BUF, "+FTPSIZE:1,");
		if(index)
		{
			i = atoi(index + 11);
			Log_Info("FTPSIZE ERROR:%s", get_error_reason(i));
		}
		else
		{
			Log_Info("FTPSIZE ERROR:Unknown,%s", AT_R_BUF);
		}
	}
	return 0;
}

//------------------------------------------------HTTP服务---------------------------------------------------
/**********************************************************************************************************
* 函数名称： int AT_Module_Http_Post(char* data, int size)
* 函数说明： HTTP POST提交数据
* 输入参数： 数据缓存区指针， 发送数据的大小
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int AT_Module_Http_Post(char *data, int size)
{
	char *index;
	if(!AT_Module_Bearer_Config()) return 0;
	AT_Parameter_Setting("AT+HTTPINIT\r", "OK");									//初始化HTTP服务
	AT_Parameter_Setting("AT+HTTPPARA = \"CID\",1\r", "OK");						//设置HTTP参数
	AT_Parameter_Setting("AT+HTTPPARA = \"CONTENT\",\"application/octet-stream\"\r", "OK");	//设置HTTP参数
	AT_Parameter_Setting("AT+HTTPPARA = \"UA\",\"Cloud IOT-200\"\r", "OK");			//设置HTTP参数
	AT_Parameter_Setting("AT+HTTPPARA = \"TIMEOUT\",30\r", "OK");					//设置HTTP参数
	sprintf(AT_CMD, "AT+HTTPPARA = \"URL\",\"http://%s:%u/Cloud-IOT-200\"\r", sys_cfg.server_ip, sys_cfg.server_port);
	AT_Parameter_Setting(AT_CMD, "OK");	//设置HTTP参数
	sprintf(AT_CMD, "AT+HTTPDATA=%d,5000\r", size);
	if(AT_Parameter_Setting(AT_CMD, "DOWNLOAD"))									//设置HTTP参数
	{
		UART_SendReceive(SIM800, 5100, data, size, AT_R_BUF, AT_R_BUF_SIZE);
		if(strstr(AT_R_BUF, "OK"))													//发送完毕
		{
			AT_Parameter_Setting("AT+HTTPACTION=1\r", "OK");						//POST提交数据
			UART_Receive(SIM800, 31000, AT_R_BUF, AT_R_BUF_SIZE);			//接收返回状态
			index = strstr(AT_R_BUF, "200,");
			if(index)
			{
				index += 4;
				return atoi(index);
			}
			else
			{
				printf("\r\nHTTP POST Failed.");			
			}
		}
	}
	AT_Parameter_Setting("AT+HTTPTERM\r", "OK");									//关闭HTTP服务
	return 0;
}

/**********************************************************************************************************
* 函数名称： int AT_Module_Http_READ(char *data, int size, char** buf)
* 函数说明： 读取HTTP 求情返回的数据
* 输入参数： 数据缓存区指针, 缓存区大小，输出数据缓存区指针
* 返回参数： 返回接收数据长度
**********************************************************************************************************/
int AT_Module_Http_READ(char *data, int size, char** buf)
{
	int  i;
	char *index;
	i = sprintf(AT_CMD, "AT+HTTPREAD=0,%d\r", size);
	UART_SendReceive(SIM800, 5000, AT_CMD, i, data, size);
	AT_Parameter_Setting("AT+HTTPTERM\r", "OK");									//关闭HTTP服务
	index = strstr(data, "HTTPREAD:");
	if(index)
	{
		index += 9;
		i = atoi(index);
		index = strstr(index, "\r\n") + 2;
		index[i] = '\0';
		*buf = index;
//		printf("\r\nsize=%d\r\n",i);		
		return i;
	}
	Log_Info("HTTP read failure.");
	return 0;
}






