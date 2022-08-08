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

#include "../Inc/uart_utils.h"

CommandTypes ValidCommands[NUM_COMMANDS] = {RELAY_ON, RELAY_OFF, SEND_WIEGAND};
__IO uint8_t uart_tx_ready = 1;
ParserStateType parser_state;
__IO uint8_t uart_rx_buffer;
__IO CommandType command;
__IO uint8_t rx_ready = 0;

void parser_init(void) {
    parser_state.state = WAIT_START_PACKET;
}

int8_t parse_next_byte(char byte, ParserStateType* state) {
    uint8_t result = -1;
    int chan = 0;
    uint32_t cardID = 0;
    int commandID = 0;

    switch((state->state)) {
        case(WAIT_START_PACKET):
            if(byte == '$') {
                memset(state->buffer, 0, MAX_PACKET_LEN);
                state->data_cnt = 0;
                state->buffer[state->data_cnt] = byte;
                state->data_cnt++;
                state->state = WAIT_END_PACKET;
            }
            break;
        case(WAIT_END_PACKET):
            if (state->data_cnt < MAX_PACKET_LEN) {
                if(byte=='\n') {
                    if (state->data_cnt > 0) {
                        if (sscanf(&state->buffer[0], "$%d: card=%lu :command=%d", &chan, &cardID, &commandID) == 3) {
                            if ((commandID < NUM_COMMANDS)&&((uint16_t)chan < NUM_OF_CHANNELS)) {
                                state->cardID = cardID;
                                state->command = ValidCommands[commandID];
                                state->channel = (uint16_t)chan;
                                result = 1;
                            }
                        }
                    }
                    state->state = WAIT_START_PACKET;
                } else {
                    state->buffer[state->data_cnt] = byte;
                    state->data_cnt++;
                    state->state = WAIT_END_PACKET;
                }
            } else {
                state->state = WAIT_START_PACKET;
            }
            break;
        default:
            state->state = WAIT_START_PACKET;
            break;
    }
    return result;
}

void uart_try_transmit(uint8_t *pData, uint16_t Size) {
    if (uart_tx_ready == 1) {
        uart_tx_ready = 0;
        HAL_UART_Transmit_DMA(&hlpuart1, pData, Size);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &hlpuart1) {
        uart_tx_ready = 1;
    }
}

void rx_start(void) {
    HAL_UART_Receive_IT(&hlpuart1, (uint8_t*)&uart_rx_buffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &hlpuart1) {
        if (parse_next_byte(uart_rx_buffer, &parser_state) > 0) {
            command.cardID = parser_state.cardID;
            command.command = parser_state.command;
            command.channel = parser_state.channel;
            rx_ready = 1;
        }
        rx_start();
    }
}
