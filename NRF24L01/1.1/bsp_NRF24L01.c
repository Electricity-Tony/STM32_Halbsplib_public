/**
 * @file bsp_NRF24L01.c
 * @brief NRF24L01�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-12-19
 * @copyright 
 * @par ��־:
 *   V1.0 �淶��NRF24L01�շ������⣬ʵ�ֻ�������
 *	 V1.1 ��������Ƿ����߶�μ�⣬�޸�RX��ð���״̬λ�жϣ��޸�IO�趨��ʽ
 */
 
 
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_NRF24L01.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/                              

//�궨��6��ͨѶ��ַ��Ӧͨ��
#define NRF_CH0_ADDRESS 0x01
#define NRF_CH1_ADDRESS 0x02
#define NRF_CH2_ADDRESS 0x04
#define NRF_CH3_ADDRESS 0x08
#define NRF_CH4_ADDRESS 0x10
#define NRF_CH5_ADDRESS 0x20

#define RF_CH0 40  //����ͨ��Ƶ��ֵ��0~127


//���÷�������յ�ַ
const uint8_t TX0_ADDRESS[TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //���÷��ͽ��յ�ַ
const uint8_t RX0_ADDRESS[RX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01};

uint8_t NRF_RX_date[33];

//�Զ�Ӧ�𣬲�ʹ���Զ�Ӧ��ʱ��ע��
#define EN_NRF_ACK

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/


/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
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
  * ��������: ���24L01�Ƿ����
  * �������: ��
  * �� �� ֵ: 0���ɹ�;1��ʧ��
  * ˵    ������          
  */ 
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5];
	uint8_t i,chech_flag = 1;
  
	while(chech_flag)
	{
		for(i=0;i<5;i++) buf[i]=0xA5;
		
		NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
		NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ
		
		for(i=0;i<5;i++)if(buf[i]!=0xA5)break;	 							   
		if(i!=5)chech_flag = 1;//���24L01����	
		else chech_flag = 0;
	}
	return 0;		 //��⵽24L01
}	 	 

/**
  * ��������: SPIд�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:ָ���Ĵ�����ַ
  *           
  */ 
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  NRF24L01_SPI_CS_ENABLE();                 //ʹ��SPI����
  status =SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ����� 
  SPIx_ReadWriteByte(&hspi_NRF24L01,value);      //д��Ĵ�����ֵ
  NRF24L01_SPI_CS_DISABLE();                 //��ֹSPI����	   
  return(status);       			//����״ֵ̬
}

/**
  * ��������: ��ȡSPI�Ĵ���ֵ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:Ҫ���ļĴ���
  *           
  */ 
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����		
  SPIx_ReadWriteByte(&hspi_NRF24L01,reg);   //���ͼĴ�����
  reg_val=SPIx_ReadWriteByte(&hspi_NRF24L01,0XFF);//��ȡ�Ĵ�������
  NRF24L01_SPI_CS_DISABLE();          //��ֹSPI����		    
  return(reg_val);           //����״ֵ̬
}	

/**
  * ��������: ��ָ��λ�ö���ָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: �˴ζ�����״̬�Ĵ���ֵ 
  * ˵    ������
  *           
  */ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,uint8_t_ctr;	   
  
  NRF24L01_SPI_CS_ENABLE();           //ʹ��SPI����
  status=SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi_NRF24L01,0XFF);//��������
  }
  NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
  return status;        //���ض�����״ֵ̬
}

/**
  * ��������: ��ָ��λ��дָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:�Ĵ���(λ��)  *pBuf:����ָ��  len:���ݳ���
  *           
  */ 
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;	    
 	NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����
  status = SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPIx_ReadWriteByte(&hspi_NRF24L01,*pBuf++); //д������	 
  }
  NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
  return status;          //���ض�����״ֵ̬
}				   

/**
  * ��������: ����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: �������״��
  * ˵    ����txbuf:�����������׵�ַ
  *           
  */ 
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	NRF24L01_CE_LOW();
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE_HIGH();//��������	 
  
	while(NRF24L01_IRQ_PIN_READ()!=0);//�ȴ�������� 

	
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	 	
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}

/**
  * ��������:����NRF24L01����һ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta!=0x0e)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    

/**
  * ��������: �ú�����ʼ��NRF24L01��RXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_LOW();	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC 
	
	//ʹ���Զ�Ӧ��
	#ifdef  EN_NRF_ACK	
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,NRF_CH0_ADDRESS);    //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,NRF_CH0_ADDRESS);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  #endif
	
	//��ʹ���Զ�Ӧ��
	#ifndef EN_NRF_ACK
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);    //ʧ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x00);//ʧ��ͨ��0�Ľ��յ�ַ  	
	#endif
	
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,RF_CH0);	     //����RFͨ��Ƶ��Ϊ:RF_CH0		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	
  NRF24L01_CE_HIGH(); //CEΪ��,�������ģʽ 
  HAL_Delay(1);
}						 

/**
  * ��������: �ú�����ʼ��NRF24L01��TXģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *           
  */ 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_LOW();	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX0_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX0_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

	//ʹ���Զ�Ӧ��
	#ifdef  EN_NRF_ACK
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,NRF_CH0_ADDRESS);     //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,NRF_CH0_ADDRESS); //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0xff);//�����Զ��ط����ʱ��:4000us + 86us;����Զ��ط�����:15��
	#endif
	
	//��ʹ���Զ�Ӧ��
	#ifndef EN_NRF_ACK
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);     //ʧ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x00); //ʧ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x00);//�����Զ��ط����ʱ��:4000us + 86us;����Զ��ط�����:15��
	#endif

	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,RF_CH0);       //����RFͨ��Ϊ��RF_CH0
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE_HIGH();//CEΪ��,10us����������
  HAL_Delay(1);

}

