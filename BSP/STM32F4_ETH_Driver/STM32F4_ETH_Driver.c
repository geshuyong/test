/**********************************************************************************************************
** 文件名: 		STM32F4_ETH_Driver.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		以太网驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/ 
#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "STM32F4_ETH_Driver.h"
#include "Includes.h"

/* Ethernet Rx & Tx DMA Descriptors */
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
/* Ethernet Driver Receive buffers  */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; 

/* Ethernet Driver Transmit buffers */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; 

/**********************************************************************************************************
* 函数名称:	 void STM32F4_ETH_GPIO_Config(void)
* 函数说明： 以太网GPIO配置
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_ETH_GPIO_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC , ENABLE); //使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);				//使能SYSCFG时钟

	/************************* MCO 引脚配置 *****************************/
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	/************************* MCO 引脚配置END **************************/
	RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1); 					//输出时钟给PHY HSE clock = 25MHz 在 PA8 pin (MCO) 	
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);	//RMII Media 接口选择
	/********************** ETHERNET 接口引脚配置 ***********************
	ETH_RST  -------------------------> PD10
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_RMII_REF_CLK------------------> PA1
	ETH_RMII_CRS_DV ------------------> PA7
	ETH_RMII_RXD0 --------------------> PC4
	ETH_RMII_RXD1 --------------------> PC5
	ETH_RMII_TX_EN -------------------> PB11
	ETH_RMII_TXD0 --------------------> PB12
	ETH_RMII_TXD1 --------------------> PB13	
	*******************************************************************/

	/* Configure PA1, PA2 and PA7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

	/* Configure PB11, PB12 and PB13 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
	
	/* Configure PD10 */	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIOD->BSRRL = GPIO_Pin_10;					//不进行复位
}

/*************************************************************************************************************
* 函数名称:	 static void NVIC_Configuration(void)
* 函数说明： 中断优先级分组方式，中断优先级，使能配置
* 输入参数： 无
* 返回参数： 无
*************************************************************************************************************/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************************************************
* 函数名称:	void STM32F4_Ethernet_Init(void)
* 函数说明： 以太网MAC和DMA配置
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_Ethernet_Cfg(void)
{
	ETH_InitTypeDef		ETH_InitStructure;	
	unsigned int		i;
	/* ETHERNET Configuration --------------------------------------------------*/
	ETH_StructInit(&ETH_InitStructure);
	/*------------------------   MAC   -----------------------------------*/
	ETH_InitStructure.ETH_AutoNegotiation 			= ETH_AutoNegotiation_Enable;
	ETH_InitStructure.ETH_LoopbackMode 				= ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission 		= ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip 		= ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll 				= ETH_ReceiveAll_Disable;
	ETH_InitStructure.ETH_BroadcastFramesReception 	= ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode 			= ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter 	= ETH_MulticastFramesFilter_Perfect;
	ETH_InitStructure.ETH_UnicastFramesFilter 		= ETH_UnicastFramesFilter_Perfect;
	#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload			= ETH_ChecksumOffload_Enable;
	#endif

	/*------------------------   DMA   -----------------------------------------*/
	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
	the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
	if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame 	= ETH_DropTCPIPChecksumErrorFrame_Enable; 
	ETH_InitStructure.ETH_ReceiveStoreForward 			= ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward 			= ETH_TransmitStoreForward_Enable;
	ETH_InitStructure.ETH_ForwardErrorFrames 			= ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames 	= ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate 			= ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats 			= ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst 					= ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength 				= ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength 				= ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration 				= ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);							//使能以太网接收中断
	
	ETH_MACAddressConfig(ETH_MAC_Address0, sys_cfg.mac); 							//配置网络控制器的MAC地址。
	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB); 				//初始化发送描述符列表：链模式
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB); 				//初始化接收描述符列表：链模式
	for(i=0; i<ETH_RXBUFNB; i++)			 										//使能以太网接收中断
	{
		ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
	}
	
#ifdef CHECKSUM_BY_HARDWARE
	for(i=0; i<ETH_TXBUFNB; i++)   													//启用校验和插入为Tx帧
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull); 
	}		 
#endif
}


/**********************************************************************************************************
* 函数名称:	void STM32F4_Ethernet_Init(void)
* 函数说明： 以太网初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void STM32F4_Ethernet_Init(void)
{
	RCC_ClocksTypeDef  	rcc_clocks;
	unsigned int		hclk = 60000000, tmpreg = 0,timeout = 0;
	OS_ERR 				err;	
	NVIC_Configuration();	
	STM32F4_ETH_GPIO_Config();						//以太网控制器引脚配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);	//使能以太网时钟


	//PHY进行硬件引脚复位
	GPIOD->BSRRH = GPIO_Pin_10;	
	OSTimeDly(5,OS_OPT_TIME_DLY,&err);
	GPIOD->BSRRL = GPIO_Pin_10;	
	
	ETH_DeInit();									//初始化以太网控制器默认配置 Reset ETHERNET on AHB Bus
	ETH_SoftwareReset();							//软复位以太网控制器
	timeout = 0; 		
	while(ETH_GetSoftwareResetStatus() == SET)		//等待以太网控制器复位成功
	{
		timeout++;
		if(timeout%200) 
		{					
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		}		
		if(timeout>20000) 
		{			
			RUN_LEN_ON;
			LED_RUN_ON;
			GPIOD->BSRRH = GPIO_Pin_10;	
			OSTimeDly(100,OS_OPT_TIME_DLY,&err);
			GPIOD->BSRRL = GPIO_Pin_10;	
			RUN_LEN_OFF;
			LED_RUN_OFF;
			OSTimeDly(400,OS_OPT_TIME_DLY,&err);
			timeout = 0;
		}
	} 
	/*---------------------- ETHERNET MACMIIAR Configuration -------------------*/
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR;
	/* Clear CSR Clock Range CR[2:0] bits */
	tmpreg &= MACMIIAR_CR_MASK;
	/* Get hclk frequency value */
	RCC_GetClocksFreq(&rcc_clocks);
	hclk = rcc_clocks.HCLK_Frequency;

	/* Set CR bits depending on hclk value */
	if((hclk >= 20000000)&&(hclk < 35000000))
	{
	/* CSR Clock Range between 20-35 MHz */
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div16;
	}
	else if((hclk >= 35000000)&&(hclk < 60000000))
	{
	/* CSR Clock Range between 35-60 MHz */ 
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div26;
	}  
	else if((hclk >= 60000000)&&(hclk < 100000000))
	{
	/* CSR Clock Range between 60-100 MHz */ 
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
	}  
	else if((hclk >= 100000000)&&(hclk < 150000000))
	{
	/* CSR Clock Range between 100-150 MHz */ 
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;
	}
	else /* ((hclk >= 150000000)&&(hclk <= 168000000)) */
	{
	/* CSR Clock Range between 150-168 MHz */ 
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div102;    
	}

	/* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
	ETH->MACMIIAR = (uint32_t)tmpreg;  	

	timeout = 0; 		
	while(1)	//检测PHY复位成功
	{
		tmpreg  = (ETH_ReadPHYRegister(PHY_ADDRESS, PHY_ID1))<<16;
		tmpreg |=  ETH_ReadPHYRegister(PHY_ADDRESS, PHY_ID2);
		if((tmpreg&PHY_ID_MASK)==PHY_ID)
		{
			break;
		}
		timeout++;	
		if(timeout>200) 
		{			
			RUN_LEN_ON;
			LED_RUN_ON;
			GPIOD->BSRRH = GPIO_Pin_10;	
			OSTimeDly(100,OS_OPT_TIME_DLY,&err);
			GPIOD->BSRRL = GPIO_Pin_10;	
			RUN_LEN_OFF;
			LED_RUN_OFF;
			OSTimeDly(400,OS_OPT_TIME_DLY,&err);
			timeout = 0;
		}
	} 
    ETH_WritePHYRegister(PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);	//设置自动协商
	STM32F4_Ethernet_Cfg();	
}


/**********************************************************************************************************
* 函数名称:	 void ETH_Linked_Check(void)
* 函数说明： 检查以太网是否已经连接并且已经协商完成
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void ETH_Linked_Check(void)
{
	static char			Link_status = 0;
	unsigned int		phy_id;
	phy_id  = (ETH_ReadPHYRegister(PHY_ADDRESS, PHY_ID1))<<16;
	phy_id |=  ETH_ReadPHYRegister(PHY_ADDRESS, PHY_ID2);
	if((!(ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BCR) & PHY_AutoNegotiation)) || ((phy_id&PHY_ID_MASK)!=PHY_ID))
	{
		Log_Debug("Ethernet  Reinit.");	
		STM32F4_Ethernet_Init();
	}	
	if((ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR) & PHY_Linked_Status) && (ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete))
	{	
		if(Link_status == 0)
		{	
			Link_status = 1;
			Log_Debug("PHY Linked.");			
			STM32F4_Ethernet_Cfg();
			ETH_Start();																	//启用MAC和DMA传输和接收			
		}
	}
	else
	{
		Link_status = 0;
		ETH_Stop();
	}
}

/**********************************************************************************************************
 函数原型：void ETH_IRQHandler(void)
 入口参数：无
 出口参数：无
 函数功能：以太网中断 
**********************************************************************************************************/
void ETH_IRQHandler(void)
{
	OSIntEnter();
	while(ETH_GetRxPktSize(DMARxDescToGet) != 0) 
	{		
		LwIP_Pkt_Handle();
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);		
	
	
	OSIntExit();
}









