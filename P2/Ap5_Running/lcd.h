#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx_hal.h"

//Cabeceras de las funciones
void delay (uint32_t n_microsegundos);
void LCD_reset(void);
void LCD_wr_data (unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_Init(void);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_symbolToLocalBuffer_L3(uint8_t symbol);
void LCD_symbolToLocalBuffer(uint8_t line,uint8_t symbol);
void LCD_escribir_linea(uint8_t line,unsigned char buffer_escritura[]);
  
#endif
