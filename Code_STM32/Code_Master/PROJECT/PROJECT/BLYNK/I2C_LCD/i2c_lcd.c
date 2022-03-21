#include <i2c_lcd.h>
#include "stm32f1xx_hal.h"

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODE 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSOR 0x10
#define LCD_FUNCTIONSET 0x20

#define LCD_DISPLAY_D 0x04
#define LCD_DISPLAY_C 0x02
#define LCD_DISPLAY_B 0x01

#define LCD_FUNCTION_F 0x04
#define LCD_FUNCTION_N 0x08
#define LCD_FUNCTION_DL 0x10

#define LCD_LIGHT (1 << 3)
#define LCD_EN (1 << 2)
#define LCD_RS (1 << 0)

extern I2C_HandleTypeDef hi2c1;

void LCD_CMD(uint8_t cmd)
{
	uint8_t cmd_4_7 = (0xF0 & cmd);
	uint8_t cmd_0_3 = (0xF0 & (cmd << 4));
	uint8_t sendCmd[4]=
	{
		cmd_4_7 | LCD_EN | LCD_LIGHT,
		cmd_4_7 | LCD_LIGHT,
		cmd_0_3 | LCD_EN | LCD_LIGHT,
		cmd_0_3 | LCD_LIGHT,
	};
	HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_LCD, sendCmd, sizeof(sendCmd),1000);
}

void LCD_Write(uint8_t data)
{
	uint8_t data_4_7 = (0xF0 & data);
	uint8_t data_0_3 = (0xF0 & (data << 4));

	uint8_t sendData[4] =
	{
		data_4_7 | LCD_EN | LCD_LIGHT | LCD_RS,
		data_4_7 | LCD_LIGHT | LCD_RS,
		data_0_3 | LCD_EN | LCD_LIGHT | LCD_RS,
		data_0_3 | LCD_LIGHT | LCD_RS
	};
	HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_LCD, sendData, sizeof(sendData), 1000);
}

void LCD_Init(void)
{
	HAL_Delay(40); //Delay 40ms after power-up

	LCD_CMD(0x30);
	HAL_Delay(15); // Delay 5ms

	LCD_CMD(0x30);
	HAL_Delay(15);

	LCD_CMD(0x30);
	HAL_Delay(10);

	LCD_CMD(0x20);
	HAL_Delay(10);

	LCD_CMD(LCD_FUNCTIONSET | LCD_FUNCTION_N);
	HAL_Delay(10);

	LCD_CMD(LCD_DISPLAYCONTROL);
	HAL_Delay(10);

	LCD_CMD(LCD_CLEARDISPLAY);
	HAL_Delay(10);

	LCD_CMD(0x06);
	HAL_Delay(10);

	LCD_CMD(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
	HAL_Delay(10);
}

void LCD_setCursor(uint8_t row, uint8_t col)
{
	uint8_t maskData;
	maskData = (col)&0x0F;
	if(row==0)
	{
	  maskData |= (0x80);
		LCD_CMD(maskData);
	}
	else
	{
	   maskData |= (0xc0);
		 LCD_CMD(maskData);
	}
}

void LCD_sendString(uint8_t *str)
{
  while(*str)
	{
		LCD_Write(*str++);
	}
}

void LCD_Clear(void)
{
 LCD_CMD(LCD_CLEARDISPLAY);
 HAL_Delay(3);
}
