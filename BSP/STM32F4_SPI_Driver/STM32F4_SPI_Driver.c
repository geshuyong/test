/**********************************************************************************************************
** 文件名: 		STM32F4_SPI_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		SPI接口驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/ 
#include "stm32f4xx.h"
#include "os.h"

OS_SEM  	SPI1_Sem;			//写或者读取等待信号量，中断发送信号量
OS_MUTEX 	SPI1_mutex;
/**********************************************************************************************************
* 函数名称:	 static void NVIC_Configuration(void)
* 函数说明： 中断优先级分组方式，中断优先级，使能配置
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
//优先级分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

// DMA2 STREAM0 Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel 	= DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
	
// DMA2 STREAM5 Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel 	= DMA2_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

/**********************************************************************************************************
* 函数名称： void STM32F4_SPI1_Init(void)
* 函数说明： SPI1 GPIO以及SPI外设初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_SPI1_Init(void)
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    SPI_InitTypeDef		SPI_InitStructure;
	OS_ERR				err;
	static 	char 		flag = 0x00;
	if(flag == 0x00)
	{
		flag = 0X5A;
		OSSemCreate(&SPI1_Sem,"SPI1 Sem",(OS_SEM_CTR)0,&err);				//创建信号量		
		OSMutexCreate(&SPI1_mutex, "SPI1_mutex", &err);			
	}
	OSMutexPend(&SPI1_mutex, 0,OS_OPT_PEND_BLOCKING, 0, &err);	

	//外设时钟开启
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	//复用功能开启
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); 

	//SPI  SCK、MISO、MOSI引脚初始化
    GPIO_InitStructure.GPIO_Pin     	= GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode    	= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    	= GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   	= GPIO_Speed_50MHz;     
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//片选初始化
    GPIO_InitStructure.GPIO_Pin     	= GPIO_Pin_3|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode    	= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    	= GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   	= GPIO_Speed_50MHz;     
    GPIO_Init(GPIOD, &GPIO_InitStructure);	 

	//SPI外设初始化
    SPI_InitStructure.SPI_Direction 	= SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode 			= SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize 		= SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL 			= SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA 			= SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS 			= SPI_NSS_Soft;
    SPI_InitStructure.SPI_FirstBit 		= SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //速度选择
    SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	
	GPIOD->BSRRL = GPIO_Pin_3;	//W25Q_1 CS引脚的拉高
	GPIOD->BSRRL = GPIO_Pin_7;	//W25Q_2 CS引脚的拉高	
	
	NVIC_Configuration();
	OSMutexPost(&SPI1_mutex, OS_OPT_POST_NONE|OS_OPT_POST_NO_SCHED, &err);	
}

/**********************************************************************************************************
* 函数名称： unsigned char STM32F4_SPI1_Read_Write(unsigned char data)
* 函数说明： 读写一个字节
* 输入参数： 写入字节
* 返回参数： 读出字节
**********************************************************************************************************/
unsigned char STM32F4_SPI1_Read_Write(unsigned char data)
{
	while((SPI1->SR & SPI_I2S_FLAG_TXE)  == RESET){}
  	SPI1->DR = data;
  	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET){}
  	return SPI1->DR;
}


/**********************************************************************************************************
* 函数名称:	char STM32F4_SPI1_DMA_Send(const unsigned char *buffer,unsigned int buffer_size)
* 函数说明：	SPI采用DMA进行数据发送
* 输入参数： 发送缓冲区大小	 buffer_size  ；发送缓冲区指针 buffer
* 返回参数： 返回0表示成功， 非0表示失败
**********************************************************************************************************/
char STM32F4_SPI1_DMA_Send(const unsigned char *buffer,unsigned int buffer_size)
{
	DMA_InitTypeDef  	DMA_InitStructure;
	static char			Dummy_byte;
	OS_ERR 				err;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,DISABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,DISABLE);	
	
	DMA_DeInit(DMA2_Stream0);
	DMA_DeInit(DMA2_Stream5);

	SPI1->DR;																		//接送前读一次SPI3->DR，保证接收缓冲区为空		
	DMA_InitStructure.DMA_Channel 				= DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)(&(SPI1->DR)) ;			//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)(&Dummy_byte);			//内存地址
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralToMemory;		//DMA传输方向单向
	DMA_InitStructure.DMA_BufferSize 			= buffer_size;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Disable;			//设置DMA的内存递增模式，不使能
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;				//设置DMA的优先级别为高
	DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_INC4;
	DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_INC4;
	
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);										//初始化DMA通道


	while((SPI1->SR&1<<1)==0);														//等待TXE置1，发送缓冲为空
	DMA_InitStructure.DMA_Channel 				= DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t) (&(SPI1->DR)) ;		//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;		//DMA传输方向单向
	DMA_InitStructure.DMA_BufferSize 			= buffer_size;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_Medium;				//设置DMA的优先级别为低
	DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_INC4;
	DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_INC4;
	
	DMA_Init(DMA2_Stream5,&DMA_InitStructure);										//初始化DMA通道	

	OSSemSet(&SPI1_Sem,0,&err);														//清空信号量

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);  									//使能指定通道中断

	DMA_Cmd(DMA2_Stream0,ENABLE);													//启用DMA传输
	DMA_Cmd(DMA2_Stream5,ENABLE);													//启用DMA传输	
	
	OSSemPend(&SPI1_Sem,2000,OS_OPT_PEND_BLOCKING,0,&err);							//等待信号量，超时时间为2000ms
	
	if(DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF5) == RESET)
	{
		return 1;
	}
			
	while(( SPI1->SR & 0x02 ) == 0 );   											//等待TXE为1
	while(( SPI1->SR & 0x80 ) == 1 );  												//等待忙标志为0
	return 0;
}

/**********************************************************************************************************
* 函数名称:	char STM32F4_SPI1_DMA_Receive(const unsigned char *buffer,unsigned int buffer_size)
* 函数说明：	SPI采用DMA进行数据接收
* 输入参数： 发送缓冲区大小	 buffer_size  ；发送缓冲区指针 buffer
* 返回参数： 返回0表示成功， 非0表示失败
**********************************************************************************************************/
char STM32F4_SPI1_DMA_Receive(const unsigned char *buffer,unsigned int buffer_size)
{
	DMA_InitTypeDef  	DMA_InitStructure;
	static const char 	Dummy_byte = 0xff;
	OS_ERR 				err;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,DISABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,DISABLE);	
	
	DMA_DeInit(DMA2_Stream0);
	DMA_DeInit(DMA2_Stream5);

	SPI1->DR;																		//接送前读一次SPI3->DR，保证接收缓冲区为空		
	DMA_InitStructure.DMA_Channel 				= DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)(&(SPI1->DR)) ;			//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)buffer;					//内存地址
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralToMemory;		//DMA传输方向单向
	DMA_InitStructure.DMA_BufferSize 			= buffer_size;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，使能
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;				//设置DMA的优先级别为高
	DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_INC4;
	DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_INC4;
	
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);										//初始化DMA通道


	while((SPI1->SR&1<<1)==0);														//等待TXE置1，发送缓冲为空
	DMA_InitStructure.DMA_Channel 				= DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t) (&(SPI1->DR)) ;		//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)(&Dummy_byte);			//内存地址
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;		//DMA传输方向单向
	DMA_InitStructure.DMA_BufferSize 			= buffer_size;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Disable;			//设置DMA的内存递增模式，不使能
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;		//外设数据字长	
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;			//内存数据字长
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;					//设置DMA的传输模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_Medium;				//设置DMA的优先级别为低
	DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_INC4;
	DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_INC4;
	
	DMA_Init(DMA2_Stream5,&DMA_InitStructure);										//初始化DMA通道	

	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);  									//使能指定通道中断

	OSSemSet(&SPI1_Sem,0,&err);														//清空信号量

	DMA_Cmd(DMA2_Stream0,ENABLE);													//启用DMA传输
	DMA_Cmd(DMA2_Stream5,ENABLE);													//启用DMA传输	

	OSSemPend(&SPI1_Sem,2000,OS_OPT_PEND_BLOCKING,0,&err);							//等待信号量，超时时间为2000ms		

	if(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET)
	{
		return 1;
	}

	while(( SPI1->SR & 0x80 ) == 1 );  												//等待忙标志为0
	return 0;
}

/**********************************************************************************************************
* 函数名称:	 void DMA2_Stream0_IRQHandler(void)
* 函数说明： DMA2_Stream0 中断
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void DMA2_Stream0_IRQHandler(void)
{
	OS_ERR err;	
	OSIntEnter();	
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,DISABLE);  	
	OSSemPost(&SPI1_Sem,OS_OPT_POST_1,&err);
	OSIntExit();
}

/**********************************************************************************************************
* 函数名称:	 void DMA2_Stream5_IRQHandler(void)
* 函数说明： DMA2_Stream5
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void DMA2_Stream5_IRQHandler(void)
{
	OS_ERR err;		
	OSIntEnter();
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,DISABLE);	
	OSSemPost(&SPI1_Sem,OS_OPT_POST_1,&err);
	OSIntExit();
}











