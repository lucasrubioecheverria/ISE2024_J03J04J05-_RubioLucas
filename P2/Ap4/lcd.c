#include "main.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"
#include <string.h>

//Estructura del GPIO
GPIO_InitTypeDef GPIO_InitStruct;
//Manejador del timer
TIM_HandleTypeDef htim7;

//Buffer de datos a meter en el LCD
unsigned char buffer [512];
uint32_t positionL1=0;
uint32_t positionL2=256;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

void delay (uint32_t n_microsegundos){
  //Configuración del Timer 7
  htim7.Instance = TIM7;
  htim7.Init.Prescaler=2;
  htim7.Init.Period=28*n_microsegundos; //Registro de autorecarga
  __HAL_RCC_TIM7_CLK_ENABLE();
  HAL_TIM_Base_Init(&htim7);
  HAL_TIM_Base_Start(&htim7);//Iniciamos el timer
	
	while (!__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)){}
  HAL_TIM_Base_Stop(&htim7);//Paramos el timer
	HAL_TIM_Base_DeInit(&htim7);
  __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
}


//Función para la configuración del SPI
void LCD_reset(void){
  SPIdrv->Initialize (NULL);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8) , 1000000); //Deberia ser 20 MHz, es paar ver en el Pulseview
	
  __HAL_RCC_GPIOA_CLK_ENABLE(); //RESET
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	
	__HAL_RCC_GPIOF_CLK_ENABLE(); //A0
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	
	__HAL_RCC_GPIOD_CLK_ENABLE(); //CHIP SELECT
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	
	//Reset del LCD
	delay(1);  //Un microsegundo
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay(1000); //Un milisegundo
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	
}

void LCD_wr_data(unsigned char data){
	ARM_SPI_STATUS stat;
	//CS = 0
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	//A0 = 1
	HAL_GPIO_WritePin (GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
	//Escribir dato
	SPIdrv->Send(&data, sizeof(data));
	do{
		stat=SPIdrv->GetStatus();		
	}while(stat.busy);
	delay(1);
	//CS=1
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd){
	ARM_SPI_STATUS stat;
	//CS = 0
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	//A0 = 0
	HAL_GPIO_WritePin (GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
	//Escribir comando
	SPIdrv->Send(&cmd, sizeof(cmd));
	do{
		stat=SPIdrv->GetStatus();		
	}while(stat.busy);
	delay(1);
	//CS=1
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_Init(){
	LCD_wr_cmd(0xAE);
	LCD_wr_cmd(0xA2);
	LCD_wr_cmd(0xA0);
	LCD_wr_cmd(0xC8);
	LCD_wr_cmd(0x22);
	LCD_wr_cmd(0x2F);
	LCD_wr_cmd(0x40);
	LCD_wr_cmd(0xAF);
	LCD_wr_cmd(0x81);
	LCD_wr_cmd(0x15);
	LCD_wr_cmd(0xA4);
	LCD_wr_cmd(0xA6);
}

void LCD_update(void){
 //memset(buffer, 0x00, sizeof(buffer));
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }

}

void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
  //memset(buffer, 0x00, sizeof(buffer));
  uint8_t i,value1, value2;
  uint16_t offset=0;
  offset=25*(symbol-' ');
  for(i=0; i<12;i++){
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
  
    buffer[i+positionL1]=value1;
    buffer[i+128+positionL1]=value2;
  }  
  positionL1=positionL1+Arial12x12[offset];
  
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
  //memset(buffer, 0x00, sizeof(buffer));
  uint8_t i,value1, value2;
  uint16_t offset=0;
  offset=25*(symbol-' ');
  
 
  for(i=0; i<12;i++){
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
  
    buffer[i+positionL2]=value1;
    buffer[i+128+positionL2]=value2;
  }  
  positionL2=positionL2+Arial12x12[offset];
}

void LCD_symbolToLocalBuffer(uint8_t line,uint8_t symbol){
  if(line==1){
    LCD_symbolToLocalBuffer_L1(symbol);
  }else if(line==2){
    LCD_symbolToLocalBuffer_L2(symbol);
  }
 }

 void LCD_escribir_linea(uint8_t line,unsigned char buffer_escritura[]){
   int i, j;
	 if (line == 1){
     for(i=0; i<strlen(buffer_escritura); i++){
      LCD_symbolToLocalBuffer_L1(buffer_escritura[i]);
     }
  }else if (line == 2){
	  for(j=0; j<strlen(buffer_escritura); j++){
		LCD_symbolToLocalBuffer_L2(buffer_escritura[j]);
	}
    
  positionL1=0;
	positionL2=256;
 }
}
 
void Clean_L1(void){
  
  int i;
  for( i=0;i<256;i++){
    buffer[i]=0x00;
  }
	LCD_update();
}

void Clean_L2(void){
  int i;
  for(i=256; i< 512; i++){
    buffer[i]=0x00;
  }
	LCD_update();
  
}

 