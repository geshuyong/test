/*******************************************************************************
** 文件名: 		Cloud_IOT_200_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 作者: 		盖书永 
** 生成日期: 	2011-04-10
** 功能:		Cloud IOT-200设备底层驱动
** 相关文件:	无
** 修改日志：	 
*******************************************************************************/

#include "Includes.h"

#define		SWITCH_IN1			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) 

#define		SWITCH_IN2			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) 

#define		SWITCH_IN3			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) 

#define		SWITCH_IN4			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) 

#define		SWITCH_OUT1			GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_14)
#define		SWITCH_OUT2			GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10)

#define		SWITCH_OUT1_OFF		GPIOB->BSRRH = GPIO_Pin_10
#define		SWITCH_OUT1_ON		GPIOB->BSRRL = GPIO_Pin_10 

#define		SWITCH_OUT2_OFF		GPIOB->BSRRH = GPIO_Pin_14   
#define		SWITCH_OUT2_ON		GPIOB->BSRRL = GPIO_Pin_14 

#define		LED_IN1_OFF			GPIOD->BSRRH = GPIO_Pin_15 
#define		LED_IN1_ON			GPIOD->BSRRL = GPIO_Pin_15 

#define		LED_IN2_OFF			GPIOC->BSRRH = GPIO_Pin_3 
#define		LED_IN2_ON			GPIOC->BSRRL = GPIO_Pin_3 

#define		LED_IN3_OFF			GPIOC->BSRRH = GPIO_Pin_0 
#define		LED_IN3_ON			GPIOC->BSRRL = GPIO_Pin_0 

#define		LED_IN4_OFF			GPIOD->BSRRH = GPIO_Pin_0 
#define		LED_IN4_ON			GPIOD->BSRRL = GPIO_Pin_0 

#define		LED_OUT1_OFF		GPIOB->BSRRH = GPIO_Pin_9 
#define		LED_OUT1_ON			GPIOB->BSRRL = GPIO_Pin_9 

#define		LED_OUT2_OFF		GPIOE->BSRRH = GPIO_Pin_6  
#define		LED_OUT2_ON			GPIOE->BSRRL = GPIO_Pin_6 


//--------------------------------------------开关量输入输出-------------------------------------------------------

/*******************************************************************************
* 函数名称： void Switch_Init(void)
* 函数说明： 开关量初始化
* 输入参数： 无
* 返回参数： 无
*******************************************************************************/
void Switch_Init(void)
{
    GPIO_InitTypeDef    	GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

	//开关量输入
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;     
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;				
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;     
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//开关量输出
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_10|GPIO_Pin_14;				
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;     
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//LED输出	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;				  
    GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0|GPIO_Pin_3;				 
    GPIO_Init(GPIOC, &GPIO_InitStructure);		
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0|GPIO_Pin_13|GPIO_Pin_15;				 
    GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;				 
    GPIO_Init(GPIOE, &GPIO_InitStructure);	
}

/*******************************************************************************
* 函数名称： void Set_Input_LED_Status(int addr, int status)
* 函数说明： 设置开关量输入LED状态
* 输入参数： LED地址，状态
* 返回参数： 无
*******************************************************************************/
void Set_Input_LED_Status(int addr, int status)
{
	switch(addr)
	{
		case 1:
			if(status)	LED_IN1_ON;
			else		LED_IN1_OFF;
			break;
		case 2:
			if(status)	LED_IN2_ON;
			else		LED_IN2_OFF;
			break;	
		case 3:
			if(status)	LED_IN3_ON;
			else		LED_IN3_OFF;
			break;	
		case 4:
			if(status)	LED_IN4_ON;
			else		LED_IN4_OFF;
			break;	
		default:
			break;	
	}
}

/*******************************************************************************
* 函数名称： void Set_Output_LED_Status(int addr, int status)
* 函数说明： 设置开关量输出LED状态
* 输入参数： LED地址，状态
* 返回参数： 无
*******************************************************************************/
void Set_Output_LED_Status(int addr, int status)
{
	switch(addr)
	{
		case 1:
			if(status)	LED_OUT1_ON;
			else		LED_OUT1_OFF;
			break;
		case 2:
			if(status)	LED_OUT2_ON;
			else		LED_OUT2_OFF;
			break;		
		default:
			break;	
	}
}


/*******************************************************************************
* 函数名称： int Read_switch_input_status(int addr)
* 函数说明： 读取开关量输入状态
* 输入参数： 开关量地址号
* 返回参数： 状态
*******************************************************************************/
int Read_switch_input_status(int addr)
{
	switch(addr)
	{
		case 1:
			return !SWITCH_IN1;
		case 2:
			return !SWITCH_IN2;		
		case 3:
			return !SWITCH_IN3;		
		case 4:
			return !SWITCH_IN4;	
		default:
			return 0;
	}		
}

/*******************************************************************************
* 函数名称： void Write_switch_output_status(int addr, int status)
* 函数说明： 置开关量输出状态
* 输入参数： 开关量地址号, 设置状态
* 返回参数： 无
*******************************************************************************/
void Write_switch_output_status(int addr, int status)
{
	switch(addr)
	{
		case 1:
			if(status)	SWITCH_OUT1_ON;
			else		SWITCH_OUT1_OFF;
			break;
		case 2:
			if(status)	SWITCH_OUT2_ON;
			else		SWITCH_OUT2_OFF;			
			break;		
		default:
			break;
	}		
}


