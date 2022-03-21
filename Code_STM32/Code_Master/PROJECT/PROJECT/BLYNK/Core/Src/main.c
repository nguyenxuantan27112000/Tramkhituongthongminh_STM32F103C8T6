#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "dht11.h"
#include "i2c_lcd.h"
#include "ds1307.h"

void SystemClock_Config(void);

void SendData(float A, float B,  float C, float D);
void chuongtrinhcambien(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Received(void);
void clearbufferEnd(void);
void XuLyData(char *Data);

void Out_Door_Temp();
void In_Door_Temp();
void Time_Display();
void In_Door_Display();
void Out_Door_Display();
void esp_trans();

void microDelay (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

float A = 0.1;
float B = 0.2;
float C = 0.3;
float D = 0.4;
long last = 0;
int i = 0;

uint8_t count;
uint8_t isPress;
DS1307_TIME time ={
  .sec = 0,
  .min = 0,
  .hour = 23,
  .day = 6,
  .date = 3,
  .month = 9,
  .year = 21,
};

uint8_t buf0[25];
uint8_t buf1[25];
uint8_t buf2[25];
uint8_t buf3[25];

uint8_t RHI, RHD, TCI, TCD;


float xTmp = 0;
float xHmd = 0;
float yTmp = 0;
float yHmd = 0;
uint8_t strCopy[30] = {0};
uint8_t strCpy[30] = {0};
uint8_t strCpy1[30] = {0};
uint8_t strCpy2[30] = {0};
uint8_t strCpy3[30] = {0};
uint8_t strCpy4[30] = {0};
uint8_t strCpy6[30] = {0};
uint8_t Data_Trans[80] = {0};

char Str_A[100];
char Str_B[100];
char Str_C[100];
char Str_D[100];
char JSON[100];
char rx_buffer[200];
uint8_t rx_data;
unsigned int rx_index = 0;

cJSON *str_json, *st_A , *st_B , *st_C , *st_D , *st_Mode;




int main(void)
{
 
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	I2C_CONFIG(&hi2c1);
  HAL_TIM_Base_Start(&htim1);
  LCD_Init();
  SET_TIME(&time);
	HAL_UART_Receive_IT(&huart1,&rx_data,1);
	last = HAL_GetTick();
 
  while (1)
  {
		 
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
    {
       HAL_Delay(200);
       if((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0) && (isPress == 0))
       {
         while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==0);
         count++;
         isPress = 1;
         LCD_Clear();
         if(count > 2)
         {
           count = 0;
         }
       }
       else
       {
         isPress = 0;
       }

    }

    if(count == 1)
    {
      In_Door_Temp();
      Out_Door_Temp();
      In_Door_Display();
      esp_trans();


    }
    else if (count ==2)
    {
      In_Door_Temp();
      Out_Door_Temp();
      Out_Door_Display();
      esp_trans();

    }
    else
    {
      Time_Display();
      In_Door_Temp();
      Out_Door_Temp();
      esp_trans();
    }
       

		/*if(HAL_GetTick() - last >= 1000)
	  {
	
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//Read_DataDHT();
			
		  chuongtrinhcambien();
		  //SendData(A,B,C,D);
		  last = HAL_GetTick();
	  } */
		
  }
  
}
void Out_Door_Temp(void)
{
  HAL_UART_Receive(&huart3, strCpy, sizeof(strCpy), 100);
  strncpy(buf2,strCpy+6,4);
  xTmp = atof(buf2);
  strncpy(buf3,strCpy+24,4);
  xHmd = atof(buf3);




}

void In_Door_Temp(void)
{
  if(DHT11_Start())
                   {
                     RHI = DHT11_Read(); // Relative humidity integral
                     RHD = DHT11_Read(); // Relative humidity decimal
                     TCI = DHT11_Read(); // Celsius integral
                     TCD = DHT11_Read(); // Celsius decimal

                   }

  sprintf((char*)strCopy,"TMP: %d.%d C   IN", TCI, TCD);
  sprintf((char*)strCpy3,"%d.%d",TCI,TCD);
  yTmp = atof(strCpy3);


  sprintf((char*)strCpy6,"RH:  %d.%d %% DOOR", RHI, RHD);
  sprintf((char*)strCpy4,"%d.%d",RHI,RHD);
  yHmd =  atof(strCpy4);
}

void Time_Display(void)
{
      GET_TIME(&time);
 sprintf((char*) buf0, "%02d/%02d/20%02d", time.date, time.month, time.year);
 sprintf((char*) buf1, "%02d:%02d:%02d", time.hour, time.min, time.sec);
    LCD_setCursor(0, 3);
    LCD_sendString(buf0);
    LCD_setCursor(1, 4);
    LCD_sendString(buf1);
}


void In_Door_Display(void)
{

                        // sprintf((char*)root,"TMP: %.1f",yTmp);
                         LCD_setCursor(0, 0);
                         LCD_sendString(strCopy);


                        // sprintf((char*)root1,"RH: %.1f",yHmd);

                         LCD_setCursor(1, 0);
                         LCD_sendString(strCpy6);




}
void Out_Door_Display(void)
{
  sprintf((char*) strCpy1, "TMP: %.1f C  OUT", xTmp);
  LCD_setCursor(0, 0);
  LCD_sendString(strCpy1);


  sprintf((char*) strCpy2, "RH:  %.1f %% DOOR", xHmd);
   LCD_setCursor(1, 0);
   LCD_sendString(strCpy2);





}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart -> Instance == USART1)
	{
	
		last = HAL_GetTick();
		Received();
		HAL_UART_Receive_IT(&huart1,&rx_data,1);

	}
}
void Received(void)
{
	//rx_buffer
	if(rx_data != '\n')
	{
		rx_buffer[rx_index++] = rx_data;
	}
	else
	{
		// xu ly

		//printf("Data nhan duoc: %s\n", rx_buffer);
		//printf("Data nhan duoc ESP: %s\n", rx_buffer);
		// yLyJson(rx_buffer);
		HAL_UART_Transmit(&huart2,(uint8_t *)&rx_buffer,strlen(rx_buffer),1000);
		HAL_UART_Transmit(&huart2,(uint8_t *)&"\n",strlen("\n"),1000);
		XuLyData(rx_buffer);
		clearbufferEnd();
	}
}

void clearbufferEnd(void)
{
	rx_index = 0;
	for(int i = 0 ; i < 200 ; i++)
	{
		rx_buffer[i] = 0;
	}
	last = HAL_GetTick();
}

void XuLyData(char *Data)
{
	str_json = cJSON_Parse(Data);
	if(!str_json)
	{
		//printf("JSON ERROR!\r\n");
		return;
	}
	else
	{
		//printf("JSON OK!\r\n");
		if(cJSON_GetObjectItem(str_json, "SEND"))
		{
			//SendData(ND , DA);
			SendData( A,  B, C, D);
			last = HAL_GetTick();
		}
		
		last = HAL_GetTick();
		cJSON_Delete(str_json);
	}
}
void chuongtrinhcambien(void)
{  
   A = atof(buf2);
   C = atof(strCpy3);
   B = atof(buf3);
   D = atof(strCpy4);
	//A ++;
	//B ++;
	//C ++;
	//D ++;
}
void SendData(float A, float B, float C, float D)
{
	for (int i = 0 ; i < 100 ; i++)
	{
		JSON[i] = 0;
		Str_A[i] = 0;
		Str_B[i] = 0;
		Str_C[i] = 0;
		Str_D[i] = 0;
		//Str_TB1[i] = 0;
	
	}
	// Chuyen d? li?u v? char
	sprintf(Str_A, "%.1f", A);
	sprintf(Str_B, "%.1f", B);
	sprintf(Str_C, "%.1f", C);
	sprintf(Str_D, "%.1f", D);
	//sprintf(Str_TB1, "%d", trangthaiden);


	strcat(JSON,"{\"A\":\"");
	strcat(JSON,Str_A);
	strcat(JSON,"\",");


	strcat(JSON,"\"B\":\"");
	strcat(JSON,Str_B);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"C\":\"");
	strcat(JSON,Str_C);
	strcat(JSON,"\",");

  strcat(JSON,"\"D\":\"");
	strcat(JSON,Str_D);
	strcat(JSON,"\"}\n");


	/*strcat(JSON,"\"TB1\":\"");
	strcat(JSON,Str_TB1);
	strcat(JSON,"\"}\n");*/

	
	HAL_UART_Transmit(&huart2,(uint8_t *)&JSON,strlen(JSON),1000); // PC
	HAL_UART_Transmit(&huart1,(uint8_t *)&JSON,strlen(JSON),1000); // ESP 
}

void esp_trans(void)
{
  if(HAL_GetTick() - last >= 1000)
      {
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        chuongtrinhcambien();
       //SendData(A, B, C, D);
        last = HAL_GetTick();
      }

}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

