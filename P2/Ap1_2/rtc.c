#include "rtc.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include <stdio.h>
#include <string.h>

RTC_HandleTypeDef RtcHandle;
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
RTC_AlarmTypeDef alarmRtc;

extern osThreadId_t tid_ThAlarm;

//Funci�n para la inicializaci�n del RTC y los valores iniciales de este
void init_RTC(void){
	
	//Inicio valores RTC
	uint8_t horas = 0x12;
	uint8_t minutos = 0x00;
	uint8_t segundos = 0x00;
	uint8_t dia = 0x27;
	uint8_t nombre_dia = RTC_WEEKDAY_TUESDAY;
	uint8_t mes = RTC_MONTH_FEBRUARY;
	uint8_t ano = 0x24;
	
	__HAL_RCC_PWR_CLK_ENABLE(); //Habilita el reloj del m�dulo de administraci�n de energ�a (PWR).
	HAL_PWR_EnableBkUpAccess(); //Habilita el acceso al dominio de backup para permitir la configuraci�n del RTC.
  __HAL_RCC_RTC_ENABLE(); //Habilita el reloj del RTC.
	
	RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24; //Formato de la hora
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV; //Predivisor As�ncrono
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV; //Predivisor s�ncrono
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	
	if (HAL_RTC_Init(&RtcHandle) != HAL_OK){
		printf("ERROR: No se ha podido iniciar el RTC\n\r");
  }

	set_dia_hora(horas, minutos, segundos, dia, nombre_dia, mes, ano);//Configuramos la hora y fecha del RTC
	
	set_alarm(); //Configuramos la alarma para el primer minuto
	
}

//Funci�n para configurar los valores del RTC
void set_dia_hora(uint8_t horas, uint8_t minutos, uint8_t segundos, uint8_t dia, uint8_t nombre_dia, uint8_t mes, uint8_t ano){
	
	//Configuraci�n del d�a, mes y a�o
	sdatestructure.Year = ano;
  sdatestructure.Month = mes;
  sdatestructure.Date = dia;
  sdatestructure.WeekDay = nombre_dia;
	if (HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    printf("ERROR: No se ha establecido correctamente la fecha\n\r");
	}
	
	//Configuraci�n de la hora, minutos y segundos
	stimestructure.Hours = horas;
  stimestructure.Minutes = minutos;
  stimestructure.Seconds = segundos;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    printf("ERROR: No se ha establecido correctamente la hora\n\r");
	}
}

//Funci�n para obtener el tiempo y la fecha
tiempoyfecha get_tiempo_fecha(void){
	
	tiempoyfecha tiempo_fecha;
	//Obtenci�n tiempo
	HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	sprintf(tiempo_fecha.tiempo,"Hora: %.2d:%.2d:%.2d",stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
	//Obtenci�n fecha
	HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
	sprintf(tiempo_fecha.fecha,"Fecha: %02d / %02d / %02d",sdatestructure.Date,sdatestructure.Month,sdatestructure.Year);
	return tiempo_fecha;
}

void set_alarm(void){
	
	HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
	
	uint8_t mins_alarm=stimestructure.Minutes+1; //Alarma en el siguiente minuto
	uint8_t horas_alarm=stimestructure.Hours;
	
	if(mins_alarm==60){
		mins_alarm=0;
		if(horas_alarm==23){
			horas_alarm=0;
		}else{
			horas_alarm++;
		}
	}
	
	alarmRtc.AlarmTime.Hours = horas_alarm;
  alarmRtc.AlarmTime.Minutes = mins_alarm;
  alarmRtc.AlarmTime.Seconds = stimestructure.Seconds;
  alarmRtc.AlarmTime.SubSeconds = 0x0;
  alarmRtc.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  alarmRtc.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  alarmRtc.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  alarmRtc.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  alarmRtc.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarmRtc.AlarmDateWeekDay = 0x1;
  alarmRtc.Alarm = RTC_ALARM_A;//Tipo de Alrma A
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);//Configuramos la prioridad de la interrupci�n del RTC
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);//Habilitamos la interrupci�n por RTC
	
	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &alarmRtc, RTC_FORMAT_BIN) != HAL_OK) {
		printf("ERROR: No se ha establecido la alarma correctamente\n\r");
  }
}

void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&RtcHandle);//Configuramos nuestra interrupci�n para nuestro manejador
	osThreadFlagsSet(tid_ThAlarm ,0x00000001U);//Activamos el flag que espera el Thread de Alarma para iniciar ambos timers
	set_alarm();//Volvemos a establecer la alarma para el siguiente minuto
}


//Timers: Peri�dico para que el LED verde parpadee cada 500ms y Timeout de 5 segundos
/*----- Periodic Timer Example -----*/
osTimerId_t tim_500ms;                            // timer id
static uint32_t exec2;                          // argument for the timer call back function
 
// Periodic Timer Function
static void Timer500_Callback (void const *arg) {
  // add user code here
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); //Parpadeo LED verde
}

int Init_Timer_500 (void) {
	
	osStatus_t status; 
	
	// Create periodic timer
  exec2 = 2U;
  tim_500ms = osTimerNew((osTimerFunc_t)&Timer500_Callback, osTimerPeriodic, &exec2, NULL);
//  if (tim_500ms != NULL) {  // Periodic timer created
//    // start timer with periodic 1000ms interval
//    status = osTimerStart(tim_500ms, 1000U);            
//    if (status != osOK) {
//      return -1;
//    }
//  }
  return NULL;
	
}
 
/*----- One-Shoot Timer Example -----*/
osTimerId_t tim_5s;                            // timer id
static uint32_t exec1;                          // argument for the timer call back function

// One-Shoot Timer Function
static void Timer5_Callback (void const *arg) {
  // add user code here
	osTimerStop(tim_500ms); //Paramos el timer de 500ms
}

int Init_Timer5 (void) {
  osStatus_t status;                            // function return status
 
  // Create one-shoot timer
  exec1 = 1U;
  tim_5s = osTimerNew((osTimerFunc_t)&Timer5_Callback, osTimerOnce, &exec1, NULL);
//  if (tim_5s != NULL) {  // One-shot timer created
//    // start timer with delay 100ms
//    status = osTimerStart(tim_5s, 100U); 
//    if (status != osOK) {
//      return -1;
//    }
//  }
	return NULL;
}


