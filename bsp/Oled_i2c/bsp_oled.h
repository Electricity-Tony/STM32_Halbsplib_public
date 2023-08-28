#ifndef __BSP_OLED_H
#define __BSP_OLED_H			  	 

#include "stm32f1xx.h"
#include "main.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

#define Oled_address 0x78
#define Oled_WrtCMD 0x00
#define Oled_WrtData 0x40


#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

#define USE_HARDWARE_I2C	//ʹ��Ӳ��I2C����ʹ��ģ��I2C��ע��




#ifndef USE_HARDWARE_I2C
-----------------OLED IIC�˿ڶ���----------------
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(I2C_SCL_GPIO_Port,I2C_SCL_Pin,GPIO_PIN_RESET);//SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(I2C_SCL_GPIO_Port,I2C_SCL_Pin,GPIO_PIN_SET);

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,GPIO_PIN_RESET)//SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,GPIO_PIN_SET)

//���I2C��������
void IIC_Start();
void IIC_Stop();
void IIC_Wait_Ack();
void Write_IIC_Byte(unsigned char IIC_Byte);
#else

#define OLED_hi2c hi2c1	//OLEDʹ�õ�Ӳ��I2Cͨ��

#endif // USE_HARDWARE_I2C
				 



//OLED�����ú���
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void Delay_50ms(unsigned int Del_50ms);
void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);
void Picture();
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void OLED_WR_Byte(unsigned dat, unsigned cmd);
#endif  
	 



