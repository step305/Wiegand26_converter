// Copyright (c) 2022.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction, including
//  without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//   of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
//   following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
//  OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.

//
// Created by step305 on 06.08.2022.
//

#ifndef WIEGAND26_CONVERTER_WIEGAND26_LIB_H
#define WIEGAND26_CONVERTER_WIEGAND26_LIB_H

#include "timer_utils.h"
#include "main.h"

typedef struct {
    GPIO_TypeDef* in_port_D0;
    GPIO_TypeDef* in_port_D1;
    uint16_t in_pin_D0;
    uint16_t in_pin_D1;
    GPIO_TypeDef* out_port_D0;
    GPIO_TypeDef* out_port_D1;
    uint16_t out_pin_D0;
    uint16_t out_pin_D1;
    TIM_HandleTypeDef* us_timer;
} WiegandPins;

typedef struct {
    TIM_HandleTypeDef* us_timer;
    GPIO_TypeDef* in_port_D0;
    GPIO_TypeDef* in_port_D1;
    uint16_t in_pin_D0;
    uint16_t in_pin_D1;
    GPIO_TypeDef* out_port_D0;
    GPIO_TypeDef* out_port_D1;
    uint16_t out_pin_D0;
    uint16_t out_pin_D1;
    __IO uint32_t cardH;
    __IO uint32_t cardL;
    __IO uint32_t recent_timestamp;
    uint32_t code;
    __IO int16_t bit_count;
    int16_t wiegand_type;
} Wiegand26State;

extern volatile uint8_t wiegand26_interrupt_enable;
extern Wiegand26State wiegand26_state;

void wiegand26_init(WiegandPins* config);
void wiegand26_write(uint32_t cardID);
void wiegand_read_D0();
void wiegand_read_D1();
uint8_t check_available_wiegand_code();
uint32_t wiegand_get_cardId(__IO uint32_t codelow);
uint32_t wiegand_get_code();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif //WIEGAND26_CONVERTER_WIEGAND26_LIB_H
