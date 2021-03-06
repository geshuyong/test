/**********************************************************************************************************
** 文件名: 		TTS_Driver.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.11
** 作者: 		盖书永
** 生成日期: 	2015年1月15日13:52:17
** 功能:		TTS 语音服务驱动
** 相关文件:					
** 修改日志：	 
**********************************************************************************************************/
#ifndef _TTS_DRIVER_H_
#define _TTS_DRIVER_H_

#define	INTI_OK			0X4A
#define	CMD_OK			0X41
#define	CMD_ERR			0X45
#define	BUSY			0X4E
#define	IDLE			0X4F

/**********************************************************************************************************
* 函数名称： void TTS_START(void) 
* 函数说明： 在调用语音服务之前，先调用此函数，目的防止多任务使用语音服务，导致冲突，语音服务结束后调用TTS_END()
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_START(void);

/**********************************************************************************************************
* 函数名称： void TTS_END(void)
* 函数说明： 语音服务结束后，调用此函数。
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_END(void);

/**********************************************************************************************************
* 函数名称： void TTS_Init(void)
* 函数说明： TTS初始化
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_Init(void);

/**********************************************************************************************************
* 函数名称： int TTS_Reset(void)
* 函数说明： 复位
* 输入参数： 无
* 返回参数： 无
**********************************************************************************************************/
void TTS_Reset(void);

/**********************************************************************************************************
* 函数名称： int TTS_Check_Status(void);
* 函数说明： 状态查询
* 输入参数： 无
* 返回参数： 获取成功返回响应状态，获取失败返回0
**********************************************************************************************************/
int TTS_Check_Status(void);

/**********************************************************************************************************
* 函数名称： int TTS_Begin_Sleep(void)
* 函数说明： 进入睡眠
* 输入参数： 无
* 返回参数： 睡眠成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Begin_Sleep(void);

/**********************************************************************************************************
* 函数名称： int TTS_Begin_Wake(void)
* 函数说明： 唤醒
* 输入参数： 无
* 返回参数： 唤醒成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Begin_Wake(void);

/**********************************************************************************************************
* 函数名称： int TTS_Speech_Synthesis(char *txt, int len, char Encoding, char outline)
* 函数说明： 语音合成
* 输入参数： 输入文本缓存指针，文本缓存区长度，编码方式（0：GB2312、1：GBK、3：BIG5、3：UNICODE、4:UTF8）,输出线路：0-SIM800,1-AOUT
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Speech_Synthesis(char *txt, int len, char Encoding, char outline);

/**********************************************************************************************************
* 函数名称： int TTS_Speech_Synthesis(char *buf, int len, char Encoding)
* 函数说明： 停止语音合成
* 输入参数： 无
* 返回参数： 成功返回1，失败返回0
**********************************************************************************************************/
int TTS_Stop_Speech_Synthesis(void);

#endif
