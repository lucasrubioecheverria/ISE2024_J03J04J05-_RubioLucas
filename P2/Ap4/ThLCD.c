#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "lcd.h"
#include "rtc.h"
#include "ThLCD.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThLCD;                        // thread id
 
void ThLCD (void *argument);                   // thread function
 
int Init_ThLCD (void) {
 
  tid_ThLCD = osThreadNew(ThLCD, NULL, NULL);
  if (tid_ThLCD == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThLCD (void *argument) {
 
  while (1) {
		tiempoyfecha datos;
		datos=get_tiempo_fecha();
//    Clean_L1();
//    Clean_L2();
		LCD_escribir_linea(2, datos.fecha);
		LCD_escribir_linea(1, datos.tiempo);
		LCD_update();
		osDelay(1000);
    osThreadYield();                            // suspend thread
  }
}
