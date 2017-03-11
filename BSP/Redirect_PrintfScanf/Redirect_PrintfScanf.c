/**********************************************************************************************************
** 文件名: 		SRedirect_PrintfScanf.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2014年11月17日19:26:43
** 功能:		重定向Printf和Scanf接口
** 相关文件:	
** 修改日志：	 
**********************************************************************************************************/ 
#include "Includes.h"
/**********************************************************************************************************
* 函数名称 : int fputc(int ch, FILE *f)
* 函数说明 : printf重定向到串口
* 输入参数 : None
* 输出参数 : None
* 返回参数 : None
**********************************************************************************************************/
int fputc(int ch, FILE *f)
{ 
	#if		UART1_PRINFT_SCANF
	USART_ClearFlag(USART1, USART_FLAG_TXE);
	USART_ClearFlag(USART1, USART_FLAG_TC);	
	USART_SendData(USART1, (u8) ch);
	while((USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)) ;	
	#endif
	#if		UART2_PRINFT_SCANF
	USART_ClearFlag(USART2, USART_FLAG_TXE);
	USART_ClearFlag(USART2, USART_FLAG_TC);	
	USART_SendData(USART2, (u8) ch);
	while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)) ;
	#endif	
	#if		UART3_PRINFT_SCANF
	USART_ClearFlag(USART3, USART_FLAG_TXE);
	USART_ClearFlag(USART3, USART_FLAG_TC);	
	USART_SendData(USART3, (u8) ch);
	while((USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)) ;
	#endif		
	#if		UART4_PRINFT_SCANF
	USART_ClearFlag(UART4, USART_FLAG_TXE);
	USART_ClearFlag(UART4, USART_FLAG_TC);	
	USART_SendData(UART4, (u8) ch);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)) ;
	#endif		
	#if		UART5_PRINFT_SCANF
	USART_ClearFlag(UART5, USART_FLAG_TXE);
	USART_ClearFlag(UART5, USART_FLAG_TC);	
	USART_SendData(UART5, (u8) ch);
	while((USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)) ;
	#endif		
	return ch;
}

/**********************************************************************************************************
* 函数名称 : int fgetc(FILE *f)
* 函数说明 : scanf重定向到串口
* 输入参数 : None
* 输出参数 : None
* 返回参数 : 读取到的字符
**********************************************************************************************************/
int fgetc(FILE *f)
{
	#if		UART1_PRINFT_SCANF
	while((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET));
	return (USART_ReceiveData(USART1));
	#endif	
	#if		UART2_PRINFT_SCANF
	while((USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET));
	return (USART_ReceiveData(USART2));
	#endif	
	#if		UART3_PRINFT_SCANF
	while((USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET));
	return (USART_ReceiveData(USART3));
	#endif	
	#if		UART4_PRINFT_SCANF
	while((USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET));
	return (USART_ReceiveData(UART4));
	#endif		
	#if		UART5_PRINFT_SCANF
	while((USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET));
	return (USART_ReceiveData(UART5));
	#endif	
	return 0;
}

/**********************************************************************************************************
* 函数名称 : int osprintf(char* str , ...)
* 函数说明 : 从串口打印数据，操作系统可重入
* 输入参数 : 格式化参数
* 返回参数 : 返回字符串长度
**********************************************************************************************************/
int osprintf(char* str , ...)
{
	va_list	ap;		
	int		len;	
	char 	*buf;
	CPU_SR_ALLOC();		
	buf = (char*)ram_malloc(1024); 			//开辟内存空间,单条日志的最大长度为LOG_MAX_SIZE.
	if(buf==NULL) return 0;
	CPU_CRITICAL_ENTER();
	va_start(ap, str);
	len = vsnprintf(buf, 1024, str, ap);
	va_end(ap);	
	CPU_CRITICAL_EXIT();	
	if(len>0)
	{
		UART_Send(RS232_1,5000,buf,len);	
	}
	ram_free(buf); 					  		//释放内存空间	
	return len;
}




/***********************************文件结束***********************************/
