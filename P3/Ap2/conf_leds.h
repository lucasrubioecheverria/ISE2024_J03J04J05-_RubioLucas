#include "stm32f4xx_hal.h"

int32_t LED_Initialize (void);
int32_t LED_Uninitialize (void);
int32_t LED_On (uint32_t num);
int32_t LED_Off (uint32_t num);
int32_t LED_SetOut (uint32_t val);
uint32_t LED_GetCount (void);
void conf_boton(void);
