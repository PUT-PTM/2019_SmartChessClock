
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

/* USER CODE BEGIN Includes */
#include "stm32_tm1637.h"
#include "stm32_i2c2.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//PREPROCESOR
#define UI_PLAYER1_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)		//Zapalenie diody przycisku 1
#define UI_PLAYER1_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)	//Zgaszenie diody przycisku 1
#define UI_PLAYER1_BUTTON		GPIO_PIN_2												//Uchwyt przycisku gracza 1

#define UI_PLAYER2_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)		//Zapalenie diody przycisku 2
#define UI_PLAYER2_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)	//Zgaszenie diody przycisku 2
#define UI_PLAYER2_BUTTON		GPIO_PIN_13												//Uchwyt przycisku gracza 2

#define DEBUG_DIODE1_ON			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET)		//Zapalenie niebieskiej diody na plytce
#define DEBUG_DIODE1_OFF		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET)	//Zapalenie niebieskiej diody na plytce
#define UI_PAUSE_BUTTON			GPIO_PIN_1												//Uchwyt przycisku start/stop

//Definicje presetów czasowych (poniżej inicjalizacja w funkcji PresetInit)
#define NUMBER_OF_PRESETS		12

#define BULLET_1_0				0
#define BULLET_2_1				1
#define BLITZ_3_0				2
#define BLITZ_3_2				3
#define BLITZ_5_0				4
#define	BLITZ_5_3				5
#define	RAPID_10_0				6
#define	RAPID_15_0				7
#define	RAPID_15_15				8
#define	RAPID_25_0				9
#define	RAPID_25_10				10
#define CLASSICAL_30_0			11

#define BLUETOOTH_PRESET		12

//STRUKTURA CZASU
struct _time
{
	volatile uint8_t minutes;
	volatile uint8_t seconds;
	volatile uint16_t miliseconds;
};

//PRESETY CZASOWE
struct _preset
{
	struct _time time;
	volatile uint8_t increment;
};

struct _preset presets[NUMBER_OF_PRESETS];	//Struktura zawierająca presety czasowe

//STRUKTURA KOMUNIKATU BLUETOOTH
struct _bluetooth
{
	volatile uint8_t operation;					//S -> start/stop	T -> zmiana gracza	P -> preset czasowy
	volatile uint8_t player1time[4];			//Znacznik czasowy gracza 1 w milisekundach
	volatile uint8_t player2time[4];			//Znacznik czasowy gracza 2 w milisekundach
};

//ZMIENNE GLOBALNE
volatile uint8_t _currentPlayer;		//Zmienna oznaczajaca któremu z graczy uplywa czas
volatile uint8_t _increment;			//Zmienna oznaczajaca dodawany czas po wcisnieciu przycisku w sekundach
volatile uint8_t _pause = 1;			//Zmienna wyznaczajaca pauze w pomiarze czasu
volatile uint8_t _gameOver = 0;			//Zmienna wyznaczająca koniec gry w przypadku gdy jednemu z graczy upłynie czas
volatile uint8_t _refresh = 0;			//Zmienna wykorzystywana do odświeżania wyświetlaczy zegara
volatile int8_t _presetSelect = 0;		//Zmienna wykorzystywana przy wyborze presetu zegara
volatile uint8_t _display;				//Zmienna wykorzysytwana przy miganiu wyświetlaczami przy pauzie


struct _time PLAYER1_TIME;	//Struktura przechowująca informacje o czasie gracza nr 1
struct _time PLAYER2_TIME;	//Struktura przechowująca informacje o czasie gracza nr 2


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM10_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* FUNKCJE DO OBS�?UGI MODU�?U BLUETOOTH -------------------------------------------------------------*/

struct _bluetooth buildBluetoothMessage(char operation)
{
	struct _bluetooth out;
	uint32_t p1t, p2t;

	p1t = PLAYER1_TIME.miliseconds + PLAYER1_TIME.seconds * 1000 + PLAYER1_TIME.minutes * 60000;
	p2t = PLAYER2_TIME.miliseconds + PLAYER2_TIME.seconds * 1000 + PLAYER2_TIME.minutes * 60000;

	out.operation = operation;
	out.player1time[0] = (p1t >> 24) & 0xFF;
	out.player1time[1] = (p1t >> 16) & 0xFF;
	out.player1time[2] = (p1t >> 8) & 0xFF;
	out.player1time[3] = p1t & 0xFF;

	out.player2time[0] = (p2t >> 24) & 0xFF;
	out.player2time[1] = (p2t >> 16) & 0xFF;
	out.player2time[2] = (p2t >> 8) & 0xFF;
	out.player2time[3] = p2t & 0xFF;

	return out;
}

void sendBluetoothMessage(char operation)
{
	struct _bluetooth message = buildBluetoothMessage(operation);
	uint16_t messageSize = 9;
	uint8_t messageBytes[9];

	messageBytes[0] = message.operation;
	messageBytes[1] = message.player1time[0];
	messageBytes[2] = message.player1time[1];
	messageBytes[3] = message.player1time[2];
	messageBytes[4] = message.player1time[3];

	messageBytes[5] = message.player2time[0];
	messageBytes[6] = message.player2time[1];
	messageBytes[7] = message.player2time[2];
	messageBytes[8] = message.player2time[3];


	HAL_UART_Transmit_IT(&huart3, messageBytes, messageSize);
}

struct _bluetooth receiveBluetoothMessage()
{
	struct _bluetooth out;
	uint16_t messageSize = 9;
	uint8_t messageBytes[9];


	HAL_UART_Receive_IT(&huart3, messageBytes, messageSize);

	out.operation = messageBytes[0];


	out.player1time[0] = messageBytes[1];
	out.player1time[1] = messageBytes[2];
	out.player1time[2] = messageBytes[3];
	out.player1time[3] = messageBytes[4];

	out.player2time[0] = messageBytes[5];
	out.player2time[1] = messageBytes[6];
	out.player2time[2] = messageBytes[7];
	out.player2time[3] = messageBytes[8];

	return out;
}

void addPresetFromBluetooth()
{
	struct _bluetooth message = receiveBluetoothMessage();

	uint32_t baseTime, increment;
	uint8_t baseTimeArr[4], incrementArr[4];

	//Konwersja komunikatu na integery
	baseTimeArr[0] = message.player1time[0];
	baseTimeArr[1] = message.player1time[1];
	baseTimeArr[2] = message.player1time[2];
	baseTimeArr[3] = message.player1time[3];

	incrementArr[0] = message.player2time[0];
	incrementArr[1] = message.player2time[1];
	incrementArr[2] = message.player2time[2];
	incrementArr[3] = message.player2time[3];


	baseTime = *(int *)baseTimeArr;
	increment = *(int *)incrementArr;

	//Uzupełnienie struktury presetu
	unsigned int divider;

	//Określenie liczby minut
	divider = baseTime / 60000;
	baseTime -= divider * 60000;
	presets[BLUETOOTH_PRESET].time.minutes = divider * 60000;

	//Określenie liczby sekund
	divider = baseTime / 1000;
	baseTime -= divider * 1000;
	presets[BLUETOOTH_PRESET].time.minutes = divider * 1000;

	//Określenie liczby milisekund
	presets[BLUETOOTH_PRESET].time.miliseconds = baseTime;

	//Przypisanie liczby sekund do inkrementu
	presets[BLUETOOTH_PRESET].increment = increment / 1000;
}

/* FUNKCJE MIERZĄCE/OBS�?UGUJĄCE CZAS---------------------------------------------------------------*/
void PresetInit()
{
	//BULLET 1+0
	presets[BULLET_1_0].time.miliseconds = 0;
	presets[BULLET_1_0].time.seconds = 0;
	presets[BULLET_1_0].time.minutes = 1;
	presets[BULLET_1_0].increment = 0;

	//BULLET 2+1
	presets[BULLET_2_1].time.miliseconds = 0;
	presets[BULLET_2_1].time.seconds = 0;
	presets[BULLET_2_1].time.minutes = 2;
	presets[BULLET_2_1].increment = 1;

	//BLITZ 3+0
	presets[BLITZ_3_0].time.miliseconds = 0;
	presets[BLITZ_3_0].time.seconds = 0;
	presets[BLITZ_3_0].time.minutes = 3;
	presets[BLITZ_3_0].increment = 0;

	//BLITZ 3+2
	presets[BLITZ_3_2].time.miliseconds = 0;
	presets[BLITZ_3_2].time.seconds = 0;
	presets[BLITZ_3_2].time.minutes = 3;
	presets[BLITZ_3_2].increment = 2;

	//BLITZ 5+0
	presets[BLITZ_5_0].time.miliseconds = 0;
	presets[BLITZ_5_0].time.seconds = 0;
	presets[BLITZ_5_0].time.minutes = 5;
	presets[BLITZ_5_0].increment = 0;

	//BLITZ 5+3
	presets[BLITZ_5_3].time.miliseconds = 0;
	presets[BLITZ_5_3].time.seconds = 0;
	presets[BLITZ_5_3].time.minutes = 5;
	presets[BLITZ_5_3].increment = 3;

	//RAPID 10+0
	presets[RAPID_10_0].time.miliseconds = 0;
	presets[RAPID_10_0].time.seconds = 0;
	presets[RAPID_10_0].time.minutes = 10;
	presets[RAPID_10_0].increment = 0;

	//RAPID 15+0
	presets[RAPID_15_0].time.miliseconds = 0;
	presets[RAPID_15_0].time.seconds = 0;
	presets[RAPID_15_0].time.minutes = 15;
	presets[RAPID_15_0].increment = 0;

	//RAPID 15+15
	presets[RAPID_15_15].time.miliseconds = 0;
	presets[RAPID_15_15].time.seconds = 0;
	presets[RAPID_15_15].time.minutes = 15;
	presets[RAPID_15_15].increment = 15;

	//RAPID 25+0
	presets[RAPID_25_0].time.miliseconds = 0;
	presets[RAPID_25_0].time.seconds = 0;
	presets[RAPID_25_0].time.minutes = 25;
	presets[RAPID_25_0].increment = 0;

	//RAPID 25+10
	presets[RAPID_25_10].time.miliseconds = 0;
	presets[RAPID_25_10].time.seconds = 0;
	presets[RAPID_25_10].time.minutes = 25;
	presets[RAPID_25_10].increment = 10;

	//CLASSICAL 30+0
	presets[CLASSICAL_30_0].time.miliseconds = 0;
	presets[CLASSICAL_30_0].time.seconds = 0;
	presets[CLASSICAL_30_0].time.minutes = 30;
	presets[CLASSICAL_30_0].increment = 0;

	//BLUETOOTH
	presets[BLUETOOTH_PRESET].time.miliseconds = 0;
	presets[BLUETOOTH_PRESET].time.seconds = 0;
	presets[BLUETOOTH_PRESET].time.minutes = 99;
	presets[BLUETOOTH_PRESET].increment = 99;


}

void setClocks(int presetIndex)						//Funkcja ustawia czas obu zegarów z numeru presetu
{
	PLAYER1_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER1_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER1_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	PLAYER2_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER2_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER2_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	_increment = presets[presetIndex].increment;
}

int decrement(struct _time* clock)					//Funkcja pomniejsza wartośc struktury czasu o 1ms - jeżeli czas upłynął całkowicie - funkcja zwraca wartośc 1
{
	if(clock->miliseconds <= 0)
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

void increment(struct _time* clock, int secondInc)	//Funkcja inkrementująca dany zegar o podaną liczbę sekund
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

void clockTick()									//FUNCKJA ZMNIEJSZAJĄCA CZAS ZEGARA WYWO�?YWANA W PRZERWANIU TIMERA i całą logikę z tym związaną
{
	if(_currentPlayer == 1)
	{
		if(decrement(&PLAYER1_TIME) == 0)	//Jeżeli graczowi jeszcze nie skończył się czas
		{

		}
		else	//Graczowi 1 skończył się czas
		{
				_pause = 1;
				_gameOver = 1;
				HAL_TIM_Base_Stop_IT(&htim2);
				HAL_TIM_Base_Stop_IT(&htim9);
		}
	}

	else if(_currentPlayer == 2)
	{
		if(decrement(&PLAYER2_TIME) == 0)	//Jeżeli graczowi jeszcze nie skończył się czas
		{

		}
		else	//Graczowi 2 skończył się czas
		{
				_pause = 1;
				_gameOver = 1;
				HAL_TIM_Base_Stop_IT(&htim2);
				HAL_TIM_Base_Stop_IT(&htim9);
		}
	}
}

void clockIncrement()								//FUNCKJA INKREMENTUJĄCA CZAS ZEGARA WYWO�?YWANA W PRZERWANIU PRZYCISKÓW i całą logikę z tym związaną
{
	if(_pause == 0)	//Jeżeli gra jest spauzowania nie wykonujemy inkrementacji
	{

	if(_currentPlayer == 1)
		{
			increment(&PLAYER1_TIME, _increment);
		}

	else if(_currentPlayer == 2)
		{
			increment(&PLAYER2_TIME, _increment);
		}

	}
}

void incrementPreset()		//Funkcja inkrementująca kursor presetu
{
	if(_presetSelect < NUMBER_OF_PRESETS)
		_presetSelect++;
}

void decrementPreset()		//Funkcja dekrementująca kursor presetu
{
	if(_presetSelect > 0)
		_presetSelect--;
}

/*-------------------------------------------------------------------------------------------------
FUNKCJE UI I PRZERWANIA--------------------------------------------------------------------------*/
void onDisplays()
{
	tm1637SetBrightness('8');
	tm1637SetBrightness2('8');
	_display = 1;
}

void offDisplays()
{
	tm1637SetBrightness('0');
	tm1637SetBrightness2('0');
	_display = 0;
}

int timeToDisplay(struct _time* clock)	//Fukcja zwraca integera do wyświetlenia na ekranie
{
	return clock->minutes * 100 + clock->seconds;
}

void switchPlayers()				//Funkcja zmienajaca któremu graczowi ma uplywac czas
{
	if(_gameOver == 0)				//Jeżeli gra nie dobiegła jeszcze końca
	{
		if(_currentPlayer == 1)
		{
			UI_PLAYER1_DIODE_OFF;
			UI_PLAYER2_DIODE_ON;
			clockIncrement();		//Inkrementacja czasu jest w osobnej funkcji żeby umożliwic zmianę graczy na pauzie bez dodawania czasu
			_currentPlayer = 2;

		   	//Wysłanie komunikatu bluetooth o zmianie gracza do aplikacji
		   	sendBluetoothMessage('T');

			return;
		}

		if(_currentPlayer == 2)
		{
			UI_PLAYER1_DIODE_ON;
			UI_PLAYER2_DIODE_OFF;
			clockIncrement();
			_currentPlayer = 1;

			//Wysłanie komunikatu bluetooth o zmianie gracza do aplikacji
			sendBluetoothMessage('T');

			return;
		}
	}
}

void switchTimer()				//Funkcja przełączająca timer podczas pauzowania/wznawiania gry
{
	if(_gameOver == 0)
	{
	   	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);

	   	if(_pause == 1)								//Wnowienie gry po zatrzymaniu
	   	{
	   		_pause = 0;

	   		if(_currentPlayer == 1)
	   		{
	   			HAL_TIM_Base_Start_IT(&htim2);		//Wznowienie timera odmierzającego czas graczowi 1
	   		}
	   		else if(_currentPlayer == 2)
	   		{
	   			HAL_TIM_Base_Start_IT(&htim9);
	   		}

	   	}

	   	else if(_pause == 0)						//Spauzowanie gry
	   	{
	   		_pause = 1;
	   		HAL_TIM_Base_Stop_IT(&htim2);			//Zatrzymanie timera odmierzającego czas
	   		HAL_TIM_Base_Stop_IT(&htim9);
	  	}

	   	//Wysłanie komunikatu bluetooth o starcie/stopie do aplikacji
	   	sendBluetoothMessage('S');

	 }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 		//Przerwania GPIO
{
   	if(GPIO_Pin == UI_PAUSE_BUTTON)					//Wcisniecie przycisku START/STOP
   	{
   			HAL_TIM_Base_Start_IT(&htim10);
   	}

   	if(GPIO_Pin == UI_PLAYER1_BUTTON)			//Wcisniecie przycisku PLAYER1
   	{
   			HAL_TIM_Base_Start_IT(&htim10);
   	}

   	if(GPIO_Pin == UI_PLAYER2_BUTTON)			//Wcisniecie przycisku PLAYER2
   	{
   			HAL_TIM_Base_Start_IT(&htim10);
   	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//Przerwania Timerów
{
   	if(htim->Instance == TIM2)	//Przepełnienie timera nr 2 -> upłynięcie milisekundy gracza 1
   	{
         clockTick();
         //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);	Można na oscyloskopie sprawdzic, czy timer odmierza dokładnie 1 ms
    }

   	if(htim->Instance == TIM9)	//Przepełnienie timera nr 9 -> upłynięcie milisekundy gracza 2
   	{
   	     clockTick();
   	}

   	if(htim->Instance == TIM3)	//Przepełnienie timera nr 3 -> odświeżenie wyświetlaczy
   	{
   		_refresh = 1;
   	}

   	if(htim->Instance == TIM10)	//Przepełnienie timera nr 10 -> debouncing przycisków
   	{
   		//FUNKCJA RESET - WSZYSTKIE TRZY PRZYCISKI WCIŚNIĘTE
   		if(HAL_GPIO_ReadPin(GPIOB, UI_PAUSE_BUTTON) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, UI_PLAYER1_BUTTON) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, UI_PLAYER2_BUTTON) == GPIO_PIN_RESET)
   		{
   			//Na razie programowo
   			int i;
   			for(i = 0; i < 1000000; ++i);
   			NVIC_SystemReset();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PAUSE_BUTTON) == GPIO_PIN_RESET)			//Przycisk pauzy
   		{
   			if(_presetSelect != -1)		//Wciśnięcie przysicku pauzy rozpoczyna rozgrywkę
   			{
   				if(_presetSelect == BLUETOOTH_PRESET)	//Jeżeli gracz wybrał preset ósmy, czyli czeka na preset z aplikacji...
   				{
   					addPresetFromBluetooth();
   				}

   				setClocks(_presetSelect);
   				_presetSelect = -1;
   				_pause = 1;
   			}

   			else
   				switchTimer();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PLAYER1_BUTTON) == GPIO_PIN_RESET)		//Gracz 1
   		{
   			if(_presetSelect != -1)
   				decrementPreset();

   			else if(_currentPlayer == 1)
   			   	switchPlayers();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PLAYER2_BUTTON) == GPIO_PIN_RESET)		//Gracz 2
   		{
   			if(_presetSelect != -1)
   			   	incrementPreset();

   			else if(_currentPlayer == 2)
   				switchPlayers();
   		}

   	   	HAL_TIM_Base_Stop_IT(&htim10);
   	   	TIM10->CNT = 0;
   	}
}
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
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  //INICJALIACJA WYŚWIETLACZY I EXTI
  tm1637Init();
  tm1637Init2();
  onDisplays();										//Włączenie obu wyświetlaczy
  HAL_GPIO_EXTI_IRQHandler(UI_PLAYER1_BUTTON);		//Odświeżenie EXTI
  HAL_GPIO_EXTI_IRQHandler(UI_PLAYER2_BUTTON);
  HAL_GPIO_EXTI_IRQHandler(UI_PAUSE_BUTTON);


  //INICJALIZACJA ZEGARA---------------------------------------------------------------------------
  PresetInit();						//Wypełnienie pamięci presetami
  HAL_TIM_Base_Start_IT(&htim3);	//Uruchomienie timera odświeżającego wyświetlacze


  //WYBÓR PRESETU CZASOWEGO------------------------------------------------------------------------
  while(1)		//Wciśnięcie przycisku pauzy rozpocznie rozgrywkę z wybranym presetem
  {
	  if(_refresh == 1)
	  {
		  tm1637DisplayDecimal(_presetSelect + 1, 0);
		  tm1637DisplayDecimal2(presets[_presetSelect].time.minutes * 100 + presets[_presetSelect].increment, 1);
	  	  _refresh = 0;
	  }

	  if(_presetSelect == -1)
		  break;
  }

  _currentPlayer = 1;				//Domyślnie gracz 1 "ma ruch"
  UI_PLAYER1_DIODE_ON;				//Włączenie diody gracza 1

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //ROZGRYWKA--------------------------------------------------------------------------------------
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  if(_refresh == 1)
	  {
		  tm1637DisplayDecimal(timeToDisplay(&PLAYER1_TIME), !((PLAYER1_TIME.miliseconds / 500) && (_currentPlayer == 1)));
		  tm1637DisplayDecimal2(timeToDisplay(&PLAYER2_TIME), !((PLAYER2_TIME.miliseconds / 500) && (_currentPlayer == 2)));
		  _refresh = 0;
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
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* TIM6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* TIM10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM10_IRQn);
  /* TIM9_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM9_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM9_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 159;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM6 init function */
static void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 799;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 9999;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM9 init function */
static void MX_TIM9_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 15;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 999;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim9, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM10 init function */
static void MX_TIM10_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 159;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 499;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim10, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_TIMER_GPIO_Port, DEBUG_TIMER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PAUSE_BUTTON_Pin PLAYER1_BUTTON_Pin PLAYER2_BUTTON_Pin */
  GPIO_InitStruct.Pin = PAUSE_BUTTON_Pin|PLAYER1_BUTTON_Pin|PLAYER2_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PLAYER1_DIODE_Pin PLAYER2_DIODE_Pin */
  GPIO_InitStruct.Pin = PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_TIMER_Pin */
  GPIO_InitStruct.Pin = DEBUG_TIMER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_TIMER_GPIO_Port, &GPIO_InitStruct);

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
