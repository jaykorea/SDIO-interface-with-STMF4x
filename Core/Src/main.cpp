/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ArduinoWrapper.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
FATFS myFatFS;
FATFS *fs;
FIL myFile;
UINT myBytes;
FRESULT fr;
FILINFO fno;

DWORD fre_clust, fre_sect, tot_sect;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool flag = false;
int i;

char Filename[] = "Log00000.CSV";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
    	i++;
        HAL_GPIO_TogglePin(GPIOC, red_Pin);
        HAL_GPIO_TogglePin(GPIOC, green_Pin);
        //printf("time : ");
        //printf("%ld\r\n", millis());
        //printf("GPIO_PIN_RED = %d, SysTick = %ld\r\n", HAL_GPIO_ReadPin(GPIOC, red_Pin), HAL_GetTick());
        //printf("GPIO_PIN_Green = %d, SysTick = %ld\r\n", HAL_GPIO_ReadPin(GPIOC, green_Pin), HAL_GetTick());
    }
    if (i > 5) { flag = true; i=0; }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SDIO_SD_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_FATFS_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);

   delay(100);
   if(f_mount(&myFatFS, SDPath, 1) == FR_OK)
   {
 	  printf("SD Mount 'Success'\r\n");
 	  fr = f_getfree("", &fre_clust, &fs);
 	    /* Get total sectors and free sectors */
 	    tot_sect = (fs->n_fatent - 2) * fs->csize;
 	    fre_sect = fre_clust * fs->csize;
 	    /* Print the free space (assuming 512 bytes/sector) */
 	    printf("SD card stauts : %lu MB total drive space.\r\n", (tot_sect / 2)/997);
 	    printf("Remaining space : %lu MB available.\r\n", (fre_sect / 2)/997);

 //	    if (((fre_sect/2)/997) < 10)
 //	    {
 //			용량 부족시 삭제하는 루틴 만들기
 //	    }

 	  for (unsigned int k = 0; k < 100000; k++)
 	  {
 		Filename[3] = k / 10000 + '0'; //만자리
 		Filename[4] = ((k % 10000) / 1000) + '0';//천자리
 		Filename[5] = ((k % 1000) / 100) + '0';//백자리
 		Filename[6] = ((k % 100) / 10) + '0'; //십자리
 		Filename[7] = k % 10 + '0'; //일자리
 		delay(1);
 		fr = f_open(&myFile, Filename, FA_WRITE | FA_CREATE_NEW);
 		if (fr == FR_OK)
 		{
 			break;
 		}
 	  }
 	  printf("Logging to : ");
 	  printf("%s\r\n", Filename);
 	  f_printf(&myFile, "%s", "First Boot");
 	  f_close(&myFile);
   }
   else { printf("SD Mount 'Failed'\r\n"); NVIC_SystemReset(); }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (flag == true) {
	 	      //printf("flag : ");
	 	      //printf("%d\r\n", flag);
	 		  HAL_GPIO_TogglePin(Ld2_GPIO_Port, Ld2_Pin);
	 		  flag = false;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
