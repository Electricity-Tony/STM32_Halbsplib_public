/**
 * @file bsp_NRF24L01.c
 * @brief NRF24L01�弶֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-09-27
 * @copyright 
 * @par ��־:
 *   V1.0 �淶��NRF24L01�շ������⣬ʵ�ֻ�������
 */



#ifndef __BSP_NRF24L01_H__
#define __BSP_NRF24L01_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define NRF24L01_SPIx                                 SPI2
#define NRF24L01_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define NRF24L01_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI2_CLK_DISABLE()

#define NRF24L01_SPI_GPIO_ClK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE() 
//#define NRF24L01_SPI_GPIO_PORT                        GPIOB
//#define NRF24L01_SPI_SCK_PIN                          GPIO_PIN_3
//#define NRF24L01_SPI_MISO_PIN                         GPIO_PIN_4
//#define NRF24L01_SPI_MOSI_PIN                         GPIO_PIN_5

//#define NRF24L01_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOF_CLK_ENABLE()    
//#define NRF_CS_GPIO_Port                          GPIOB
//#define NRF_CS_Pin                           GPIO_PIN_7
#define NRF24L01_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET)
#define NRF24L01_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_SET)

//#define NRF24L01_CE_CLK_ENABLE()                      __HAL_RCC_GPIOF_CLK_ENABLE()    
//#define NRF_CE_GPIO_Port                              GPIOB
//#define NRF24L01_CE_PIN                               GPIO_PIN_6
#define NRF24L01_CE_LOW()                             HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)
#define NRF24L01_CE_HIGH()                            HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)

//#define NRF24L01_IRQ_CLK_ENABLE()                     __HAL_RCC_GPIOF_CLK_ENABLE()    
//#define NRF_IRQ_GPIO_Port                             GPIOB  
//#define NRF_IRQ_Pin                             			GPIO_PIN_8
#define NRF24L01_IRQ_PIN_READ()                       HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port,NRF_IRQ_Pin)

// NRF24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH                                  5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH                                  5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH                                32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH                                32  	//32�ֽڵ��û����ݿ��

//NRF24L01�Ĵ�����������
#define NRF_READ_REG                                  0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG                                 0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD                                   0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD                                   0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX                                      0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX                                      0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL                                   0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP                                           0xFF  //�ղ���,����������״̬�Ĵ���	 
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG                                        0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                                                            //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA                                         0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR                                     0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW                                      0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR                                    0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH                                         0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;������0~127
#define RF_SETUP                                      0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS                                        0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                                                            //bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX  		                                  0x10  //�ﵽ����ʹ����ж�
#define TX_OK   		                                  0x20  //TX��������ж�
#define RX_OK   		                                  0x40  //���յ������ж�

#define OBSERVE_TX                                    0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD                                            0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0                                    0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1                                    0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
//ע:P2~5ͨ��д���ֽ�ʱֻ��д��һ���ֽ�
#define RX_ADDR_P2                                    0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3                                    0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4                                    0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5                                    0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;

#define TX_ADDR                                       0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0                                      0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1                                      0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2                                      0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3                                      0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4                                      0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5                                      0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF_FIFO_STATUS                               0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                                                            //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
                              
/* ��չ���� ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi2;
#define hspi_NRF24L01 hspi2
extern uint8_t NRF_RX_date[33];

/* �������� ------------------------------------------------------------------*/

void NRF24L01_RX_Mode(void);					//����Ϊ����ģʽ
void NRF24L01_TX_Mode(void);					//����Ϊ����ģʽ
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);//д������
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);	//��������		  
uint8_t NRF24L01_Read_Reg(uint8_t reg);					//���Ĵ���
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);		//д�Ĵ���
uint8_t NRF24L01_Check(void);						//���24L01�Ƿ����
uint8_t NRF24L01_TxPacket(uint8_t *txbuf);				//����һ����������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);				//����һ����������


#endif 
