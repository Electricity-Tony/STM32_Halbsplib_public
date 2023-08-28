/**
 * @file bsp_NRF24L01.c
 * @brief NRF24L01板级支持包
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-12-19
 * @copyright 
 * @par 日志:
 *   V1.0 规范化NRF24L01收发驱动库，实现基本功能
 *	 V1.1 修正检测是否在线多次检测，修改RX获得包的状态位判断，修改IO设定方式
 */
 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_NRF24L01.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/                              

//宏定义6组通讯地址对应通道
#define NRF_CH0_ADDRESS 0x01
#define NRF_CH1_ADDRESS 0x02
#define NRF_CH2_ADDRESS 0x04
#define NRF_CH3_ADDRESS 0x08
#define NRF_CH4_ADDRESS 0x10
#define NRF_CH5_ADDRESS 0x20

#define RF_CH0 40  //设置通信频道值，0~127


//设置发送与接收地址
const uint8_t TX0_ADDRESS[TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //设置发送接收地址
const uint8_t RX0_ADDRESS[RX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01};

uint8_t NRF_RX_date[33];

//自动应答，不使用自动应答时请注释
#define EN_NRF_ACK

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/


/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi,uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
  {
    d_read=0xFF;
  }
  return d_read; 
}

/**
  * 函数功能: 检测24L01是否存在
  * 输入参数: 无
  * 返 回 值: 0，成功;1，失败
  * 说    明：无          
  */ 
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5];
	uint8_t i,chech_flag = 1;
  
	while(chech_flag)
	{
		for(i=0;i<5;i++) buf[i]=0xA5;
		
		NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
		NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址
		
		for(i=0;i<5;i++)if(buf[i]!=0xA5)break;	 							   
		if(i!=5)chech_flag = 1;//检测24L01错误	
		else chech_flag = 0;
	}
	return 0;		 //检测到24L01
}	 	 

/**
  * 函数功能: SPI写寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:指定寄存器地址
  *           
  */ 
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  NRF24L01_SPI_CS_ENABLE();                 //使能SPI传输
  status =SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器号 
  SPIx_ReadWriteByte(&hspi_NRF24L01,value);      //写入寄存器的值
  NRF24L01_SPI_CS_DISABLE();                 //禁止SPI传输	   
  return(status);       			//返回状态值
}

/**
  * 函数功能: 读取SPI寄存器值
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:要读的寄存器
  *           
  */ 
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	NRF24L01_SPI_CS_ENABLE();          //使能SPI传输		
  SPIx_ReadWriteByte(&hspi_NRF24L01,reg);   //发送寄存器号
  reg_val=SPIx_ReadWriteByte(&hspi_NRF24L01,0XFF);//读取寄存器内容
  NRF24L01_SPI_CS_DISABLE();          //禁止SPI传输		    
  return(reg_val);           //返回状态值
}	

/**
  * 函数功能: 在指定位置读出指定长度的数据
  * 输入参数: 无
  * 返 回 值: 此次读到的状态寄存器值 
  * 说    明：无
  *           
  */ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,uint8_t_ctr;	   
  
  NRF24L01_SPI_CS_ENABLE();           //使能SPI传输
  status=SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器值(位置),并读取状态值   	   
 	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi_NRF24L01,0XFF);//读出数据
  }
  NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
  return status;        //返回读到的状态值
}

/**
  * 函数功能: 在指定位置写指定长度的数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:寄存器(位置)  *pBuf:数据指针  len:数据长度
  *           
  */ 
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;	    
 	NRF24L01_SPI_CS_ENABLE();          //使能SPI传输
  status = SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器值(位置),并读取状态值
  for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPIx_ReadWriteByte(&hspi_NRF24L01,*pBuf++); //写入数据	 
  }
  NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
  return status;          //返回读到的状态值
}				   

/**
  * 函数功能: 启动NRF24L01发送一次数据
  * 输入参数: 无
  * 返 回 值: 发送完成状况
  * 说    明：txbuf:待发送数据首地址
  *           
  */ 
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	NRF24L01_CE_LOW();
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE_HIGH();//启动发送	 
  
	while(NRF24L01_IRQ_PIN_READ()!=0);//等待发送完成 

	
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	 	
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}

/**
  * 函数功能:启动NRF24L01接收一次数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta!=0x0e)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    

/**
  * 函数功能: 该函数初始化NRF24L01到RX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_LOW();	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC 
	
	//使用自动应答
	#ifdef  EN_NRF_ACK	
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,NRF_CH0_ADDRESS);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,NRF_CH0_ADDRESS);//使能通道0的接收地址  	 
  #endif
	
	//不使用自动应答
	#ifndef EN_NRF_ACK
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);    //失能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x00);//失能通道0的接收地址  	
	#endif
	
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,RF_CH0);	     //设置RF通信频率为:RF_CH0		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	
  NRF24L01_CE_HIGH(); //CE为高,进入接收模式 
  HAL_Delay(1);
}						 

/**
  * 函数功能: 该函数初始化NRF24L01到TX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_LOW();	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX0_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

	//使用自动应答
	#ifdef  EN_NRF_ACK
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,NRF_CH0_ADDRESS);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,NRF_CH0_ADDRESS); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0xff);//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
	#endif
	
	//不使用自动应答
	#ifndef EN_NRF_ACK
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);     //失能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x00); //失能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x00);//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
	#endif

	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,RF_CH0);       //设置RF通道为：RF_CH0
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE_HIGH();//CE为高,10us后启动发送
  HAL_Delay(1);

}

