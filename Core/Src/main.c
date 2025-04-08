/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "ili9341.h"
#include "XPT2046_touch.h"
#include "colors.h"
#include "GPS.h"
#include "map.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NMEA_BUFFER_SIZE 128
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Print_Vbat_Temp(float Vref);
void Get_Time_For_LCD(void);
void GPS_PrintSatellites(char *sat_str);
void SetTime(void);
uint8_t GetDayOfWeek(uint8_t date, uint8_t month, uint16_t year);
void GPSfoundPrint(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t nmeaIndex = 0;
uint8_t rxByte;
char nmeaBuffer[NMEA_BUFFER_SIZE]; //__attribute__((section (".ccmram")));
volatile bool GPS_UART_flag = 0;
volatile bool messageReady = 0;
volatile bool TP_flag = 0;
uint16_t x = 0, y = 0;


//GPS_Data WorkGpsData;
const char *testGPRMCmessage = "$GPRMC,121030.00,A,5141.78292,N,03909.76459,E,0.087,,140225,,,A*76";
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	float Vref = 2.980;			//Vref ADC from your Board
	uint32_t for_adc_time = 0;
	uint32_t for_sat_time = 0;
	uint32_t for_gps_time = 0;
	int ProgStatus = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
   //__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_FSMC_Init();
  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  lcdDisplayOn();
  lcdInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  if (lcdGetControllerID() != 0x9341)  //Check LCD driver and his Model
	  Error_Handler();

  lcdFillRGB(COLOR_WHITE);
  HAL_UART_Receive_IT(&huart2, &rxByte, 1);

  HAL_Delay(1000);						//Delay for GPS_UART receive
  if (!GPS_UART_flag){					//GPS module print status
  	lcdSetTextFont(&Font16);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 180);
	lcdSetTextColor(COLOR_RED, COLOR_WHITE);
	lcdPrintf("GPS module not found!");
    }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch(ProgStatus){
	  case 0:					//Status: GPS not fixed, print: satellites, Vbat, CPU temp, Time, Date

	  if ((HAL_GetTick() - for_adc_time) > 500) {
	  Print_Vbat_Temp(Vref);
	  Get_Time_For_LCD();
  	  for_adc_time = HAL_GetTick();
	  }

	  if ((HAL_GetTick() - for_sat_time) > 2000 && (GPS_UART_flag)) {
	  LCD_PrintSat(' ', '0');
	  for_sat_time = HAL_GetTick();
	  }


	  if (messageReady) {					//NMEA Packet received
          	  messageReady = 0;

          	  if ((strncmp(nmeaBuffer, "$GPRMC", 6) == 0 || strncmp(nmeaBuffer, "$GNRMC", 6) == 0) && strlen(nmeaBuffer) > 60) {	//Check GPRMC packet

	          	  if(GPS_ProcessChar(nmeaBuffer) == 'A') {
	          		  ProgStatus++;
	          		  SetTime();
	          		  GPSfoundPrint();
	          		  HAL_UART_Receive_IT(&huart2, &rxByte, 1);

	          		  break;
	          	  }
          	  }
          	  else if ((strncmp(nmeaBuffer, "$GPGSV", 6) == 0 || strncmp(nmeaBuffer, "$GNGSV", 6) == 0) && (strlen(nmeaBuffer) > 15)) {	//Check GPGSV packet
          	  GPS_PrintSatellites(nmeaBuffer);
          	  for_sat_time = HAL_GetTick();
          	  }
          	HAL_UART_Receive_IT(&huart2, &rxByte, 1);
	  	  }
	  break;

	  case 1:		//Status: GPS found, check GoToMap button push, print: satellites, Vbat, CPU temp, Time, Date

		  if ((HAL_GetTick() - for_adc_time) > 1000) {
		  Print_Vbat_Temp(Vref);
		  Get_Time_For_LCD();
		  for_adc_time = HAL_GetTick();
		  }

		  if (messageReady) {					//NMEA Packet received
		      messageReady = 0;

		      if (strncmp(nmeaBuffer, "$GPRMC", 6) == 0 || strncmp(nmeaBuffer, "$GNRMC", 6) == 0){	//GPRMC Packet received
		    	  ProgStatus--;

		    	  if (strlen(nmeaBuffer) >= 60 && GPS_ProcessChar(nmeaBuffer) == 'A') {
		    	  		  ProgStatus++;
		    	  }
		    	  else lcdFillRGB(COLOR_WHITE);
		      }
		      HAL_UART_Receive_IT(&huart2, &rxByte, 1);
		  }

		  if (TP_flag) {
			  TP_flag = 0;
			  //lcdDrawPixel(x, y, COLOR_GREEN);
		  	  if ((x >= 110) && (x <= 201) && (y >= 100) && (y <= 135))	{	// Check 'GoToMap' button press
		  			  ProgStatus++;
		  			  PrintBigMapFirst();
		  			  PrintCoordinatesPoint(GPS_GetData()->latitude, GPS_GetData()->longitude);
		  			  for_gps_time = HAL_GetTick();
		  			  HAL_UART_Receive_IT(&huart2, &rxByte, 1);
		  	  }
		  }

	  break;

	  case 2:		//Status: Print Map and point coordinates, check GPS loss


		  if (messageReady) {					//NMEA Packet received
		  		  messageReady = 0;
		  		  if ((strncmp(nmeaBuffer, "$GPRMC", 6) == 0 || strncmp(nmeaBuffer, "$GNRMC", 6) == 0) && strlen(nmeaBuffer) > 60) {	//Check GPRMC packet
		  			  if(GPS_ProcessChar(nmeaBuffer) == 'A') {
		  				  PrintBigMap();
		  				  PrintCoordinatesPoint(GPS_GetData()->latitude, GPS_GetData()->longitude);
		  				  for_gps_time = HAL_GetTick();
		  			  }
		  		  }
		  		HAL_UART_Receive_IT(&huart2, &rxByte, 1);
		  }

		  if (HAL_GetTick() - for_gps_time > 2000) {
			  LCD_GPS_lost();
			  for_gps_time = HAL_GetTick();
		  }

	  break;
	  /*тут нужно добавить кейс с алгоритмом переключения изображений карты и алгоритм зуммирования
	  с другими изображениями, но пока не решен вопрос источника таких изображений, с привязкой к координатам*/

	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Print_Vbat_Temp(float Vref)
{
	uint16_t adcValue;
	uint16_t adc2Value;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adcValue = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adc2Value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	LCD_PrintVbat((float)adcValue / 4095 * Vref);  //Vref=3.1 -> 6.2 because Vbat/2 for ADC, if source is Vbat
	LCD_PrintCPU_t(((float)adc2Value / 4095 * Vref * 1000 - 760.0) / 2.5 + 25);	//Vref=3.1 3100
}

void SetTime(void)				//Load Time and Date to RTC
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	sTime.Hours = GPS_GetData()->hour;
	sTime.Minutes = GPS_GetData()->minutes;
	sTime.Seconds = GPS_GetData()->seconds;
	if (sTime.Hours > 23)
		return;
	if (sTime.Minutes > 59)
		return;
	if (sTime.Seconds > 59)
		return;
	HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
	sDate.Date = GPS_GetData()->date;
	sDate.Month = GPS_GetData()->month;
	sDate.Year = GPS_GetData()->year;
	if (sDate.Date > 31)
		return;
	if (sDate.Month > 12)
		return;
	sDate.WeekDay = GetDayOfWeek(sDate.Date, sDate.Month, (2000 + sDate.Year));
	HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BIN);
}

uint8_t GetDayOfWeek(uint8_t date, uint8_t month, uint16_t year)
{
    if (month < 3) {
        month += 12;
        year--;
    }
    uint8_t k = year % 100;
    uint8_t j = year / 100;
    uint8_t h = (date + ((13 * (month + 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j)) % 7;	//Zeller's algorithm
    uint8_t rtcWeekDay = ((h + 5) % 7) + 1;	//conversion to RTC format

    return rtcWeekDay;
}

void Get_Time_For_LCD(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, FORMAT_BIN);
	LCD_PrintTime(sTime);
	LCD_PrintDate(sDate);
}

void GPS_PrintSatellites(char *sat_str)
{
	char *p;
	int count = 0;
	while((p = strchr(sat_str, ',')) != NULL) {

	if(count == 2) {
		LCD_PrintSat(*(p+1), *(p+2));
		return;
	}
	sat_str = p + 1;
	count++;
	}

}

void GPSfoundPrint()
{
  lcdSetTextFont(&Font24);
  lcdSetCursor(80, lcdGetHeight() - lcdGetTextFont()->Height - 150);
  lcdSetTextColor(COLOR_RED, COLOR_WHITE);
  lcdPrintf("%s", "GPS Found");
  lcdDrawImage(110, 100, &bmGoToMap);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        if (rxByte == '$') {
            // Start NMEA packet
            nmeaIndex = 0;
        }
        if (nmeaIndex < NMEA_BUFFER_SIZE) {
            nmeaBuffer[nmeaIndex++] = rxByte;

            // Check end of message (\r\n)
            if (rxByte == '\n' && nmeaIndex > 1 && nmeaBuffer[nmeaIndex-2] == '\r') {
                messageReady = 1;
                GPS_UART_flag = 1;
  		        nmeaIndex = 0;
            	  if (HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == 0) {		//Test "GPRMC" packet parsing if key1 is pressed
            		  strcpy(nmeaBuffer, testGPRMCmessage);
            	  }
                return;
            }
        }
        HAL_UART_Receive_IT(&huart2, &rxByte, 1);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if (GPIO_Pin == IRQ_TOUCH_Pin) {
	  if(XPT2046_TouchPressed() && XPT2046_TouchGetCoordinates(&x, &y)) {
	   TP_flag = 1;   // Tap flag
	  }
	}
}
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
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
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
