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

#include "../Inc/wiegand26_lib.h"

Wiegand26State wiegand26_state;
volatile uint8_t wiegand26_interrupt_enable = 1;

uint32_t wiegand_get_code() {
    return wiegand26_state.code;
}

void wiegand_read_D0() {
    wiegand26_state.bit_count++;
    if (wiegand26_state.bit_count > 31) {
        wiegand26_state.cardH |= ((0x80000000 & wiegand26_state.cardL) >> 31);
        wiegand26_state.cardH <<= 1;
        wiegand26_state.cardL <<= 1;
    }
    else {
        wiegand26_state.cardL <<= 1;
    }
    wiegand26_state.recent_timestamp = HAL_GetTick();
}

void wiegand_read_D1() {
    wiegand26_state.bit_count++;
    if(wiegand26_state.bit_count > 31) {
        wiegand26_state.cardH |= ((0x80000000 & wiegand26_state.cardL) >> 31);
        wiegand26_state.cardH <<= 1;
        wiegand26_state.cardL |= 1;
        wiegand26_state.cardL <<= 1;
    }
    else {
        wiegand26_state.cardL |= 1;
        wiegand26_state.cardL <<= 1;
    }
    wiegand26_state.recent_timestamp = HAL_GetTick();
}

uint32_t wiegand_get_cardId(__IO uint32_t codelow) {
    return (codelow & 0x1FFFFFE) >> 1; //only Wiegand26 is supported
}

uint8_t check_available_wiegand_code() {
    uint32_t cardID;
    uint32_t timestamp = HAL_GetTick();
    uint8_t result = 0;

    if((timestamp - wiegand26_state.recent_timestamp) > 25) {
        if(wiegand26_state.bit_count == 26) { //only Wiegand26 is supported
            wiegand26_state.cardL >>= 1;
            cardID = wiegand_get_cardId(wiegand26_state.cardL);
            wiegand26_state.bit_count = 0;
            wiegand26_state.cardL = 0;
            wiegand26_state.cardH = 0;
            wiegand26_state.code = cardID;
            result = 1;
        }
        else {
            wiegand26_state.recent_timestamp = timestamp;
            wiegand26_state.bit_count = 0;
            wiegand26_state.cardL = 0;
            wiegand26_state.cardH = 0;
        }
    }
    return result;
}

void wiegand26_write_bit(uint8_t bit) {
    GPIO_TypeDef* port = (bit == 0) ? wiegand26_state.port_D0 : wiegand26_state.port_D1;
    uint16_t pin = (bit == 0) ? wiegand26_state.pin_D0 : wiegand26_state.pin_D1;

    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    delay_us(wiegand26_state.us_timer, 80);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    delay_us(wiegand26_state.us_timer, 240);
}

void wiegand26_init(WiegandPins* config) {
    wiegand26_state.us_timer = config->us_timer;
    wiegand26_state.port_D0 = config->port_D0;
    wiegand26_state.port_D1 = config->port_D1;
    wiegand26_state.pin_D0 = config->pin_D0;
    wiegand26_state.pin_D1 = config->pin_D1;
}

void wiegand26_write(uint32_t cardID) {
    uint32_t tmp = cardID;
    uint16_t parity_even_bit = 0;
    uint16_t parity_odd_bit = 1;
    uint8_t n = 0;

    for (n=0; n < 12; n++) {
        parity_odd_bit ^= (tmp & 1);
        tmp >>= 1;
    }
    for (n=12; n < 24; n++) {
        parity_even_bit ^= (tmp & 1);
        tmp >>= 1;
    }

    wiegand26_write_bit(parity_even_bit);
    for (n=0; n < 24; n++) {
        wiegand26_write_bit((cardID >> (23-n)) & 1);
    }
    wiegand26_write_bit(parity_odd_bit);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(wiegand26_interrupt_enable && GPIO_Pin == wiegand26_state.pin_D0)	{
        wiegand_read_D0();
    }
    else if(wiegand26_interrupt_enable && GPIO_Pin == wiegand26_state.pin_D1) {
        wiegand_read_D1();
    }
}
