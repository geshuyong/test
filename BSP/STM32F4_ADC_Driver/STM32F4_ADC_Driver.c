/**********************************************************************************************************
** 文件名: 		STM32F4_ADC_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		ADC驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/ 
#include "Includes.h"

unsigned short   AD_Value[N][M];		//用来存放ADC转换结果，也是DMA的目标地址
unsigned short   After_filter[M];		//用来存放求平均值之后的结果

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

	NVIC_InitStructure.NVIC_IRQChannel 	= DMA2_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

/**********************************************************************************************************
* 函数名称： void STM32F4_ADC1_GPIO_Init(void)
* 函数说明： ADC1 GPIO 初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_ADC1_GPIO_Init(void)
{
//	GPIO_InitTypeDef		GPIO_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;										//PB0 通道8
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;									//模拟输入
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;								//不带上下拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);											//初始化  
	
	NVIC_Configuration();
}

/**************************************************************************************************************************************************************************************		 
* 函数名称： void STM32F4_ADC_DMA_Configuration(void)
* 函数说明： ADC的DMA传输配置
* 输入参数： 无
* 返回参数： 无
**************************************************************************************************************************************************************************************/
void STM32F4_ADC_DMA_Configuration(void)
{
	DMA_InitTypeDef  	DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
	
	DMA_DeInit(DMA2_Stream4);
	DMA_InitStructure.DMA_Channel 				= DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)&ADC1->DR ;				//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)&AD_Value ;				//内存地址
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralToMemory;		//DMA传输方向单向
	DMA_InitStructure.DMA_BufferSize 			= N*M;
	DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，不使能
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;				//设置DMA的内存递增模式，不使能
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;	//外设数据字长	
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_PeripheralDataSize_HalfWord;	//内存数据字长
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Circular;				//设置DMA的传输模式
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;				//设置DMA的优先级别为高
	DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst 		= DMA_MemoryBurst_Single;
	
	DMA_Init(DMA2_Stream4,&DMA_InitStructure);										//初始化DMA通道	
	DMA_ITConfig(DMA2_Stream4,DMA_IT_TC,ENABLE);  									//使能指定通道中断
	
	DMA_Cmd(DMA2_Stream4,ENABLE);													//启用DMA传输	
}

/**********************************************************************************************************
* 函数名称： void STM32F4_ADC1_Init(void)
* 函数说明： ADC1 GPIO以及ADC外设初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_ADC1_Init(void)
{
	ADC_CommonInitTypeDef 	ADC_CommonInitStructure;
	ADC_InitTypeDef       	ADC_InitStructure;

	STM32F4_ADC1_GPIO_Init();														//GPIO初始化
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 							//使能ADC1时钟

	ADC_DeInit();

	ADC_CommonInitStructure.ADC_Mode 			 = ADC_Mode_Independent;			//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode 	 = ADC_DMAAccessMode_Disabled ; 	//DMA失能
	ADC_CommonInitStructure.ADC_Prescaler 		 = ADC_Prescaler_Div4;				//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);										//初始化

	ADC_InitStructure.ADC_Resolution 			 = ADC_Resolution_12b;				//12位模式
	ADC_InitStructure.ADC_ScanConvMode 			 = ENABLE ;							//ADC工作在多通道模式还是单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode 	 = ENABLE ;							//设置为DISABLE,ADC工作在单次模式。ENABLE，工作在连续模式	
	ADC_InitStructure.ADC_ExternalTrigConvEdge 	 = ADC_ExternalTrigConvEdge_None;	//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_ExternalTrigConv 		 = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign 			 = ADC_DataAlign_Right;				//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion 		 = M;								//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Init(ADC1, &ADC_InitStructure);												//ADC初始化
	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_18, 2, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
	
	ADC_TempSensorVrefintCmd(ENABLE);												//温度传感器开启
	ADC_VBATCmd(ENABLE);															//电池电压检测开启

	STM32F4_ADC_DMA_Configuration();												//DMA 初始化
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
	ADC_DMACmd(ADC1, ENABLE);														//使能ADC1 DMA传输
	ADC_Cmd(ADC1, ENABLE);															//开启AD转换器		
	ADC_SoftwareStartConv(ADC1);	
}

/**************************************************************************************************************************************************************************************	
* 函数名称:	void DMA2_Stream4_IRQHandler(void)
* 函数说明：DMA2_Stream4
* 输入参数：无
* 返回参数：无
**************************************************************************************************************************************************************************************/
void DMA2_Stream4_IRQHandler(void)
{
	OSIntEnter();
	DMA_ClearFlag(DMA2_Stream4,DMA_FLAG_TCIF4);
	filter();
	OSIntExit();
}

/**************************************************************************************************************************************************************************************		 
* 函数名称： void filter(void)
* 函数说明： 去除最大值和最小值后，平均值滤波		
* 输入参数： 无
* 返回参数： 无
**************************************************************************************************************************************************************************************/
void filter(void)
{
	int max,min,sum=0 ;
	unsigned char i,count ;
	for(i=0;i<M;i++)
	{	
		max = AD_Value[0][i];
		min = AD_Value[0][i];
		sum = AD_Value[0][i]; 
		for(count=1;count<N;count++)
		{	
			if(min>AD_Value[count][i])
			{
				min = AD_Value[count][i];		
			}
			else if(max<AD_Value[count][i])
			{
				max = AD_Value[count][i];		
			}
			sum+=AD_Value[count][i];			
		}	
		sum = sum-min-max;
		After_filter[i]=sum/(N-2) ;		
		sum=0 ;
	}
}

/**************************************************************************************************************************************************************************************		 
* 函数名称： float STM32F4_Get_Temprate(void)
* 函数说明： 获取温度值
* 输入参数： 无
* 返回参数： 无
**************************************************************************************************************************************************************************************/
float STM32F4_Get_Temprate(void)
{
 	float value;
	value = (float)After_filter[0]*(3.3/4096);		//电压值
//	value = (value -0.76)/0.0025 + 25; 				//转换为温度值 
	value = (value -0.76f)*400 + 25; 				//转换为温度值 
	return value;	
}
/**************************************************************************************************************************************************************************************		 
* 函数名称： float STM32F4_Get_VBAT(void)
* 函数说明： 获取电池电压值
* 输入参数： 无
* 返回参数： 无
**************************************************************************************************************************************************************************************/
float STM32F4_Get_VBAT(void)
{
 	float value;
	value = (float)After_filter[1]*(3.3/4096)*2;	//电压值
	return value;
}


















