/*
********************************************************
            LIBRARY TM1638
             * TRUNG *
            ************
///////////////////////////////////////////////////////
            DATE:    15/01/2025
            VER 1.0
********************************************************
*/
#include "TM1638.h"

void TM1638_DIO_Status(TM1638 *TM1638_PP, uint8_t Mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    if(TM1638_PP->DIO_PORT == ((GPIO_TypeDef *)GPIOA_BASE)) 
        { __HAL_RCC_GPIOA_CLK_ENABLE();}
    else if(TM1638_PP->DIO_PORT == ((GPIO_TypeDef *)GPIOB_BASE))
        { __HAL_RCC_GPIOB_CLK_ENABLE();}
    else
        { __HAL_RCC_GPIOC_CLK_ENABLE();}
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, GPIO_PIN_RESET);
    /*Configure GPIO pin */
    GPIO_InitStruct.Pin = TM1638_PP->DIO_PIN;
    //1: INPUT, 0: OUTPUT_PP
    if(Mode == 1) {GPIO_InitStruct.Mode = GPIO_MODE_INPUT;}
    else{
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    } 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TM1638_PP->DIO_PORT, &GPIO_InitStruct);
}

void TM1638_Init(TM1638 *TM1638_PP, uint16_t DIO_PIN, GPIO_TypeDef *DIO_PORT,
                        uint16_t CLK_PIN, GPIO_TypeDef *CLK_PORT,
                        uint16_t STB_PIN, GPIO_TypeDef *STB_PORT)
{
    TM1638_PP->DIO_PIN = DIO_PIN;
    TM1638_PP->CLK_PIN = CLK_PIN;
    TM1638_PP->STB_PIN = STB_PIN;

    TM1638_PP->DIO_PORT = DIO_PORT;
    TM1638_PP->CLK_PORT = CLK_PORT;
    TM1638_PP->STB_PORT = STB_PORT;
}

void TM1638_sendData(TM1638 *TM1638_PP, uint8_t data)
{
    uint8_t i;
    TM1638_DIO_Status(TM1638_PP, 0);
    HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, GPIO_PIN_RESET);
    for (i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_SET);
        data >>= 1;
    }
    HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, GPIO_PIN_SET);
}

void TM1638_sendCommand(TM1638 *TM1638_PP, uint8_t cmd)
{
    HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN, GPIO_PIN_RESET);
    TM1638_sendData(TM1638_PP, cmd);
}

void TM1638_display(TM1638 *TM1638_PP, uint8_t pos, uint8_t data)
{
    TM1638_sendCommand(TM1638_PP, 0x40); // Set address mode
    HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN, GPIO_PIN_RESET);
    TM1638_sendData(TM1638_PP, data);
    TM1638_sendData(TM1638_PP, 0xC0 | pos);
    HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN, GPIO_PIN_SET);
}

uint8_t *TM1638_readKeys(TM1638 *TM1638_PP, uint8_t *keys)
{
    uint8_t j, i;
    TM1638_DIO_Status(TM1638_PP, 1);
    TM1638_sendCommand(TM1638_PP, 0x42);
    HAL_Delay(1);
    TM1638_DIO_Status(TM1638_PP, 0);
    HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, GPIO_PIN_SET);
    for(j=0;j<5;j++){
		for(i=0;i<8;i++){
			HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_RESET);
			keys[j]=keys[j]>>1;
			HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_SET);
			TM1638_DIO_Status(TM1638_PP, 1);
			if (HAL_GPIO_ReadPin(TM1638_PP->DIO_PORT,  TM1638_PP->DIO_PIN)==1) 
                {keys[j]=keys[j]|0X80;}
		}
	}
    HAL_GPIO_WritePin(TM1638_PP->CLK_PORT, TM1638_PP->CLK_PIN, GPIO_PIN_RESET);
	TM1638_DIO_Status(TM1638_PP, 0);
	HAL_GPIO_WritePin(TM1638_PP->DIO_PORT, TM1638_PP->DIO_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN,GPIO_PIN_SET);

    return keys;
}

void TM1638_displayNumber(TM1638 *TM1638_PP, uint8_t *Num, uint8_t *LED_CODE){
  TM1638_sendCommand(TM1638_PP, 0x03);
  TM1638_sendCommand(TM1638_PP, 0x40);
  TM1638_sendCommand(TM1638_PP, 0xc0);

  for(uint8_t i = 0; i < 8; i++)
  {
    TM1638_sendData(TM1638_PP, LED_CODE[Num[i]]);
	TM1638_sendData(TM1638_PP, 0xC0);
  }
  TM1638_sendCommand(TM1638_PP, 0x8F);
  HAL_GPIO_WritePin(TM1638_PP->STB_PORT, TM1638_PP->STB_PIN ,GPIO_PIN_SET);
}
