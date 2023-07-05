/**
 * @file bsp_DYSV.c
 * @brief ����DYSV����֧�ְ�
 * @author Tony_Wang
 * @version 1.0
 * @date 2020-10-5
 * @copyright 
 * @par ��־:
 *   V1.0 �����������ڷ��͹��ܣ��ɽ���ͨ�Ų���
 */
#include "bsp_DYSV.h"
#include "usart.h"




//�����жϽ��պͷ�������
uint8_t aTx_DYSV_StartMassage[aTx_DYSV_size] = "Dysv Success\r\n";
/* Buffer used for reception */
uint8_t Rx_DYSV_Buffer[aRx_DYSV_size+2],Rx_DYSV_Date[aRx_DYSV_size+2];	//���ý����ж������С
uint8_t aRx_DYSV_Buffer;	//���ý����ж������С
uint8_t Rx_Cnt_DYSV_DYSV = 0;        //���ջ������
uint8_t  cAmStr_DYSV[] = "�������\r\n";



//���ù�����ؿ�������
uint8_t PlayFromBegin[4]={0xAA,0X02,0x00,0xAC};
uint8_t Playmusic01[6]={0xAA,0X07,0x02,0x00,0x01,0xB4};
uint8_t Stop[4]={0xAA,0x03,0x00,0xAD};



/**
* ��������: �жϽ��ܺ���ʹ�ܳ�ʼ��,ʧ���жϷ��ͺ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    �����ɹ��󴮿ڶ˷���  "USART-interrupt Success"
  */
void USART_DYSV_Interrupt_Init(void)
{
	__HAL_UART_DISABLE_IT(fusart_DYSV_DYSV,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(fusart_DYSV_DYSV,UART_IT_RXNE);
	HAL_UART_Receive_IT(fusart_DYSV_DYSV, (uint8_t *)&aRx_DYSV_Buffer, 1);
}




/**
* ��������: ��������ͺ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����
  */
void USART_DYSV_TX_Music(uint8_t TxDate[8])
{
	//uint8_t length=sizeof(TxDate);
	uint8_t i;
	for(i=0;i<8;i++)
	{
	HAL_UART_Transmit(fusart_DYSV_DYSV,(uint8_t *)TxDate+i , 1,0xFFFF);
	//HAL_UART_Transmit(&huart1,(uint8_t *)TxDate+i , 1,0xFFFF);
	}
	
}


#ifdef EXTERN_USART
/**
  * @brief Rx Transfer completed callbacks
  * @param huart: uart handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
	/* NOTE : This function should not be modified, when the callback is needed,
			  the HAL_UART_RxCpltCallback can be implemented in the user file
	*/
	if (huart->Instance == USART_DYSV)	// �ж������ĸ����ڴ������ж�
	{
		if (Rx_Cnt_DYSV >= aRx_DYSV_size + 2)  //����ж�
		{
			Rx_Cnt_DYSV = 0;
			memset(Rx_DYSV_Buffer, 0x00, sizeof(Rx_DYSV_Buffer));
			//HAL_UART_Transmit(fusart_DYSV, (uint8_t*)& cAmStr_DYSV, sizeof(cAmStr_DYSV), 0xFFFF);
		}
		else
		{
			Rx_DYSV_Buffer[Rx_Cnt_DYSV++] = aRx_DYSV_Buffer;   //��������ת��

			if ((Rx_DYSV_Buffer[Rx_Cnt_DYSV - 1] == 0x0A) && (Rx_DYSV_Buffer[Rx_Cnt_DYSV - 2] == 0x0D)) //�жϽ���λ
			{
				//HAL_UART_Transmit(fusart_DYSV, (uint8_t*)& Rx_DYSV_Buffer, Rx_Cnt_DYSV, 0xFFFF); //���յ�����Ϣ���ͳ�ȥ	
				memcpy(Rx_DYSV_Date, Rx_DYSV_Buffer, aRx_DYSV_size + 2);
				Rx_Cnt_DYSV = 0;
				memset(Rx_DYSV_Buffer, 0x00, sizeof(Rx_DYSV_Buffer)); //�������
			}
		}
		HAL_UART_Receive_IT(fusart_DYSV, (uint8_t*)& aRx_DYSV_Buffer, 1);   //�ٿ��������ж�
	}
}
#endif // EXTERN_USART


