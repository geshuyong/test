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
#include "Includes.h"

static OS_SEM	 	uart1_txd_sem;
static OS_SEM	 	uart2_txd_sem;
static OS_SEM	 	uart3_txd_sem;
static OS_SEM	 	uart6_txd_sem;

static OS_SEM	 	uart1_rxd_sem;
static OS_SEM	 	uart2_rxd_sem;
static OS_SEM	 	uart3_rxd_sem;
static OS_SEM	 	uart6_rxd_sem;

static OS_MUTEX		uart1_mutex;
static OS_MUTEX		uart2_mutex;
static OS_MUTEX		uart3_mutex;
static OS_MUTEX		uart6_mutex;


static unsigned char init_flag = 0;	//信号量和互斥信号量创建标志位，防止重复重建。

/**********************************************************************************************************
* 函数名称：void STM32F4_UART1_Init(unsigned int BaudRate)
* 函数说明：串口1初始化，默认8数据位，1停止位，无校验位
* 输入参数：波特率
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART1_Init(unsigned int BaudRate)
{
	OS_ERR	err;
    GPIO_InitTypeDef    	GPIO_InitStructure;
    USART_InitTypeDef   	USART_InitStructure; 
	NVIC_InitTypeDef		NVIC_InitStructure; 
	if(!(init_flag & 0x01))
	{
		init_flag |= 0x01;
		OSSemCreate(&uart1_txd_sem, "uart1_txd_sem", 0, &err);		
		OSSemCreate(&uart1_rxd_sem, "uart1_rxd_sem", 0, &err);	
		OSMutexCreate(&uart1_mutex, "uart1_mutex", &err);		
	}	
	OSMutexPend(&uart1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);		
	
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    RCC_AHB1PeriphClockCmd(UART1_TXD_GPIO_AHBPeriph|UART1_RXD_GPIO_AHBPeriph,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(UART1_TXD_GPIOX, UART1_TXD_GPIO_PinSource, GPIO_AF_USART1);
    GPIO_PinAFConfig(UART1_RXD_GPIOX, UART1_RXD_GPIO_PinSource, GPIO_AF_USART1);
    
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType		= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Pin			= UART1_TXD_GPIO_Pin;    
    GPIO_Init(UART1_TXD_GPIOX, &GPIO_InitStructure);	
    GPIO_InitStructure.GPIO_Pin			= UART1_RXD_GPIO_Pin; 
    GPIO_Init(UART1_RXD_GPIOX, &GPIO_InitStructure);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,  ENABLE);	
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_25MHz; 
    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_8;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化RS485_EN1控制引脚
	
    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_1;    
    GPIO_Init(GPIOD, &GPIO_InitStructure);							//初始化RS485_EN2控制引脚

    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_14|GPIO_Pin_15;    
    GPIO_Init(GPIOE, &GPIO_InitStructure);							//初始化UART1_SW控制引脚

    USART_InitStructure.USART_BaudRate	= BaudRate;					//比特率设定
    USART_InitStructure.USART_WordLength= USART_WordLength_8b;		//数据位长度
    USART_InitStructure.USART_StopBits	= USART_StopBits_1;			//停止位
    USART_InitStructure.USART_Parity	= USART_Parity_No;			//校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
    USART_InitStructure.USART_Mode		= USART_Mode_Rx | USART_Mode_Tx;//收发模式      
    USART_Init(USART1, &USART_InitStructure);						//初始化
    USART_Cmd(USART1, ENABLE);										//使能串口
	
	OSMutexPost(&uart1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);		
}

/**********************************************************************************************************
* 函数名称：void STM32F4_UART2_Init(unsigned int BaudRate)
* 函数说明：串口2初始化，默认8数据位，1停止位，无校验位
* 输入参数：波特率
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART2_Init(unsigned int BaudRate)
{
	OS_ERR	err;
    GPIO_InitTypeDef    	GPIO_InitStructure;
    USART_InitTypeDef   	USART_InitStructure; 
	NVIC_InitTypeDef		NVIC_InitStructure; 
	if(!(init_flag & 0x02))
	{
		init_flag |= 0x02;
		OSSemCreate(&uart2_txd_sem, "uart2_txd_sem", 0, &err);		
		OSSemCreate(&uart2_rxd_sem, "uart2_rxd_sem", 0, &err);	
		OSMutexCreate(&uart2_mutex, "uart2_mutex", &err);
	}
	OSMutexPend(&uart2_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 

    RCC_AHB1PeriphClockCmd(UART2_TXD_GPIO_AHBPeriph|UART2_RXD_GPIO_AHBPeriph,  ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_PinAFConfig(UART2_TXD_GPIOX, UART2_TXD_GPIO_PinSource, GPIO_AF_USART2);
    GPIO_PinAFConfig(UART2_RXD_GPIOX, UART2_RXD_GPIO_PinSource, GPIO_AF_USART2);
    
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType		= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Pin			= UART2_TXD_GPIO_Pin;    
    GPIO_Init(UART2_TXD_GPIOX, &GPIO_InitStructure);	
    GPIO_InitStructure.GPIO_Pin			= UART2_RXD_GPIO_Pin; 
    GPIO_Init(UART2_RXD_GPIOX, &GPIO_InitStructure);	

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD,  ENABLE);	
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_25MHz; 	
    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_0|GPIO_Pin_13;    
    GPIO_Init(GPIOD, &GPIO_InitStructure);							//初始化RS485_EN1和RS485_EN2控制引脚
		
    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_0|GPIO_Pin_1;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化UART2_SW控制引脚	
	
    USART_InitStructure.USART_BaudRate  = BaudRate;					//比特率设定
    USART_InitStructure.USART_WordLength= USART_WordLength_8b;		//数据位长度
    USART_InitStructure.USART_StopBits  = USART_StopBits_1;			//停止位
    USART_InitStructure.USART_Parity    = USART_Parity_No;			//校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
    USART_InitStructure.USART_Mode      = USART_Mode_Rx | USART_Mode_Tx;//收发模式      
    USART_Init(USART2, &USART_InitStructure);						//初始化
    USART_Cmd(USART2, ENABLE);										//使能串口

	OSMutexPost(&uart2_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称：void STM32F4_UART3_Init(unsigned int BaudRate)
* 函数说明：串口3初始化，默认8数据位，1停止位，无校验位
* 输入参数：波特率
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART3_Init(unsigned int BaudRate)
{
	OS_ERR	err;
    GPIO_InitTypeDef    	GPIO_InitStructure;
    USART_InitTypeDef   	USART_InitStructure; 
	NVIC_InitTypeDef		NVIC_InitStructure; 
	if(!(init_flag & 0x04))
	{
		init_flag |= 0x04;
		OSSemCreate(&uart3_txd_sem, "uart3_txd_sem", 0, &err);		
		OSSemCreate(&uart3_rxd_sem, "uart3_rxd_sem", 0, &err);	
		OSMutexCreate(&uart3_mutex, "uart3_mutex", &err);
	}
	OSMutexPend(&uart3_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);		
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

    RCC_AHB1PeriphClockCmd(UART3_TXD_GPIO_AHBPeriph|UART3_RXD_GPIO_AHBPeriph,  ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_PinAFConfig(UART3_TXD_GPIOX, UART3_TXD_GPIO_PinSource, GPIO_AF_USART3);
    GPIO_PinAFConfig(UART3_RXD_GPIOX, UART3_RXD_GPIO_PinSource, GPIO_AF_USART3);
    
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType		= GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Pin			= UART3_TXD_GPIO_Pin;    
    GPIO_Init(UART3_TXD_GPIOX, &GPIO_InitStructure);	
    GPIO_InitStructure.GPIO_Pin			= UART3_RXD_GPIO_Pin; 
    GPIO_Init(UART3_RXD_GPIOX, &GPIO_InitStructure);	
	
    USART_InitStructure.USART_BaudRate  = BaudRate;					//比特率设定
    USART_InitStructure.USART_WordLength= USART_WordLength_8b;		//数据位长度
    USART_InitStructure.USART_StopBits  = USART_StopBits_1;			//停止位
    USART_InitStructure.USART_Parity    = USART_Parity_No;			//校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
    USART_InitStructure.USART_Mode      = USART_Mode_Rx | USART_Mode_Tx;//收发模式      
    USART_Init(USART3, &USART_InitStructure);						//初始化
    USART_Cmd(USART3, ENABLE);										//使能串口

	OSMutexPost(&uart3_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称：void STM32_UART6_Init(unsigned int BaudRate)
* 函数说明：串口6初始化，默认8数据位，1停止位，无校验位
* 输入参数：波特率
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART6_Init(unsigned int BaudRate)
{
	OS_ERR	err;
    GPIO_InitTypeDef    	GPIO_InitStructure;
    USART_InitTypeDef   	USART_InitStructure; 
	NVIC_InitTypeDef		NVIC_InitStructure; 
	if(!(init_flag & 0x08))
	{
		init_flag |= 0x08;
		OSSemCreate(&uart6_txd_sem, "uart6_txd_sem", 0, &err);	
		OSSemCreate(&uart6_rxd_sem, "uart6_rxd_sem", 0, &err);	
		OSMutexCreate(&uart6_mutex, "uart6_mutex", &err);
	}
	OSMutexPend(&uart6_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);		
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

 	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);  

    RCC_AHB1PeriphClockCmd(UART6_TXD_GPIO_AHBPeriph|UART6_RXD_GPIO_AHBPeriph,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_PinAFConfig(UART6_TXD_GPIOX, UART6_TXD_GPIO_PinSource, GPIO_AF_USART6);
    GPIO_PinAFConfig(UART6_RXD_GPIOX, UART6_RXD_GPIO_PinSource, GPIO_AF_USART6);
    
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType		= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Pin			= UART6_TXD_GPIO_Pin;    
    GPIO_Init(UART6_TXD_GPIOX, &GPIO_InitStructure);	
    GPIO_InitStructure.GPIO_Pin			= UART6_RXD_GPIO_Pin; 
    GPIO_Init(UART6_RXD_GPIOX, &GPIO_InitStructure);	

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,  ENABLE);	
    GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_2MHz; 	
    GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_0|GPIO_Pin_2;    
    GPIO_Init(GPIOC, &GPIO_InitStructure);							//初始化SW控制引脚

    USART_InitStructure.USART_BaudRate	= BaudRate;					//比特率设定
    USART_InitStructure.USART_WordLength= USART_WordLength_8b;		//数据位长度
    USART_InitStructure.USART_StopBits  = USART_StopBits_1;			//停止位
    USART_InitStructure.USART_Parity    = USART_Parity_No;			//校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
    USART_InitStructure.USART_Mode      = USART_Mode_Rx | USART_Mode_Tx;//收发模式      
    USART_Init(USART6, &USART_InitStructure);						//初始化
    USART_Cmd(USART6, ENABLE);										//使能串口

	OSMutexPost(&uart6_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称:	void STM32F4_UART1_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA进行发送。支持多线程，发送过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，发送缓存区指针，要发送的字节数(最大可传输65535字节)，发送阻塞超时时间
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART1_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;

	OSMutexPend(&uart1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream7);

	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART1->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;	
	DMA_Init(DMA2_Stream7,&DMA_InitStructure);									//初始化DMA通道

	GPIOB->BSRRH = GPIO_Pin_8;													//置RS485_EN1引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN2引脚为低电平，使485为接收模式
	switch(port)
	{
		case 1:
			GPIOB->BSRRL = GPIO_Pin_1;											//置RS485_EN引脚为高电平，使RS485_1为发送模式
			GPIOE->BSRRH = GPIO_Pin_14;											//UART1_SW0置低电平
			GPIOE->BSRRL = GPIO_Pin_15;											//UART1_SW1置高电平
			break;
		case 2:
			GPIOD->BSRRL = GPIO_Pin_1;											//置RS485_EN引脚为高电平，使RS485_2为发送模式
			GPIOE->BSRRL = GPIO_Pin_14;											//UART1_SW0置高电平
			GPIOE->BSRRH = GPIO_Pin_15;											//UART1_SW1置低电平		
			break;
		case 3:
			GPIOE->BSRRH = GPIO_Pin_14;											//UART1_SW0置低电平
			GPIOE->BSRRH = GPIO_Pin_15;											//UART1_SW1置低电平		
			break;	
		default:
			GPIOE->BSRRL = GPIO_Pin_14;											//UART1_SW0置高电平
			GPIOE->BSRRL = GPIO_Pin_15;											//UART1_SW1置高电平					
			break;
	}

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART1,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA2_Stream7,ENABLE);												//启用DMA传输
	OSSemSet(&uart1_txd_sem, 0, &err);	
	if(timeout==0)timeout=2000;
	OSSemPend(&uart1_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间，防止硬件错误导致导致资源无法释放

	GPIOB->BSRRH = GPIO_Pin_8;													//置RS485_EN1引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN2引脚为低电平，使485为接收模式
	
	OSMutexPost(&uart1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART1_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA接收数据放入指定大小的缓存区。支持多线程，接收过程中线程会线程会被阻塞
* 输入参数：接收缓存区指针，缓存区大小(最大可传输65535字节)，接收阻塞超时时间
* 返回参数：接收到的字节数，返回-1：接收超时
**********************************************************************************************************/
int STM32F4_UART1_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;

	OSMutexPend(&uart1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream2);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART1->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;		
	DMA_Init(DMA2_Stream2,&DMA_InitStructure);									//初始化DMA通道

	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART1, USART_FLAG_IDLE);	
	USART_ReceiveData(USART1);
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart1_rxd_sem, 0, &err);		
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA2_Stream2,ENABLE);												//启用DMA传输
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断

	OSSemPend(&uart1_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		buffer[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = buffer_size - DMA_GetCurrDataCounter(DMA2_Stream2);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<buffer_size) buffer[len] = 0;
	}
	DMA_Cmd(DMA2_Stream2,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart1_mutex, OS_OPT_POST_NONE, &err);	
	return len;
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART1_DMA_SendReceive(int port,unsigned int baud_rate, char *in,int inlen,char *out,int outlen,unsigned int timeout)
* 函数说明：USART采用DMA进行发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，比特率，发送缓存区，发送字节数，接收缓存区，接收缓存区大小，发送接收超时时间
* 返回参数：接收到的字节数，返回-1：接收和发送超时
**********************************************************************************************************/
int STM32F4_UART1_DMA_SendReceive(int port,unsigned int baud_rate, char *in,int inlen,char *out,int outlen,unsigned int timeout)
{
	DMA_InitTypeDef		DMA_InitStructure;
    USART_InitTypeDef	USART_InitStructure; 	
	OS_ERR				err;
	int					i,j=0,len;

	OSMutexPend(&uart1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	
	USART_InitStructure.USART_BaudRate	= baud_rate;							//比特率设定
	USART_InitStructure.USART_WordLength= USART_WordLength_8b;					//数据位长度
	USART_InitStructure.USART_StopBits	= USART_StopBits_1;						//停止位
	USART_InitStructure.USART_Parity	= USART_Parity_No;						//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
	USART_InitStructure.USART_Mode		= USART_Mode_Rx | USART_Mode_Tx;		//收发模式  
	USART_Init(USART1, &USART_InitStructure);									//初始化
	USART_Cmd(USART1, ENABLE);													//使能串口
	
	GPIOB->BSRRH = GPIO_Pin_8;													//置RS485_EN1引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN2引脚为低电平，使485为接收模式
	switch(port)
	{
		case 1:
			GPIOD->BSRRL = GPIO_Pin_1;											//置RS485_EN引脚为高电平，使RS485_1为发送模式
			GPIOE->BSRRH = GPIO_Pin_14;											//UART1_SW0置低电平
			GPIOE->BSRRL = GPIO_Pin_15;											//UART1_SW1置高电平
			break;
		case 2:
			GPIOB->BSRRL = GPIO_Pin_8;											//置RS485_EN引脚为高电平，使RS485_2为发送模式
			GPIOE->BSRRL = GPIO_Pin_14;											//UART1_SW0置高电平
			GPIOE->BSRRH = GPIO_Pin_15;											//UART1_SW1置低电平		
			break;
		case 3:
			GPIOE->BSRRH = GPIO_Pin_14;											//UART1_SW0置低电平
			GPIOE->BSRRH = GPIO_Pin_15;											//UART1_SW1置低电平		
			break;	
		default:
			GPIOE->BSRRL = GPIO_Pin_14;											//UART1_SW0置高电平
			GPIOE->BSRRL = GPIO_Pin_15;											//UART1_SW1置高电平					
			break;
	}	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream2);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART1->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)out;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= outlen;
	DMA_Init(DMA2_Stream2,&DMA_InitStructure);									//初始化DMA通道

	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART1, USART_FLAG_IDLE);	
	USART_ReceiveData(USART1);
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart1_rxd_sem, 0, &err);		
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA2_Stream2,ENABLE);												//启用DMA传输
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断
//-------------------------------------------------------------------------------------------

	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)in;						//内存地址
	DMA_InitStructure.DMA_BufferSize 		= inlen;	
	DMA_Init(DMA2_Stream7,&DMA_InitStructure);									//初始化DMA通道

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART1,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA2_Stream7,ENABLE);												//启用DMA传输
	OSSemSet(&uart1_txd_sem, 0, &err);	
	if(timeout==0)
	{
		OSSemPend(&uart1_txd_sem, 2000, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间，防止硬件错误导致导致资源无法释放
	}
	else
	{
		OSSemPend(&uart1_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);		//等待超时时间，防止硬件错误导致导致资源无法释放
	}
	
	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN2引脚为低电平，使485为接收模式
	GPIOB->BSRRH = GPIO_Pin_8;													//置RS485_EN1引脚为低电平，使485为接收模式
	
//-------------------------------------------------------------------------------------------
	OSSemPend(&uart1_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		out[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = outlen - DMA_GetCurrDataCounter(DMA2_Stream2);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<outlen) out[len] = 0;
	}
	DMA_Cmd(DMA2_Stream2,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart1_mutex, OS_OPT_POST_NONE, &err);	
	return len;
}

/**********************************************************************************************************
* 函数名称:	void STM32F4_UART2_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA进行发送。支持多线程，发送过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，发送缓存区指针，要发送的字节数(最大可传输65535字节)，发送阻塞超时时间
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART2_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	
	OSMutexPend(&uart2_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream6);

	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART2->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;	
	DMA_Init(DMA1_Stream6,&DMA_InitStructure);									//初始化DMA通道

	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN3引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_13;													//置RS485_EN4引脚为低电平，使485为接收模式
	
	switch(port)
	{
		case 1:
			GPIOD->BSRRL = GPIO_Pin_13;											//置RS485_EN引脚为高电平，使RS485_3为发送模式
			GPIOB->BSRRH = GPIO_Pin_0;											//UART2_SW0置低电平
			GPIOB->BSRRL = GPIO_Pin_1;											//UART2_SW1置高电平
			break;
		case 2:
			GPIOD->BSRRL = GPIO_Pin_1;											//置RS485_EN引脚为高电平，使RS485_4为发送模式
			GPIOB->BSRRL = GPIO_Pin_0;											//UART2_SW0置高电平
			GPIOB->BSRRH = GPIO_Pin_1;											//UART2_SW1置低电平		
			break;
		case 3:
			GPIOB->BSRRH = GPIO_Pin_0;											//UART2_SW0置低电平
			GPIOB->BSRRH = GPIO_Pin_1;											//UART2_SW1置低电平		
			break;	
		default:
			GPIOB->BSRRL = GPIO_Pin_0;											//UART2_SW0置高电平
			GPIOB->BSRRL = GPIO_Pin_1;											//UART2_SW1置高电平					
			break;
	}

	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART2,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA1_Stream6,ENABLE);												//启用DMA传输
	OSSemSet(&uart2_txd_sem, 0, &err);	
	if(timeout==0)timeout=2000;		
	OSSemPend(&uart2_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间30秒，防止硬件错误导致导致资源无法释放

	GPIOD->BSRRH = GPIO_Pin_13;													//置RS485_EN3引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_1;													//置RS485_EN4引脚为低电平，使485为接收模式

	OSMutexPost(&uart2_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART2_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA接收数据放入指定大小的缓存区。支持多线程，接收过程中线程会线程会被阻塞
* 输入参数：接收缓存区指针，缓存区大小(最大可传输65535字节)，接收超时时间
* 返回参数：接收到的字节数，返回-1：接收超时
**********************************************************************************************************/
int STM32F4_UART2_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;
	
	OSMutexPend(&uart2_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream5);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART2->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;		
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);									//初始化DMA通道
	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART2, USART_FLAG_IDLE);	
	USART_ReceiveData(USART2);
	USART_ClearFlag(USART2, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart2_rxd_sem, 0, &err);		
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA1_Stream5,ENABLE);												//启用DMA传输
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断

	OSSemPend(&uart2_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		buffer[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = buffer_size - DMA_GetCurrDataCounter(DMA1_Stream5);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<buffer_size) buffer[len] = 0;
	}
	DMA_Cmd(DMA1_Stream5,DISABLE);												//禁用DMA传输	
	OSMutexPost(&uart2_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART2_DMA_SendReceive(int port,unsigned int baud_rate, char *in,int inlen,char *out,int outlen,unsigned int timeout)
* 函数说明：USART采用DMA进行发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，比特率，发送缓存区，发送字节数，接收缓存区，接收缓存区大小，发送接收超时时间
* 返回参数：接收到的字节数，返回-1：接收和发送超时
**********************************************************************************************************/
int STM32F4_UART2_DMA_SendReceive(int port,unsigned int baud_rate, char *in,int inlen,char *out,int outlen,unsigned int timeout)
{
	DMA_InitTypeDef		DMA_InitStructure;
    USART_InitTypeDef	USART_InitStructure; 	
	OS_ERR				err;
	int					i,j=0,len;

	OSMutexPend(&uart2_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	
	USART_InitStructure.USART_BaudRate	= baud_rate;							//比特率设定
	USART_InitStructure.USART_WordLength= USART_WordLength_8b;					//数据位长度
	USART_InitStructure.USART_StopBits	= USART_StopBits_1;						//停止位
	USART_InitStructure.USART_Parity	= USART_Parity_No;						//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
	USART_InitStructure.USART_Mode		= USART_Mode_Rx | USART_Mode_Tx;		//收发模式  
	USART_Init(USART2, &USART_InitStructure);									//初始化
	USART_Cmd(USART2, ENABLE);													//使能串口
	
	GPIOD->BSRRH = GPIO_Pin_0;													//置RS485_EN3引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_13;													//置RS485_EN4引脚为低电平，使485为接收模式
	switch(port)
	{
		case 1:
			GPIOD->BSRRL = GPIO_Pin_13;											//置RS485_EN引脚为高电平，使RS485_3为发送模式
			GPIOB->BSRRH = GPIO_Pin_0;											//UART2_SW0置低电平
			GPIOB->BSRRL = GPIO_Pin_1;											//UART2_SW1置高电平
			break;
		case 2:
			GPIOD->BSRRL = GPIO_Pin_0;											//置RS485_EN引脚为高电平，使RS485_4为发送模式
			GPIOB->BSRRL = GPIO_Pin_0;											//UART2_SW0置高电平
			GPIOB->BSRRH = GPIO_Pin_1;											//UART2_SW1置低电平		
			break;
		case 3:
			GPIOB->BSRRH = GPIO_Pin_0;											//UART2_SW0置低电平
			GPIOB->BSRRH = GPIO_Pin_1;											//UART2_SW1置低电平		
			break;	
		default:
			GPIOB->BSRRL = GPIO_Pin_0;											//UART2_SW0置高电平
			GPIOB->BSRRL = GPIO_Pin_1;											//UART2_SW1置高电平					
			break;
	}

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream5);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART2->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)out;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= outlen;
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);									//初始化DMA通道
	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART2, USART_FLAG_IDLE);	
	USART_ReceiveData(USART2);
	USART_ClearFlag(USART2, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart2_rxd_sem, 0, &err);		
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA1_Stream5,ENABLE);												//启用DMA传输
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断
//-------------------------------------------------------------------------------------------

	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)in;						//内存地址
	DMA_InitStructure.DMA_BufferSize 		= inlen;	
	DMA_Init(DMA1_Stream6,&DMA_InitStructure);									//初始化DMA通道

	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART2,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA1_Stream6,ENABLE);												//启用DMA传输
	OSSemSet(&uart2_txd_sem, 0, &err);	
	if(timeout==0)
	{
		OSSemPend(&uart2_txd_sem, 2000, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间，防止硬件错误导致导致资源无法释放	
	}
	else
	{
		OSSemPend(&uart2_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);		//等待超时时间，防止硬件错误导致导致资源无法释放	
	}
	
	GPIOD->BSRRH = GPIO_Pin_0;													//置RS485_EN3引脚为低电平，使485为接收模式
	GPIOD->BSRRH = GPIO_Pin_13;													//置RS485_EN4引脚为低电平，使485为接收模式	
	
//-------------------------------------------------------------------------------------------
	OSSemPend(&uart2_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		out[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = outlen - DMA_GetCurrDataCounter(DMA1_Stream5);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<outlen) out[len] = 0;	
	}
	DMA_Cmd(DMA1_Stream5,DISABLE);												//禁用DMA传输	
	OSMutexPost(&uart2_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
* 函数名称:	void STM32F4_UART3_DMA_Send(char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA进行发送。支持多线程，发送过程会被阻塞，可设定阻塞超时时间
* 输入参数：发送缓存区指针，要发送的字节数(最大可传输65535字节)，发送阻塞超时时间
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART3_DMA_Send(char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	
	OSMutexPend(&uart3_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream3);

	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART3->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;		
	DMA_Init(DMA1_Stream3,&DMA_InitStructure);									//初始化DMA通道

	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART3,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA1_Stream3,ENABLE);												//启用DMA传输	

	OSSemSet(&uart3_txd_sem, 0, &err);	
	if(timeout==0)timeout=2000;	
	OSSemPend(&uart3_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间30秒，防止硬件错误导致导致资源无法释放
	
	OSMutexPost(&uart3_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART3_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA接收数据放入指定大小的缓存区。支持多线程，接收过程中线程会线程会被阻塞
* 输入参数：接收缓存区指针，缓存区大小(最大可传输65535字节)，接收超时时间
* 返回参数：接收到的字节数，返回-1：接收超时
**********************************************************************************************************/
int STM32F4_UART3_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;
	
	OSMutexPend(&uart3_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream1);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART3->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;		
	DMA_Init(DMA1_Stream1,&DMA_InitStructure);									//初始化DMA通道
	
	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART3, USART_FLAG_IDLE);	
	USART_ReceiveData(USART3);
	USART_ClearFlag(USART3, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart3_rxd_sem, 0, &err);		
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA1_Stream1,ENABLE);												//启用DMA传输
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断

	OSSemPend(&uart3_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		buffer[0] = 0;
	}
	else
	{
		for(i=0; i<40; i++)
		{
			len = buffer_size - DMA_GetCurrDataCounter(DMA1_Stream1);
			if(len!=j)
			{
				j=len;
				OSTimeDly(25,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}			
		if(len<buffer_size) buffer[len] = 0;	
	}
	DMA_Cmd(DMA1_Stream1,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart3_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
* 函数名称:	STM32F4_UART3_DMA_SendReceive(char *in,int inlen,char *out,int outlen,unsigned int timeout)
* 函数说明：USART采用DMA进行发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间
* 输入参数：发送缓存区，发送字节数，接收缓存区，接收缓存区大小，发送接收超时时间
* 返回参数：接收到的字节数，返回-1：接收和发送超时
**********************************************************************************************************/
int STM32F4_UART3_DMA_SendReceive(char *in,int inlen,char *out,int outlen,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;

	OSMutexPend(&uart3_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream1);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART3->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)out;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= outlen;
	DMA_Init(DMA1_Stream1,&DMA_InitStructure);									//初始化DMA通道
	
	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART3, USART_FLAG_IDLE);	
	USART_ReceiveData(USART3);
	USART_ClearFlag(USART3, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart3_rxd_sem, 0, &err);		
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA1_Stream1,ENABLE);												//启用DMA传输
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断
//-------------------------------------------------------------------------------------------

	DMA_DeInit(DMA1_Stream3);
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)in;						//内存地址
	DMA_InitStructure.DMA_BufferSize 		= inlen;	
	DMA_Init(DMA1_Stream3,&DMA_InitStructure);									//初始化DMA通道

	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送
	USART_ITConfig(USART3,USART_IT_TC,ENABLE); 									//使能串口发送完成中断
	DMA_Cmd(DMA1_Stream3,ENABLE);												//启用DMA传输
	OSSemSet(&uart3_txd_sem, 0, &err);			
	if(timeout==0)
	{
		OSSemPend(&uart3_txd_sem, 2000, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间，防止硬件错误导致导致资源无法释放	
	}
	else
	{
		OSSemPend(&uart3_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);		//等待超时时间，防止硬件错误导致导致资源无法释放	
	}

//-------------------------------------------------------------------------------------------
	OSSemPend(&uart3_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		out[0] = 0;
	}
	else
	{
		for(i=0; i<40; i++)
		{
			len = outlen - DMA_GetCurrDataCounter(DMA1_Stream1);
			if(len!=j)
			{
				j=len;
				OSTimeDly(25,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<outlen) out[len] = 0;	
	}
	DMA_Cmd(DMA1_Stream1,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart3_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
* 函数名称:	void STM32F4_UART6_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA进行发送。支持多线程，发送过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，发送缓存区指针，要发送的字节数(最大可传输65535字节)，发送阻塞超时时间
* 返回参数：无
**********************************************************************************************************/
void STM32F4_UART6_DMA_Send(int port,char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	
	OSMutexPend(&uart6_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	DMA_DeInit(DMA2_Stream6);
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART6->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_5 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;	
	DMA_Init(DMA2_Stream6,&DMA_InitStructure);									//初始化DMA通道

	switch(port)
	{
		case 1:																	//电源管理和主板采集通讯通道
			GPIOC->BSRRH = GPIO_Pin_0;											//置UART6_SW0引脚为低电平
			GPIOC->BSRRH = GPIO_Pin_2;											//置UART6_SW1引脚为低电平
			break;
		case 2:																	//LED指示灯管理通道
			GPIOC->BSRRL = GPIO_Pin_0;											//置UART6_SW0引脚为高电平			
			GPIOC->BSRRH = GPIO_Pin_2;											//置UART6_SW1引脚为低电平		
			break;	
		case 3:
			GPIOC->BSRRH = GPIO_Pin_0;											//置UART6_SW0引脚为低电平
			GPIOC->BSRRL = GPIO_Pin_2;											//置UART6_SW1引脚为高电平			
			break;	
		case 4:
			GPIOC->BSRRL = GPIO_Pin_0;											//置UART6_SW0引脚为高电平
			GPIOC->BSRRL = GPIO_Pin_2;											//置UART6_SW1引脚为高电平			
			break;	
		default:
			break;
	}

	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ITConfig(USART6,USART_IT_TC,ENABLE); 									//使能串口发送完成中断	
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送	
	DMA_Cmd(DMA2_Stream6,ENABLE);												//启用DMA传输

	OSSemSet(&uart6_txd_sem, 0, &err);	
	if(timeout==0)timeout=2000;
	OSSemPend(&uart6_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间30秒，防止硬件错误导致导致资源无法释放
		
	OSMutexPost(&uart6_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称:	int STM32F4_UART6_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
* 函数说明：USART采用DMA接收数据放入指定大小的缓存区。支持多线程，接收过程中线程会线程会被阻塞
* 输入参数：接收缓存区指针，缓存区大小(最大可传输65535字节)，接收超时时间
* 返回参数：接收到的字节数，返回-1：接收超时
**********************************************************************************************************/
int STM32F4_UART6_DMA_Receive(char *buffer,int buffer_size,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;
	
	OSMutexPend(&uart6_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream1);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART6->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_5 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= buffer_size ;		
	DMA_Init(DMA2_Stream1,&DMA_InitStructure);									//初始化DMA通道

	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART6, USART_FLAG_IDLE);	
	USART_ReceiveData(USART6);
	USART_ClearFlag(USART6, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart6_rxd_sem, 0, &err);		
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA2_Stream1,ENABLE);												//启用DMA传输
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断

	OSSemPend(&uart6_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		buffer[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = buffer_size - DMA_GetCurrDataCounter(DMA2_Stream1);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}			
		if(len<buffer_size) buffer[len] = 0;		
	}
	DMA_Cmd(DMA2_Stream1,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart6_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
* 函数名称:	STM32F4_UART6_DMA_SendReceive(int port,char *in,int inlen,char *out,int outlen,unsigned int timeout)
* 函数说明：USART采用DMA进行发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间
* 输入参数：通道，发送缓存区，发送字节数，接收缓存区，接收缓存区大小，发送接收超时时间
* 返回参数：接收到的字节数，返回-1：接收和发送超时
**********************************************************************************************************/
int STM32F4_UART6_DMA_SendReceive(int port,char *in,int inlen,char *out,int outlen,unsigned int timeout)
{
	DMA_InitTypeDef	DMA_InitStructure;
	OS_ERR			err;
	int				i,j=0,len;

	OSMutexPend(&uart6_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	
	
	switch(port)
	{
		case 1:																	//电源管理和主板采集通讯通道
			GPIOC->BSRRH = GPIO_Pin_0;											//置UART6_SW0引脚为低电平
			GPIOC->BSRRH = GPIO_Pin_2;											//置UART6_SW1引脚为低电平
			break;
		case 2:																	//LED指示灯管理通道
			GPIOC->BSRRL = GPIO_Pin_0;											//置UART6_SW0引脚为高电平			
			GPIOC->BSRRH = GPIO_Pin_2;											//置UART6_SW1引脚为低电平		
			break;	
		case 3:
			GPIOC->BSRRH = GPIO_Pin_0;											//置UART6_SW0引脚为低电平
			GPIOC->BSRRL = GPIO_Pin_2;											//置UART6_SW1引脚为高电平			
			break;	
		case 4:
			GPIOC->BSRRL = GPIO_Pin_0;											//置UART6_SW0引脚为高电平
			GPIOC->BSRRL = GPIO_Pin_2;											//置UART6_SW1引脚为高电平			
			break;	
		default:
			break;
	}	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream1);
	
	DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_MemoryInc 		= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t) (&(USART6->DR)) ;		//外设地址
	DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;		
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_PeripheralInc 	= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能	
	DMA_InitStructure.DMA_Priority 			= DMA_Priority_High;				//设置DMA的优先级别
	DMA_InitStructure.DMA_Channel 			= DMA_Channel_5 ;
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr 	= (uint32_t)out;					//内存地址
	DMA_InitStructure.DMA_BufferSize 		= outlen;		
	DMA_Init(DMA2_Stream1,&DMA_InitStructure);									//初始化DMA通道

	//执行软件序列清零和写0清零
	USART_GetFlagStatus(USART6, USART_FLAG_IDLE);	
	USART_ReceiveData(USART6);
	USART_ClearFlag(USART6, USART_FLAG_RXNE);
	//清零结束
	OSSemSet(&uart6_rxd_sem, 0, &err);		
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);								//采用DMA方式发送
	DMA_Cmd(DMA2_Stream1,ENABLE);												//启用DMA传输
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE); 								//使能串口空闲中断
//-------------------------------------------------------------------------------------------	

	DMA_DeInit(DMA2_Stream6);
	DMA_InitStructure.DMA_DIR 				= DMA_DIR_MemoryToPeripheral ;		//DMA传输方向单向
	DMA_InitStructure.DMA_Memory0BaseAddr	= (uint32_t)in;						//内存地址
	DMA_InitStructure.DMA_BufferSize 		= inlen;	
	DMA_Init(DMA2_Stream6,&DMA_InitStructure);									//初始化DMA通道

	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ITConfig(USART6,USART_IT_TC,ENABLE); 									//使能串口发送完成中断	
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);								//采用DMA方式发送	
	DMA_Cmd(DMA2_Stream6,ENABLE);												//启用DMA传输
	OSSemSet(&uart6_txd_sem, 0, &err);		
	if(timeout==0)
	{
		OSSemPend(&uart6_txd_sem, 2000, OS_OPT_PEND_BLOCKING, 0, &err);			//等待超时时间，防止硬件错误导致导致资源无法释放	
	}
	else
	{
		OSSemPend(&uart6_txd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);		//等待超时时间，防止硬件错误导致导致资源无法释放	
	}


//-------------------------------------------------------------------------------------------
	OSSemPend(&uart6_rxd_sem, timeout, OS_OPT_PEND_BLOCKING, 0, &err);
	if(err==OS_ERR_TIMEOUT) 
	{	
		len    = -1;
		out[0] = 0;
	}
	else
	{
		for(i=0; i<25; i++)
		{
			len = outlen - DMA_GetCurrDataCounter(DMA2_Stream1);
			if(len!=j)
			{
				j=len;
				OSTimeDly(20,OS_OPT_TIME_DLY,&err);
			}
			else break;
		}	
		if(len<outlen) out[len] = 0;	
	}
	DMA_Cmd(DMA2_Stream1,DISABLE);												//禁用DMA传输
	OSMutexPost(&uart6_mutex, OS_OPT_POST_NONE, &err);	
	return len;	
}

/**********************************************************************************************************
 函数原型：void USART1_IRQHandler(void)  
 入口参数：无
 出口参数：无
 函数功能：USART1中断发生的中断处理函数
**********************************************************************************************************/
void USART1_IRQHandler(void)                                 
{     
	OS_ERR err;	
	OSIntEnter(); 
	if(USART_GetFlagStatus(USART1, USART_FLAG_TC)==SET)
	{
		USART_ITConfig(USART1,USART_IT_TC,DISABLE); 								//禁用串口发送完成中断
		OSSemPost(&uart1_txd_sem, OS_OPT_POST_1, &err);	
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_IDLE)==SET)
	{
		USART_ITConfig(USART1,USART_IT_IDLE,DISABLE); 								//禁用串口空闲中断
		OSSemPost(&uart1_rxd_sem, OS_OPT_POST_1, &err);	
	}	
	OSIntExit();  
} 

/**********************************************************************************************************
 函数原型：void USART2_IRQHandler(void)  
 入口参数：无
 出口参数：无
 函数功能：USART2中断发生的中断处理函数
**********************************************************************************************************/
void USART2_IRQHandler(void)                                 
{     
	OS_ERR err;	
	OSIntEnter(); 
	if(USART_GetFlagStatus(USART2, USART_FLAG_TC)==SET)
	{
		USART_ITConfig(USART2,USART_IT_TC,DISABLE); 								//禁用串口发送完成中断
		OSSemPost(&uart2_txd_sem, OS_OPT_POST_1, &err);	
	}
	if(USART_GetFlagStatus(USART2, USART_FLAG_IDLE)==SET)
	{
		USART_ITConfig(USART2,USART_IT_IDLE,DISABLE); 								//禁用串口空闲中断
		OSSemPost(&uart2_rxd_sem, OS_OPT_POST_1, &err);	
	}	
	OSIntExit();  
} 

/**********************************************************************************************************
 函数原型：void USART3_IRQHandler(void)  
 入口参数：无
 出口参数：无
 函数功能：USART3中断发生的中断处理函数
**********************************************************************************************************/
void USART3_IRQHandler(void)                                 
{     
	OS_ERR err;	
	OSIntEnter(); 
	if(USART_GetFlagStatus(USART3, USART_FLAG_TC)==SET)
	{
		USART_ITConfig(USART3,USART_IT_TC,DISABLE); 								//禁用串口发送完成中断
		OSSemPost(&uart3_txd_sem, OS_OPT_POST_1, &err);	
	}
	if(USART_GetFlagStatus(USART3, USART_FLAG_IDLE)==SET)
	{
		USART_ITConfig(USART3,USART_IT_IDLE,DISABLE); 								//禁用串口空闲中断
		OSSemPost(&uart3_rxd_sem, OS_OPT_POST_1, &err);	
	}	
	OSIntExit();  
} 

/**********************************************************************************************************
 函数原型：void USART6_IRQHandler(void)  
 入口参数：无
 出口参数：无
 函数功能：USART6中断发生的中断处理函数
**********************************************************************************************************/
void USART6_IRQHandler(void)                                 
{     
	OS_ERR err;	
	OSIntEnter(); 
	if(USART_GetFlagStatus(USART6, USART_FLAG_TC)==SET)
	{
		USART_ITConfig(USART6,USART_IT_TC,DISABLE); 								//禁用串口发送完成中断
		OSSemPost(&uart6_txd_sem, OS_OPT_POST_1, &err);	
	}
	if(USART_GetFlagStatus(USART6, USART_FLAG_IDLE)==SET)
	{
		USART_ITConfig(USART6,USART_IT_IDLE,DISABLE); 								//禁用串口空闲中断
		OSSemPost(&uart6_rxd_sem, OS_OPT_POST_1, &err);	
	}	
	OSIntExit();  
} 

/**********************************************************************************************************
* 函数名称:	void UART_Init(UART_OPT port, unsigned int baud_rate)
* 函数说明：UART串口进行初始化
* 输入参数：UART端口号，波特率
* 返回参数：无
**********************************************************************************************************/
void UART_Init(UART_OPT port, unsigned int baud_rate)
{
	switch(port)
	{
		case RS485_1:
		case RS485_2:			
		case RS232_1:		
			STM32F4_UART1_Init(baud_rate);
			break;
		case RS485_3:
		case RS485_4:			
		case RS232_2:	
			STM32F4_UART2_Init(baud_rate);
			break;
		case SIM800:
			STM32F4_UART3_Init(baud_rate);
			break;	
		case TTS100:
		case TFT:
		case PM100:		
		case MIAN:		
		case LED:
			STM32F4_UART6_Init(baud_rate);
			break;	
		default:
			Log_Error("Invalid UART port.");
			break;
	}
}

/**********************************************************************************************************
* 函数名称:	void UART_Send(UART_OPT port,unsigned int timeout, char *buffer, unsigned int buffer_size)
* 函数说明：UART串口发送数据，支持多线程
* 输入参数：UART端口号，发送阻塞超时时间，发送缓存区，发送数据字节数
* 返回参数：无
**********************************************************************************************************/
void UART_Send(UART_OPT port,unsigned int timeout, char *buffer, unsigned int buffer_size)
{
	switch(port)
	{
		case RS485_1:
			STM32F4_UART1_DMA_Send(1,buffer,buffer_size,timeout);
			break;
		case RS485_2:
			STM32F4_UART1_DMA_Send(2,buffer,buffer_size,timeout);
			break;		
		case RS232_1:
			STM32F4_UART1_DMA_Send(3,buffer,buffer_size,timeout);
			break;			
		case RS485_3:
			STM32F4_UART2_DMA_Send(1,buffer,buffer_size,timeout);
			break;
		case RS485_4:
			STM32F4_UART2_DMA_Send(2,buffer,buffer_size,timeout);
			break;		
		case RS232_2:
			STM32F4_UART2_DMA_Send(3,buffer,buffer_size,timeout);
			break;		
		case SIM800:
			STM32F4_UART3_DMA_Send(buffer,buffer_size,timeout);		
			break;	
		case TTS100:
			STM32F4_UART6_DMA_Send(4,buffer,buffer_size,timeout);
			break;	
		case TFT:
			STM32F4_UART6_DMA_Send(3,buffer,buffer_size,timeout);			
			break;				
		case PM100:
			STM32F4_UART6_DMA_Send(1,buffer,buffer_size,timeout);			
			break;				
		case MIAN:
			STM32F4_UART6_DMA_Send(1,buffer,buffer_size,timeout);			
			break;				
		case LED:
			STM32F4_UART6_DMA_Send(2,buffer,buffer_size,timeout);			
			break;			
		default:
			Log_Error("Invalid UART port.");
			break;
	}
}

/**********************************************************************************************************
* 函数名称:	int UART_Receive(UART_OPT port, unsigned int timeout, char *buffer, unsigned short buffer_size)
* 函数说明：UART串口接收数据，接收过程会被阻塞，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，接收阻塞超时时间，接收缓存区，接收缓存区大小（单位字节）
* 返回参数：返回接收到的字节数,返回-1：接收超时，-2：无效串口号
**********************************************************************************************************/
int UART_Receive(UART_OPT port, unsigned int timeout, char *buffer, unsigned short buffer_size)
{
	switch(port)
	{
		case RS485_1:
		case RS485_2:		
		case RS232_2:
			return STM32F4_UART1_DMA_Receive(buffer,buffer_size,timeout);		
		case RS485_3:
		case RS485_4:	
		case RS232_1:
			return STM32F4_UART2_DMA_Receive(buffer,buffer_size,timeout);		
		case SIM800:
			return STM32F4_UART3_DMA_Receive(buffer,buffer_size,timeout);
		case TTS100:
		case TFT:			
		case PM100:			
		case MIAN:			
		case LED:
			return STM32F4_UART6_DMA_Receive(buffer,buffer_size,timeout);
		default:
			Log_Error("Invalid UART port.");
			return -2;
	}
}

/**********************************************************************************************************
* 函数名称:	int UART_SendReceive(UART_OPT port, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
* 函数说明：USART发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，发送接收阻塞超时时间，发送缓存区，发送字节数，接收缓存区，接收缓存区大小
* 返回参数：接收到的字节数，返回-1：接收和发送超时，-2：无效串口号
**********************************************************************************************************/
int UART_SendReceive(UART_OPT port, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
{
	switch(port)
	{
		case RS485_1:
			return STM32F4_UART1_DMA_SendReceive(1,9600,in, inlen, out, outlen,timeout);
		case RS485_2:
			return STM32F4_UART1_DMA_SendReceive(2,9600,in, inlen, out, outlen,timeout);
		case RS232_1:
			return STM32F4_UART1_DMA_SendReceive(3,9600,in, inlen, out, outlen,timeout);		
		case RS485_3:
			return STM32F4_UART2_DMA_SendReceive(1,9600,in, inlen, out, outlen,timeout);
		case RS485_4:
			return STM32F4_UART2_DMA_SendReceive(2,9600,in, inlen, out, outlen,timeout);	
		case RS232_2:
			return STM32F4_UART2_DMA_SendReceive(3,9600,in, inlen, out, outlen,timeout);	
		case SIM800:
			return STM32F4_UART3_DMA_SendReceive(in, inlen, out, outlen,timeout);
		case TTS100:
			return STM32F4_UART6_DMA_SendReceive(4,in, inlen, out, outlen,timeout);	
		case TFT:
			return STM32F4_UART6_DMA_SendReceive(3,in, inlen, out, outlen,timeout);			
		case PM100:
			return STM32F4_UART6_DMA_SendReceive(1,in, inlen, out, outlen,timeout);			
		case MIAN:
			return STM32F4_UART6_DMA_SendReceive(1,in, inlen, out, outlen,timeout);				
		case LED:
			return STM32F4_UART6_DMA_SendReceive(2,in, inlen, out, outlen,timeout);
		default:
			Log_Error("Invalid UART port.");
			return -2;
	}
}

/**********************************************************************************************************
* 函数名称:	int UartSendReceive(UART_OPT port,unsigned int baudrate, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
* 函数说明：USART发送和接收，支持多线程，发送和接收过程会被阻塞，可设定阻塞超时时间，若参数timeout置0，无数据接收，则一直阻塞，直到接收到数据
* 输入参数：UART端口号，比特率，发送接收阻塞超时时间，发送缓存区，发送字节数，接收缓存区，接收缓存区大小
* 返回参数：接收到的字节数，返回-1：接收和发送超时，-2：无效串口号
**********************************************************************************************************/
int UartSendReceive(UART_OPT port,unsigned int baudrate, unsigned int timeout, char *in, unsigned short inlen, char *out, unsigned short outlen)
{
	switch(port)
	{
		case RS485_1:
			return STM32F4_UART1_DMA_SendReceive(1,baudrate,in, inlen, out, outlen,timeout);
		case RS485_2:
			return STM32F4_UART1_DMA_SendReceive(2,baudrate,in, inlen, out, outlen,timeout);
		case RS232_1:
			return STM32F4_UART1_DMA_SendReceive(3,baudrate,in, inlen, out, outlen,timeout);		
		case RS485_3:
			return STM32F4_UART2_DMA_SendReceive(1,baudrate,in, inlen, out, outlen,timeout);
		case RS485_4:
			return STM32F4_UART2_DMA_SendReceive(2,baudrate,in, inlen, out, outlen,timeout);	
		case RS232_2:
			return STM32F4_UART2_DMA_SendReceive(3,baudrate,in, inlen, out, outlen,timeout);	
		default:
			Log_Error("Invalid UART port.");
			return -2;
	}
}
/**********************************************************************************************************
* 函数名称:	void UART_BaudRate(UART_OPT port, unsigned int baud_rate)
* 函数说明：UART串口波特率设定
* 输入参数：UART端口号，波特率
* 返回参数：无
**********************************************************************************************************/
void UART_BaudRate(UART_OPT port, unsigned int baud_rate)
{
    USART_InitTypeDef   	USART_InitStructure; 
	
	USART_InitStructure.USART_BaudRate	= baud_rate;				//比特率设定
	USART_InitStructure.USART_WordLength= USART_WordLength_8b;		//数据位长度
	USART_InitStructure.USART_StopBits	= USART_StopBits_1;			//停止位
	USART_InitStructure.USART_Parity	= USART_Parity_No;			//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//非硬件流控制
	USART_InitStructure.USART_Mode		= USART_Mode_Rx | USART_Mode_Tx;//收发模式    		
	switch(port)
	{
		case RS485_1:
		case RS485_2:			
		case RS232_1:
			USART_Init(USART1, &USART_InitStructure);				//初始化
			USART_Cmd(USART1, ENABLE);								//使能串口
			break;
		case RS485_3:
		case RS485_4:			
		case RS232_2:	   
			USART_Init(USART2, &USART_InitStructure);				//初始化
			USART_Cmd(USART2, ENABLE);								//使能串口
			break;
		case SIM800:    
			USART_Init(USART3, &USART_InitStructure);				//初始化
			USART_Cmd(USART3, ENABLE);								//使能串口
			break;	
		case TTS100:
		case TFT:
		case PM100:		
		case MIAN:		
		case LED:   
			USART_Init(USART6, &USART_InitStructure);				//初始化
			USART_Cmd(USART6, ENABLE);								//使能串口
			break;	
		default:
			Log_Error("Invalid UART port.");
			break;
	}
}

