/*******************************************************************************
** 文件名: 		STM32F4_TIM_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		定时器驱动
** 相关文件:					
** 修改日志：	 
*******************************************************************************/ 
#include "stm32f4xx_tim.h"

unsigned int TIM5_CH4_CAP_VAL[2]={0,0};


/*******************************************************************************
* 函数名称： void STM32F4_TIM2_CH1_PWM_Init(void)
* 函数说明： 定时器2 通道1 PWM初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void STM32F4_TIM2_CH1_PWM_Init(void)
{
	GPIO_InitTypeDef			GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  			TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  			
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_TIM2); 			//GPIOA15复用位定时器2
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_15; 			
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;					//复用功能
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP; 				//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP; 				//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 							//初始化
	
	//总线APB1：168M/8=21Mhz的计数频率计数到10000,PWM频率为21M/10000=2.1Khz 
	//总线APB2：84M/4 =21Mhz的计数频率计数到10000,PWM频率为21M/10000=2.1Khz 	
	TIM_TimeBaseStructure.TIM_Prescaler		= 8-1;  
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseStructure.TIM_Period		= 9999;   			  	//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//初始化TIM2 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode 		= TIM_OCMode_PWM1; 			//选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_High; 		//输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse		= 0;						//设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  						
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  				//使能TIMx在CCRx上的预装载寄存器
	TIM_ARRPreloadConfig(TIM2,ENABLE);	
	TIM_Cmd(TIM2, ENABLE);  										//使能TIMx	
}

/*********************************************************************************************************		 
函数名称: void Set_TIM2_CH1_PWM_DutyCycle(unsigned int duty)
功能 ：   调节PWM的占空比 分辨率为0.01%
入口参数：百分比 分辨率为0.01%
返回值 ： 无
*********************************************************************************************************/
void Set_TIM2_CH1_PWM_DutyCycle(double duty)
{
	unsigned int val;
	if(duty>100) 	duty = 100;
	else if(duty<0) duty = 0;
	val = (unsigned int)(duty * 10000);
	if(val>9999) val = 9999;
	TIM_SetCompare1(TIM2,val); 
}

/*******************************************************************************
* 函数名称： void STM32F4_TIM5_CH4_Cap_Init(void)
* 函数说明： 定时器5 通道4 输入捕获进行脉宽测量，最大测量脉宽为4294S，输入引脚为PIN-A3
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void STM32F4_TIM5_CH4_Cap_Init(void)
{
	GPIO_InitTypeDef			GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  			TIM_ICInitStructure;
	NVIC_InitTypeDef 			NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  			
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5); 			//GPIOA3复用位定时器5
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_3; 			
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;					//复用功能
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP; 				//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL; 			
	GPIO_Init(GPIOA,&GPIO_InitStructure); 							//初始化
	
	//总线APB1：168M/168 =1Mhz的计数频率
	//总线APB2：84M/84   =1Mhz的计数频率	
	TIM_TimeBaseStructure.TIM_Prescaler		= 168-1;  
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseStructure.TIM_Period		= 0xFFFFFFFF;   		//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);

	//初始化TIM5输入捕获参数
	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_4; 			//选择输入端 IC4映射到TI1上
	TIM_ICInitStructure.TIM_ICPolarity 	= TIM_ICPolarity_BothEdge;	//BI沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 		//配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter 	= 0x00;						//配置输入滤波器 不滤波
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	TIM_ITConfig(TIM5,TIM_IT_CC4,ENABLE);							//允许CC4IE捕获中断	

	TIM_Cmd(TIM5,ENABLE ); 											//使能定时器5
}

/*******************************************************************************
* 函数名称： void TIM5_IRQHandler(void)
* 函数说明： 定时器5中断，获取脉宽
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void TIM5_IRQHandler(void)
{ 	
	static unsigned int CaptureNumber = 0;
	static unsigned int CaptureValue1 = 0;
	static unsigned int CaptureValue2 = 0;
	if(TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET)	//捕获4发生捕获事件
	{	
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC4); 	//清除中断标志位	
		if(CaptureNumber == 0)
		{
			CaptureValue1 = TIM_GetCapture1(TIM5);	//获取当前的捕获值.
			if (CaptureValue1 > CaptureValue2)
			{
				TIM5_CH4_CAP_VAL[1] = CaptureValue1 - CaptureValue2; 
			}
			else
			{
				TIM5_CH4_CAP_VAL[1] = (0xFFFFFFFF - CaptureValue2) + CaptureValue1; 
			}
			CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
			CaptureValue2 = TIM_GetCapture1(TIM5);	//获取当前的捕获值.
			if (CaptureValue2 > CaptureValue1)
			{
				TIM5_CH4_CAP_VAL[0] = CaptureValue2 - CaptureValue1; 
			}
			else
			{
				TIM5_CH4_CAP_VAL[0] = (0xFFFFFFFF - CaptureValue1) + CaptureValue2; 
			}
			CaptureNumber = 2;
		}   
	}	
}



void inpwm_init(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* TIM3 configuration: PWM Input mode ------------------------
	The external signal is connected to TIM3 CH2 pin (PA.07), 
	The Rising edge is used as active edge,
	The TIM3 CCR2 is used to compute the frequency value 
	The TIM3 CCR1 is used to compute the duty cycle value
	------------------------------------------------------------ */

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);

	/* Select the TIM3 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);

	/* Enable the Master/Slave Mode */
	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the CC2 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC2|TIM_IT_CC1, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_CC1) == SET)
	{
		DutyCycle = TIM_GetCapture1(TIM3)+3;
		Frequency =  / DutyCycle;
		TIM_ClearFlag(TIM3,TIM_FLAG_CC1);
	}
	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_CC2) == SET)
	{
		IC2Value = TIM_GetCapture2(TIM3)+4;
		TIM_ClearFlag(TIM3,TIM_FLAG_CC2);
		
	}
}
















