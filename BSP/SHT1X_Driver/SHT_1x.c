
sbit SDA_sht=P0^0 ;//端口定义                    
sbit SCL_sht=P0^1 ;//端口定义   
//adr  command  r/w
//000   0011    0
//000   0011    1
//000   0001    1
//000   0010    1
//000   1111    0
#define STATUS_REG_W 	0x06
#define STATUS_REG_R 	0x07
#define MEASURE_TEMP 	0x03
#define MEASURE_HUMI 	0x05
#define RESET 			0x1e 
//void UART2_Send_Date(unsigned char dat);

unsigned char code CRC8_Table[256]=
{
    0,49,98,83,196,245,166,151,185,136,219,234,125,76,31,46,67,114,33,16,
    135,182,229,212,250,203,152,169,62,15,92,109,134,183,228,213,66,115,32,
    17,63,14,93,108,251,202,153,168,197,244,167,150,1,48,99,82,124,77,30,47,
    184,137,218,235,61,12,95,110,249,200,155,170,132,181,230,215,64,113,34,19,
    126,79,28,45,186,139,216,233,199,246,165,148,3,50,97,80,187,138,217,232,
    127,78,29,44,2,51,96,81,198,247,164,149,248,201,154,171,60,13,94,111,
    65,112,35,18,133,180,231,214,122,75,24,41,190,143,220,237,195,242,161,144,
    7,54,101,84,57,8,91,106,253,204,159,174,128,177,226,211,68,117,38,23,
    252,205,158,175,56,9,90,107,69,116,39,22,129,176,227,210,191,142,221,236,
    123,74,25,40,6,55,100,85,194,243,160,145,71,118,37,20,131,178,225,208,
    254,207,156,173,58,11,88,105,4,53,102,87,192,241,162,147,189,140,223,238,
    121,72,27,42,193,240,163,146,5,52,103,86,120,73,26,43,188,141,222,239,
    130,179,224,209,70,119,36,21,59,10,89,104,255,206,157,172 
};

float rh,t ;// rh:湿度 12 Bit   t:温度 14 Bit
unsigned int xdata SHT_TEMP_HUMI[2]={0xfefe,0xfefe };
//SHT_10[0]是温度高字节；SHT_10[1]是温度低字节；SHT_10[2]是湿度高字节；SHT_10[3]是湿度低字节；

/*********************************************************************************************************
函数名称: void delay_sht(void)
功能    ：延时5μs
入口参数：无
返回值  ：无
/*********************************************************************************************************/
//延时5μs
void delay_sht(void)
{
    unsigned char a ;
	//适用于1T单片机
    for(a=30;a>0;a--);
}

/*********************************************************************************************************					 
函数名称: void i2c_start(void)
功能    ：启动 总线
入口参数：无
返回值  ：无
/*********************************************************************************************************/
void trans_start(void)
{
    SDA_sht=1 ;
    SCL_sht=0 ;
    delay_sht();
    SCL_sht=1 ;
    delay_sht();
    SDA_sht=0 ;
    delay_sht();
    SCL_sht=0 ;
    delay_sht();
    SCL_sht=1 ;
    delay_sht();
    SDA_sht=1 ;
    delay_sht();
    SCL_sht=0 ;
}

/*********************************************************************************************************
函数名称: unsigned char SHT_send_byte(unsigned char dat)
功能    ：实现发送一个字节功能
入口参数：ch为要发送的数据
返回值  ：无
/*********************************************************************************************************/
unsigned char SHT_send_byte(unsigned char dat)
{
    unsigned char i,ack ;
    for(i=8;i>0;i--)
    {
        SDA_sht=dat&0x80 ;
        SCL_sht=1 ;
        delay_sht();
        SCL_sht=0 ;
        dat<<=1 ;
    }
    SDA_sht=1 ;//释放总线
    delay_sht();
    SCL_sht=1 ;
    delay_sht();
    ack=SDA_sht ;
    SCL_sht=0 ;
    return ack ;
}
/*********************************************************************************************************
函数名称: unsigned char SHT_recv_byte(bit ack)
功能    ：实现接收一字节功能
入口参数：ack==0,为应答；ack==1，为未应答。
返回值  ：接收到的数据
/*********************************************************************************************************/
unsigned char SHT_recv_byte(bit ack)
{
    unsigned char i,dat=0 ;
    SDA_sht=1 ;
    for(i=8;i>0;i--)
    {
        delay_sht();
        dat<<=1 ;
        SCL_sht=1 ;
        if(SDA_sht)
        dat|=0x01 ;
        SCL_sht=0 ;
    }
    SDA_sht=ack ;//ack==0,为应答；ack==1，为未应答。
    delay_sht();
    SCL_sht=1 ;
    delay_sht();
    SCL_sht=0 ;
    SDA_sht=1 ;
    return dat ;
    
}

/*********************************************************************************************************
函数名称: unsigned char CRC8_Check_Table(unsigned char *buf, unsigned char len)
功能    ：8位CRC校验
入口参数：*buf校验数据,校验指令MEASURE_TEMP，MEASURE_HUMI 
返回值  ：校验数据CRC
/********************************************************************************************************/
unsigned char CRC8_Check_Table(unsigned char*buf,unsigned char command)
{
    unsigned char crc8=0,i,j ;
    crc8=CRC8_Table[crc8^command];
    crc8=CRC8_Table[crc8^*buf];
    crc8=CRC8_Table[crc8^*(buf+1)];
    for(i=0;i<8;i++)
    {
        j=j<<1 ;
        crc8=crc8>>1 ;
        if(CY)
        j=j|0x01 ;
    }
    return j ;
}

/*********************************************************************************************************
函数名称: void calculates_temp_humi()
功能    ：温湿度计算
入口参数：无
返回值  ：无
/*********************************************************************************************************/
void Calculates_Temp_Humi()
{
    const float C1=-4.0 ;		//12位,系数C1
    const float C2=+0.0405 ;	//12位,系数C2
    const float C3=-0.0000028 ;	//12位,系数C3
    const float T1=+0.01 ;		//14位 @ 5V ,系数T1
    const float T2=+0.00008 ;	//14位 @ 5V ,系数T2
    float rh_lin ;				// rh_lin:  线性湿度
    float rh_true ;				// rh_true: 温度补偿湿度
    float t_C ;					// t_C   :  温度(℃)
    t_C=t*0.01-40 ;				//计算温度
    rh_lin=C3*rh*rh+C2*rh+C1 ;	//计算湿度
    rh_true=(t_C-25)*(T1+T2*rh)+rh_lin ;//计算:温度补偿湿度
    if(rh_true>100)rh_true=99.9 ;		//将湿度数据限制在正常范围之内
    if(rh_true<0.1)rh_true=0.1 ;		//即0.1% ~ 100%
    if(t_C>123)t_C=123 ;				//将温度限定在最大测量范围
    if(t_C<(-40))t_C=-40 ;				//将温度限定在最小测量范围
    SHT_TEMP_HUMI[0]=t_C*10 ;
    SHT_TEMP_HUMI[1]=rh_true*10 ;
}

/*********************************************************************************************************
函数名称: Temp_Humi_Collection()
功能    ：温湿度测量
入口参数：无
返回值  ：无
/*********************************************************************************************************/
void Temp_Humi_Collection()
{
    unsigned char Checksum ;
    static unsigned char xdata Task=1,Error_times[2]={0,0};
    //数据类型转换							 
    union union_int_char 
    {
        unsigned int dat16 ;
        unsigned char dat[2];
    }
    DAT16to8 ;
    
    //UART2_Send_Date(Task );
    //第一阶段，启动温度转换
    if(Task==1)
    {
        trans_start();//传感器没应答，从新发送指令处理
        if(SHT_send_byte(MEASURE_TEMP))
        {
            Task=1 ;
            Error_times[0]++;
        }
        else 
        {
            Task=2 ;
            SHT_50ms=0 ;
        }
    }
    
    //第二阶段， 查询温度转换完成
    if(Task==2)
    {
        //转换完成标志    
        if(!SDA_sht)
        {
            DAT16to8.dat[0]=SHT_recv_byte(0);
            DAT16to8.dat[1]=SHT_recv_byte(0);
            Checksum=SHT_recv_byte(1);//CRC校验 
            if(CRC8_Check_Table(DAT16to8.dat,MEASURE_TEMP)==Checksum)
            {
                t=DAT16to8.dat16 ;
                if(Error_times[0])Error_times[0]--;
                Task=3 ;
            }
            else 
            {
                Error_times[0]++;
                Task=1 ;
            }
        }
        //防止芯片死机，导致SDA_sht没有响应！
        else if(SHT_50ms==20)
        {
            SHT_50ms=0 ;
            Error_times[0]++;
            Task=1 ;
        }
    }
    
    //第三阶段， 启动湿度转换	  
    if(Task==3)
    {
        trans_start();//传感器没应答，从新发送指令处理	  
        if(SHT_send_byte(MEASURE_HUMI))
        {
            Task=3 ;
            Error_times[1]++;
        }
        else 
        {
            Task=4 ;
            SHT_50ms=0 ;
        }
    }
    
    //第四阶段， 查询湿度转换完成
    if(Task==4)
    {
        //转换完成标志 
        if(!SDA_sht)
        {
            DAT16to8.dat[0]=SHT_recv_byte(0);
            DAT16to8.dat[1]=SHT_recv_byte(0);
            Checksum=SHT_recv_byte(1);
            if(CRC8_Check_Table(DAT16to8.dat,MEASURE_HUMI)==Checksum)
            {
                rh=DAT16to8.dat16 ;
                if(Error_times[1])Error_times[1]--;
                Task=5 ;
            }
            else 
            {
                Error_times[1]++;
                Task=1 ;
            }
        }
        //防止芯片死机，导致SDA_sht没有响应！
        else if(SHT_50ms==20)
        {
            SHT_50ms=0 ;
            Error_times[1]++;
            Task=1 ;
        }
    }
    
    //第五阶段， 数据解析
    if(Task==5)
    {
        Calculates_Temp_Humi();
        Task=1 ;
    }
    //错误滤波
    if(Error_times[0]>=10||Error_times[1]>=10)
    {
        Error_times[0]=9 ;
        Error_times[1]=9 ;
        SHT_TEMP_HUMI[0]=0xfefe ;//FE故障指示
        SHT_TEMP_HUMI[1]=0xfefe ;//FE故障指示	  	  
    }
}


///*********************************************************************************************************
//函数名称: void SHT_HEX_to_ASCII()
//功能    ：16进制转ASCII码
//入口参数：无
//返回值  ：无
///*********************************************************************************************************/ 
//void SHT_HEX_to_ASCII()
//{
//    unsigned char xdata SHT_TEMP_HUMI_ASCII[29]={"TMEP=+00.0℃ / HUMI=00.0%RH"};
//    unsigned char i;
//	SHT_TEMP_HUMI_ASCII[5] = '+';	
//	SHT_TEMP_HUMI_ASCII[6]  = SHT_TEMP_HUMI[0]/100+0x30;
//	SHT_TEMP_HUMI_ASCII[7]  = SHT_TEMP_HUMI[0]%100/10+0x30;
//	SHT_TEMP_HUMI_ASCII[9]  = SHT_TEMP_HUMI[0]%10+0x30 ;
//
//	SHT_TEMP_HUMI_ASCII[20] = SHT_TEMP_HUMI[1]/100+0x30;
//	SHT_TEMP_HUMI_ASCII[21] = SHT_TEMP_HUMI[1]%100/10+0x30;
//	SHT_TEMP_HUMI_ASCII[23] = SHT_TEMP_HUMI[1]%10+0x30 ;
//	SHT_TEMP_HUMI_ASCII[27]	= 0x0d;
//	SHT_TEMP_HUMI_ASCII[28]	= 0x0a;
//	for(i=0;i<29;i++)
//	UART2_Send_Date(SHT_TEMP_HUMI_ASCII[i]);
//}




