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

#ifndef WIEGAND26_CONVERTER_PARSER_H
#define WIEGAND26_CONVERTER_PARSER_H

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

typedef enum ParserStatesEnum {WAIT_START_PACKET,
                               WAIT_END_PACKET} ParserStates;
typedef enum CommandTypesEnum {EMPTY,
                               RELAY_ON,
                               RELAY_OFF,
                               SEND_WIEGAND} CommandTypes;

#define NUM_COMMANDS	3
#define MAX_PACKET_LEN	255
#define NUM_OF_CHANNELS 4

#pragma pack(1)
typedef struct {
    ParserStates state;
    CommandTypes command;
    char buffer[MAX_PACKET_LEN];
    uint16_t data_cnt;
    uint32_t cardID;
    uint16_t channel;
} ParserStateType;
#pragma pack()

#pragma pack(1)
typedef struct {
    uint32_t cardID;
    CommandTypes command;
    uint16_t channel;
} CommandType;
#pragma pack()

extern __IO uint8_t uart_tx_ready;
extern ParserStateType parser_state;
extern __IO uint8_t uart_rx_buffer;
extern __IO CommandType command;
extern __IO uint8_t rx_ready;

void parser_init(void);
int8_t parse_next_byte(char dat, ParserStateType* state);
void uart_try_transmit(uint8_t *pData, uint16_t Size);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void rx_start(void);


#endif //WIEGAND26_CONVERTER_PARSER_H
