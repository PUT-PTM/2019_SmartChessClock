
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"
#include "stm32_tm1637.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//PREPROCESOR
#define UI_PLAYER1_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)		//Zapalenie diody przycisku 1
#define UI_PLAYER1_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)	//Zgaszenie diody przycisku 1
#define UI_PLAYER1_BUTTON		GPIO_PIN_11												//Uchwyt przycisku gracza 1

#define UI_PLAYER2_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)		//Zapalenie diody przycisku 2
#define UI_PLAYER2_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)	//Zgaszenie diody przycisku 2
#define UI_PLAYER2_BUTTON		GPIO_PIN_13												//Uchwyt przycisku gracza 2

#define DEBUG_DIODE1_ON			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET)		//Zapalenie niebieskiej diody na plytce
#define DEBUG_DIODE1_OFF		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET)	//Zapalenie niebieskiej diody na plytce

//STRUKTURA CZASU
struct _time
{
	uint8_t minutes;
	uint8_t seconds;
	uint16_t miliseconds;
};

//PRESETY CZASOWE
struct _preset
{
	struct _time time;
	uint8_t increment;
};

const struct _preset presets[] =	//{min, sec, ms, increment}
{
		{1, 0, 0, 0}, {2, 1, 0, 1}, {3, 0, 0, 0}, {3, 0, 0, 2}, {5, 0, 0, 0}, {5, 0, 0, 3}, {10, 0 ,0 ,0}, {15, 0, 0, 15}
};

//ZMIENNE GLOBALNE
uint8_t _currentPlayer;	//Zmienna oznaczajaca któremu z graczy uplywa czas
uint8_t _increment;		//Zmienna oznaczajaca dodawany czas po wcisnieciu przycisku w sekundach
_Bool _pause;			//Zmienna wyznaczajaca pauze w pomiarze czasu
_Bool gameOver;			//Zmienna wyznaczająca koniec gry w przypadku gdy jednemu z graczy upłynie czas

struct _time PLAYER1_TIME;	//Struktura przechowująca informacje o czasie gracza nr 1
struct _time PLAYER2_TIME;	//Struktura przechowująca informacje o czasie gracza nr 2


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* FUNKCJE MIERZĄCE/OBSŁUGUJĄCE CZAS---------------------------------------------------------------*/
void setClocks(int presetIndex)		//Funkcja ustawia czas obu zegarów z numeru presetu
{
	presetIndex--;

	PLAYER1_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER1_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER1_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	PLAYER2_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER2_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER2_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	_increment = presets[presetIndex].increment;
}

int decrementClock(struct _time* clock)		//Funkcja pomniejsza wartośc struktury czasu o 1ms - jeżeli czas upłynął całkowicie - funkcja zwraca wartośc 1
{
	if(clock->miliseconds == 0)
	{
		if(clock->seconds == 0)
		{
			if(clock->minutes == 0)
			{
				return 1;		//Koniec czasu - zwracamy 1
			}
			else	{clock->minutes--; clock->seconds = 59; clock->miliseconds = 999;}
					//Jeżeli liczba minut jest większa od zera odejmujemy jedną minutę z licznika i przestawiamy sekundy oraz milisekundy
		}
		else {clock->seconds--; clock->miliseconds = 999;}
			//Jeżeli liczba sekund jest większa od zera odejmujemy jedną sekundę z zegara i przestawiamy milisekundy
	}
	else
		clock->miliseconds--;
		//Jeżeli liczba milisekund jest większa od 0 odejmujemy jedną milisekundę z zegara

	return 0;
}

void incrementClock(struct _time* clock, int secondInc)	//Funkcja inkrementująca dany zegar o podaną liczbę sekund
{
	if(secondInc == 0)	//Jeżeli inkrementacja jest "zerowa"
		return;

	clock->seconds += secondInc;

	if(clock->seconds >= 60)	//Jeżeli przekroczymy zakres 1 minuty dodając sekundy
	{
		clock->minutes++;
		clock->seconds -= 60;
	}
}

void clockTick()	//FUNCKJA ZMNIEJSZAJĄCA CZAS ZEGARA WYWOŁYWANA W PRZERWANIU TIMERA
{
	if(_currentPlayer == 1)
	{
		if(decrementClock(&PLAYER1_TIME) == 0)	//Jeżeli graczowi jeszcze nie skończył się czas
		{

		}
		else	//Graczowi skończył się czas
		{

		}
	}

	else if(_currentPlayer == 1)
	{
		if(decrementClock(&PLAYER2_TIME) == 0)	//Jeżeli graczowi jeszcze nie skończył się czas
		{

		}
		else	//Graczowi skończył się czas
		{

		}
	}
}

void clockSwitch()	//FUNCKJA INKREMENTUJĄCA CZAS ZEGARA WYWOŁYWANA W PRZERWANIU PRZYCISKÓW
{
	if(_currentPlayer == 1)
		{
			incrementClock(&PLAYER1_TIME, _increment);
		}

	else if(_currentPlayer == 1)
		{
			incrementClock(&PLAYER2_TIME, _increment);
		}
}

/*-------------------------------------------------------------------------------------------------
FUNKCJE UI I PRZERWANIA--------------------------------------------------------------------------*/
void switchPlayers()	//Funkcja zmienajaca któremu graczowi ma uplywac czas
{
	if(_currentPlayer == 1)
	{
		_currentPlayer = 2;
		UI_PLAYER1_DIODE_OFF;
		UI_PLAYER2_DIODE_ON;

		clockSwitch();
		return;
	}

	if(_currentPlayer == 2)
	{
		UI_PLAYER1_DIODE_ON;
		UI_PLAYER2_DIODE_OFF;
		_currentPlayer = 1;

		clockSwitch();
		return;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //Przerwania GPIO
{
   	if(GPIO_Pin == GPIO_PIN_10)					//Wcisniecie przycisku START
   	{
   		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
   		_pause = !_pause;						//Przełączenie pauzy
   	}

   	else if(GPIO_Pin == UI_PLAYER1_BUTTON)			//Wcisniecie przycisku PLAYER1
   	{
   			if(_currentPlayer == 1)
   				switchPlayers();
   	}

   	else if(GPIO_Pin == UI_PLAYER2_BUTTON)			//Wcisniecie przycisku PLAYER2
   	{
   			if(_currentPlayer == 2)
   				switchPlayers();
   	}
}

//-------------------------------------------------------------------------------------------------

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */


  //INICJALIZACJA ZEGARA---------------------------------------------------------------------------
  _currentPlayer = 1;
  setClocks(5);
  UI_PLAYER1_DIODE_ON;
  //WYBÓR PRESETU POWINIEN NASTĄPIC PÓŹNIEJ, W CELACH TESTOWYCH IGNORUJEMY WYBÓR PRESETU

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_DIODE1_GPIO_Port, DEBUG_DIODE1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : START_BUTTON_Pin PLAYER1_BUTTON_Pin PLAYER2_BUTTON_Pin */
  GPIO_InitStruct.Pin = START_BUTTON_Pin|PLAYER1_BUTTON_Pin|PLAYER2_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PLAYER1_DIODE_Pin PLAYER2_DIODE_Pin */
  GPIO_InitStruct.Pin = PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_DIODE1_Pin */
  GPIO_InitStruct.Pin = DEBUG_DIODE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_DIODE1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
