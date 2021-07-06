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
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "math.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char TxDataBuffer[32] =
{ 0 };
char RxDataBuffer[32] =
{ 0 };
uint16_t ADCin = 0;
uint64_t _micro = 0;
float Vlow = 0;
uint16_t dataOut = 0;
float halftime = 250;
uint16_t STATE_Display = 0;
float Vhigh = 3.3;
uint16_t Vhighr = 4096;
uint16_t Vlowr = 0;
float m = 1;
float angle = 0;
uint16_t statem[2] = {};
float freq = 1;
uint16_t ufreq = 10;
uint16_t count = 0;
uint16_t dcount = 0;
float sintime = 1.5/(2*M_PI*4096);
uint16_t m2 = 0;
uint8_t DACConfig = 0b0011;
uint16_t sinstart = 0;
uint16_t sinstop = 0;
uint16_t Mode = 0;
uint16_t Hz = 0;
float time = 0;
uint16_t duty = 50;
uint16_t on = 1;
float x = 1;
float y= 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM11_Init(void);
void UARTRecieveAndResponsePolling();
int16_t UARTRecieveIT();
/* USER CODE BEGIN PFP */
void MCP4922SetOutput(uint8_t Config, uint16_t DACOutput);
uint64_t micros();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_SPI3_Init();
  MX_TIM3_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADCin, 1);

	HAL_GPIO_WritePin(LOAD_GPIO_Port, LOAD_Pin, GPIO_PIN_RESET);
	enum State_Display{
		StateDisplay_start =0,
			  StateMode = 10,
			  StateMenuNo_1 = 20,
			  StateNo_1 = 30,
			  StateNo_1FP = 40,
			  StateNo_1F = 50,
			  StateNo_1VP = 60,
			  StateNo_1V = 70,
			  StateVhp = 80,
			  StateVh = 90,
			  StateVlp = 100,
			  StateVl = 110,
			  StateNo_1SP = 200,
			  StateNo_1S = 210,
			  StateMenuNo_2 = 300,
			  StateNo_2 = 310,
			  StateMenuNo_3 = 500,
			  StateNo_3 = 510,
			  StateDutyP = 520,
			  StateDuty = 530
	};
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		if(m==1){
			m2=1;
		}
		if(m==-1){
			m2=2;
		}
		statem[0] = m2;
		static uint64_t timestamp = 0;
		HAL_UART_Receive_IT(&huart2,  (uint8_t*)RxDataBuffer, 32);
		int16_t inputchar = UARTRecieveIT();
		if(inputchar!=-1)
		{
			sprintf(TxDataBuffer, "In:[%c]\r\n", inputchar);
			HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
		}
		switch(STATE_Display){
		case StateDisplay_start://0
			sprintf(TxDataBuffer, "Mode.\r\n[1]:sawtooth\r\n[2]:sine wave\r\n[3]:square wave\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
		  	STATE_Display = StateMode;
		  	break;
		case StateMode://10
			switch(inputchar){
			case -1:
				break;
			case '1':
				count = 0;
				STATE_Display = StateMenuNo_1;//20
				Mode = 1;
				break;
			case '2':
				count = 0;
				STATE_Display = StateMenuNo_2;//300
				Mode = 2;
				break;
			case '3':
				count = 0;
				STATE_Display = StateMenuNo_3;//500
				Mode = 3;
				break;
			default:
				  count+=1;
				  if(count==2)
				  {
					  count=0;
					  STATE_Display = StateDisplay_start;//0
				  }
			}
			break;
		case StateMenuNo_1://20
			  sprintf(TxDataBuffer,"sawtooth\r\n[f]:frequency\r\n[v]:Vhigh,Vlow\r\n[s]:slope\r\n[x]:x\r\n");
			  HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
			  STATE_Display = StateNo_1;//30
			  break;
		case StateNo_1://30
			switch(inputchar){
			case -1:
				break;
			case 'f':
				count = 0;
				STATE_Display = StateNo_1FP;//40
				break;
			case 'v':
				count = 0;
				STATE_Display = StateNo_1VP;//60
				break;
			case 's':
				count = 0;
				STATE_Display = StateNo_1SP;//200
				break;
			case 'x':
				count =0;
				STATE_Display = StateDisplay_start;//0
				break;
			default:
				count+=1;
				if(count==2){
					count = 0;
					STATE_Display = StateMenuNo_1;//20
				}
			}break;
		case StateNo_1FP://40
			  sprintf(TxDataBuffer,"frequency\r\n[+]:+0.1Hz\r\n[-]:-0.1Hz\r\n[x]:x\r\n");
			  HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
			  STATE_Display = StateNo_1F;//50
			  break;
		case StateNo_1F://50
					switch(inputchar){
					case -1:
						break;
			case '+':
				Hz = 0;
				count+=1;
				if(count==2){
					count = 0;
					STATE_Display = StateNo_1FP;//40
				}
				if(ufreq<=99){
				ufreq+=1;
				}
				freq = ufreq/10.0;
				halftime =  250/freq;
				break;
			case '-':
				count+=1;
				if(count==2){
					count = 0;
					STATE_Display = StateNo_1FP;//40
				}
				if(ufreq>=1){
					ufreq-=1;
					freq=ufreq/10.0;
					halftime = 250/freq;
				}
				if(ufreq==0){
					Hz = 1;
					freq=0;
					halftime=0;
				}


				break;
			case 'x':
				count =0;
				if(Mode==1){
				STATE_Display = StateMenuNo_1;}//20
				else if(Mode==2){
				STATE_Display = StateMenuNo_2;}//300
				else if(Mode==3){
				STATE_Display = StateMenuNo_3;}//500
				break;
			default:
				count+=1;
				if(count==2){
					count = 0;
					STATE_Display = StateNo_1FP;//40
				}
			}break;
		case StateNo_1VP://60
			sprintf(TxDataBuffer,"Voltage\r\n[h]:high\r\n[l]:low\r\n[x]:x\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
			STATE_Display = StateNo_1V;//70
			break;
			case StateNo_1V://70
				switch(inputchar){
				case -1:
					break;
				case 'h':
					count = 0;
					STATE_Display = StateVhp;//80
					break;
				case 'l':
					count = 0;
					STATE_Display = StateVlp;//100
					break;
				case 'x':
					count =0;
					if(Mode==1){
					STATE_Display = StateMenuNo_1;}//30
					else if(Mode==2){
					STATE_Display = StateMenuNo_2;}//300
					else if(Mode==3){
					STATE_Display = StateMenuNo_3;}//500
					break;
				default:
					count+=1;
					if(count==2){
						count = 0;
						STATE_Display = StateNo_1VP;}//60
				}break;
				case StateVhp://80
					sprintf(TxDataBuffer,"Vhigh\r\n[+]:+\r\n[-]:-\r\n[x]:x\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
					STATE_Display = StateVh;//90
					break;
				case StateVh://90
					switch(inputchar)
					{
						case -1:
							break;
						case '+':
							count+=1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateVhp;//80
							}
							Vhigh+=0.1;
							if(Vhigh>=3.3)
							{
								Vhigh=3.3;
							}
							Vhighr = Vhigh*4096/3.3;
							Vlowr = Vlow*4096/3.3;
							break;
						case '-':
							count+=1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateVhp;//80
							}
								Vhigh-=0.1;
							if(Vhigh<=0.08)
							{
								Vhigh=0.1;
							}
							if(Vhigh-0.001<=Vlow&Vlow!=0)
							{
								Vlow=Vhigh-0.1;
							}
							if(Vhigh-0.001<=Vlow&Vlow==0)
							{
								Vhigh+=0.1;
							}
							Vhighr = Vhigh*4096/3.3;
							Vlowr = Vlow*4096/3.3;
							break;
						case 'x':
							count =0;
							STATE_Display = StateNo_1VP;//60
							break;
						default:
							count+=1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateVhp;//80
							}
					}break;
					case StateVlp://100
						sprintf(TxDataBuffer,"Vlow\r\n[+]:+\r\n[-]:-\r\n[x]:x\r\n");
						HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
						STATE_Display = StateVl;//110
						break;
					case StateVl://110
						switch(inputchar)
						{
							case -1:
								break;
							case '+':
								count+=1;
								if(count==2)
								{
									count = 0;
									STATE_Display = StateVlp;//100
								}
								Vlow+=0.1;
								if(Vlow>=3.18)
								{
									Vlow=3.2;
								}
								if(Vlow>=Vhigh-0.001&Vhigh<=3.3)
								{
									Vhigh=Vlow+0.1;
								}
								if(Vlow>=Vhigh-0.001&Vhigh==3.3)
								{
									Vlow-=0.1;
								}
								Vhighr = Vhigh*4096/3.3;
								Vlowr = Vlow*4096/3.3;
								break;
							case '-':
								count+=1;
								if(count==2)
								{
									count = 0;
									STATE_Display = StateVlp;//100
								}
									Vlow-=0.1;
								if(Vlow<=0.08)
								{
									Vlow=0;
								}
								Vhighr = Vhigh*4096/3.3;
								Vlowr = Vlow*4096/3.3;
								break;
							case 'x':
								count =0;
								STATE_Display = StateNo_1VP;//60
								break;
							default:
								count+=1;
								if(count==2)
								{
									count = 0;
									STATE_Display = StateVlp;//60
								}
						}break;


				case StateNo_1SP://200
					sprintf(TxDataBuffer,"slope\r\n[+]:+\r\n[-]:-\r\n[x]:x\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
					STATE_Display = StateNo_1S;//210
					break;
					case StateNo_1S://210
						switch(inputchar){
						case -1:
							break;
						case '+':
							count += 1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateNo_1SP;//200
							}
							if(m==-1){
								m=1;
							}
							if(statem[0]==1&statem[1]==2){
								dataOut=Vlowr;
							}
							break;
						case '-':
							count += 1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateNo_1SP;//200
							}
							if(m==1){
								m=-1;
							}
							if(statem[0]==2&statem[1]==1){
								dataOut=Vhighr;
							}
							break;
						case 'x':
							count =0;
							STATE_Display = StateMenuNo_1;//20
							break;
						default:
							count+=1;
							if(count==2)
							{
								count = 0;
								STATE_Display = StateNo_1SP;//200
							}
						}break;
						case StateMenuNo_2://300
							  sprintf(TxDataBuffer,"sine\r\n[f]:freq\r\n[v]:Vhigh,Vlow\r\n[x]:x\r\n");
							  HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
							  STATE_Display = StateNo_2;//310
							  break;
						case StateNo_2://310
							switch(inputchar){
							case -1:
								break;
							case 'f':
								count = 0;
								STATE_Display = StateNo_1FP;//40
								break;
							case 'v':
								count = 0;
								STATE_Display = StateNo_1VP;//60
								break;
							case 'x':
								count =0;
								STATE_Display = StateDisplay_start;//0
								break;
							default:
								count+=1;
								if(count==2){
									count = 0;
									STATE_Display = StateMenuNo_2;//310
								}
							}break;
							case StateMenuNo_3://500
								  sprintf(TxDataBuffer,"square\r\n[f]:freq\r\n[v]:Vhigh,Vlow\r\n[d]:duty\r\n[x]:x\r\n");
								  HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
								  STATE_Display = StateNo_3;//510
								  break;
							case StateNo_3://510
								switch(inputchar){
								case -1:
									break;
								case 'f':
									count = 0;
									STATE_Display = StateNo_1FP;//40
									break;
								case 'v':
									count = 0;
									STATE_Display = StateNo_1VP;//60
									break;
								case 'd':
									count = 0;
									STATE_Display = StateDutyP;
									break;
								case 'x':
									count =0;
									STATE_Display = StateDisplay_start;//0
									break;
								default:
									count+=1;
									if(count==2){
										count = 0;
										STATE_Display = StateMenuNo_3;//510
									}
								}break;
								case StateDutyP://520
									  sprintf(TxDataBuffer,"Duty cycle\r\n[+]:+\r\n[-]:-\r\n[x]:x\r\n");
									  HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer),1000);
									  STATE_Display = StateDuty;//510
									  break;
								case StateDuty://510
									switch(inputchar){
									case -1:
										break;
									case '+':
										count+=1;
										if(count>=2){
											count=0;
											STATE_Display = StateDutyP;
										}
										if(duty<=99){
										duty+=1;}
										break;
									case '-':
										count+=1;
										if(count>=2){
											count=0;
											STATE_Display = StateDutyP;
										}
										if(duty>=1){
											duty-=1;
										}
										break;
									case 'x':
										count =0;
										STATE_Display = StateMenuNo_3;//0
										break;
									default:
										count+=1;
										if(count==2){
											count = 0;
											STATE_Display = StateDutyP;//510
										}
									}break;


		}
		if(Mode==1|Mode==2){
			time =  halftime*((4096.0)/(Vhighr-Vlowr));
		}
		if(Mode==3){
			time = halftime;
		}
		if (micros() - timestamp > time)
		{
			timestamp = micros();
			if(Hz==1)
			{
				dataOut=(Vhighr+Vlowr)/2;
			}
			if((Mode==1)&(ufreq>=1)&(m==-1))
			{
				dataOut+=m;
				if(dataOut<=Vlowr)
				{
					dataOut+=Vhighr-Vlowr;
				}
			}
			if(Mode==1&ufreq>=1&m==1)
			{
				dataOut+=1;
				if(dataOut>=Vhighr)
				{
					dataOut = (dataOut%Vhighr) + Vlowr;
				}
			}
			if(Mode==2&ufreq>=1){
				sintime+=4096/(4096*freq*(Vhighr-Vlowr));
				angle = 2*M_PI*freq*sintime;
				dataOut=((Vhighr-Vlowr)/2)*sin(angle)+((Vhighr+Vlowr)/2);
			}
			if(Mode==3&Hz==0){
				dcount+=1;
				if(dcount<duty*4096.0/(100.0*y)){
					dataOut=Vhighr;
				}
				if(dcount>=duty*4096/(100.0*y))
				{
					if(duty<100){
					dataOut=Vlowr;
					if(dcount>x*4096/(y)){
						dcount=0;}
					if(duty==100){
						count=0;
					}
					}
				}
			}
			if (hspi3.State == HAL_SPI_STATE_READY
					&& HAL_GPIO_ReadPin(SPI_SS_GPIO_Port, SPI_SS_Pin)
							== GPIO_PIN_SET)
			{
				MCP4922SetOutput(DACConfig, dataOut);
			}
		}
		statem[1]=statem[0];
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the analog watchdog
  */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
  AnalogWDGConfig.HighThreshold = 0;
  AnalogWDGConfig.LowThreshold = 0;
  AnalogWDGConfig.Channel = ADC_CHANNEL_0;
  AnalogWDGConfig.ITMode = DISABLE;
  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 99;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 99;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 65535;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LOAD_GPIO_Port, LOAD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LOAD_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_SS_Pin */
  GPIO_InitStruct.Pin = SPI_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SHDN_Pin */
  GPIO_InitStruct.Pin = SHDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHDN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void UARTRecieveAndResponsePolling()
{
	char Recieve[32]={0};

	HAL_UART_Receive(&huart2, (uint8_t*)Recieve, 4, 1000);

	sprintf(TxDataBuffer, "Received:[%s]\r\n", Recieve);
	HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer), 1000);

}


int16_t UARTRecieveIT()
{
	static uint32_t dataPos =0;
	int16_t data=-1;
	if(huart2.RxXferSize - huart2.RxXferCount!=dataPos)
	{
		data=RxDataBuffer[dataPos];
		dataPos= (dataPos+1)%huart2.RxXferSize;
	}
	return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	sprintf(TxDataBuffer, "Received:[%s]\r\n", RxDataBuffer);
	HAL_UART_Transmit_IT(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer));
}
void MCP4922SetOutput(uint8_t Config, uint16_t DACOutput)
{
	uint32_t OutputPacket = (DACOutput & 0x0fff) | ((Config & 0xf) << 12);
	HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi3, &OutputPacket, 1);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi3)
	{
		HAL_GPIO_WritePin(SPI_SS_GPIO_Port, SPI_SS_Pin, GPIO_PIN_SET);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim11)
	{
		_micro += 65535;
	}
}

inline uint64_t micros()
{
	return htim11.Instance->CNT + _micro;
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
