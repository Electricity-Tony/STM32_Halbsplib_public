
#include "bsp_key.h"


_KeyTypedef key_up = {key_up_GPIO_Port,key_up_Pin,0,0,1,0,1000};
_KeyTypedef key_down = {key_down_GPIO_Port,key_down_Pin,0,0,1,0,1000};

/**
* @brief  ��ȡ��ǰ�����İ���״̬
* @details  
* @param  _KeyTypedef* Key(������׼�ṹ��)
* @retval  ���°����ṹ��״̬
*/
void bsp_key_Read(_KeyTypedef* Key)
{
	//������ǰ״̬�������Now_State�д�ţ���ʾ��ƽ���
	Key->Now_State = HAL_GPIO_ReadPin(Key->GPIO_Port,Key->Pin);
}


/**
* @brief  ��������ʱ�����
* @details  �������£��ɿ���Ϊ����
* @param  _KeyTypedef* Key(������׼�ṹ��)
*/
void bsp_key_State_Update(_KeyTypedef* Key)
{
	bsp_key_Read(Key);


	//����״̬�жϣ������ж��м�̬�������Ture_State�д��
	//0��ʾû�а��»��ڳ����м�̬�У�1��ʾ�̰�������2��ʾ��������
	//Ĭ��Now_StateΪ�͵�ƽ��ʾ����
	if(Key->time_flag > Key->time_check)
	{
		//����ʱ���־λ��������������
		Key->time_flag=0;
		Key->Ture_State = 2;
	}
	if(Key->Now_State==0) 
	{
		//�����м�̬������ʱ���־λ����
		Key->time_flag++;
		Key->Ture_State = 0;
	}
	else if(10< Key->time_flag && Key->time_flag < Key->time_check)
	{
		//�̰�����
		Key->time_flag=0;
		Key->Ture_State = 1;
	}
	else Key->Ture_State = 0;	//һֱû�а���

	//�жϰ���״̬�仯��Ĭ�϶�ֵ̬���̰�����
	if (Key->State != 1 && Key.Ture_State == 1)
	{
		Key->State = 1;
	}
	else if (Key->State != 0 && Key.Ture_State == 1)
	{
		Key->State = 0;
	}
	else if (Key->State.Ture_State == 2)
	{
		Key->State = 2;
	}


	
}


