/**********************************************************************************************************
** 文件名: 		STM32F4_UART_Driver.h
** 版本：  		2.0
** 工作环境: 	RealView MDK-ARM 5.17
** 作者: 		盖书永 
** 生成日期: 	2016年7月6日17:19:50
** 功能:		UART初始化及接收和发送功能封装
** 相关文件:	无
** 修改日志：	无
**********************************************************************************************************/
#ifndef __STM32F4_UART_DRIVER_H
#define __STM32F4_UART_DRIVER_H

//------------------------------UART1引脚端口-----------------------------------
#if	0
//UART1_TXD
#define	UART1_TXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOA
#define UART1_TXD_GPIOX					GPIOA
#define UART1_TXD_GPIO_Pin				GPIO_Pin_9
#define UART1_TXD_GPIO_PinSource		GPIO_PinSource9
//UART1_RXD
#define	UART1_RXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOA
#define UART1_RXD_GPIOX					GPIOA
#define UART1_RXD_GPIO_Pin				GPIO_Pin_10
#define UART1_RXD_GPIO_PinSource		GPIO_PinSource10
#else
//UART1_TXD
#define	UART1_TXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOB
#define UART1_TXD_GPIOX					GPIOB
#define UART1_TXD_GPIO_Pin				GPIO_Pin_6
#define UART1_TXD_GPIO_PinSource		GPIO_PinSource6
//UART1_RXD
#define	UART1_RXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOB
#define UART1_RXD_GPIOX					GPIOB
#define UART1_RXD_GPIO_Pin				GPIO_Pin_7
#define UART1_RXD_GPIO_PinSource		GPIO_PinSource7
#endif
//-----------------------------------end----------------------------------------

//------------------------------UART2引脚端口-----------------------------------
//UART2_TXD
#define	UART2_TXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOD
#define UART2_TXD_GPIOX					GPIOD
#define UART2_TXD_GPIO_Pin				GPIO_Pin_5
#define UART2_TXD_GPIO_PinSource		GPIO_PinSource5
//UART2_RXD
#define	UART2_RXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOD
#define UART2_RXD_GPIOX					GPIOD
#define UART2_RXD_GPIO_Pin				GPIO_Pin_6
#define UART2_RXD_GPIO_PinSource		GPIO_PinSource6
//-----------------------------------end----------------------------------------

//------------------------------UART3引脚端口-----------------------------------
//UART3_TXD
#define	UART3_TXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOD
#define UART3_TXD_GPIOX					GPIOD
#define UART3_TXD_GPIO_Pin				GPIO_Pin_8
#define UART3_TXD_GPIO_PinSource		GPIO_PinSource8
//UART3_RXD
#define	UART3_RXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOD
#define UART3_RXD_GPIOX					GPIOD
#define UART3_RXD_GPIO_Pin				GPIO_Pin_9
#define UART3_RXD_GPIO_PinSource		GPIO_PinSource9
//-----------------------------------end----------------------------------------

//------------------------------UART6引脚端口-----------------------------------
//UART6_TXD
#define	UART6_TXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOC
#define UART6_TXD_GPIOX					GPIOC
#define UART6_TXD_GPIO_Pin				GPIO_Pin_6
#define UART6_TXD_GPIO_PinSource		GPIO_PinSource6
//UART6_RXD
#define	UART6_RXD_GPIO_AHBPeriph		RCC_AHB1Periph_GPIOC
#define UART6_RXD_GPIOX					GPIOC
#define UART6_RXD_GPIO_Pin				GPIO_Pin_7
#define UART6_RXD_GPIO_PinSource		GPIO_PinSource7
//-----------------------------------end----------------------------------------

typedef enum {
	RS485_1,			//RS485 1
	RS485_2,			//RS485 2
	RS485_3,			//RS485 3
	RS485_4,			//RS485 4
	RS232_1,			//RS232 1	
	RS232_2,			//RS232 2
	SIM800,				//SIM800
	TTS100,				//TTS100
	TFT, 				//串口屏
	PM100,				//电源管理
	MIAN, 				//主板
	LED, 				//LED指示灯
}UART_OPT;			//串口端口选择

/**********************************************************************************************************
* 函数名称:	void UART_Init(UART_POT port, unsigned int baud_rate)
* 函数说明：UART串口进行初始化
* 输入参数：UART端口号，波特率
* 返回参数：无
**********************************************************************************************************/
void UART_Init(UART_OPT port, unsigned int baud_rate);

/**********************************************************************************************************
* 函数名称:	void UART_BaudRate(UART_OPT port, unsigned int baud_rate)
* 函数说明：UART串口波特率设定
* 输入参数：UART端口号，波特率
* 返回参数：无
**********************************************************************************************************/
void UART_BaudRate(UART_OPT port, unsigned int baud_rate);

/**********************************************************************************************************
* 函数名称:	void UART_Send(UART_OPT port,unsigned int timeout, char *buffer, unsigned int buffer_size)
* 函数说明：UART串口发送数据，支持多线程
* 输入参数：UART端口号，发送阻塞超时时间，发送缓存区，发送数据字节数
* 返回参数：无
**********************************************************************************************************/
void UART_Send(UART_OPT port,unsigned int timeout, char *buffer, unsigned int buffer_size);

/**********************************************************************************************************
* 函数名称:	int UART_Receive(UART_OPT port, unsigned int timeout, char *buffer, unsigned short buffer_size)
* 函数说明：UART串口接收数据，接收过程会被阻塞，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，接收阻塞超时时间，接收缓存区，接收缓存区大小（单位字节）
* 返回参数：返回接收到的字节数,返回-1：接收超时，-2：无效串口号
**********************************************************************************************************/
int UART_Receive(UART_OPT port, unsigned int timeout, char *buffer, unsigned short buffer_size);

/**********************************************************************************************************
* 函数名称:	int UART_SendReceive(UART_OPT port, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
* 函数说明：USART发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，发送接收阻塞超时时间，发送缓存区，发送字节数，接收缓存区，接收缓存区大小
* 返回参数：接收到的字节数，返回-1：接收和发送超时，-2：无效串口号
**********************************************************************************************************/
int UART_SendReceive(UART_OPT port, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen);

/**********************************************************************************************************
* 函数名称:	int UartSendReceive(UART_OPT port,unsigned int baudrate, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
* 函数说明：USART发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，比特率，发送接收阻塞超时时间，发送缓存区，发送字节数，接收缓存区，接收缓存区大小
* 返回参数：接收到的字节数，返回-1：接收和发送超时，-2：无效串口号
**********************************************************************************************************/
int UartSendReceive(UART_OPT port,unsigned int baudrate, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen);

#endif

