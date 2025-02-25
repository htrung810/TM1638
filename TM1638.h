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

#ifndef TM1638_H
#define TM1638_H
#include "stm32f1xx_hal.h"

typedef struct TM1638
{
    uint16_t DIO_PIN;
    uint16_t CLK_PIN;
    uint16_t STB_PIN;

    GPIO_TypeDef *DIO_PORT;
    GPIO_TypeDef *CLK_PORT;
    GPIO_TypeDef *STB_PORT;
}TM1638;

void TM1638_DIO_Status(TM1638 *TM1638_PP, uint8_t Mode);

void TM1638_Init(TM1638 *TM1638_PP, uint16_t DIO_PIN, GPIO_TypeDef *DIO_PORT,
                        uint16_t CLK_PIN, GPIO_TypeDef *CLK_PORT,
                        uint16_t STB_PIN, GPIO_TypeDef *STB_PORT);
void TM1638_sendData(TM1638 *TM1638_PP, uint8_t data);
void TM1638_sendCommand(TM1638 *TM1638_PP, uint8_t cmd);
void TM1638_display(TM1638 *TM1638_PP, uint8_t pos, uint8_t data);
uint8_t *TM1638_readKeys(TM1638 *TM1638, uint8_t *keys);
void TM1638_displayNumber(TM1638 *TM1638_PP, uint8_t *Num, uint8_t *LED_CODE);

#endif
